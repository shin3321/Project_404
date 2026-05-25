// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// 레이저 활성화 함수
	void ActivateLaser(FVector StartLocation, ELaserMode Mode, ELaserType Type, float MoveSpeed);

	// 레이저 비활성화 함수
	void DeactivateLaser();

	// Getter
	ELaserMode GetLaserMode();

	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageGEClass() const { return DamageGEClass; }

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
private:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentMode)
	ELaserMode CurrentMode;

	UFUNCTION()
	void OnRep_CurrentMode();
	
	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

protected:
	// 레이저 데미지를 정의한 GameplayEffect 클래스 (블루프린트에서 설정)
	UPROPERTY(EditAnywhere, Category = "Laser | Combat")
	TSubclassOf<UGameplayEffect> DamageGEClass;

private:
	UPROPERTY(Replicated)
	ELaserType CurrentType;
	UPROPERTY(Replicated)
	float Speed;
	UPROPERTY(Replicated)
	FVector MoveDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MinY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MaxY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MinZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MaxZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MinX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser|Bounds", meta = (AllowPrivateAccess = "true"))
	float MaxX = 0.0f;
};
