#pragma once

struct FCCDIKChainLink;

namespace AbyssAnimation
{
	bool SolveCCDIK_Impl(TArray<FCCDIKChainLink>& InOutChain, const FVector& TargetPosition,
		float Precision, int32 MaxIteration, bool bStartFromTail, bool bEnableRotationLimit, 
		const TArray<float>& RotationLimitPerJoints);
}
