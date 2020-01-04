// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GASDocumentation/GASDocumentation.h"
#include "GDCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EGDHitReactDirection, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGDCharacterBase*, Character);

/**
* The base Character class for the game. Everything with an AbilitySystemComponent in this game will inherit from this class.
* This class should not be instantiated and instead subclassed.
*/
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

	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterDiedDelegate OnCharacterDied;

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EGDAbilityInputID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable)
	EGDHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void PlayHitReact(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual void PlayHitReact_Implementation(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual bool PlayHitReact_Validate(FGameplayTag HitDirection, AActor* DamageCauser);


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

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMaxStamina() const;
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter|Attributes")
	float GetMoveSpeedBaseValue() const;


	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual void FinishDying();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Instead of TWeakObjectPtrs, you could just have UPROPERTY() hard references or no references at all and just call
	// GetAbilitySystem() and make a GetAttributeSetBase() that can read from the PlayerState or from child classes.
	// Just make sure you test if the pointer is valid before using.
	// I opted for TWeakObjectPtrs because I didn't want a shared hard reference here and I didn't want an extra function call of getting
	// the ASC/AttributeSet from the PlayerState or child classes every time I referenced them in this base class.

	TWeakObjectPtr<class UGDAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UGDAttributeSetBase> AttributeSetBase;

	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|GDCharacter")
	FText CharacterName;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|Animation")
	UAnimMontage* DeathMontage;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGDGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();


	/**
	* Setters for Attributes. Only use these in special cases like Respawning, otherwise use a GE to change Attributes.
	* These change the Attribute's Base Value.
	*/

	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
};
