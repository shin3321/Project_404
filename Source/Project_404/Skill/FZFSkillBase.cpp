// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/FZFSkillBase.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

AFZFSkillBase::AFZFSkillBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AFZFSkillBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && LifeTime > 0.0f)
	{
		SetLifeSpan(LifeTime);
	}
}

void AFZFSkillBase::InitializeSkill(AActor* InSkillOwner)
{
	SkillOwner = InSkillOwner;

	if (SkillOwner)
	{
		SourceASC = GetASCFromActor(SkillOwner);
	}
}

UAbilitySystemComponent* AFZFSkillBase::GetASCFromActor(AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return nullptr;
	}

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		return ASI->GetAbilitySystemComponent();
	}

	return nullptr;
}

bool AFZFSkillBase::IsServer() const
{
	return HasAuthority();
}
