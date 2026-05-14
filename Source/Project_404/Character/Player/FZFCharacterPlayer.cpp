#include "Character/Player/FZFCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFPlayerSet.h"
#include "FZFPlayerState.h"

#include "Inventory/FZFInventoryComponent.h"
#include "GameplayTag/FZFGameplayTags.h"
#include "Interface/FZFInteractableInterface.h"
#include "Animation/FZFPlayerAnimInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Game/FZFGameMode.h"
#include "Inventory/FZFHUD.h"
#include "Inventory/FZFHeldItemComponent.h"
#include "Components/PrimitiveComponent.h"


AFZFCharacterPlayer::AFZFCharacterPlayer()
{
	// 기본 설정
	// 컨트롤러의 회전 값을 받아서 설정하는 옵션 비활성화
	bUseControllerRotationPitch = false;	// Y축 회전, 위아래는 카메라만 까딱이게 함 (캡슐 통째로 눕지 않게)
	bUseControllerRotationYaw = true;		// Z축 회전, 마우스 좌우 회전 시 캐릭터 몸통(캡슐)도 같이 회전 
	bUseControllerRotationRoll = false;		// X축 회전

	PrimaryActorTick.bCanEverTick = true;
	// 무브먼트 설정
	// 캐릭터가 이동하는 방향에 맞게 회전을 해주는 옵션
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;


	// 카메라 컴포넌트 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// 캡슐 컴포넌트(Root)에 붙이고 Z값을 눈높이로 올리기
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // 눈높이에 맞춰 Z축 위치 조절 (캐릭터 크기에 맞게 수정 필요)
	// 마우스 입력(컨트롤러 회전)에 따라 카메라가 상하좌우로 움직이도록 설정
	Camera->bUsePawnControlRotation = true;

	// 캡슐 컴포넌트에 맞춰 스켈레탈 메시의 위치(바닥)와 방향(정면) 정렬
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -86.8f), FRotator(0.0f, -90.0f, 0.0f));

	// 메시 에셋 지정

	// 전신 매쉬
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Game/Project404/Character/Player/SkeletalMesh/SK_SciFITrooper-01.SK_SciFITrooper-01"));
	// 팔 매쉬
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmMeshRef(TEXT("/Game/Project404/Character/Player/SkeletalMesh/2_Hand.2_Hand"));

	if (GetMesh() && CharacterMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
		GetMesh()->SetOwnerNoSee(true); // 나에게는 내 몸통이 안 보임 (그림자는 보임)
	}

	// 팔 메시 설정
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterArmMesh"));
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->SetOnlyOwnerSee(true); // 나에게만 보임
	ArmMesh->bCastDynamicShadow = false; // 그림자 제거
	ArmMesh->CastShadow = false;

	// 팔 매시 위치 및 회전
	ArmMesh->SetRelativeLocation(FVector(10.0f, 0.0f, -108.0f));
	ArmMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	// 팔 전용 메쉬 할당
	if (ArmMeshRef.Succeeded())
	{
		ArmMesh->SetSkeletalMesh(ArmMeshRef.Object);
	}

	// 점프 관련 설정
	GetCharacterMovement()->JumpZVelocity = 800.0f; // 점프 힘
	GetCharacterMovement()->GravityScale = 1.6f; // 중력 배율

	// ArmMesh ABP 설정
	static ConstructorHelpers::FClassFinder<UAnimInstance> ArmABPRef(TEXT("/Game/Project404/Character/Player/Animation/ABP_Player.ABP_Player_C"));

	if (ArmABPRef.Succeeded())
	{
		ArmMesh->SetAnimInstanceClass(ArmABPRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextRef(TEXT("/Game/Project404/Input/IMC_Default.IMC_Default"));
	if (DefaultMappingContextRef.Succeeded())
	{
		// 기본 IMC 할당
		DefaultMappingContext = DefaultMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Game/Project404/Input/Actions/IA_Move.IA_Move"));
	if (MoveActionRef.Succeeded())
	{
		// 1인칭용 MoveAction 변수에 할당
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Game/Project404/Input/Actions/IA_Look.IA_Look"));
	if (LookActionRef.Succeeded())
	{
		// 1인칭용 LookAction 변수에 할당
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractActionRef(TEXT("/Game/Project404/Input/Actions/IA_Interact.IA_Interact"));
	if (InteractActionRef.Succeeded())
	{
		// 상호작용 변수에 할당
		InteractAction = InteractActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DropItemActionRef(TEXT("/Game/Project404/Input/Actions/IA_DropItem.IA_DropItem"));
	if (DropItemActionRef.Succeeded())
	{
		// 상호작용 변수에 할당
		DropItemAction = InteractActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/Project404/Input/Actions/IA_Jump.IA_Jump"));

	if (JumpActionRef.Succeeded())
	{
		// 점프 액션
		JumpAction = JumpActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Game/Project404/Input/Actions/IA_Run.IA_Run"));
	if (RunActionRef.Succeeded())
	{
		// 달리기 액션
		RunAction = RunActionRef.Object;
	}

	// GAS
	// 의도적으로 nullptr로 밀어줌 -> PlayerState의 ASC값을 대입할거라서 혼선방지용
	ASC = nullptr;

	// 네트워크 설정
	bReplicates = true;

	// Inventory 추가
	InventoryComponent = CreateDefaultSubobject<UFZFInventoryComponent>(TEXT("InventoryComponent"));


	// 슬롯 1~5 입력 액션 에셋 로드
	static ConstructorHelpers::FObjectFinder<UInputAction> Slot1ActionRef(TEXT("/Game/Project404/Input/Actions/IA_Slot1.IA_Slot1"));
	if (Slot1ActionRef.Succeeded())
	{
		Slot1Action = Slot1ActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Slot2ActionRef(TEXT("/Game/Project404/Input/Actions/IA_Slot2.IA_Slot2"));
	if (Slot2ActionRef.Succeeded())
	{
		Slot2Action = Slot2ActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Slot3ActionRef(TEXT("/Game/Project404/Input/Actions/IA_Slot3.IA_Slot3"));
	if (Slot3ActionRef.Succeeded())
	{
		Slot3Action = Slot3ActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Slot4ActionRef(TEXT("/Game/Project404/Input/Actions/IA_Slot4.IA_Slot4"));
	if (Slot4ActionRef.Succeeded())
	{
		Slot4Action = Slot4ActionRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Slot5ActionRef(TEXT("/Game/Project404/Input/Actions/IA_Slot5.IA_Slot5"));
	if (Slot5ActionRef.Succeeded())
	{
		Slot5Action = Slot5ActionRef.Object;
	}

	// 손에 들 아이템을 관리하는 컴포넌트 생성
	HeldItemComponent = CreateDefaultSubobject<UFZFHeldItemComponent>(TEXT("HeldItemComponent"));
}


void AFZFCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 인벤토리 위젯 올리기
	if (InventoryComponent)
	{
		InventoryComponent->ShowInventory();
	}

	// IMC 플레이어에 적용시키기(IMC_Default)
	ApplyMappingContext(DefaultMappingContext);

	// 0.1초마다 아이템 감지 함수를 실행
	FTimerHandle DetectionTimerHandle;
	GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &AFZFCharacterPlayer::DetectInteractable, 0.1f, true);

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UFZFHUD>(GetWorld(), HUDWidgetClass);
		HUDWidget->AddToViewport();
		HUDWidget->HideWidget();
		HUDWidget->SetCrosshairNormal();
	}
}

void AFZFCharacterPlayer::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void AFZFCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	/**
	 * [서버 측 GAS 초기화]
	 * PossessedBy는 서버에서 컨트롤러가 캐릭터를 점유했을 때 호출됩니다.
	 * 이 시점에는 PlayerState가 유효함이 보장되므로, 서버 측 ASC에
	 * Owner(PlayerState)와 Avatar(Character) 정보를 등록합니다.
	 */
	InitAbilitySystem();
}

void AFZFCharacterPlayer::InitAbilitySystem()
{
	Super::InitAbilitySystem();

	int32 InputID = 0;
	if (AFZFPlayerState* PS = GetPlayerState<AFZFPlayerState>())
	{
		/** * [GAS 핵심 설정]
		 * 데이터 보존을 위해 PlayerState에 생성된 AbilitySystemComponent(ASC)를
		 * 현재 Character(Avatar)의 ASC 변수에 할당하여 참조합니다.
		 * 이를 통해 캐릭터가 죽고 리스폰되어도 동일한 PlayerState의 ASC를 유지
		 */
		ASC = Cast<UFZFAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// ASC의 원본은 PlayerState이므로 InOwner를 PlayerState로 설정
		// 시각적으로 표현하는 AvatarActor는 Player
		ASC->InitAbilityActorInfo(PS, this);
		AttributeSet = PS->GetPlayerSet();
		for (const auto& StartupAbility : StartupAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartupAbility);
			ASC->GiveAbility(StartSpec);
		}

		if (PassiveRegenEffectClass)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(PassiveRegenEffectClass, 1.0f, EffectContext);
			if (SpecHandle.IsValid())
			{
				// 서버에서 적용 시 클라이언트로 자동 복제됨
				ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
			}
		}
	}
}

void AFZFCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 바인딩 - 향상된 입력 시스템 컴포넌트를 활용해서 설정.
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		// 입력 바인딩 -> 이벤트와 함수를 연결

		// Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFZFCharacterPlayer::Move);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFZFCharacterPlayer::Look);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFZFCharacterPlayer::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFZFCharacterPlayer::JumpEnd);

		// Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AFZFCharacterPlayer::RunStart);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AFZFCharacterPlayer::RunEnd);

		// Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFZFCharacterPlayer::Interact);

		// DropItem
		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Started, this, &AFZFCharacterPlayer::DropSelectedItem);

		// 숫자키 입력과 슬롯 선택 함수 연결
		EnhancedInputComponent->BindAction(Slot1Action, ETriggerEvent::Started, this, &AFZFCharacterPlayer::SelectSlot1);
		EnhancedInputComponent->BindAction(Slot2Action, ETriggerEvent::Started, this, &AFZFCharacterPlayer::SelectSlot2);
		EnhancedInputComponent->BindAction(Slot3Action, ETriggerEvent::Started, this, &AFZFCharacterPlayer::SelectSlot3);
		EnhancedInputComponent->BindAction(Slot4Action, ETriggerEvent::Started, this, &AFZFCharacterPlayer::SelectSlot4);
		EnhancedInputComponent->BindAction(Slot5Action, ETriggerEvent::Started, this, &AFZFCharacterPlayer::SelectSlot5);

	}
}

void AFZFCharacterPlayer::ApplyMappingContext(UInputMappingContext* InMappingContext)
{
	// 인자 유효성 검사
	if (!InMappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyMappingContext : MappingContext is null!"));
		return;
	}

	// 컨트롤러 및 서브시스템 가져오기
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputSystem)
			{
				// 기존 매핑 제거
				InputSystem->ClearAllMappings();

				// 새로운 입력 매핑 컨텍스트를 우선순위 0으로 추가 적용
				InputSystem->AddMappingContext(InMappingContext, 0);

				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = false;

				UE_LOG(LogTemp, Log, TEXT("ApplyMappingContext : %s 적용 완료"), *InMappingContext->GetName());
			}
		}
	}

}

