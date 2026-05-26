#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/FZFInteractableInterface.h"
#include "FZFRobotWorkbench.generated.h"


// 로봇 제작대에서 상호작용 가능한 영역
UENUM(BlueprintType)
enum class EFZFRobotWorkbenchSlot : uint8
{
	None UMETA(DisplayName = "None"),

	// 제작대 본체 상호작용 영역
	// 이 영역에 상호작용하면 현재 선택한 로봇 부품을 자동으로 맞는 위치에 넣는다.
	Workbench UMETA(DisplayName = "Workbench")
};

// 라인트레이스에 맞은 컴포넌트를 받기 위한 전방 선언
class UPrimitiveComponent;

// 로봇 제작대의 각 부품 슬롯 충돌 박스에 사용할 컴포넌트
class UBoxComponent;

// 제작대 위에 로봇 부품을 미리보기로 보여줄 StaticMeshComponent
class UStaticMeshComponent;

// 기본 아이템 데이터 클래스
class UFZFItemData;

// 로봇 부품 아이템 데이터 클래스
class UFZFRobotPartItemData;

// 상호작용하는 플레이어 캐릭터 클래스
class AFZFCharacterPlayer;

//파티클 시스템 클래스
class UNiagaraSystem;
class USceneComponent;


// 로봇 제작대 액터
// 플레이어가 라인트레이스로 제작대의 슬롯을 바라보고 상호작용할 수 있도록
// IFZFInteractableInterface를 구현한다.
UCLASS()
class PROJECT_404_API AFZFRobotWorkbench : public AActor, public IFZFInteractableInterface
{
	GENERATED_BODY()

public:
	// 생성자
	// Tick 사용 여부, 기본값 등을 설정한다.
	AFZFRobotWorkbench();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 플레이어가 로봇 제작대와 상호작용했을 때 호출되는 함수
	// HitComponent를 통해 어느 슬롯을 눌렀는지 판단한다.
	virtual void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) override;

	// 상호작용 UI에 표시할 이름을 반환하는 함수
	// 예: "Body Slot", "Left Arm Slot", "Craft" 같은 텍스트
	virtual FText GetInteractableName(UPrimitiveComponent* HitComponent) const override;

protected:
	// 게임 시작 시 호출되는 함수
	// BP에 배치한 BoxComponent, StaticMeshComponent를 찾아서 연결할 예정
	virtual void BeginPlay() override;

protected:
	// 로봇 제작대 전체 상호작용 박스
	// 플레이어가 이 박스를 바라보고 상호작용하면 현재 선택한 로봇 부품을 자동으로 맞는 위치에 넣는다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Interaction")
	TObjectPtr<UBoxComponent> WorkbenchInteractionBoxRef;

protected:
	// 제작대 위에 보여줄 몸통 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Preview")
	TObjectPtr<UStaticMeshComponent> RobotBodyMeshRef;

	// 제작대 위에 보여줄 왼팔 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Preview")
	TObjectPtr<UStaticMeshComponent> RobotLArmMeshRef;

	// 제작대 위에 보여줄 오른팔 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Preview")
	TObjectPtr<UStaticMeshComponent> RobotRArmMeshRef;

	// 제작대 위에 보여줄 왼다리 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Preview")
	TObjectPtr<UStaticMeshComponent> RobotLLegMeshRef;

	// 제작대 위에 보여줄 오른다리 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Preview")
	TObjectPtr<UStaticMeshComponent> RobotRLegMeshRef;

protected:
	// 부품 장착 성공 시 재생할 나이아가라 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotWorkbench|Effect")
	TObjectPtr<UNiagaraSystem> InsertPartEffect;

	// 몸통 장착 이펙트 위치
	UPROPERTY()
	TObjectPtr<USceneComponent> BodyEffectPointRef;

	// 왼팔 장착 이펙트 위치
	UPROPERTY()
	TObjectPtr<USceneComponent> LArmEffectPointRef;

	// 오른팔 장착 이펙트 위치
	UPROPERTY()
	TObjectPtr<USceneComponent> RArmEffectPointRef;

	// 왼다리 장착 이펙트 위치
	UPROPERTY()
	TObjectPtr<USceneComponent> LLegEffectPointRef;

	// 오른다리 장착 이펙트 위치
	UPROPERTY()
	TObjectPtr<USceneComponent> RLegEffectPointRef;

protected:
	// 몸통 장착 전 보여줄 가이드 메시
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RobotBodyGuideMeshRef;

	// 왼팔 장착 전 보여줄 가이드 메시
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RobotLArmGuideMeshRef;

	// 오른팔 장착 전 보여줄 가이드 메시
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RobotRArmGuideMeshRef;

	// 왼다리 장착 전 보여줄 가이드 메시
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RobotLLegGuideMeshRef;

	// 오른다리 장착 전 보여줄 가이드 메시
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> RobotRLegGuideMeshRef;

public:
	// 현재 몸통 슬롯에 들어간 부품 데이터
	UPROPERTY(ReplicatedUsing=OnRep_RobotWorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFRobotPartItemData> CurrentBodyPart;

	// 현재 왼팔 슬롯에 들어간 부품 데이터
	UPROPERTY(ReplicatedUsing=OnRep_RobotWorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFRobotPartItemData> CurrentLArmPart;

	// 현재 오른팔 슬롯에 들어간 부품 데이터
	UPROPERTY(ReplicatedUsing=OnRep_RobotWorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFRobotPartItemData> CurrentRArmPart;

	// 현재 왼다리 슬롯에 들어간 부품 데이터
	UPROPERTY(ReplicatedUsing=OnRep_RobotWorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFRobotPartItemData> CurrentLLegPart;

	// 현재 오른다리 슬롯에 들어간 부품 데이터
	UPROPERTY(ReplicatedUsing=OnRep_RobotWorkbenchParts, Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFZFRobotPartItemData> CurrentRLegPart;

	UFUNCTION()
	void OnRep_RobotWorkbenchParts();
	
	// 선택한 아이템의 MaterialTag를 보고 맞는 로봇 부품 위치에 넣는 함수
	bool TryInsertRobotPart(UFZFItemData* ItemData, AFZFCharacterPlayer* Interactor);
	
	// 서버에게 실행해 달라고 요청하는 함수 (Server RPC)
	UFUNCTION(Server, Reliable)
	void Server_TryInsertRobotPart(UFZFItemData* ItemData, AFZFCharacterPlayer* Interactor);
	
private:
	// 맞은 컴포넌트가 제작대인지 조립 버튼인지 판별하는 함수
	EFZFRobotWorkbenchSlot GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const;	
	
	// 모든 로봇 부품이 들어갔는지 확인하고 조립을 시도하는 함수
	bool TryCraftRobot();

	// 현재 들어간 부품 상태에 따라 미리보기 메시를 보이거나 숨기는 함수
	void UpdatePreviewMeshes();

	// 지정한 위치에 부품 장착 이펙트를 재생한다.
	UFUNCTION(NetMulticast, Unreliable)
	void PlayInsertPartEffect(USceneComponent* EffectPoint);
	
	class UFZFSoundManager* SoundManager;
};
