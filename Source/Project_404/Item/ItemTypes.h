#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM()
enum class EItemType : uint8
{
    Equipment      UMETA(DisplayName = "Equipment"),
    Consumable     UMETA(DisplayName = "Consumable"),
    CraftMaterial  UMETA(DisplayName = "Craft Material"),
    Crystal         UMETA(DisplayName = "Ore")
};

UENUM()
enum class EEquipmentType : uint8
{
    Melee      UMETA(DisplayName = "Melee"),
    Ranged     UMETA(DisplayName = "Ranged"),
    Throwable  UMETA(DisplayName = "Throwable")
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