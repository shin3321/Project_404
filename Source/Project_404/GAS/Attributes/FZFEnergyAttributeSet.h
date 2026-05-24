// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FZFEnergyAttributeSet.generated.h"

 // GAS 매크로 정의
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECT_404_API UFZFEnergyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFZFEnergyAttributeSet();

public:
	ATTRIBUTE_ACCESSORS(UFZFEnergyAttributeSet, MaxHP)
	ATTRIBUTE_ACCESSORS(UFZFEnergyAttributeSet, HP)
	ATTRIBUTE_ACCESSORS(UFZFEnergyAttributeSet, Damage)
protected:

	// 멀티플레이 변수 복제를 위한 함수 선언
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
protected:
	// 클라이언트 측 동기화 알림 함수 (RepNotify)
	UFUNCTION()
	virtual void OnRep_HP(const FGameplayAttributeData& OldHP);

	UFUNCTION()
	virtual void OnRep_MaxHP(const FGameplayAttributeData& OldMaxHP);
	
private:
	// 현재 내구도 (타수 개념으로 사용 가능)
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_HP, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData HP;

	// 최대 내구도
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHP, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHP;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Damage;

};
