// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "AsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * 把所有属性变化都注册了同一个回调
 *
 * BlueprintType
 *
 * 可以作为蓝图中的变量的类型
 * 
 * meta=(ExposedAsyncProxy = AsyncTask)
 *
 * 加上这个之后，蓝图中的 EndTask 节点会暴露一个 AsyncTask 的输入，然后在 EndTask 中调用 SetReadyToDestroy(); MarkAsGarbage(); 就可以
 * 结束 AsyncTask ，否则在蓝图中调用 EndTask 是无效的。
 * https://forums.unrealengine.com/t/is-there-a-way-to-manually-terminate-ublueprintasyncactionbase/135399
 */
/**
 * Blueprint node to automatically register a listener for all attribute changes in an AbilitySystemComponent.
 * Useful to use in UI.
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class GASDOCUMENTATION_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;
	
	/**
	 * 以下是两个工厂方法，作为蓝图中的异步节点
	 * 
	 * BlueprintCallable
	 *
	 * 蓝图中可调用
	 *
	 * meta = (BlueprintInternalUseOnly = "true")
	 *
	 * 一般情况下引擎会为所有 BlueprintCallable 自动生成一个蓝图节点，这个 meta 会让引擎跳过这一步骤，UBlueprintAsyncActionBase 中的静
	 * 态方法，有一个特殊的生成过程，这是因为这不是一般的函数节点，而是一个 “latent node”。如果不加上这个 meta ，蓝图中会有两个节点（一个按照
	 * 一般过程生成，一个做了特殊处理（节点的右上角有一个表），只有后者能用）。
	 * https://www.reddit.com/r/unrealengine/comments/vwqjms/what_does_blueprintinternaluseonly_do_for/
	 */
	
	// Listens for an attribute changing.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);

	// Listens for an attribute changing.
	// Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes);

	/**
	 * 必须手动调用这个方法来结束异步任务
	 */
	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	void AttributeChanged(const FOnAttributeChangeData& Data);
};
