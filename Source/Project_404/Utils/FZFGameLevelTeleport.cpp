#include "Utils/FZFGameLevelTeleport.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/FZFCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFZFGameLevelTeleport::AFZFGameLevelTeleport()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TransferVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TransferVolume"));

	RootComponent = TransferVolume;

	TransferVolume->InitBoxExtent(FVector(100.f, 200.f, 500.f));
	// 충돌 설정
	TransferVolume->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AFZFGameLevelTeleport::BeginPlay()
{
	Super::BeginPlay();

	TransferVolume->OnComponentBeginOverlap.AddDynamic(this, &AFZFGameLevelTeleport::OnOverlapBegin);
	TransferVolume->OnComponentEndOverlap.AddDynamic(this, &AFZFGameLevelTeleport::OnOverlapEnd);

	if (TeleportWidgetInstance == nullptr)
	{
		TeleportWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), TeleportWidgetClass);
	}
}

// Called every frame
void AFZFGameLevelTeleport::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFZFGameLevelTeleport::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (Cast<AFZFCharacterBase>(OtherActor))
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			EnableInput(PlayerController);
			if (TeleportWidgetInstance != nullptr && !TeleportWidgetInstance->IsInViewport())
			{
				TeleportWidgetInstance->AddToViewport();
			}
			if (InputComponent)
			{
				InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AFZFGameLevelTeleport::OnTeleportKeyPressed);
			}
		}
	}
}

void AFZFGameLevelTeleport::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (TeleportWidgetInstance != nullptr && TeleportWidgetInstance->IsInViewport())
	{
		TeleportWidgetInstance->RemoveFromParent();
	}
	if (AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(OtherActor))
	{
		DisableInput(Cast<APlayerController>(Character->GetController()));
	}
}

void AFZFGameLevelTeleport::OnTeleportKeyPressed()
{
	EnterLobbyLevel();
}

void AFZFGameLevelTeleport::EnterLobbyLevel()
{
	FString LevelPath = TEXT("/Game/Project404/Map/FZFGameLevel");
	GetWorld()->ServerTravel(LevelPath);
}
