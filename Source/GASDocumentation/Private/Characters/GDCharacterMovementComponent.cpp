// Copyright 2020 Dan Kestranek.


#include "Characters/GDCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Characters/GDCharacterBase.h"
#include "GameplayTagContainer.h"

UGDCharacterMovementComponent::UGDCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	ADSSpeedMultiplier = 0.5f;
}

float UGDCharacterMovementComponent::GetMaxSpeed() const
{
	AGDCharacterBase* Owner = Cast<AGDCharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
		return 0.0f;
	}

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (RequestToStartADS)
	{
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}

void UGDCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//The Flags parameter contains the compressed input flags that are stored in the saved move.
	//UpdateFromCompressed flags simply copies the flags from the saved move into the movement component.
	//It basically just resets the movement component to the state when the move was made so it can simulate from there.
	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client * UGDCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UGDCharacterMovementComponent* MutableThis = const_cast<UGDCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FGDNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UGDCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UGDCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

void UGDCharacterMovementComponent::StartAimDownSights()
{
	RequestToStartADS = true;
}

void UGDCharacterMovementComponent::StopAimDownSights()
{
	RequestToStartADS = false;
}

void UGDCharacterMovementComponent::FGDSavedMove::Clear()
{
	Super::Clear();

	SavedRequestToStartSprinting = false;
	SavedRequestToStartADS = false;
}

uint8 UGDCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool UGDCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr & NewMove, ACharacter * Character, float MaxDelta) const
{
	//Set which moves can be combined together. This will depend on the bit flags that are used.
	if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartADS != ((FGDSavedMove*)&NewMove)->SavedRequestToStartADS)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UGDCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter * Character, float InDeltaTime, FVector const & NewAccel, FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UGDCharacterMovementComponent* CharacterMovement = Cast<UGDCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartADS = CharacterMovement->RequestToStartADS;
	}
}

void UGDCharacterMovementComponent::FGDSavedMove::PrepMoveFor(ACharacter * Character)
{
	Super::PrepMoveFor(Character);

	UGDCharacterMovementComponent* CharacterMovement = Cast<UGDCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
	}
}

UGDCharacterMovementComponent::FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(const UCharacterMovementComponent & ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UGDCharacterMovementComponent::FGDNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FGDSavedMove());
}
