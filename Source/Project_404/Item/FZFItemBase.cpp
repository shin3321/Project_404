// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FZFItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Net/UnrealNetwork.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Item/ItemTypes.h"
#include "Item/CraftMaterial/FZFCraftPartItemData.h"
#include "Item/CraftMaterial/FZFRobotPartItemData.h"

// Sets default values
AFZFItemBase::AFZFItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // 네트워크 복제 설정
    bReplicates = true;
    SetReplicates(true);

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);

    WeaponParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponParticle"));
    WeaponParticle->SetupAttachment(Trigger);
    WeaponParticle->SetAutoActivate(false);

    MaterialParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MaterialParticle"));
    MaterialParticle->SetupAttachment(Trigger);
    MaterialParticle->SetAutoActivate(false);

    RobotPartParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RobotPartParticle"));
    RobotPartParticle->SetupAttachment(Trigger);
    RobotPartParticle->SetAutoActivate(false);

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> WeaponVFXObj(
        TEXT("/Game/Project404/Item/VFX/Niagara/NS_EuipmentItem.NS_EuipmentItem")
    );

    if (WeaponVFXObj.Succeeded())
    {
        WeaponParticleSystem = WeaponVFXObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> VFXMaterialObj(
        TEXT("/Game/Project404/Item/VFX/Niagara/NS_EquipmentMaterialItem.NS_EquipmentMaterialItem")
    );

    if (VFXMaterialObj.Succeeded())
    {
        MaterialParticleSystem = VFXMaterialObj.Object;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> RobotPartVFXObj(
        TEXT("/Game/Project404/Item/VFX/Niagara/NS_RobotPartItem.NS_RobotPartItem")
    );

    if (RobotPartVFXObj.Succeeded())
    {
        RobotPartParticleSystem = RobotPartVFXObj.Object;
    }
}

void AFZFItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFZFItemBase, ItemData);
}

void AFZFItemBase::OnRep_ItemData()
{
    ApplyItemData();
}

void AFZFItemBase::BeginPlay()
{
    Super::BeginPlay();

    ApplyItemData();
}

void AFZFItemBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    ApplyItemData();
}

void AFZFItemBase::InitializeItem(UFZFItemData* InItemData)
{
    ItemData = InItemData;

    ApplyItemData();
}

void AFZFItemBase::ApplyItemData()
{
    if (!ItemData || !ItemData->Mesh)
    {
        DeactivateAllItemParticles();
        return;
    }

    Mesh->SetStaticMesh(ItemData->Mesh);
       
    if (ItemData->bAutoFitMeshToTrigger)
    {
        ApplyAutoFitMeshScale();
    }
    else
    {
        Mesh->SetRelativeScale3D(ItemData->MeshScale);
    }

    UpdateItemParticle();
}

void AFZFItemBase::ApplyAutoFitMeshScale()
{
    const FBoxSphereBounds MeshBounds = ItemData->Mesh->GetBounds();

    const FVector MeshSize = MeshBounds.BoxExtent * 2.0f;
    const FVector TriggerSize = Trigger->GetUnscaledBoxExtent() * 2.0f;

    if (MeshSize.X <= KINDA_SMALL_NUMBER ||
        MeshSize.Y <= KINDA_SMALL_NUMBER ||
        MeshSize.Z <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    const float ScaleX = TriggerSize.X / MeshSize.X;
    const float ScaleY = TriggerSize.Y / MeshSize.Y;
    const float ScaleZ = TriggerSize.Z / MeshSize.Z;

    const float FinalScale = FMath::Min3(ScaleX, ScaleY, ScaleZ);

    Mesh->SetRelativeScale3D(FVector(FinalScale));
}

void AFZFItemBase::UpdateItemParticle()
{
    DeactivateAllItemParticles();

    if (!ItemData)
    {
        return;
    }

    if (ItemData->ItemType == EItemType::Equipment)
    {
        if (WeaponParticle && WeaponParticleSystem)
        {
            WeaponParticle->SetAsset(WeaponParticleSystem);
            WeaponParticle->Activate(true);
        }

        return;
    }

    if (ItemData->ItemType == EItemType::CraftMaterial && Cast<UFZFRobotPartItemData>(ItemData))
    {
        if (RobotPartParticle && RobotPartParticleSystem)
        {
            RobotPartParticle->SetAsset(RobotPartParticleSystem);
            RobotPartParticle->Activate(true);
        }

        return;
    }

    if (ItemData->ItemType == EItemType::CraftMaterial && Cast<UFZFCraftPartItemData>(ItemData))
    {
        if (MaterialParticle && MaterialParticleSystem)
        {
            MaterialParticle->SetAsset(MaterialParticleSystem);
            MaterialParticle->Activate(true);
        }

        return;
    }
}

void AFZFItemBase::DeactivateAllItemParticles()
{
    if (WeaponParticle)
    {
        WeaponParticle->Deactivate();
    }

    if (MaterialParticle)
    {
        MaterialParticle->Deactivate();
    }

    if (RobotPartParticle)
    {
        RobotPartParticle->Deactivate();
    }
}


void AFZFItemBase::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
    if (!HasAuthority())
    {
        return;
    }

    if (!Interactor)
    {
        return;
    }

    UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();
    if (!Inventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact: Inventory is null"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Interact: ItemData=%s"),
        ItemData ? *ItemData->ItemId.ToString() : TEXT("NULL"));

    if (Inventory->AddItem(ItemData))
    {
        Destroy();
    }
}

FText AFZFItemBase::GetInteractableName(UPrimitiveComponent* HitComponent) const
{
    if (!ItemData)
    {
        return FText::GetEmpty();
    }

    return ItemData->ItemName;
}

// 바닥에 배치될 때 커스텀 회전값.
void AFZFItemBase::ApplyGroundRotation()
{
    if (!ItemData)
    {
        return;
    }

    FRotator GroundRot = ItemData->GroundRotation;

    if (ItemData->bRandomGroundYaw)
    {
        GroundRot.Yaw += FMath::RandRange(0.0f, 360.0f);
    }

    //SetActorRotation(GroundRot);
    Mesh->SetRelativeRotation(GroundRot);
}

// 바닥에 딱 맞게 배치하는 방식.
void AFZFItemBase::PlaceOnGround()
{
    UWorld* World = GetWorld();
    if (!World || !Mesh)
    {
        return;
    }

    FVector ActorLocation = GetActorLocation();

    FVector TraceStart = ActorLocation + FVector(0.0f, 0.0f, 10.0f);
    FVector TraceEnd = ActorLocation - FVector(0.0f, 0.0f, 1000.0f);

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        Params
    );

    if (!bHit)
    {
        return;
    }

    const FBoxSphereBounds Bounds = Mesh->Bounds;

    const float MeshBottomZ = Bounds.Origin.Z - Bounds.BoxExtent.Z;
    const float ActorZ = GetActorLocation().Z;

    const float BottomOffsetFromActor = ActorZ - MeshBottomZ;

    FVector NewLocation = GetActorLocation();
    NewLocation.X = HitResult.Location.X;
    NewLocation.Y = HitResult.Location.Y;
    NewLocation.Z = HitResult.Location.Z + BottomOffsetFromActor;

    SetActorLocation(NewLocation);
}