#include "CCD.h"
#include "CCDIK.h"

bool AbyssAnimation::SolveCCDIK_Impl(TArray<FCCDIKChainLink>& InOutChain, const FVector& TargetPosition,
	float Precision, int32 MaxIteration, bool bStartFromTail, bool bEnableRotationLimit,
	const TArray<float>& RotationLimitPerJoints)
{
	struct Local
	{
		static bool UpdateChainLink(TArray<FCCDIKChainLink>& Chain, int32 LinkIndex, const FVector& TargetPosition,
			bool bInEnableRotationLimit, const TArray<float>& InRotationLimitPerJoints)
		{
			const int32 TipBoneLinkIndex = Chain.Num() - 1;

			ensure(Chain.IsValidIndex(TipBoneLinkIndex));
			FCCDIKChainLink& CurrentLink = Chain[LinkIndex];

			FVector TipPos = Chain[TipBoneLinkIndex].Transform.GetLocation();

			FTransform& CurrentLinkTransform = CurrentLink.Transform;
			
			FVector ToEnd = TipPos - CurrentLinkTransform.GetLocation();
			ToEnd.Normalize();
			FVector ToTarget = TargetPosition - CurrentLinkTransform.GetLocation();
			ToTarget.Normalize();

			double RotationLimitPerJointInRadian = FMath::DegreesToRadians(InRotationLimitPerJoints[LinkIndex]);
			double Angle = FMath::ClampAngle(FMath::Acos(FVector::DotProduct(ToEnd, ToTarget)), -RotationLimitPerJointInRadian, RotationLimitPerJointInRadian);
			bool bCanRotate = (FMath::Abs(Angle) > DOUBLE_KINDA_SMALL_NUMBER) &&
				(!bInEnableRotationLimit || RotationLimitPerJointInRadian > CurrentLink.CurrentAngleDelta);

			if(bCanRotate)
			{
				if(bInEnableRotationLimit)
				{
					if(RotationLimitPerJointInRadian < CurrentLink.CurrentAngleDelta + Angle)
					{
						Angle = RotationLimitPerJointInRadian - CurrentLink.CurrentAngleDelta;
						if(Angle <= DOUBLE_KINDA_SMALL_NUMBER)
						{
							return false;
						}
					}

					CurrentLink.CurrentAngleDelta += Angle;
				}

				FVector RotationAxis = FVector::CrossProduct(ToEnd, ToTarget);
				if(RotationAxis.SizeSquared() > 0.f)
				{
					RotationAxis.Normalize();
					
					FQuat DeltaRotation(RotationAxis, Angle);

					FQuat NewRotation = DeltaRotation * CurrentLinkTransform.GetRotation();
					NewRotation.Normalize();
					CurrentLinkTransform.SetRotation(NewRotation);

					if(LinkIndex > 0)
					{
						const FCCDIKChainLink& Parent = Chain[LinkIndex - 1];
						CurrentLink.LocalTransform = CurrentLinkTransform.GetRelativeTransform(Parent.Transform);
						CurrentLink.LocalTransform.NormalizeRotation();
					}

					FTransform CurrentParentTransform = CurrentLinkTransform;
					for(int32 ChildLinkIndex = LinkIndex + 1; ChildLinkIndex <= TipBoneLinkIndex; ++ChildLinkIndex)
					{
						FCCDIKChainLink& ChildIterLink = Chain[ChildLinkIndex];
						const FTransform LocalTransform = CurrentLink.LocalTransform;
						ChildIterLink.Transform = LocalTransform * CurrentParentTransform;
						ChildIterLink.Transform.NormalizeRotation();
						CurrentParentTransform = ChildIterLink.Transform;
					}

					return true;
				}
			}

			return false;
		}		
	};

	return false;
}
