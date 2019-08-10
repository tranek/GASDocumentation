// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GDFloatingStatusBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDFloatingStatusBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetManaPercentage(float ManaPercentage);
};
