#include "Crafting/FZFRobotWorkbench.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Item/FZFItemData.h"
#include "Item/CraftMaterial/FZFRobotPartItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Utils/FZFUtils.h"

#include "Kismet/GameplayStatics.h"
#include "Manager/FZFSoundManager.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

#include "Manager/FZFSoundManager.h"


AFZFRobotWorkbench::AFZFRobotWorkbench()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicates(true);
}

void AFZFRobotWorkbench::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFZFRobotWorkbench, CurrentBodyPart);
	DOREPLIFETIME(AFZFRobotWorkbench, CurrentRLegPart);
	DOREPLIFETIME(AFZFRobotWorkbench, CurrentLLegPart);
	DOREPLIFETIME(AFZFRobotWorkbench, CurrentRArmPart);
	DOREPLIFETIME(AFZFRobotWorkbench, CurrentLArmPart);
}

namespace
{
	template <typename T>
	T* FindComponentByName(AActor* Owner, const FString& ComponentName)
	{
		if (Owner == nullptr)
		{
			return nullptr;
		}

		TArray<T*> Components;
		Owner->GetComponents<T>(Components);

		for (T* Component : Components)
		{
			if (Component && Component->GetName() == ComponentName)
			{
				return Component;
			}
		}

		return nullptr;
	}
}

void AFZFRobotWorkbench::BeginPlay()
{
	Super::BeginPlay();

	// BP에 배치한 제작대 전체 상호작용 박스를 이름으로 찾아 변수에 연결한다.
	WorkbenchInteractionBoxRef = FZFFindComponentByName<UBoxComponent>(this, TEXT("WorkbenchInteractionBox"));

	// BP에 배치한 로봇 부품 미리보기 메시를 이름으로 찾아 변수에 연결한다.
	RobotBodyMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("RobotBodyPreviewMesh"));
	RobotLArmMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLArmPreviewMesh"));
	RobotRArmMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRArmPreviewMesh"));
	RobotLLegMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLLegPreviewMesh"));
	RobotRLegMeshRef = FZFFindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRLegPreviewMesh"));

	// 상호작용 박스가 라인트레이스에 맞도록 충돌 설정을 맞춘다.
	auto SetupInteractionBox = [](UBoxComponent* Box)
	{
		if (Box == nullptr)
		{
			return;
		}

		// 물리 충돌은 하지 않고 라인트레이스 같은 Query만 받는다.
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// 기본적으로 모든 채널을 무시한다.
		Box->SetCollisionResponseToAllChannels(ECR_Ignore);

		// GA_Interact의 LineTrace가 사용하는 Visibility 채널만 Block한다.
		Box->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	};

	SetupInteractionBox(WorkbenchInteractionBoxRef);

	// 시작할 때는 장착된 부품이 없으므로 모든 미리보기 메시를 숨긴다.
	UpdatePreviewMeshes();

	//파티클 위치 컴포넌트 찾기
	BodyEffectPointRef = FindComponentByName<USceneComponent>(this, TEXT("BodyEffectPoint"));
	LArmEffectPointRef = FindComponentByName<USceneComponent>(this, TEXT("LArmEffectPoint"));
	RArmEffectPointRef = FindComponentByName<USceneComponent>(this, TEXT("RArmEffectPoint"));
	LLegEffectPointRef = FindComponentByName<USceneComponent>(this, TEXT("LLegEffectPoint"));
	RLegEffectPointRef = FindComponentByName<USceneComponent>(this, TEXT("RLegEffectPoint"));

	//가이드 메쉬찾기
	RobotBodyGuideMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotBodyGuideMesh"));
	RobotLArmGuideMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLArmGuideMesh"));
	RobotRArmGuideMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRArmGuideMesh"));
	RobotLLegGuideMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLLegGuideMesh"));
	RobotRLegGuideMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRLegGuideMesh"));

	SoundManager = GetGameInstance()->GetSubsystem<UFZFSoundManager>();
	if (SoundManager)
	{
		UE_LOG(LogTemp, Log, TEXT("SoundManager가 생성되었습니다."));
	}
}

