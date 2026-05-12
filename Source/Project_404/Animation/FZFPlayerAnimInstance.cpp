// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FZFPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FZFAbilitySystemComponent.h" 
#include "AbilitySystemInterface.h"
#include "GameplayTag/FZFGameplayTags.h"

UFZFPlayerAnimInstance::UFZFPlayerAnimInstance()
{
    bIsRunning = false;
}

void UFZFPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UFZFPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

    if (IAbilitySystemInterface* ASCHolder = Cast<IAbilitySystemInterface>(Owner))
    {
        UFZFAbilitySystemComponent* ASC = Cast<UFZFAbilitySystemComponent>(ASCHolder->GetAbilitySystemComponent());
        if (ASC)
        {
            bIsRunning = ASC->HasMatchingGameplayTag(FZFGameplayTags::State_Movement_Run);
        }
    }
}
