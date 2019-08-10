// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GDPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API AGDPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void CreateHUD();

	class UGDHUDWidget* GetHUD();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UGDHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASDocumentation|UI")
	class UGDHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;
};
