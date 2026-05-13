// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFLaserActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLaserDeactive, AFZFLaserActor*, Laser);

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class ELaserMode : uint8
{
	Inactive,
	Fixed,
	Moving
};

UENUM(BlueprintType)
enum class ELaserType : uint8
{
	Horizon,
	Vertical
};
UCLASS()
class PROJECT_404_API AFZFLaserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFZFLaserActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 레이저 활성화 함수
	void ActivateLaser(FVector StartLocation, ELaserMode Mode, ELaserType Type, float MoveSpeed);
	
	// 레이저 비활성화 함수
	void DeactivateLaser();
	
	// Getter
	ELaserMode GetLaserMode();

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category = "Laser")
	FOnLaserDeactive OnLaserDeactive;

protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	// 레이저 충돌 판정을 위한 박스 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionComp;

	// 레이저 빔 시각 효과 (나이아가라 이펙트 사용 권장)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* LaserEffectComp;



	// UFUNCTION()
	// void OnLaserOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	ELaserMode CurrentMode;
	ELaserType CurrentType;
	float Speed;
	FVector MoveDirection;
	
	float MinZ;
	float MaxZ;
	float MinY;
	float MaxY;
	float MinX;
	float MaxX;
};
