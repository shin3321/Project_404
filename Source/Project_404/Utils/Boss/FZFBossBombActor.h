// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "FZFBossBombActor.generated.h"

UCLASS()
class PROJECT_404_API AFZFBossBombActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFBossBombActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void Explode();

	// 서버가 모든 클라이언트에게 이펙트를 재생하라고 지시하는 RPC
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastExplode();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* BombBodyMesh;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraSystem* ExplosionFX;
	
	FTimerHandle ExplosionTimerHandle;
	
private:
	class AFZFTestBoss* BossActor = nullptr;
};
