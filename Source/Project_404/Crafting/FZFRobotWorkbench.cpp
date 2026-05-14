#include "Crafting/FZFRobotWorkbench.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"

#include "Item/FZFItemData.h"
#include "Item/CraftMaterial/FZFRobotPartItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Inventory/FZFInventoryComponent.h"

namespace
{
    // 액터 안에 있는 컴포넌트 중 이름이 같은 컴포넌트를 찾아 반환하는 함수
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

AFZFRobotWorkbench::AFZFRobotWorkbench()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFZFRobotWorkbench::BeginPlay()
{
	Super::BeginPlay();

	// BP에 배치한 제작대 전체 상호작용 박스를 이름으로 찾아 변수에 연결한다.
	WorkbenchInteractionBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("WorkbenchInteractionBox"));

	// BP에 배치한 조립 버튼 박스를 이름으로 찾아 변수에 연결한다.
	CraftButtonBoxRef = FindComponentByName<UBoxComponent>(this, TEXT("CraftButtonBox"));

	// BP에 배치한 로봇 부품 미리보기 메시를 이름으로 찾아 변수에 연결한다.
	RobotBodyMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotBodyPreviewMesh"));
	RobotLArmMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLArmPreviewMesh"));
	RobotRArmMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRArmPreviewMesh"));
	RobotLLegMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotLLegPreviewMesh"));
	RobotRLegMeshRef = FindComponentByName<UStaticMeshComponent>(this, TEXT("RobotRLegPreviewMesh"));

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
	SetupInteractionBox(CraftButtonBoxRef);

	// 시작할 때는 장착된 부품이 없으므로 모든 미리보기 메시를 숨긴다.
	UpdatePreviewMeshes();
}

void AFZFRobotWorkbench::Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent)
{
	// 상호작용한 플레이어가 유효하지 않으면 종료
	if (!IsValid(Interactor))
	{
		return;
	}

	// 플레이어의 인벤토리 컴포넌트 가져오기
	UFZFInventoryComponent* Inventory = Interactor->GetInventoryComponent();
	if (!IsValid(Inventory))
	{
		return;
	}

	// 라인트레이스에 맞은 컴포넌트가 제작대인지 조립 버튼인지 판별
	EFZFRobotWorkbenchSlot HitSlot = GetSlotFromHitComponent(HitComponent);

	switch (HitSlot)
	{
	case EFZFRobotWorkbenchSlot::Workbench:
	{
		// 현재 선택 중인 아이템 데이터 가져오기
		UFZFItemData* SelectedItemData = Inventory->GetSelectedItemData();

		// 선택한 아이템의 MaterialTag를 보고 맞는 로봇 부품 위치에 넣기 시도
		if (TryInsertRobotPart(SelectedItemData) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to insert robot part."));
			return;
		}

		// 부품 넣기에 성공했으면 인벤토리에서 선택 아이템 제거
		Inventory->RemoveSelectedItem();
		break;
	}

	case EFZFRobotWorkbenchSlot::Crafting:
	{
		// 모든 부품이 들어갔는지 확인하고 로봇 조립 시도
		if (TryCraftRobot() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to craft robot."));
			return;
		}

		break;
	}

	default:
		break;
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

	case EFZFRobotWorkbenchSlot::Crafting:
		// 조립 버튼을 바라볼 때 표시할 이름
		return FText::FromString(TEXT("Craft Robot"));

	default:
		return FText::GetEmpty();
	}
}

EFZFRobotWorkbenchSlot AFZFRobotWorkbench::GetSlotFromHitComponent(UPrimitiveComponent* HitComponent) const
{
	// 라인트레이스에 맞은 컴포넌트가 없으면 슬롯 없음
	if (HitComponent == nullptr)
	{
		return EFZFRobotWorkbenchSlot::None;
	}

	// 제작대 전체 상호작용 박스를 맞췄다면 부품 넣기 영역으로 처리
	if (HitComponent == WorkbenchInteractionBoxRef)
	{
		return EFZFRobotWorkbenchSlot::Workbench;
	}

	// 조립 버튼 박스를 맞췄다면 조립 영역으로 처리
	if (HitComponent == CraftButtonBoxRef)
	{
		return EFZFRobotWorkbenchSlot::Crafting;
	}

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

bool AFZFRobotWorkbench::TryInsertRobotPart(UFZFItemData* ItemData)
{
	// 선택한 아이템 데이터가 없으면 실패
	if (ItemData == nullptr)
	{
		return false;
	}

	// 선택한 아이템이 로봇 부품 데이터인지 확인
	UFZFRobotPartItemData* RobotPartData = Cast<UFZFRobotPartItemData>(ItemData);
	if (RobotPartData == nullptr)
	{
		return false;
	}

	// 몸통 부품이면 몸통 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotBody"))
	{
		if (CurrentBodyPart != nullptr)
		{
			return false;
		}

		CurrentBodyPart = RobotPartData;
		UpdatePreviewMeshes();
		return true;
	}

	// 왼팔 부품이면 왼팔 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotLArm"))
	{
		if (CurrentLArmPart != nullptr)
		{
			return false;
		}

		CurrentLArmPart = RobotPartData;
		UpdatePreviewMeshes();
		return true;
	}

	// 오른팔 부품이면 오른팔 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotRArm"))
	{
		if (CurrentRArmPart != nullptr)
		{
			return false;
		}

		CurrentRArmPart = RobotPartData;
		UpdatePreviewMeshes();
		return true;
	}

	// 왼다리 부품이면 왼다리 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotLLeg"))
	{
		if (CurrentLLegPart != nullptr)
		{
			return false;
		}

		CurrentLLegPart = RobotPartData;
		UpdatePreviewMeshes();
		return true;
	}

	// 오른다리 부품이면 오른다리 위치에 넣는다.
	if (RobotPartData->MaterialTag == TEXT("RobotRLeg"))
	{
		if (CurrentRLegPart != nullptr)
		{
			return false;
		}

		CurrentRLegPart = RobotPartData;
		UpdatePreviewMeshes();
		return true;
	}

	// 로봇 부품 태그가 아니면 실패
	return false;
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
		return false;
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



