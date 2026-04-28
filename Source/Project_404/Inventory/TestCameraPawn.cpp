// TestCameraPawn.cpp

#include "TestCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"

#include "Item/FZFItemBase.h"

#include "FZFItemDataComponent.h"

void ATestCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 라인트레이스 시작 위치를 카메라 위치로 설정
    FVector Start = Camera->GetComponentLocation();

    // 카메라가 바라보는 방향으로 500.f 거리만큼 끝 위치 설정
    FVector End = Start + Camera->GetForwardVector() * 500.f;

    // 라인트레이스 충돌 결과를 저장할 변수
    FHitResult Hit;

    // 라인트레이스 옵션 설정
    FCollisionQueryParams Params;

    // 자기 자신은 라인트레이스 충돌 대상에서 제외
    Params.AddIgnoredActor(this);

    // Start부터 End까지 Visibility 채널로 라인트레이스 실행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    // 라인트레이스 디버그 선 표시
    // 맞은 액터가 있으면 초록색, 없으면 빨간색
    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);

    // 매 프레임마다 현재 바라보는 아이템 초기화
    CurrentTargetItem = nullptr;

    // 라인트레이스가 액터에 맞았는지 확인
    if (bHit && Hit.GetActor())
    {
        // 맞은 액터가 아이템 베이스 클래스인지 확인
        AFZFItemBase* ItemActor = Cast<AFZFItemBase>(Hit.GetActor());

        // 아이템 액터라면 현재 타겟 아이템으로 저장
        if (ItemActor)
        {
            CurrentTargetItem = ItemActor;
        }
    }
}

ATestCameraPawn::ATestCameraPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(Root);
    SpringArm->TargetArmLength = 0.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    InventoryComponent = CreateDefaultSubobject<UFZFInventoryComponent>(TEXT("InventoryComponent"));
}

void ATestCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("Move Forward / Backward"), this, &ATestCameraPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("Move Right / Left"), this, &ATestCameraPawn::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn Right / Left Mouse"), this, &ATestCameraPawn::Turn);
    PlayerInputComponent->BindAxis(TEXT("Look Up / Down Mouse"), this, &ATestCameraPawn::LookUp);

    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ATestCameraPawn::TraceItem);
}

void ATestCameraPawn::BeginPlay()
{
    Super::BeginPlay();

    //인벤토리 위젯 올리기
    if (InventoryComponent)
    {
        InventoryComponent->ShowInventory();
    }
}

void ATestCameraPawn::MoveForward(float Value)
{
    if (Controller && Value != 0.f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Forward, Value);
    }
}

void ATestCameraPawn::MoveRight(float Value)
{
    if (Controller && Value != 0.f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Right, Value);
    }
}

void ATestCameraPawn::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void ATestCameraPawn::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}


void ATestCameraPawn::TraceItem()
{
    // 현재 바라보고 있는 아이템이나 인벤토리 컴포넌트가 없으면 종료
    if (!CurrentTargetItem || !InventoryComponent)
    {
        return;
    }

    // 현재 아이템 액터에서 아이템 데이터 가져오기
    UFZFItemData* ItemData = CurrentTargetItem->GetItemData();

    // 아이템 데이터가 없으면 종료
    if (!ItemData)
    {
        return;
    }

    // 가져온 아이템 데이터를 인벤토리에 추가
    const bool bAdded = InventoryComponent->AddItem(ItemData);

    // 인벤토리에 정상적으로 추가됐을 때만
    if (bAdded)
    {
        // 월드에 놓여 있던 아이템 액터 제거
        CurrentTargetItem->Destroy();

        // 현재 타겟 아이템 참조 초기화
        CurrentTargetItem = nullptr;
    }
}