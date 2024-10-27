#include "FABR.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(FABR)

namespace AbyssAnimation
{
	bool SolveFABRIK(TArray<FFABRIKChainLink_Abyss>& InOutChain, const FVector& TargetPosition,
	double MaximumReach, double Precision, int32 MaxIteration)
	{
		bool bBoneLocationUpdated = false;
		const double RootToTargetDistSq = FVector::DistSquared(InOutChain[0].Position, TargetPosition); 
		const int32 NumChainLinks = InOutChain.Num();

		if(RootToTargetDistSq > FMath::Square(MaximumReach))
		{
			for(int32 LinkIndex = 1; LinkIndex < NumChainLinks; ++LinkIndex)
			{
				const FFABRIKChainLink_Abyss& ParentLink = InOutChain[LinkIndex - 1];
				FFABRIKChainLink_Abyss& CurrentLink = InOutChain[LinkIndex];
				CurrentLink.Position = ParentLink.Position + (TargetPosition - ParentLink.Position).GetUnsafeNormal() * CurrentLink.Length;
			}
			bBoneLocationUpdated = true;
		}
		else
		{
			const int32 TipBoneLinkIndex = NumChainLinks - 1;

			double Slop = FVector::Dist(InOutChain[TipBoneLinkIndex].Position, TargetPosition);
			if(Slop > Precision)
			{
				InOutChain[TipBoneLinkIndex].Position = TargetPosition;

				int32 IterationCout = 0;
				while ((Slop > Precision) && (IterationCout++ < MaxIteration))
				{
					//Root位置保持不变
					for(int32 LinkIndex = TipBoneLinkIndex - 1; LinkIndex > 0; ++LinkIndex)
					{
						FFABRIKChainLink_Abyss& CurrentLink = InOutChain[LinkIndex]; 
						const FFABRIKChainLink_Abyss& ChildLink = InOutChain[LinkIndex + 1];

						CurrentLink.Position = ChildLink.Position + (CurrentLink.Position - ChildLink.Position).GetUnsafeNormal() * ChildLink.Length;
					}

					for(int32 LinkIndex = 1; LinkIndex < TipBoneLinkIndex; ++LinkIndex)
					{
						const FFABRIKChainLink_Abyss& ParentLink = InOutChain[LinkIndex - 1];
						FFABRIKChainLink_Abyss& CurrentLink = InOutChain[LinkIndex];

						CurrentLink.Position = ParentLink.Position + (CurrentLink.Position - ParentLink.Position).GetUnsafeNormal() * CurrentLink.Length;
					}

					Slop = FMath::Abs(InOutChain[TipBoneLinkIndex].Length - FVector::Dist(InOutChain[TipBoneLinkIndex - 1].Position, TargetPosition));
				}
			}
		}
		
		return bBoneLocationUpdated;
	}
}
