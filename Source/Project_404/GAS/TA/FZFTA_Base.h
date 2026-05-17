// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "FZFTA_Base.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API AFZFTA_Base : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	// 판정에 사용할 소켓 이름 (예: "Hand_R_Socket", "Muzzle_Socket")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	FName StartSocketName;

	// 소켓을 사용할지, 아니면 기존처럼 캐릭터 정면에서 시작할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	bool bUseSocket = false;

	bool bShowDebug = false;
};
