// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatures/GameFeatureAction_AddGameplayCuePath.h"

UGameFeatureAction_AddGameplayCuePath::UGameFeatureAction_AddGameplayCuePath()
{
	DirectoryPathsToAdd.Add(FDirectoryPath{ TEXT("/GameplayCues") });
}
