#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "FZFEnergyRelay.generated.h"

class UFZFAbilitySystemComponent;
class UFZFEnergyAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnergyRelayDestroyedEvent, AFZFEnergyRelay*, Relay);

UCLASS()
class PROJECT_404_API AFZFEnergyRelay : public AActor, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AFZFEnergyRelay();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void HandleBossWaitingStarted();

    UFUNCTION()
    void HandleBossWaitingEnded();

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    bool IsDead() const
    {
        return bDead;
    }

    // 동력원 파괴 호출 함수.
    UFUNCTION()
    void HandleDead();

    // 기둥이 땅속에서 위로 올라오는 함수
    UFUNCTION(BlueprintCallable)
    void Appear();

    // 기둥이 다시 땅속으로 내려가는 함수
    UFUNCTION(BlueprintCallable)
    void Disappear();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_bIsShown();

public:
    UPROPERTY(BlueprintAssignable, Category = "Relay|Event")
    FEnergyRelayDestroyedEvent OnRelayDestroyed;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UFZFAbilitySystemComponent> ASC;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
    TObjectPtr<UFZFEnergyAttributeSet> EnergyAttributeSet;

    bool bDead = false;

    UPROPERTY(ReplicatedUsing = OnRep_bIsShown)
    bool bIsShown = false;

protected:
    // 보스 정보 받아오기.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
    TObjectPtr<class AFZFBoss> TargetBoss;

    // 완전히 올라온 위치
    FVector ShownLocation;

    // 땅속에 숨겨진 위치
    FVector HiddenLocation;

    // 이동 시작 위치
    FVector StartLocation;

    // 이동 목표 위치
    FVector TargetLocation;

    // 맵에 배치된 위치 기준으로 등장 위치를 얼마나 보정할지
    UPROPERTY(EditAnywhere, Category = "EnergyRelay")
    FVector ShownLocationOffset = FVector::ZeroVector;

    // 맵에 배치된 위치 기준으로 숨겨질 위치를 얼마나 보정할지
    UPROPERTY(EditAnywhere, Category = "EnergyRelay")
    FVector HiddenLocationOffset = FVector(0.f, 0.f, -300.f);

    // 이동에 걸리는 시간
    UPROPERTY(EditAnywhere, Category = "EnergyRelay")
    float MoveDuration = 1.0f;

    // 이동 시작 후 지난 시간
    float ElapsedTime = 0.f;

    // 현재 이동 중인지 여부
    bool bMoving = false;
};