void AFZFRobotWorkbench::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
	if (!IsValid(Interactor))
	{
		return;
	}

	UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();
	if (!IsValid(Inventory))
	{
		return;
	}

	EFZFRobotWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);

	if (HitSlot == EFZFRobotWorkbenchSlot::Workbench)
	{
		UFZFItemData* SelectedItemData = Inventory->GetSelectedItemData();

		// TryInsertRobotPart 내부에서 서버 요청(RPC) 또는 직접 실행을 처리함.
		// 성공 여부와 인벤토리 제거, 조립 확인은 모두 서버에서 안전하게 수행됨.
		TryInsertRobotPart(SelectedItemData);
	}
}

FText AFZFRobotWorkbench::GetInteractableName(UPrimitiveComponent* HitComponent) const
{
	const EFZFRobotWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);

	switch (HitSlot)
	{
	case EFZFRobotWorkbenchSlot::Workbench:
		// 제작대 본체를 바라볼 때 표시할 이름
		return FText::FromString(TEXT("Robot Workbench"));

	default:
		return FText::GetEmpty();
	}
}

void AFZFRobotWorkbench::OnRep_RobotWorkbenchParts()
{
	UpdatePreviewMeshes();
}

EFZFRobotWorkbenchSlot AFZFRobotWorkbench::GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const
{
	// 라인트레이스에 맞은 컴포넌트가 없으면 슬롯 없음 처리
	if (HitComponent == nullptr)
	{
		return EFZFRobotWorkbenchSlot::None;
	}

	// 제작대 전체 상호작용 박스를 맞췄다면 부품 넣기 영역으로 처리
	if (HitComponent == WorkbenchInteractionBoxRef)
	{
		return EFZFRobotWorkbenchSlot::Workbench;
	}

	// 제작대 상호작용 영역이 아니면 None
	return EFZFRobotWorkbenchSlot::None;
}

void AFZFRobotWorkbench::UpdatePreviewMeshes()
{
	// 몸통 부품이 들어가 있으면 몸통 메시를 보이게 하고,
	// 없으면 숨긴다.
	if (RobotBodyMeshRef)
	{
		RobotBodyMeshRef->SetVisibility(CurrentBodyPart != nullptr);
	}

	// 왼팔 부품이 들어가 있으면 왼팔 메시를 보이게 하고,
	// 없으면 숨긴다.
	if (RobotLArmMeshRef)
	{
		RobotLArmMeshRef->SetVisibility(CurrentLArmPart != nullptr);
	}

	// 오른팔 부품이 들어가 있으면 오른팔 메시를 보이게 하고,
	// 없으면 숨긴다.
	if (RobotRArmMeshRef)
	{
		RobotRArmMeshRef->SetVisibility(CurrentRArmPart != nullptr);
	}

	// 왼다리 부품이 들어가 있으면 왼다리 메시를 보이게 하고,
	// 없으면 숨긴다.
	if (RobotLLegMeshRef)
	{
		RobotLLegMeshRef->SetVisibility(CurrentLLegPart != nullptr);
	}

	// 오른다리 부품이 들어가 있으면 오른다리 메시를 보이게 하고,
	// 없으면 숨긴다.
	if (RobotRLegMeshRef)
	{
		RobotRLegMeshRef->SetVisibility(CurrentRLegPart != nullptr);
	}
}

void AFZFRobotWorkbench::PlayInsertPartEffect_Implementation(USceneComponent* EffectPoint)
{
	// 나이아가라 시스템이나 위치 컴포넌트가 없으면 실행하지 않는다.
	if (InsertPartEffect == nullptr || EffectPoint == nullptr)
	{
		return;
	}

	// EffectPoint 위치에 나이아가라 시스템을 생성한다.
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		InsertPartEffect,
		EffectPoint->GetComponentLocation(),
		EffectPoint->GetComponentRotation()
	);
}

bool AFZFRobotWorkbench::TryInsertRobotPart(UFZFItemData* ItemData)
{
	if (HasAuthority())
	{
		// 이미 서버라면 바로 실행
		Server_TryInsertRobotPart_Implementation(ItemData);
	}
	else
	{
		// 클라이언트라면 서버에 요청을 보냄
		Server_TryInsertRobotPart(ItemData);
	}
	return true;
}

