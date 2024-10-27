// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "AbyssAnimInstance.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS(Config = Game)
class ABYSS_API UAbyssAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UAbyssAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

private:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:

	// 可映射到蓝图变量的游戏标签。当标签被添加或删除时，变量将自动更新。
	// 应该使用这些变量来代替手动查询游戏标签。
	UPROPERTY(EditDefaultsOnly, Category="GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category="Character State Data",meta=(AllowPrivateAccess="true"))
	float GroundDistance = -1.f;
};
