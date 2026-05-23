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
	// 멀티플레이 필수 : 리플리케이션 속성 등록 함수 선언
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
public:
	ATTRIBUTE_ACCESSORS(UFZFPlayerSet, Stamina);
	ATTRIBUTE_ACCESSORS(UFZFPlayerSet, MaxStamina);

public:
	UPROPERTY(BlueprintReadOnly, Category = Stat, ReplicatedUsing = OnRep_Stamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = Stat, ReplicatedUsing = OnRep_MaxStamina, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxStamina;
};
