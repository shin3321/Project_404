// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"
#include "FZFStore.generated.h"

UCLASS()
class PROJECT_404_API AFZFStore : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFZFStore();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void ProcessPurchase(APlayerController* Buyer, FName ItemId, float ItemCost);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StoreMesh;
};
