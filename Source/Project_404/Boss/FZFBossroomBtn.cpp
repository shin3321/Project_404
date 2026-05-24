#include "Boss/FZFBossroomBtn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Character/Player/FZFCharacterPlayer.h"

AFZFBossroomBtn::AFZFBossroomBtn()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 생성
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 상호작용 범위 박스 콜리전 생성
	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	InteractionBox->SetupAttachment(RootComponent);

	// 오버랩 전용 충돌 설정
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionBox->SetGenerateOverlapEvents(true);

	// 오버랩 함수 연결
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AFZFBossroomBtn::OnBoxBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AFZFBossroomBtn::OnBoxEndOverlap);

	OverlappingPlayer = nullptr;
}

void AFZFBossroomBtn::BeginPlay()
{
	Super::BeginPlay();
}

// 현재 플레이어가 범위 안에 있는지 확인
bool AFZFBossroomBtn::CanInteract(APawn* InPawn) const
{
	return OverlappingPlayer && InPawn && OverlappingPlayer == InPawn;
}

// HUD에 표시할 상호작용 이름 반환
FText AFZFBossroomBtn::GetInteractableName(UPrimitiveComponent* HitComponent) const
{
	return InteractableName;
}

// 인터페이스 상호작용 함수
void AFZFBossroomBtn::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
	// 플레이어가 없으면 종료
	if (!Interactor)
	{
		return;
	}

	// 버튼 범위 안에 있는 플레이어만 홀드 시작 가능
	if (!CanInteract(Cast<APawn>(Interactor)))
	{
		return;
	}

	// 플레이어에게 보스방 홀드 시작 요청
	Interactor->BeginBossroomHold(this);
}

// 플레이어가 범위에 들어왔을 때
void AFZFBossroomBtn::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	APawn* OverlappedPawn = Cast<APawn>(OtherActor);
	if (OverlappedPawn)
	{
		OverlappingPlayer = OverlappedPawn;
	}
}

// 플레이어가 범위에서 나갔을 때
void AFZFBossroomBtn::OnBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	AFZFCharacterPlayer* Player = Cast<AFZFCharacterPlayer>(OtherActor);
	if (!Player)
	{
		return;
	}

	// 이 버튼 범위에서 나간 플레이어라면 홀드 취소
	if (OtherActor == OverlappingPlayer)
	{
		Player->StopBossroomHold();
		OverlappingPlayer = nullptr;
	}
}