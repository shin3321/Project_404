// TestCameraPawn.cpp

#include "TestCameraPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"

#include "FZFItemDataComponent.h"

void ATestCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector Start = Camera->GetComponentLocation();
    FVector End = Start + Camera->GetForwardVector() * 500.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);

    CurrentTargetItem = nullptr;

    if (bHit && Hit.GetActor())
    {
        AActor* HitActor = Hit.GetActor();
        UFZFItemDataComponent* ItemDataComp = HitActor->FindComponentByClass<UFZFItemDataComponent>();

        if (ItemDataComp)
        {
            CurrentTargetItem = HitActor;
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
    // 현재 바라보는 아이템이나 인벤토리 컴포넌트가 없으면 종료
    if (!CurrentTargetItem || !InventoryComponent)
    {
        return;
    }

    // 현재 타겟 액터에서 아이템 데이터 컴포넌트 찾기
    UFZFItemDataComponent* ItemDataComp = CurrentTargetItem->FindComponentByClass<UFZFItemDataComponent>();
    if (!ItemDataComp)
    {
        return;
    }

    //획득 아이템 데이터 확인용 로그
    if (ItemDataComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Pickup ID: %d"), ItemDataComp->ItemData.ID);
        UE_LOG(LogTemp, Warning, TEXT("Pickup Icon: %s"),
            ItemDataComp->ItemData.Icon ? *ItemDataComp->ItemData.Icon->GetName() : TEXT("None"));
    }

    // 아이템 데이터를 인벤토리에 추가
    const bool bAdded = InventoryComponent->AddItem(ItemDataComp->ItemData);

    // 추가 성공 시 월드 아이템 제거 후 현재 타겟 초기화
    if (bAdded)
    {
        CurrentTargetItem->Destroy();
        CurrentTargetItem = nullptr;
    }
}
