#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM()
enum class EItemType : uint8
{
    Equipment      UMETA(DisplayName = "Equipment"),
    Consumable     UMETA(DisplayName = "Consumable"),
    CraftMaterial  UMETA(DisplayName = "Craft Material"),
    Crystal         UMETA(DisplayName = "Crystal")
};

// 애니메이션 재생을 위한 세분화된 타입 분기.
UENUM()
enum class EEquipmentType : uint8
{
    None    UMETA(DisplayName = "None"),
    Pistol  UMETA(DisplayName = "Pistol"),
    Rifle   UMETA(DisplayName = "Rifle"),
    Sword   UMETA(DisplayName = "Sword"),
    Throwable UMETA(DisplayName = "Throwable")
};

UENUM()
enum class ECraftSlotType : uint8
{
    None      UMETA(DisplayName = "None"),
    BasePart  UMETA(DisplayName = "Base Part"),
    CorePart  UMETA(DisplayName = "Core Part")
};

UENUM()
enum class EConsumableEffectType : uint8
{
    HealHP,
    BuffAttack,
    BuffSpeed
};

UENUM()
enum class EThrowableEffectType : uint8
{
    Explosion,
    Flash,
    Trap
};