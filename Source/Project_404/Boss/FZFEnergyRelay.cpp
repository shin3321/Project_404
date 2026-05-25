#include "Boss/FZFEnergyRelay.h"
#include "Character/Monster/Boss/FZFBoss.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFEnergyAttributeSet.h"
#include "Net/UnrealNetwork.h"

AFZFEnergyRelay::AFZFEnergyRelay()
{
    // Tick에서 Lerp 이동 처리를 해야 하므로 Tick 활성화
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

    ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>(TEXT("AbilitySystem"));
    ASC->SetIsReplicated(true);

    EnergyAttributeSet = CreateDefaultSubobject<UFZFEnergyAttributeSet>(TEXT("EnergyAttributeSet"));
}

void AFZFEnergyRelay::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFZFEnergyRelay, bIsShown);
}

void AFZFEnergyRelay::OnRep_bIsShown()
{
    if (bIsShown)
    {
        Appear();
    }
    else
    {
        Disappear();
    }
}

void AFZFEnergyRelay::BeginPlay()
{
    Super::BeginPlay();

    if (ASC)
    {
        ASC->InitAbilityActorInfo(this, this);
    }

    // 맵에 배치된 기본 위치 저장
    FVector BaseLocation = GetActorLocation();

    // 완전히 등장했을 때 위치
    ShownLocation = BaseLocation + ShownLocationOffset;

    // 땅속에 숨겨졌을 때 위치
    HiddenLocation = BaseLocation + HiddenLocationOffset;

    // 처음 시작 위치를 숨김 위치로 설정 (단, 이미 나타난 상태라면 예외)
    if (!bIsShown)
    {
        SetActorLocation(HiddenLocation);
    }
    else
    {
        SetActorLocation(ShownLocation);
    }


    if (TargetBoss)
    {
        TargetBoss->OnBossWaitingStarted.AddDynamic(
            this,
            &AFZFEnergyRelay::HandleBossWaitingStarted
        );

        TargetBoss->OnBossWaitingEnded.AddDynamic(
            this,
            &AFZFEnergyRelay::HandleBossWaitingEnded
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("[Relay] ASC: %s / AttrSet: %s / HasAuthority: %d"),
        *GetNameSafe(ASC),
        *GetNameSafe(EnergyAttributeSet),
        HasAuthority());
}

void AFZFEnergyRelay::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 이동 중이 아니면 아무것도 하지 않음
    if (!bMoving)
    {
        return;
    }

    // 이동 시작 후 지난 시간 누적
    ElapsedTime += DeltaTime;

    // 현재 진행률 계산
    // 0이면 시작 위치, 1이면 목표 위치
    float Alpha = ElapsedTime / MoveDuration;

    // Alpha 값이 0~1 범위를 넘지 않도록 제한
    Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

    // 시작 위치와 목표 위치 사이를 Alpha 비율만큼 보간
    FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);

    // 계산된 위치로 액터 이동
    SetActorLocation(NewLocation);

    // 목표 위치까지 이동이 끝났으면 이동 중지
    if (Alpha >= 1.f)
    {
        bMoving = false;
    }
}

UAbilitySystemComponent* AFZFEnergyRelay::GetAbilitySystemComponent() const
{
    UE_LOG(LogTemp, Warning, TEXT("[Relay] GetASC called"));
    return ASC;
}


void AFZFEnergyRelay::HandleBossWaitingStarted()
{
    if (!HasAuthority())
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Relay] WaitingStarted: %s / bDead=%d"),
        *GetName(),
        bDead);

    if (bDead)
    {
        return;
    }

    bIsShown = true;
    Appear();
}

void AFZFEnergyRelay::HandleBossWaitingEnded()
{
    if (!HasAuthority())
    {
        return;
    }

    bIsShown = false;
    Disappear();
}

void AFZFEnergyRelay::HandleDead()
{
    UE_LOG(LogTemp, Warning, TEXT("[Relay] HandleDead: %s / bDead=%d"),
        *GetName(),
        bDead);

    if (bDead)
    {
        return;
    }

    bDead = true;

    // 보스가 Open/Close하는 델리게이트 제거
    if (TargetBoss)
    {
        TargetBoss->OnBossWaitingStarted.RemoveDynamic(
            this,
            &AFZFEnergyRelay::HandleBossWaitingStarted
        );

        TargetBoss->OnBossWaitingEnded.RemoveDynamic(
            this,
            &AFZFEnergyRelay::HandleBossWaitingEnded
        );
    }

    // 보스 고리 연출 델리게이트 발행
    OnRelayDestroyed.Broadcast(this);

    // 동력원 Close
    Disappear(); // 혹은 Destroy()
}


void AFZFEnergyRelay::Appear()
{
    UE_LOG(LogTemp, Warning, TEXT("[Relay] Appear: %s / Current=%s / Shown=%s"),
        *GetName(),
        *GetActorLocation().ToString(),
        *ShownLocation.ToString()
    );

    // 현재 위치를 이동 시작 위치로 저장
    StartLocation = GetActorLocation();

    // 목표 위치를 완전히 올라온 위치로 설정
    TargetLocation = ShownLocation;

    // 이동 시간 초기화
    ElapsedTime = 0.f;

    // 이동 시작
    bMoving = true;
}

void AFZFEnergyRelay::Disappear()
{
    UE_LOG(LogTemp, Warning, TEXT("[Relay] Disappear: %s / Current=%s / Hidden=%s"),
        *GetName(),
        *GetActorLocation().ToString(),
        *HiddenLocation.ToString()
    );

    // 현재 위치를 이동 시작 위치로 저장
    StartLocation = GetActorLocation();

    // 목표 위치를 땅속 위치로 설정
    TargetLocation = HiddenLocation;

    // 이동 시간 초기화
    ElapsedTime = 0.f;

    // 이동 시작
    bMoving = true;
}