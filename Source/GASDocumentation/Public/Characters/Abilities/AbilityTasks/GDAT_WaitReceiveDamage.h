// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GDAT_WaitReceiveDamage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWaitReceiveDamageDelegate, class UGDAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

/**
 * Waits until the Ability Owner receives damage.
 */
UCLASS()
class GASDOCUMENTATION_API UGDAT_WaitReceiveDamage : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintAssignable)
	FWaitReceiveDamageDelegate OnDamage;

	virtual void Activate() override;

	UFUNCTION()
	void OnDamageReceived(class UGDAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	// Wait until the ability owner receives damage.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UGDAT_WaitReceiveDamage* WaitReceiveDamage(UGameplayAbility* OwningAbility, bool TriggerOnce);

protected:
	bool TriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
