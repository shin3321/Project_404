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
	UE_LOG(LogTemp, Warning, TEXT("=== OnTeleportKeyPressed Called ==="));
	EnterLobbyLevel();
}

void AFZFGameLevelTeleport::EnterLobbyLevel()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("클라이언트는 포탈을 탈 수 없습니다! 서버(첫 번째 창)에서 누르세요."));
		return;
	}

	// 로딩 화면을 띄운 뒤 레벨 이동
	ShowLoadingAndTravel(TEXT("FZFGameLevel?listen"));
}

void AFZFGameLevelTeleport::TravelToLobbyLevel()
{
	if (GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("=== Traveling to: %s ==="), *PendingLevelPath);

		// 저장된 레벨 경로로 이동
		GetWorld()->ServerTravel(PendingLevelPath);
	}
}

void AFZFGameLevelTeleport::ShowLoadingAndTravel(const FString& LevelPath)
{
	// 이동할 레벨 경로 저장
	PendingLevelPath = LevelPath;

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

	// 로딩 화면을 잠깐 보여준 뒤 실제 레벨 이동
	GetWorldTimerManager().SetTimer(
		LevelTravelTimerHandle,
		this,
		&AFZFGameLevelTeleport::TravelToLobbyLevel,
		3.0f,
		false
	);
}

