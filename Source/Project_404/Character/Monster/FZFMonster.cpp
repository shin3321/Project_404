// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/FZFMonster.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFAttributeSet.h"

AFZFMonster::AFZFMonster()
{
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("AbilitySystem");
	ASC->SetIsReplicated(true);

}

void AFZFMonster::BeginPlay()
{
	Super::BeginPlay();
	InitAbilitySystem();
}

void AFZFMonster::InitAbilitySystem()
{
	Super::InitAbilitySystem();
	ASC->InitAbilityActorInfo(this, this);

}
