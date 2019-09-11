// Copyright 2019 Dan Kestranek.


#include "GDPlayerController.h"
#include "AbilitySystemComponent.h"
#include "GDDamageTextWidgetComponent.h"
#include "GDHeroCharacter.h"
#include "GDPlayerState.h"
#include "UI/GDHUDWidget.h"

void AGDPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), TEXT(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AGDPlayerState* PS = GetPlayerState<AGDPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UGDHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / PS->GetMaxHealth());
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / PS->GetMaxMana());
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / PS->GetMaxStamina());
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetExperience(PS->GetXP());
	UIHUDWidget->SetGold(PS->GetGold());
	UIHUDWidget->SetHeroLevel(PS->GetCharacterLevel());

	DamageNumberClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/UI/WC_DamageText.WC_DamageText_C"));
	if (!DamageNumberClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find DamageNumberClass. If it was moved, please update the reference location in C++."), TEXT(__FUNCTION__));
	}
}

UGDHUDWidget * AGDPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void AGDPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AGDCharacterBase * TargetCharacter)
{
	UGDDamageTextWidgetComponent* DamageText = NewObject<UGDDamageTextWidgetComponent>(TargetCharacter, DamageNumberClass);
	DamageText->RegisterComponent();
	DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageText->SetDamageText(DamageAmount);
}

bool AGDPlayerController::ShowDamageNumber_Validate(float DamageAmount, AGDCharacterBase * TargetCharacter)
{
	return true;
}

void AGDPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

bool AGDPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
	return true;
}

// Server only
void AGDPlayerController::OnPossess(APawn * InPawn)
{
	Super::OnPossess(InPawn);

	AGDPlayerState* PS = GetPlayerState<AGDPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AGDPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}
