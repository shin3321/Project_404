#pragma once

#include "CoreMinimal.h"
#include "FZFBossState.generated.h"

UENUM(BlueprintType)
enum class EBossState: uint8
{
    Waiting,
    Active,
    LuredToTrap,
    PhaseTransition
};

UENUM(BlueprintType)
enum class EBossPattern : uint8
{
    None,
    SelfAttack,
    MapPattern,
};