#include "TwoBone.h"

void AbyssAnimation::SolveTwoBoneIK(const FVector& RootPos, const FVector& JointPos, const FVector& EndPos,
	const FVector& JointTarget, const FVector& Effector, FVector& OutJointPos, FVector& OutEndPos,
	bool bAllowStretching, double StartStretchRatio, double MaxStretchRatio)
{
	const double LowerLimbLength = (EndPos - JointPos).Size();
	const double UpperLimbLength = (JointPos - RootPos).Size();

	SolveTwoBoneIK_Impl(RootPos, JointPos, EndPos, JointTarget, Effector, OutJointPos, OutEndPos, LowerLimbLength, UpperLimbLength, bAllowStretching, StartStretchRatio, MaxStretchRatio);
}

void AbyssAnimation::SolveTwoBoneIK_Impl(const FVector& RootPos, const FVector& JointPos, const FVector& EndPos,
	const FVector& JointTarget, const FVector& Effector, FVector& OutJointPos, FVector& OutEndPos,
	double LowerLimbLength, double UpperLimbLength, bool bAllowStretching, double StartStretchRatio,
	double MaxStretchRatio)
{
	FVector DesiredPos = Effector;
	FVector DesiredDelta = DesiredPos - RootPos;
	double DesiredLength = DesiredDelta.Size();

	double MaxLimbLength = LowerLimbLength + UpperLimbLength;

	FVector DesiredDir{};
	if(DesiredLength < DOUBLE_KINDA_SMALL_NUMBER)
	{
		DesiredLength = DOUBLE_KINDA_SMALL_NUMBER;
		DesiredDir = FVector(1., 0., 0.);
	}
	else
	{
		DesiredDir = DesiredDelta.GetSafeNormal();
	}

	FVector JointTargetDelta = JointTarget - RootPos;
	const double JointTargetLengthSqr = JointTargetDelta.SizeSquared();

	FVector JointPlaneNormal, JointBendDir;
	if(JointTargetLengthSqr < FMath::Square(DOUBLE_KINDA_SMALL_NUMBER))
	{
		JointBendDir = FVector(0., 1., 0.);
		JointPlaneNormal = FVector(0, 0, 1);
	}
	else
	{
		JointPlaneNormal = DesiredDir ^ JointTargetDelta;
		if(JointPlaneNormal.SizeSquared() < FMath::Square(DOUBLE_KINDA_SMALL_NUMBER))
		{
			DesiredDir.FindBestAxisVectors(JointPlaneNormal, JointBendDir);
		}
		else
		{
			JointPlaneNormal.Normalize();
			
			JointBendDir = JointTargetDelta - ((JointTargetDelta | DesiredDir) * DesiredDir);
			JointBendDir.Normalize();
		}
	}

	if(bAllowStretching)
	{
		const double ScaleRange = MaxStretchRatio - StartStretchRatio;
		if(ScaleRange > DOUBLE_KINDA_SMALL_NUMBER && MaxLimbLength > DOUBLE_KINDA_SMALL_NUMBER)
		{
			const double ReachRatio = DesiredLength / MaxLimbLength;
			const double ScalingFactor = (MaxStretchRatio - 1.0) * FMath::Clamp((ReachRatio - StartStretchRatio) / ScaleRange, 0.0, 1.0);
			if(ScalingFactor > DOUBLE_KINDA_SMALL_NUMBER)
			{
				LowerLimbLength *= (1.0 + ScalingFactor);
				UpperLimbLength *= (1.0 + ScalingFactor);
				MaxLimbLength *= (1.0 + ScalingFactor);
			}
		}
	}

	OutEndPos = DesiredPos;
	OutJointPos = JointPos;

	//两边之和小于第三边,意味这Root - Effector不在以MaxLimbLength为半径的圆内，解决方式是让LowerBone和UpperBone的方向朝向Effector
	if(DesiredLength >= MaxLimbLength)
	{
		OutEndPos = RootPos + (MaxLimbLength * DesiredDir);
		OutJointPos = RootPos + (UpperLimbLength * DesiredDir);
	}
	else
	{
		const double TwoAB = 2.0 * UpperLimbLength * DesiredLength;

		//Root-OutJointPos.Length() == Root-JointPos.Length() && Joint-End.Length == OutJointPos-Effector.Length
		const double CosAngle = (TwoAB != 0.0) ? ((UpperLimbLength*UpperLimbLength) + (DesiredLength*DesiredLength) - (LowerLimbLength*LowerLimbLength)) / TwoAB : 0.0;
		const bool bReverseUpperBone = (CosAngle < 0.0);
		
		const double Angle = FMath::Acos(CosAngle);

		const double JointLineDist = UpperLimbLength * FMath::Sin(Angle);
		
		const double ProjJointDistSqr = (UpperLimbLength*UpperLimbLength) - (JointLineDist*JointLineDist);
		double ProJJointDist = (ProjJointDistSqr > 0) ? FMath::Sqrt(ProjJointDistSqr) : 0.0;
		if(bReverseUpperBone)
		{
			ProJJointDist *= -1.f;
		}

		OutJointPos = RootPos + (ProJJointDist * DesiredDir) + (JointLineDist * JointBendDir);
	}
}
