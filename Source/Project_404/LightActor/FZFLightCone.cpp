#include "LightActor/FZFLightCone.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Interface/FZFRevealableInterface.h"

AFZFLightCone::AFZFLightCone()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
    ConeMesh->SetupAttachment(Root);

    ConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    DetectBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectBox"));
    DetectBox->SetupAttachment(Root);

    DetectBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    DetectBox->SetGenerateOverlapEvents(true);
}

void AFZFLightCone::BeginPlay()
{
    Super::BeginPlay();

    DetectBox->OnComponentBeginOverlap.AddDynamic(
        this,
        &AFZFLightCone::OnBeginOverlap);

    DetectBox->OnComponentEndOverlap.AddDynamic(
        this,
        &AFZFLightCone::OnEndOverlap);
}

void AFZFLightCone::OnBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!HasAuthority())
    {
        return;
    }

    if (!OtherActor)
    {
        return;
    }

    if (OtherActor->GetClass()->ImplementsInterface(UFZFRevealableInterface::StaticClass()))
    {
        IFZFRevealableInterface::Execute_SetRevealedByLight(
            OtherActor,
            true);
    }
}

void AFZFLightCone::OnEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!HasAuthority())
    {
        return;
    }

    if (!OtherActor)
    {
        return;
    }

    if (OtherActor->GetClass()->ImplementsInterface(UFZFRevealableInterface::StaticClass()))
    {
        IFZFRevealableInterface::Execute_SetRevealedByLight(
            OtherActor,
            false);
    }
}