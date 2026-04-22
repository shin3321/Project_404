#include "Character/Player/FZFCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT(""));
	if (MoveActionRef.Succeeded())
	{
		// 1인칭용 MoveAction 변수에 할당
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT(""));
	if (LookActionRef.Succeeded())
	{
		// 1인칭용 LookAction 변수에 할당
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionRef(TEXT("/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionRef.Succeeded())
	{
		// 점프 액션
	}
}
