#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FZFGA_AttackHitCheck.generated.h"

UCLASS()
class PROJECT_404_API UFZFGA_AttackHitCheck : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFZFGA_AttackHitCheck();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// TA로부터 데이터를 전달받았을 때 호출될 콜백 함수
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

protected:
	// TA 클래스를 에디터에서 할당하기 위한 변수
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class AFZFTA_Base> TargetActorClass;
};
