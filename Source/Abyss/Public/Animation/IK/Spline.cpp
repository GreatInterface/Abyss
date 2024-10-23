#include "Spline.h"

void AbyssAnimation::SolveSpineIK_Abyss(const TArray<FTransform>& BoneTransforms,
	const FInterpCurveVector& PositionSpline, const FInterpCurveQuat& RotationSpline,
	const FInterpCurveVector& ScaleSpline, const float TotalSplineAlpha, const float TotalSplineLength,
	const FFloatMapping& Twist, const float Roll, const float Stretch, const float Offset, const EAxis::Type BoneAxis,
	const FFindParamAtFirstSphereIntersection& FindParamAtFirstSphereIntersection,
	const TArray<FQuat>& BoneOffsetRotations, const TArray<float>& BoneLengths, const float OriginalSplineLength,
	TArray<FTransform>& OutBoneTransforms)
{
	check(BoneTransforms.Num() == BoneLengths.Num());
	check(BoneTransforms.Num() == BoneOffsetRotations.Num());
	check(Twist.IsBound());
	check(FindParamAtFirstSphereIntersection.IsBound());

	OutBoneTransforms.Reset();

	const float TotalStretchRatio = FMath::Lerp(OriginalSplineLength, TotalSplineLength, Stretch) / OriginalSplineLength;

	FVector PreviousPoint; int32 StartingLinearIndex = 0; float InitialAlpha = 0.0f;
	if(Offset == 0.f)
	{
		PreviousPoint = PositionSpline.Points[0].OutVal;
	}
	else
	{
		InitialAlpha = FindParamAtFirstSphereIntersection.Execute(PositionSpline.Points[0].OutVal, Offset, StartingLinearIndex);
		PreviousPoint = PositionSpline.Eval(InitialAlpha);
	}
	
	
}
