#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_Attack.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_Attack();

	// 태그로 실행되며 호출되는 핵심 함수
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);
protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FirstPerson")
    TObjectPtr<UAnimMontage> FirstPersonAttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ThirdPerson")
    TObjectPtr<UAnimMontage> ThirdPersonAttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster")
    TObjectPtr<UAnimMontage> MonsterAttackMontage;

    // 애니메이션이 끝났을 때 호출될 함수
    UFUNCTION()
    void OnMontageCompleted();

    // 애니메이션이 재생도중에 끊겼을 때, 호출될 함수
    UFUNCTION()
    void OnMontageInterrupted();

private:

    void PlayPlayerAttack(class AFZFCharacterPlayer* CharacterPlayer,
        float AttackSpeed,
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo);

    void PlayMonsterAttack(
        class AFZFMonster* Monster,
        float AttackSpeed,
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo
    );
};