void AFZFCharacterPlayer::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (GetCharacterMovement())
	{
		UE_LOG(LogTemp, Warning, TEXT("Movement Mode: %d"),
			(int32)GetCharacterMovement()->MovementMode);
		UE_LOG(LogTemp, Warning, TEXT("Max Walk Speed: %f"),
			GetCharacterMovement()->MaxWalkSpeed);
	}

	UE_LOG(LogTemp, Log, TEXT("PawnClientRestart"));

	ApplyMappingContext(DefaultMappingContext);
}

void AFZFCharacterPlayer::ApplyAnimationsByItemAnimType(UAnimSequence* ThirdPersonIdle, UAnimSequence* FirstPersonIdle)
{
	if (GetMesh())
	{
		UFZFPlayerAnimInstance* TPAnim = Cast<UFZFPlayerAnimInstance>(GetMesh()->GetAnimInstance());

		if (TPAnim)
		{
			TPAnim->SetCurrentIdleAnim(ThirdPersonIdle);
		}
	}

	if (ArmMesh)
	{
		UFZFPlayerAnimInstance* FPAnim = Cast<UFZFPlayerAnimInstance>(ArmMesh->GetAnimInstance());

		if (FPAnim)
		{
			FPAnim->SetCurrentIdleAnim(FirstPersonIdle);
		}
	}
}

