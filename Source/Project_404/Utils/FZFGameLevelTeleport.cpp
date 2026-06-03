#include "Utils/FZFGameLevelTeleport.h"
#include "Character/FZFCharacterBase.h"
#include "Manager/FZFSoundManager.h"
#include "Game/FZFGameInstance.h"

// Sets default values
AFZFGameLevelTeleport::AFZFGameLevelTeleport()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

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
	GameInstance = Cast<UFZFGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Instance: %s"), *GameInstance->GetName());
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

	if (AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			ClientSetTeleportWidgetVisible(PC, true);
		}
	}
}

void AFZFGameLevelTeleport::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;

	if (AFZFCharacterBase* Character = Cast<AFZFCharacterBase>(OtherActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			ClientSetTeleportWidgetVisible(PC, false);
		}
	}
}

void AFZFGameLevelTeleport::ClientSetTeleportWidgetVisible_Implementation(APlayerController* PC, bool bVisible)
{
	if (!PC) return;

	if (bVisible)
	{
		EnableInput(PC);
		if (TeleportWidgetInstance != nullptr && !TeleportWidgetInstance->IsInViewport())
		{
			TeleportWidgetInstance->AddToViewport();
		}
		if (InputComponent)
		{
			InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AFZFGameLevelTeleport::OnTeleportKeyPressed);
		}
	}
	else
	{
		if (TeleportWidgetInstance != nullptr && TeleportWidgetInstance->IsInViewport())
		{
			TeleportWidgetInstance->RemoveFromParent();
		}
		DisableInput(PC);
	}
}

void AFZFGameLevelTeleport::OnTeleportKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("=== OnTeleportKeyPressed Called ==="));
	if (UGameInstance* GI = GetGameInstance())
	{
		UFZFSoundManager* SoundManager = GI->GetSubsystem<UFZFSoundManager>();
		if (SoundManager)
		{
			SoundManager->PlaySFX(FName("ButtonSFX"));
		}
	}
	EnterLobbyLevel();
}

void AFZFGameLevelTeleport::EnterLobbyLevel()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("클라이언트는 포탈을 탈 수 없습니다! 서버(첫 번째 창)에서 누르세요."));
		return;
	}

	const FString TargetLevel = TEXT("/Game/Project404/Map/FZFGameLevel?listen");
	ShowLoadingAndTravel(TargetLevel);
}

void AFZFGameLevelTeleport::TravelToLobbyLevel()
{
	if (GetWorld() && HasAuthority())
	{
		GetWorld()->ServerTravel(PendingLevelPath);
	}
}
void AFZFGameLevelTeleport::ShowLoadingAndTravel(const FString& LevelPath)
{
	if (!HasAuthority()) 
		return;

	MulticastShowLoadingWidget();

	PendingLevelPath = LevelPath;

	UE_LOG(LogTemp, Warning, TEXT("Saved PendingLevelPath: %s"), *PendingLevelPath);

	GetWorldTimerManager().SetTimer(
		LevelTravelTimerHandle,
		this,
		&AFZFGameLevelTeleport::TravelToLobbyLevel,
		3.0f,
		false
	);
}

void AFZFGameLevelTeleport::MulticastShowLoadingWidget_Implementation()
{
	// 로딩 화면 위젯 생성
	if (LoadingWidgetClass)
	{
		LoadingWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LoadingWidgetClass);

		if (LoadingWidgetInstance)
		{
			// 로딩 화면을 가장 위에 표시
			LoadingWidgetInstance->AddToViewport(999);
		}
	}
}
