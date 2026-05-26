// FZFMovingPlatform.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFMovingPlatform.generated.h"

UCLASS()
class PROJECT_404_API AFZFMovingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AFZFMovingPlatform();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly ,Category = "Moving Platform")
	float MoveHeight = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Moving Platform")
	float MoveSpeed = 1.f;

private:
	FVector StartLocation;
	float RunningTime = 0.f;
};