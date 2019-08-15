// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTaskAttributeChanged.h"

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{
	UAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->RegisterAttributeChangeListener(AbilitySystemComponent, Attribute);
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributeToListenFor = Attribute;
	return WaitForAttributeChangedTask;
}

UAsyncTaskAttributeChanged * UAsyncTaskAttributeChanged::ListenForAttributesChange(UAbilitySystemComponent * AbilitySystemComponent, TArray<FGameplayAttribute> Attributes)
{
	UAsyncTaskAttributeChanged* WaitForAttributeChangedTask = NewObject<UAsyncTaskAttributeChanged>();
	WaitForAttributeChangedTask->RegisterAttributesChangeListener(AbilitySystemComponent, Attributes);
	WaitForAttributeChangedTask->ASC = AbilitySystemComponent;
	WaitForAttributeChangedTask->AttributesToListenFor = Attributes;
	return WaitForAttributeChangedTask;
}

void UAsyncTaskAttributeChanged::BeginDestroy()
{
	if (IsValid(ASC))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (FGameplayAttribute Attribute : AttributesToListenFor)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}

	Super::BeginDestroy();
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

void UAsyncTaskAttributeChanged::RegisterAttributesChangeListener(UAbilitySystemComponent * AbilitySystemComponent, TArray<FGameplayAttribute> Attributes)
{
	if (!AbilitySystemComponent)
	{
		RemoveFromRoot();
		return;
	}

	for (FGameplayAttribute Attribute : Attributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UAsyncTaskAttributeChanged::AttributeChanged);
	}
}

void UAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData & Data)
{
	OnAttributeChanged.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}