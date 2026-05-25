#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "InputActionValue.h"
#include "FZFCharacterPlayer.generated.h"

// 전방선언.
class UInputAction;
class UInputMappingContext;
class UFZFInventoryComponent;
class UFZFHUD;
class UFZFHeldItemComponent;
class AFZFBossroomBtn;

UCLASS()
class PROJECT_404_API AFZFCharacterPlayer : public AFZFCharacterBase
{
	GENERATED_BODY()

public:
	// 숫자키 1~5번 슬롯 선택 함수
	void SelectSlot1();
	void SelectSlot2();
	void SelectSlot3();
	void SelectSlot4();
	void SelectSlot5();

public:
	AFZFCharacterPlayer();

public:
	// 인벤토리 컴포넌트에 접근할 수 있도록 Getter 추가 (GA에서 사용)
	UFZFInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	// 관전 시점 처리
	virtual void BecomeViewTarget(APlayerController* PC) override; 
	virtual void EndViewTarget(APlayerController* PC) override;

	// 팔 매쉬 Getter
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() const { return ArmMesh; }

	// 카메라에 안전하게 접근할 수 있도록 Getter 추가 (GA에서 사용)
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

	// 아이템 변경 시 Idle 애니메이션 수정.
	void ApplyAnimationsByItemAnimType(UAnimSequence* ThirdPersonIdle, UAnimSequence* FirstPersonIdle);

	// 위치값 및 회전값이 다른 애니메이션 대응을 위한, ArmMesh위치 하드코딩 수정.
	void SetArmMeshTransform(FVector Location, FRotator Rotation);
	void SetArmMeshDefaultTransform();

	virtual void SetDead() override;

	// 곡괭이 장착 및 해제.
	void EquipPickaxe();
	void UnEquipPickaxe();

	// 현재 들고있는 아이템을 가져오기 위한 Get함수.
	UFZFHeldItemComponent* GetHeldItemComponent() { return HeldItemComponent; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime)override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void InitAbilitySystem() override;

	// 입력 장치와 캐릭터 기능을 연결하기 위한 설정 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 입력 매핑 컨텍스트(IMC)를 활성화하여 캐릭터의 조작 체계를 설정
	void ApplyMappingContext(UInputMappingContext* InMappingContext);

	// 네트워크 연결 시 클라이언트 재연결
	virtual void PawnClientRestart() override;

protected:
	// 이동 함수
	void Move(const FInputActionValue& Value);

	// 회전 함수
	void Look(const FInputActionValue& Value);

	// 상호작용 함수
	void Interact();

	// 홀드 상호작용 종료
	void StopHoldInteract();


	// 곡괭이 함수.
	void TogglePickaxe();

	// 선택된 인벤토리 아이템을 버리는 입력 처리 함수
	void DropSelectedItem();

	// 달리기 함수
	void RunStart();
	void RunEnd();

	// 점프 함수
	void JumpStart();
	void JumpEnd();

	// 공격 함수
	void Attack();


	// Camera Section
		// 네트워크 상태 동기화 함수
	virtual void OnRep_PlayerState() override;

// Death Section
	// 서버가 bIsDead를 true로 바꼈을 때, 클라이언트에서 실행될 함수 오버라이드
	virtual void OnRep_IsDead() override;

public:
	UFUNCTION(Server, Reliable)
	void ServerSpawnSkillActor(
		TSubclassOf<class AFZFSkillBase> SkillActorClass,
		FVector SpawnLocation,
		FRotator SpawnRotation
	);

protected:
	// 카메라 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;


	// Mesh Section
protected:
	UPROPERTY(VisibleAnywhere, Category = Mesh, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ArmMesh;

	// Input Section
protected:

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> PickaxeAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> DropItemAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(VisibleAnywhere, Category = Input, BlueprintReadOnly)
	TObjectPtr<UInputAction> AttackAction;

	// Interact Section
protected:
	// 상호작용하면 UI로 표시하기 위한 함수
	void DetectInteractable();

	// Dead Section
public:
	virtual void HandleDeath() override;

public:
	// 보스방 버튼 홀드 시작
	void BeginBossroomHold(AFZFBossroomBtn* InBossroomBtn);

	// 보스방 버튼 홀드 종료 / 취소
	void EndBossroomHold();

	// 서버에서 보스방 홀드 시작
	UFUNCTION(Server, Reliable)
	void ServerBeginBossroomHold(AFZFBossroomBtn* InBossroomBtn);

	// 서버에서 보스방 홀드 종료
	UFUNCTION(Server, Reliable)
	void ServerEndBossroomHold();

	// Attribute 변경 핸들러 함수들
	UFUNCTION()
	void OnHpChanged(float NewValue, float MaxValue);

	UFUNCTION()
	void OnStaminaChanged(float NewValue, float MaxValue);

	protected:
	// 이전 체력을 기억하여 데미지 여부 판단
	float PreviousHP = 0.0f;

	// 현재 카메라 조준점에 들어와 있는 타겟 (UI 표시용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TWeakObjectPtr<class UPrimitiveComponent> CurrentInteractableTarget;

	// 1~5번 슬롯 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Slot1Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Slot2Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Slot3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Slot4Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> Slot5Action;

	// Inventory Section
protected:
	// 플레이어 인벤토리 데이터 관리 컴포넌트 추가
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UFZFInventoryComponent> InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UFZFHUD> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UFZFHUD> HUDWidget;

	// 현재 조준 중인 보스방 버튼 액터
	UPROPERTY(Replicated)
	TObjectPtr<AFZFBossroomBtn> CurrentBossroomBtn;

	// 홀드 총 시간 (C++ 고정값)
	float HoldRequiredTime = 2.0f;

	// 현재 누르고 있는 시간
	UPROPERTY(Replicated)
	float CurrentHoldTime = 0.0f;

	// 현재 홀드 중인지 여부
	UPROPERTY(Replicated)
	bool bHolding = false;

	// 선택된 인벤토리 아이템을 손에 들게 처리하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFZFHeldItemComponent> HeldItemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<class UFZFItemData> PickaxeData = nullptr;


	// GameplayEffect Section 
protected:
	// 스테미너 자동 재생 GE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<class UGameplayEffect> PassiveRegenStaminaEffectClass;

	// HP 자동 재생 GE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<class UGameplayEffect> PassiveRegenHPEffectClass;

	// 에디터에서 GE_Death를 할당할 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DeathGameplayEffectClass;
	
	// 곡괭이로 EnergyRelay 공격 시, 적용할 GE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> EnergyRelayDamageEffectClass;
protected:
	// 서버에서 실행될 RPC
	UFUNCTION(Server, Unreliable)
	void ServerPlaySound(FName RowName, FVector Location);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlaySound(FName RowName, FVector Location);
	
private:
	FVector LastFootstepLocation;
	UPROPERTY(EditDefaultsOnly, Category= "Sound")
	float FootstepDistance = 250.f;
	
};

