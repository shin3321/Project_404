// FZFMovingPlatform.cpp

#include "FZFMovingPlatform.h"
#include "Components/StaticMeshComponent.h"

AFZFMovingPlatform::AFZFMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetMobility(EComponentMobility::Movable);
}

void AFZFMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void AFZFMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	float OffsetZ = FMath::Sin(RunningTime * MoveSpeed) * MoveHeight;

	FVector NewLocation = StartLocation;
	NewLocation.Z += OffsetZ;

	SetActorLocation(NewLocation);
}