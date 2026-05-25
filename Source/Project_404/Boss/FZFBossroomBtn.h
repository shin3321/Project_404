#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/FZFInteractableInterface.h"
#include "FZFBossroomBtn.generated.h"

class UBoxComponent;
class APawn;
class AFZFCharacterPlayer;

UCLASS()
class PROJECT_404_API AFZFBossroomBtn : public AActor, public IFZFInteractableInterface
{
	GENERATED_BODY()

public:
	AFZFBossroomBtn();

public:
	// 해당 플레이어가 현재 버튼 범위 안에 있는지 확인
	bool CanInteract(APawn* InPawn) const;

	// HUD에 표시할 상호작용 이름 반환
	virtual FText GetInteractableName(UPrimitiveComponent* HitComponent) const override;

	// 인터페이스 상호작용 함수
	virtual void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) override;

protected:
	virtual void BeginPlay() override;

protected:
	// 상호작용 범위 박스 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BossRoom")
	TObjectPtr<UBoxComponent> InteractionBox;

	// HUD에 표시할 상호작용 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BossRoom")
	FText InteractableName = FText::FromString(TEXT("보스방 입장"));

	// 현재 버튼 범위 안에 들어와 있는 플레이어 목록
	// 멀티플레이에서 한 명만 저장하면 마지막 플레이어로 덮이기 때문에 TSet으로 관리
	UPROPERTY()
	TSet<TObjectPtr<APawn>> OverlappingPlayers;

protected:
	// 플레이어가 버튼 범위에 들어왔을 때
	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 플레이어가 버튼 범위에서 나갔을 때
	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};