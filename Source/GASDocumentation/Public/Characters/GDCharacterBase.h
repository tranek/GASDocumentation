// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GASDocumentation.h"
#include "GDCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EGDHitReactDirection, Direction);

UCLASS()
class GASDOCUMENTATION_API AGDCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGDCharacterBase(const class FObjectInitializer& ObjectInitializer);

	// Set the Hit React direction in the Animation Blueprint
	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterBaseHitReactDelegate ShowHitReact;

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EGDAbilityInputID AbilityID) const;

	UFUNCTION(BlueprintCallable)
	EGDHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);

	/**
	* Getters for attributes from GDAttributeSetBase
	**/
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMaxMana() const;
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMoveSpeedBaseValue() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TWeakObjectPtr<class UGDAbilitySystemComponent> AbilitySystemComponent;

	TWeakObjectPtr<class UGDAttributeSetBase> AttributeSetBase;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGDGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	virtual void AddCharacterAbilities();

	virtual void RemoveCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();
};
