#pragma once

namespace AbyssAnimation
{
	/**
	 *
	 *	TwoBoneIK 是一种反向运动学（IK）算法，
	 *	它通过给定的终点（Effector 位置）和关节目标（Joint Target 位置）来调整关节（Joint）和末端（End）的运动。
	 *	此算法基于几何学，处理的是骨骼在三维空间的移动问题。算法不处理旋转，仅对位置进行计算。
	 * 
	 * @param RootPos  Root Position
	 * @param JointPos  Joint Position中间关节的位置（比如肘部或膝盖）
	 * @param EndPos  End Position骨骼链条末端的位置（比如手掌或脚踝）。
	 * @param JointTarget  关节的目标位置，表示关节应该朝向的方向（用于在关节和根之间形成平面以求解 IK）
	 * @param Effector  末端的目标位置，即需要到达的目标位置。
	 * @param OutJointPos 关节的新位置
	 * @param OutEndPos  末端的新位置
	 * @param bAllowStretching 是否允许骨骼拉伸，默认情况下骨骼链条的长度固定，但可以通过拉伸来延长骨骼链条的长度。
	 * @param StartStretchRatio 拉伸开始的比例，1.0 表示在骨骼链条的原始长度下不拉伸
	 * @param MaxStretchRatio 允许的最大拉伸比例
	 */
	void SolveTwoBoneIK(const FVector& RootPos, const FVector& JointPos, const FVector& EndPos, const FVector& JointTarget, const FVector& Effector,
		FVector& OutJointPos, FVector& OutEndPos, bool bAllowStretching, double StartStretchRatio, double MaxStretchRatio);
	
	void SolveTwoBoneIK_Impl(const FVector& RootPos, const FVector& JointPos, const FVector& EndPos, const FVector& JointTarget, const FVector& Effector,
		FVector& OutJointPos, FVector& OutEndPos, double LowerLimbLength, double UpperLimbLength, bool bAllowStretching, double StartStretchRatio, double MaxStretchRatio);
}
