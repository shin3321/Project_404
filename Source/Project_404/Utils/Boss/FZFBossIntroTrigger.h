#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFBossIntroTrigger.generated.h"

class UBoxComponent;
class AFZFBossLevelManager;

UCLASS()
class PROJECT_404_API AFZFBossIntroTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFZFBossIntroTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TObjectPtr<AFZFBossLevelManager> BossLevelManager;

	bool bTriggered = false;
};
