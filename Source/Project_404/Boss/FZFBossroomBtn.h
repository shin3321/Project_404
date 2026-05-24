#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/FZFInteractableInterface.h"
#include "FZFBossroomBtn.generated.h"

class UBoxComponent;
class APawn;

UCLASS()
class PROJECT_404_API AFZFBossroomBtn : public AActor, public IFZFInteractableInterface
{
	GENERATED_BODY()

public:
	AFZFBossroomBtn();

public:
	// 현재 플레이어가 범위 안에 있는지 확인
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

	// 현재 범위 안에 들어온 플레이어
	UPROPERTY()
	TObjectPtr<APawn> OverlappingPlayer;

protected:
	// 플레이어가 범위에 들어왔을 때
	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 플레이어가 범위에서 나갔을 때
	UFUNCTION()
	void OnBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};