void AFZFCharacterPlayer::Move(const FInputActionValue& Value)
{
	// 입력 값으로부터 Vector2D 데이터 추출
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 컨트롤러의 현재 회전값 가져오기
	const FRotator Rotation = Controller->GetControlRotation();
	// 캐릭터 이동에 필요한 Yaw(좌우 회전) 값만 추출
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// 회전 행렬을 통해 현재 바라보는 방향의 앞(X)과 오른쪽(Y) 벡터를 계산
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 계산된 방향과 입력된 크기(MovementVector)를 조합하여 캐릭터를 실제로 이동
	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AFZFCharacterPlayer::Look(const FInputActionValue& Value)
{
	// 입력 값으로부터 Vector2D 데이터 추출
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 좌우 회전 (Yaw): 마우스 X축 이동량
		AddControllerYawInput(LookAxisVector.X);

		// 상하 회전 (Pitch): 마우스 Y축 이동량
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFZFCharacterPlayer::Interact()
{
	// ASC가 유효한지 확인
	if (ASC)
	{
		// "Ability.Action.Interact" 태그를 가진 Gameplay Ability를 실행
		// Native Tag를 직접 전달
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_Interact));
	}
}

void AFZFCharacterPlayer::DropSelectedItem()
{
	UE_LOG(LogTemp, Warning, TEXT("G Key DropSelectedItem Called"));
	// ASC가 유효한지 확인
	if (ASC)
	{
		// Ability.Action.DropItem 태그를 가진 어빌리티 실행
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_DropItem)
		);
	}
}

