// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GDCharacterBase.h"
#include "GameplayEffectTypes.h"
#include "GDMinionCharacter.generated.h"

/**
 * An AI controlled minion character.
 */
UCLASS()
class GASDOCUMENTATION_API AGDMinionCharacter : public AGDCharacterBase
{
	GENERATED_BODY()

public:
	AGDMinionCharacter(const class FObjectInitializer& ObjectInitializer);

protected:

	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	class UGDAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	class UGDAttributeSetBase* HardRefAttributeSetBase;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UGDFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UGDFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
