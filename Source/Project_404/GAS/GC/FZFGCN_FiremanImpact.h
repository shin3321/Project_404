// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "FZFGCN_FiremanImpact.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFGCN_FiremanImpact : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
    AFZFGCN_FiremanImpact();

    // GameplayCue가 활성화될 때 (플레이어가 불타기 시작할 때) 호출
    virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

    // GameplayCue가 해제될 때 (불타는 상태 효과가 끝났을 때) 호출
    virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

protected:
    // 블루프린트에서 불타는 나이아가라 에셋을 지정할 수 있도록 노출
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
    UParticleSystemComponent* FireFXComponent;
};
