#pragma once
#include "FABR.generated.h"

USTRUCT()
struct FFABRIKChainLink_Abyss 
{
	GENERATED_BODY();

public:
	FVector Position;

	double Length;

	int32 BoneIndex;

	int32 TransformIndex;

	FVector DefaultDirToParent;

	TArray<int32> ChildZeroLengthTransformIndices;

	FFABRIKChainLink_Abyss()
		: Position(FVector::ZeroVector)
		, Length(0.0)
		, BoneIndex(INDEX_NONE)
		, TransformIndex(INDEX_NONE)
		, DefaultDirToParent(FVector(-1.0, 0.0, 0.0))
	{}

	FFABRIKChainLink_Abyss(const FVector& InPosition, const double InLength, const FCompactPoseBoneIndex& InBoneIndex, const int32& InTransformIndex)
		: Position(InPosition)
		, Length(InLength)
		, BoneIndex(InBoneIndex.GetInt())
		, TransformIndex(InTransformIndex)
		, DefaultDirToParent(FVector(-1.0, 0.0, 0.0))
	{
	}
};


namespace AbyssAnimation
{
	bool SolveFABRIK(TArray<FFABRIKChainLink_Abyss>& InOutChain, const FVector& TargetPosition, double MaximumReach, double Precision, int32 MaxIteration);
}
