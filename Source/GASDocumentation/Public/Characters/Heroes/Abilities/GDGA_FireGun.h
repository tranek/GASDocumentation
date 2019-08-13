// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GDGameplayAbility.h"
#include "Abilities/Tasks/GDAT_PlayMontageAndWaitForEvent.h"
#include "GDProjectile.h"
#include "GDGA_FireGun.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UGDGA_FireGun : public UGDGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGDGA_FireGun();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireIronsightsMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AGDProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Range = 1000;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
