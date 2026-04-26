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

AFZFCharacterPlayer::AFZFCharacterPlayer()
{
	// 기본 설정
	// 컨트롤러의 회전 값을 받아서 설정하는 옵션 비활성화
	bUseControllerRotationPitch = false;	// Y축 회전, 위아래는 카메라만 까딱이게 함 (캡슐 통째로 눕지 않게)
	bUseControllerRotationYaw = true;		// Z축 회전, 마우스 좌우 회전 시 캐릭터 몸통(캡슐)도 같이 회전 
	bUseControllerRotationRoll = false;		// X축 회전

	// 무브먼트 설정
	// 캐릭터가 이동하는 방향에 맞게 회전을 해주는 옵션
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	// 캡슐 컴포넌트에 맞춰 스켈레탈 메시의 위치(바닥)와 방향(정면) 정렬
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 메시 에셋 지정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT(""));

	// 로드 성공했으면 설정
	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// 카메라 컴포넌트 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	// 캡슐 컴포넌트(Root)에 붙이고 Z값을 눈높이로 올리기
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f)); // 눈높이에 맞춰 Z축 위치 조절 (캐릭터 크기에 맞게 수정 필요)

	// 마우스 입력(컨트롤러 회전)에 따라 카메라가 상하좌우로 움직이도록 설정
	Camera->bUsePawnControlRotation = true;

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

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionRef.Succeeded())
	{
		// 점프 액션
	}

	// GAS
	// 의도적으로 nullptr로 밀어줌 -> PlayerState의 ASC값을 대입할거라서 혼선방지용
	ASC = nullptr;
}


void AFZFCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	ApplyMappingContext(DefaultMappingContext);
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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
		// Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AFZFCharacterPlayer::Interact);
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
				UE_LOG(LogTemp, Log, TEXT("ApplyMappingContext : %s 적용 완료"), *InMappingContext->GetName());
			}
		}
	}

}

void AFZFCharacterPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AFZFCharacterPlayer::Look(const FInputActionValue& Value)
{

}

void AFZFCharacterPlayer::Interact(const FInputActionValue& Value)
{

}
