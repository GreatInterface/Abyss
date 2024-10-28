// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddGameplayCuePath.generated.h"

UCLASS(MinimalAPI, meta=(DisplayName="Add Gameplay Cue Path"))
class UGameFeatureAction_AddGameplayCuePath final : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	
	UGameFeatureAction_AddGameplayCuePath();

	const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }

private:
	
	/**
	 * RelativeToGameContentDir : 使得系统目录选择对话框的结果为相对Content的相对路径
	 * LongPackageName : 使用UE的风格来选择Content以及子目录，或者把文件路径转换为长包名
	 */
	UPROPERTY(EditAnywhere, Category="Game Feature | Gameplay Cues", meta=(RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> DirectoryPathsToAdd;
};
