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

protected:
	// FGameplayAbilityTargetDataHandle을 리턴 (타겟데이터의 묶음)
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const;


public:
	// 판정에 사용할 소켓 이름 (예: "Hand_R_Socket", "Muzzle_Socket")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	FName StartSocketName;

	// 소켓을 사용할지, 아니면 기존처럼 캐릭터 정면에서 시작할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	bool bUseSocket = false;
	
	bool bShowDebug = false;

};
