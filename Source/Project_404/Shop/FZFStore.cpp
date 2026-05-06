// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/FZFStore.h"
#include "Manager/FZFRoomManager.h"
#include "Game/FZFGameState.h"

// Sets default values
AFZFStore::AFZFStore()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Store 메쉬 가져오기
	StoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StoreMesh"));
	RootComponent = StoreMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StoreStaticMeshRef(TEXT(""));
	if (StoreStaticMeshRef.Succeeded())
	{
		StoreMesh->SetStaticMesh(StoreStaticMeshRef.Object);
	}

	// Store 트리거 박스 설정
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFZFStore::OnTriggerBoxOverlap);
	// Store 크기 설정
	//TriggerBox->SetBoxExtent(FVector(32.f, 32.f, 32.f));
	//TriggerBox->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AFZFStore::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFZFStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFZFStore::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Todo: 상점 위젯 띄우기
}

void AFZFStore::ProcessPurchase(APlayerController* Buyer, FName ItemId, float ItemCost)
{
	// !HasAuthority() => 클라이언트의 권한이면 실패
	if (!HasAuthority()) return;
	AFZFGameState* GameState = Cast<AFZFGameState>(GetWorld()->GetGameState());
	UFZFRoomManager* RoomMgr = GameState->GetRoomManager();

	if (RoomMgr && RoomMgr->GetMoney() >= ItemCost)
	{
		RoomMgr->SpendMoney(ItemCost);

		// Todo 아이템 지급 로직 실행
	}
}