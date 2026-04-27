// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FZFItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFZFItemBase::AFZFItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

    RootComponent = Trigger;
    Mesh->SetupAttachment(Trigger);
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
    if (ItemData && ItemData->Mesh)
    {
        Mesh->SetStaticMesh(ItemData->Mesh);
        //
        //UE_LOG(LogTemp, Log, TEXT("%s"), *ItemData->MeshScale.ToString());

        if (ItemData->bAutoFitMeshToTrigger)
        {
            ApplyAutoFitMeshScale();
        }
        else
        {
            Mesh->SetRelativeScale3D(ItemData->MeshScale);
        }
    }
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

