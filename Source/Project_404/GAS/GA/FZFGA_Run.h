#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_Run.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_Run : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_Run();

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 이동속도 버프용 GE(Infinite Duration)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stamina)
	TSubclassOf<class UGameplayEffect> SprintBuffEffectClass;

	// 스프린트 버프 GE 핸들 (끝낼 때 제거용)
	FActiveGameplayEffectHandle SprintBuffEffectHandle;

	// 스프린트 끝난 뒤 2초동안 회복 막는 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stamina)
	TSubclassOf<class UGameplayEffect> NoRegenEffectClass;

	// 몇 초마다 스테미나를 한 번씩 깎을 지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stamina)
	float CostTickInterval = 0.2f;

protected:
	// 스테미나 0 이하 됐을 때
	UFUNCTION()
	void OnStaminaEmpty(const FGameplayAttribute& Attribute, float NewValue, float OldValue);

	// 스테미나 0되었을 때, 처리방식
	UFUNCTION()
	void OnStaminaThresholdChanged(bool bMatchesComparison, float CurrentValue);

	// 주기적 코스트 틱
	UFUNCTION()
	void OnCostTick();

	void StartCostTickLoop();
};
