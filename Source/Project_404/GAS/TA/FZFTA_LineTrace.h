#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "FZFTA_LineTrace.generated.h"

UCLASS()
class PROJECT_404_API AFZFTA_LineTrace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	AFZFTA_LineTrace();

	// 상위 Ability애서 Confirm을 눌렀을 때, 실행되는 함수
	virtual void ConfirmTargetingAndContinue() override;

	// 타겟팅을 시작
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }
	
	// FGameplayAbilityTargetDataHandle을 리턴 (타겟데이터의 묶음)
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;

	bool bShowDebug = false;

};
