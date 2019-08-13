// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTaskAttributeChanged.h"

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->RegisterAttributeChangeListener(AbilitySystemComponent, Attribute);
	return WaitForAttributeChangedTask;
}

void UAsyncTaskAttributeChanged::RegisterAttributeChangeListener(UAbilitySystemComponent * AbilitySystemComponent, FGameplayAttribute Attribute)
{
	if (!AbilitySystemComponent)
	{
		RemoveFromRoot();
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAsyncTaskAttributeChanged::AttributeChanged);
}

void UAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData & Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}
