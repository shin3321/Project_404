#include "Character/Player/FZFPlayerState.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFPlayerSet.h"

AFZFPlayerState::AFZFPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("ASC");
}

UAbilitySystemComponent* AFZFPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UFZFPlayerSet* AFZFPlayerState::GetPlayerSet() const
{
	return PlayerSet;
}
