#include "Boss/FZFEnergyRelay.h"

AFZFEnergyRelay::AFZFEnergyRelay()
{
    // Tick에서 Lerp 이동 처리를 해야 하므로 Tick 활성화
    PrimaryActorTick.bCanEverTick = true;
}

void AFZFEnergyRelay::BeginPlay()
{
    Super::BeginPlay();

    // 맵에 배치된 기본 위치 저장
    FVector BaseLocation = GetActorLocation();

    // 완전히 등장했을 때 위치
    ShownLocation = BaseLocation + ShownLocationOffset;

    // 땅속에 숨겨졌을 때 위치
    HiddenLocation = BaseLocation + HiddenLocationOffset;

    // 처음 시작 위치를 숨김 위치로 설정
    SetActorLocation(HiddenLocation);

    // 테스트용: 게임 시작하면 바로 등장
    Appear();

    //3초뒤 사라지게 테스트
    FTimerHandle DisappearTimerHandle; GetWorldTimerManager().SetTimer(DisappearTimerHandle, this, &AFZFEnergyRelay::Disappear, 3.0f, false);
    //Disappear();
    
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

void AFZFEnergyRelay::Appear()
{
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
    // 현재 위치를 이동 시작 위치로 저장
    StartLocation = GetActorLocation();

    // 목표 위치를 땅속 위치로 설정
    TargetLocation = HiddenLocation;

    // 이동 시간 초기화
    ElapsedTime = 0.f;

    // 이동 시작
    bMoving = true;
}