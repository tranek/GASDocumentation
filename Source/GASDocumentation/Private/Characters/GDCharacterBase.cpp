// Copyright 2020 Dan Kestranek.


#include "Characters/GDCharacterBase.h"
#include "Characters/Abilities/AttributeSets/GDAttributeSetBase.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"
#include "Characters/Abilities/GDGameplayAbility.h"
#include "Characters/GDCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/GDDamageTextWidgetComponent.h"

// Sets default values
AGDCharacterBase::AGDCharacterBase(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UGDCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	// Cache tags
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

UAbilitySystemComponent * AGDCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AGDCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 AGDCharacterBase::GetAbilityLevel(EGDAbilityInputID AbilityID) const
{
	return 1;
}

void AGDCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

EGDHitReactDirection AGDCharacterBase::GetHitReactDirection(const FVector & ImpactPoint)
{
	const FVector& ActorLocation = GetActorLocation();
	// PointPlaneDist is super cheap - 1 vector subtraction, 1 dot product.
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(ImpactPoint, ActorLocation, GetActorForwardVector());


	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		// Determine if Front or Back

		// Can see if it's left or right of Left/Right plane which would determine Front or Back
		if (DistanceToRightLeftPlane >= 0)
		{
			return EGDHitReactDirection::Front;
		}
		else
		{
			return EGDHitReactDirection::Back;
		}
	}
	else
	{
		// Determine if Right or Left

		if (DistanceToFrontBackPlane >= 0)
		{
			return EGDHitReactDirection::Right;
		}
		else
		{
			return EGDHitReactDirection::Left;
		}
	}

	return EGDHitReactDirection::Front;
}

void AGDCharacterBase::PlayHitReact_Implementation(FGameplayTag HitDirection, AActor * DamageCauser)
{
	if (IsAlive())
	{
		if (HitDirection == HitDirectionLeftTag)
		{
			ShowHitReact.Broadcast(EGDHitReactDirection::Left);
		}
		else if (HitDirection == HitDirectionFrontTag)
		{
			ShowHitReact.Broadcast(EGDHitReactDirection::Front);
		}
		else if (HitDirection == HitDirectionRightTag)
		{
			ShowHitReact.Broadcast(EGDHitReactDirection::Right);
		}
		else if (HitDirection == HitDirectionBackTag)
		{
			ShowHitReact.Broadcast(EGDHitReactDirection::Back);
		}
	}
}

bool AGDCharacterBase::PlayHitReact_Validate(FGameplayTag HitDirection, AActor * DamageCauser)
{
	return true;
}

int32 AGDCharacterBase::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
	{
		return static_cast<int32>(AttributeSetBase->GetCharacterLevel());
	}

	return 0;
}

float AGDCharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMana();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMaxMana() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxMana();
	}

	return 0.0f;
}

float AGDCharacterBase::GetStamina() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetStamina();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMaxStamina() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxStamina();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float AGDCharacterBase::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

// Run on Server and all clients
void AGDCharacterBase::Die()
{
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AGDCharacterBase::FinishDying()
{
	Destroy();
}

// Called when the game starts or when spawned
void AGDCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGDCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UGDGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AGDCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AGDCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void AGDCharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void AGDCharacterBase::SetMana(float Mana)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetMana(Mana);
	}
}

void AGDCharacterBase::SetStamina(float Stamina)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetStamina(Stamina);
	}
}

