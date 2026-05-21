#include "Character/Player/FZFPlayerController.h"

#include "Character/FZFCharacterBase.h"
#include "Character/Player/FZFCharacterPlayer.h"

#include "Manager/FZFSpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AFZFPlayerController::AFZFPlayerController()
{
}

void AFZFPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFPlayerController, bIsSpectating);
}

void AFZFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();	
	
	// 레거시 입력 바인딩 (프로젝트 설정의 ActionMappings에 IA_SpectateNext, IA_SpectatePrev가 있어야 함)
	// 만약 없다면 기본적으로 마우스 클릭으로 동작하게 할 수도 있습니다.
	InputComponent->BindAction("IA_SpectateNext", IE_Pressed, this, &AFZFPlayerController::SpectateNext);	
	InputComponent->BindAction("IA_SpectatePrev", IE_Pressed, this, &AFZFPlayerController::SpectatePrev);
	
	// 마우스 왼쪽/오른쪽 버튼으로도 관전 대상 전환 가능하도록 추가 바인딩
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AFZFPlayerController::SpectateNext);
	InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AFZFPlayerController::SpectatePrev);
}

void AFZFPlayerController::StartSpectator()
{
	bIsSpectating = true;
	
	// 서버에서 실행될 경우 클라이언트의 뷰도 변경됨 (SetViewTarget이 복제되므로)
	ChangeSpectateTarget(1);
}

void AFZFPlayerController::ChangeSpectateTarget(int32 Direction)
{
	// 관전 중이 아닐 때는 동작하지 않음
	if (!bIsSpectating) return;

	TArray<AActor*> AllPlayers;
	// AFZFCharacterBase 대신 AFZFCharacterPlayer를 사용하여 몬스터가 아닌 플레이어만 찾음
	UGameplayStatics::GetAllActorsOfClass(this, AFZFCharacterPlayer::StaticClass(), AllPlayers);
	
	if (AllPlayers.Num() == 0) return;

	TArray<AActor*> ValidPlayers;
	for (AActor* Actor : AllPlayers)
	{
		AFZFCharacterPlayer* PlayerChar = Cast<AFZFCharacterPlayer>(Actor);
		// 살아있는 플레이어만 유효한 관전 대상으로 간주
		if (PlayerChar && !PlayerChar->IsDead())
		{
			ValidPlayers.Add(Actor);
		}
	}
	
	if (ValidPlayers.Num() == 0)
	{
		// 살아있는 플레이어가 없다면 관전 대상을 자신(죽은 상태)으로 두거나 
		// 자유 시점으로 전환하는 등의 처리가 필요할 수 있습니다.
		return;
	}

	SpectateIndex += Direction;
	if (SpectateIndex >= ValidPlayers.Num())
	{
		SpectateIndex = 0;
	}
	else if (SpectateIndex < 0)
	{
		SpectateIndex = ValidPlayers.Num() - 1;
	}

	AActor* ValidPlayer = ValidPlayers[SpectateIndex];
	if (ValidPlayer)
	{
		// 뷰 대상을 해당 플레이어로 변경
		SetViewTargetWithBlend(ValidPlayer, 0.3f);
	}
}

void AFZFPlayerController::SpectateNext()
{
	if (bIsSpectating) ChangeSpectateTarget(1);
}

void AFZFPlayerController::SpectatePrev()
{
	if (bIsSpectating) ChangeSpectateTarget(-1);

}

void AFZFPlayerController::RequestSpawnItem(FName ItemId, FVector ItemSpawnLocation, FRotator SpawnRotation)
{
	ServerSpawnItem_Implementation(ItemId, ItemSpawnLocation, SpawnRotation);
}

bool AFZFPlayerController::ServerSpawnItem_Validate(FName ItemId, FVector ItemSpawnLocation, FRotator SpawnRotation)
{
	return !ItemId.IsNone();
}

void AFZFPlayerController::ServerSpawnItem_Implementation(FName ItemId, FVector ItemSpawnLocation, FRotator SpawnRotation)
{
	SpawnManager = Cast<AFZFSpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AFZFSpawnManager::StaticClass()));
	if (SpawnManager)
		SpawnManager->ServerSpawnItem(ItemId, ItemSpawnLocation, SpawnRotation);
}