#pragma once

#include "CoreMinimal.h"
#include "FZFBossState.generated.h"

UENUM(BlueprintType)
enum class EBossState: uint8
{
    //Intro,    // 입장 연출/등장 대기
    Waiting,  // 대기, 패턴 끝나고 재충전
    Active,   // 전투 진행
    LuredToTrap, 
    PhaseTransition
};