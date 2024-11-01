// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IndicatorDescriptor.generated.h"

class UIndicatorDescriptor;
class UAbyssIndicatorManagerComponent;

struct FIndicatorProjection
{
	bool Project(const UIndicatorDescriptor& IndicatorDescriptor, const FSceneViewProjectionData& InProjectionData, const FVector2f& ScreenSize, FVector& ScreenPositionWithDepth);
};

UCLASS()
class ABYSS_API UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

	UIndicatorDescriptor() = default;

public:

	UFUNCTION(BlueprintCallable)
	UObject* GetDataObject() const { return DataObject.Get(); }
	UFUNCTION(BlueprintCallable)
	void SetDataObject(UObject* InDataObject) { DataObject = InDataObject; }

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponent() const { return Component; }
	UFUNCTION(BlueprintCallable)
	void SetSceneComponent(USceneComponent* InComponent){ Component = InComponent; }

	UFUNCTION(BlueprintCallable)
	TSoftClassPtr<UUserWidget> GetIndicatorClass() const { return IndicatorWidgetClass; }
	UFUNCTION(BlueprintCallable)
	void SetIndicatorClass(TSoftClassPtr<UUserWidget> InWidgetClass)
	{
		IndicatorWidgetClass = InWidgetClass;
	}

	/**
	*
	*	初始化UI指示器 :
	*	调用此方法可以让指示器在游戏世界中关联到某个特定的场景组件（通常是一个3D对象），
	*	并指定一个用于渲染UI的指示器Widget类。
	* 
	* @param InDataObject 指示器所关联的逻辑对象，可能是一个角色、道具、任务点等，用于提供与指示器相关的数据。
	* @param InSceneComponent 这是指示器将在3D世界中绑定的场景组件。指示器将跟随该组件的位置并与其相关联。
	* @param InIndicatorWidgetClass UI 指示器的类，定义了指示器在屏幕上的外观及交互方式。该类通常是 UUserWidget 的子类，用于渲染UI内容。
	*/
	UFUNCTION(BlueprintCallable)
	void Initialize(UObject* InDataObject, USceneComponent* InSceneComponent, TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		SetDataObject(InDataObject);
		SetSceneComponent(InSceneComponent);
		SetIndicatorClass(InIndicatorWidgetClass);
	}

	UAbyssIndicatorManagerComponent* GetManagerComponent() const { return ManagerPtr.Get(); }
	void SetManagerComponent(UAbyssIndicatorManagerComponent* InManager);

private:

	UPROPERTY()
	TObjectPtr<UObject> DataObject;

	UPROPERTY()
	TObjectPtr<USceneComponent> Component;

	UPROPERTY()
	FName ComponentSocketName = NAME_None;

	UPROPERTY()
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;
	
	UPROPERTY()
	TWeakObjectPtr<UAbyssIndicatorManagerComponent> ManagerPtr;

	TWeakPtr<SWidget> Content;
	TWeakPtr<SWidget> CanvasHost;
	
	friend class SActorCanvas;
	
public:
	
	bool CanAutomaticallyRemove() const
	{
		return bAutoRemoveWhenIndicatorComponentIsNull && !IsValid(GetSceneComponent());
	}

	UFUNCTION(BlueprintCallable, BlueprintGetter)
	bool GetVisible() const { return IsValid(GetSceneComponent()) && bVisible; }
	UFUNCTION(BlueprintCallable, BlueprintSetter)
	void SetDesiredVisibility(bool InVisible)
	{
		bVisible = InVisible;
	}

	bool GetClampToScreen() const { return bClampToScreen; };
	void SetClampToScreen(bool Value) { bClampToScreen = Value; }
	
	int GetPriority() const { return Priority; }
	void SetPriority(int32 InPriority) { Priority = InPriority; }

public:
	TWeakObjectPtr<UUserWidget> IndicatorWidget;
	
private:
	UPROPERTY(BlueprintGetter=GetVisible, BlueprintSetter=SetDesiredVisibility)
	bool bVisible = true;
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	bool bClampToScreen = false;
	UPROPERTY()
	bool bAutoRemoveWhenIndicatorComponentIsNull = false;

	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	int32 Priority = 0;
};
