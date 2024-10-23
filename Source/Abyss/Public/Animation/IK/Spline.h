#pragma once
#include "SplineIK.h"

namespace AbyssAnimation
{
	void SolveSpineIK_Abyss(const TArray<FTransform>& BoneTransforms, const FInterpCurveVector& PositionSpline,
		const FInterpCurveQuat& RotationSpline, const FInterpCurveVector& ScaleSpline,
		const float TotalSplineAlpha, const float TotalSplineLength,
		const FFloatMapping& Twist, const float Roll,
		const float Stretch, const float Offset,
		const EAxis::Type BoneAxis, const FFindParamAtFirstSphereIntersection& FindParamAtFirstSphereIntersection,
		const TArray<FQuat>& BoneOffsetRotations, const TArray<float>& BoneLengths,
		const float OriginalSplineLength, TArray<FTransform>& OutBoneTransforms);	
}
