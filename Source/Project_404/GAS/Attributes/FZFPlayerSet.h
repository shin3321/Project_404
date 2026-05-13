// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributes/FZFAttributeSet.h"
#include "FZFPlayerSet.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFPlayerSet : public UFZFAttributeSet
{
	GENERATED_BODY()
	
public:
	UFZFPlayerSet();

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	ATTRIBUTE_ACCESSORS(UFZFPlayerSet, Stamina);
	ATTRIBUTE_ACCESSORS(UFZFPlayerSet, MaxStamina);

public:
	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxStamina;
};