void AFZFCharacterPlayer::RunStart()
{

	if (ASC)
	{
		
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_Run));
	}
}

void AFZFCharacterPlayer::RunEnd()
{
	if (ASC)
	{
		// GameplayTagContainer를 단일 태그로 직접 초기화
		const FGameplayTagContainer RunTag(FZFGameplayTags::Ability_Action_Run);
		
		// 해당 태그를 가진 어빌리티들을 취소(주소값을 전달하여 불필요한 복사 방지)
		ASC->CancelAbilities(&RunTag);
	}
}

void AFZFCharacterPlayer::JumpStart()
{
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FZFGameplayTags::Ability_Action_Jump));
	}
}

void AFZFCharacterPlayer::JumpEnd()
{
	StopJumping();

}

void AFZFCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	/**
	 * [클라이언트 측 GAS 초기화]
	 * 서버에서 넘어온 PlayerState가 이제 유효하므로
	 * 클라이언트 환경에서도 ASC와 AvatarActor를 연결
	 */

	InitAbilitySystem();
}

void AFZFCharacterPlayer::DetectInteractable()
{
	if (!Camera)
		return;

	if (!HUDWidget)
		return;

	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + (Camera->GetForwardVector() * 500.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	UPrimitiveComponent* NewTarget = nullptr;
	IFZFInteractableInterface* Interactable = nullptr;

	if (bHit)
	{
		UPrimitiveComponent* HitComponent = Hit.GetComponent();

		if (IsValid(HitComponent) == false)
			return;

		AActor* OwnerActor = HitComponent->GetOwner();
		Interactable = Cast<IFZFInteractableInterface>(OwnerActor);
		if (Interactable)
			NewTarget = HitComponent;
	}
	else
	{
		HUDWidget->HideWidget();
	}

	// 상태가 변했을 때만 Widget 업데이트
	if (NewTarget != CurrentInteractableTarget.Get())
	{
		CurrentInteractableTarget = NewTarget;
		if (IsValid(NewTarget) && Interactable)
		{
			const FText InteractableName = Interactable->GetInteractableName(Hit.GetComponent());
			if (!InteractableName.IsEmpty())
			{
				HUDWidget->SetTargetName(InteractableName);
				HUDWidget->ShowWidget();
				HUDWidget->SetCrosshairHighlight();
			}
			else
			{
				HUDWidget->HideWidget();
				HUDWidget->SetCrosshairNormal();
			}
		}
		else
		{
			HUDWidget->HideWidget();
			HUDWidget->SetCrosshairNormal();
		}
	}
}

// 1번 슬롯 선택
void AFZFCharacterPlayer::SelectSlot1()
{
	if (InventoryComponent)
	{
		InventoryComponent->SelectSlot(0);
	}
}

// 2번 슬롯 선택
void AFZFCharacterPlayer::SelectSlot2()
{
	if (InventoryComponent)
	{
		InventoryComponent->SelectSlot(1);
	}
}

// 3번 슬롯 선택
void AFZFCharacterPlayer::SelectSlot3()
{
	if (InventoryComponent)
	{
		InventoryComponent->SelectSlot(2);
	}
}

// 4번 슬롯 선택
void AFZFCharacterPlayer::SelectSlot4()
{
	if (InventoryComponent)
	{
		InventoryComponent->SelectSlot(3);
	}
}

// 5번 슬롯 선택
void AFZFCharacterPlayer::SelectSlot5()
{
	if (InventoryComponent)
	{
		InventoryComponent->SelectSlot(4);
	}
}