void AFZFRobotWorkbench::Server_TryInsertRobotPart_Implementation(UFZFItemData* ItemData)
{
	// 선택한 아이템 데이터가 없으면 실패
	if (ItemData == nullptr)
	{
		return;
	}

	// 선택한 아이템이 로봇 부품 데이터인지 확인
	UFZFRobotPartItemData* RobotPartData = Cast<UFZFRobotPartItemData>(ItemData);
	if (RobotPartData == nullptr)
	{
		return;
	}

	bool bInserted = false;

	// 몸통 부품이면 몸통 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotBody"))
	{
		if (CurrentBodyPart == nullptr)
		{
			CurrentBodyPart = RobotPartData;

			if (RobotBodyGuideMeshRef)
			{
				RobotBodyGuideMeshRef->SetVisibility(false);
			}

			PlayInsertPartEffect(BodyEffectPointRef);
			bInserted = true;
		}
	}
	// 왼팔 부품이면 왼팔 위치에 넣는다.
	else if (RobotPartData->MaterialTag == TEXT("RobotLArm"))
	{
		if (CurrentLArmPart == nullptr)
		{
			CurrentLArmPart = RobotPartData;
			if (RobotLArmGuideMeshRef) RobotLArmGuideMeshRef->SetVisibility(false);
			PlayInsertPartEffect(LArmEffectPointRef);
			bInserted = true;
		}
	}
	// 오른팔 부품이면 오른팔 위치에 넣는다.
	else if (RobotPartData->MaterialTag == TEXT("RobotRArm"))
	{
		if (CurrentRArmPart == nullptr)
		{
			CurrentRArmPart = RobotPartData;
			if (RobotRArmGuideMeshRef) RobotRArmGuideMeshRef->SetVisibility(false);
			PlayInsertPartEffect(RArmEffectPointRef);
			bInserted = true;
		}
	}
	// 왼다리 부품이면 왼다리 위치에 넣는다.
	else if (RobotPartData->MaterialTag == TEXT("RobotLLeg"))
	{
		if (CurrentLLegPart == nullptr)
		{
			CurrentLLegPart = RobotPartData;
			if (RobotLLegGuideMeshRef) RobotLLegGuideMeshRef->SetVisibility(false);
			PlayInsertPartEffect(LLegEffectPointRef);
			bInserted = true;
		}
	}
	// 오른다리 부품이면 오른다리 위치에 넣는다.
	else if (RobotPartData->MaterialTag == TEXT("RobotRLeg"))
	{
		if (CurrentRLegPart == nullptr)
		{
			CurrentRLegPart = RobotPartData;
			if (RobotRLegGuideMeshRef) RobotRLegGuideMeshRef->SetVisibility(false);
			PlayInsertPartEffect(RLegEffectPointRef);
			bInserted = true;
		}
	}

	FVector Location = GetActorLocation();
	SoundManager->PlaySFXAtLocation("Robot_Part_Attach", Location);
	
	if (bInserted)
	{
		UpdatePreviewMeshes();

		// 서버 측에서 상호작용한 플레이어의 인벤토리 아이템 제거
		if (APawn* InstigatorPawn = GetInstigator())
		{
			if (UFZFInventoryComponent* Inventory = InstigatorPawn->FindComponentByClass<UFZFInventoryComponent>())
			{
				Inventory->RemoveSelectedItem();
			}
		}

		TryCraftRobot();
	}
}

bool AFZFRobotWorkbench::TryCraftRobot()
{
	// 몸통 부품이 없으면 조립 실패
	if (CurrentBodyPart == nullptr)
	{
		return false;
	}

	// 왼팔 부품이 없으면 조립 실패
	if (CurrentLArmPart == nullptr)
	{
	}

	// 오른팔 부품이 없으면 조립 실패
	if (CurrentRArmPart == nullptr)
	{
		return false;
	}

	// 왼다리 부품이 없으면 조립 실패
	if (CurrentLLegPart == nullptr)
	{
		return false;
	}

	// 오른다리 부품이 없으면 조립 실패
	if (CurrentRLegPart == nullptr)
	{
		return false;
	}

	// 여기까지 왔으면 로봇 조립에 필요한 모든 부품이 들어간 상태
	UE_LOG(LogTemp, Warning, TEXT("Robot Craft Success"));

	// 지금은 완성 로봇 생성까지 하지 않고 성공 로그만 찍는다.
	// 나중에 여기서 완성 로봇 액터를 Spawn하거나,
	// 문 열기 / 퀘스트 완료 / 제작 완료 상태 처리 등을 넣으면 된다.

	return true;
}
