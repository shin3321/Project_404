#include "FZFHeldItemComponent.h"
#include "FZFHeldItemActor.h"
#include "Item/FZFItemData.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Item/FZFItemAnimSetData.h"

// Item의 능력치
#include "Item/Equipment/FZFRangedItemData.h"
#include "Item/Equipment/FZFMeleeItemData.h"
#include "Item/Equipment/FZFThrowableItemData.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/FZFPlayerAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

// GAS
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UFZFHeldItemComponent::UFZFHeldItemComponent()
{
	// Tick 필요 없으면 끔
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

class USceneComponent* UFZFHeldItemComponent::GetHeldItemRootComponent() const
{
	// 현재 손에 무기를 들고 있고, 그 무기가 유효하다면
	if (CurrentHeldItem)
	{
		// 그 무기 액터의 루트 컴포넌트를 반환
		return CurrentHeldItem->GetRootComponent();
	}

	return nullptr;
}

void UFZFHeldItemComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateUpperBodyBlendWeight();
}

void UFZFHeldItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFZFHeldItemComponent, CurrentHeldItem);
	DOREPLIFETIME(UFZFHeldItemComponent, CurrentHeldItemData);
}

FGameplayTag UFZFHeldItemComponent::GetCurrentAttackTag() const
{
	return CurrentHeldItemData ? CurrentHeldItemData->ItemAbilityTag : FGameplayTag::EmptyTag;
}

AFZFCharacterPlayer* UFZFHeldItemComponent::GetOwnerCharacter() const
{
	return Cast<AFZFCharacterPlayer>(GetOwner());
}

class USceneComponent* UFZFHeldItemComponent::GetHeldFirstItemRootComponent() const
{
	// 현재 손에 무기를 들고 있고, 그 무기가 유효하다면
	if (LocalFirstPersonHeldItem)
	{
		// 그 무기 액터의 루트 컴포넌트를 반환
		return LocalFirstPersonHeldItem->GetRootComponent();
	}

	return nullptr;
}


void UFZFHeldItemComponent::HoldItem(UFZFItemData* ItemData)
{
	// 클라이언트라면 서버에게 장착 요청
	if (!GetOwner()->HasAuthority())
	{
		ServerHoldItem(ItemData);
		return;
	}

	ServerHoldItem_Implementation(ItemData);
}

void UFZFHeldItemComponent::ServerHoldItem_Implementation(UFZFItemData* ItemData)
{
	ClearHeldItemGAS();

	DestroyThirdPersonHeldItem_Server();

	CurrentHeldItemData = ItemData;

	if (!ItemData || !HeldItemClass)
	{
		UpdateUpperBodyBlendWeight();
		UpdateHeldItemAnimation();
		DestroyLocalFirstPersonHeldItem();
		return;
	}

	SpawnThirdPersonHeldItem_Server(ItemData);

	ApplyHeldItemGAS(ItemData);

	// Listen Server의 Host 플레이어를 위한 처리
	RefreshLocalFirstPersonHeldItem();

	UpdateUpperBodyBlendWeight();
	UpdateHeldItemAnimation();
}

void UFZFHeldItemComponent::ApplyHeldItemGAS(UFZFItemData* ItemData)
{
	AFZFCharacterPlayer* OwnerCharacter = Cast<AFZFCharacterPlayer>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter is null"));
		return;
	}

	if (OwnerCharacter && ItemData && ItemData->ItemAbilityTag.IsValid())
	{
		// 캐릭터로부터 ASC를 가져옴 (IAbilitySystemInterface 구현 가정)
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerCharacter))
		{
			UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
			if (ASC)
			{
				// 아이템 장착 시 태그 부여.
				ASC->AddReplicatedLooseGameplayTag(ItemData->ItemAbilityTag);

				// 나중에 지우기 위해 현재 태그 저장 
				CurrentEquippedTag = ItemData->ItemAbilityTag;

				// 장비(Equipment) 데이터인 경우 GE 처리
				if (UFZFEquipmentItemData* EquipData = Cast<UFZFEquipmentItemData>(ItemData))
				{
					if (WeaponModifierGE.Get() && RangeDataTag.IsValid() && DamageDataTag.IsValid())
					{
						// 이 효과가 "어디서, 누구에 의해" 발생했는지에 대한 부가정보(Context)를 담을 바구니 제작
						FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();

						// 효과를 일으킨 주체(Instingator)와 원인 제공자(EffectCauser)를 설정
						EffectContext.AddInstigator(OwnerCharacter, OwnerCharacter);

						// 설정된 GE 클래스와 컨텍스트를 바탕으로, 실제 적용 가능한 실행 데이터 객체(Spec)를 생성함, 1.0f는 효과의 레벨
						FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
							WeaponModifierGE, 1.0f, EffectContext);
						if (SpecHandle.IsValid())
						{
							// SetByCaller로 사거리 주입
							SpecHandle.Data.Get()->SetSetByCallerMagnitude(RangeDataTag, EquipData->GetRange());
							// GE 내부에 미리 정의된 RangeDataTag 위치에 데이터 에셋에서 가져온 실제 사거리 수치를 동적으로 넣음
							SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageDataTag, EquipData->GetDamage());

							// GE 적용 및 핸들 저장 (나중에 해제할 때, 사용)
							// RangeEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get()); 이거와 같음
							WeaponStatEffectHandle = ASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
						}
					}
				}
			}
		}
	}
}

void UFZFHeldItemComponent::SpawnThirdPersonHeldItem_Server(UFZFItemData* ItemData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerCharacter is null"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentHeldItem = GetWorld()->SpawnActor<AFZFHeldItemActor>(
		HeldItemClass,
		FTransform::Identity,
		SpawnParams
	);

	if (!CurrentHeldItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn HeldItemActor"));
		return;
	}

	CurrentHeldItem->SetHeldItemData(ItemData);
	CurrentHeldItem->SetThirdPersonVisualMode();

	// 멀티에서 다른 사람에게 보이는 아이템은 항상 3인칭 Mesh에 붙인다.
	CurrentHeldItem->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("hand_r_Socket")
	);
}

void UFZFHeldItemComponent::ClearHeldItem()
{
	if (!GetOwner())
	{
		return;
	}

	// [로컬] 즉시 팔 매쉬 위치 초기화 (반응성)
	if (AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter())
	{
		OwnerCharacter->SetArmMeshDefaultTransform();
	}

	if (!GetOwner()->HasAuthority())
	{
		ServerClearHeldItem();
		return;
	}

	ServerClearHeldItem();
}

void UFZFHeldItemComponent::ServerClearHeldItem_Implementation()
{
	// GAS 관련 효과 / 태그 먼저 제거
	ClearHeldItemGAS();

	// 서버에 존재하는 3인칭 복제 아이템 제거
	DestroyThirdPersonHeldItem_Server();

	// 현재 아이템 데이터 초기화
	CurrentHeldItemData = nullptr;

	// 애니메이션 블렌드 갱신
	UpdateUpperBodyBlendWeight();
	UpdateHeldItemAnimation();

	// Listen Server Host용 로컬 1인칭 아이템 제거
	DestroyLocalFirstPersonHeldItem();
}

void UFZFHeldItemComponent::ClearHeldItemGAS()
{
	// GAS 관련 자원 해제
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			// 통합 무기 스탯 GE 제거 (공격력, 사거리 동시 복구)
			if (WeaponStatEffectHandle.IsValid())
			{
				ASC->RemoveActiveGameplayEffect(WeaponStatEffectHandle);
				WeaponStatEffectHandle.Invalidate(); // 핸들 초기화
			}

			// 아이템 태그 제거 : 장착 시 부여했던 LooseTag 제거
			if (CurrentEquippedTag.IsValid())
			{
				ASC->RemoveReplicatedLooseGameplayTag(CurrentEquippedTag);
				CurrentEquippedTag = FGameplayTag::EmptyTag;
			}
		}
	}
}

void UFZFHeldItemComponent::DestroyThirdPersonHeldItem_Server()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (IsValid(CurrentHeldItem))
	{
		CurrentHeldItem->Destroy();
	}

	CurrentHeldItem = nullptr;
}

void UFZFHeldItemComponent::OnRep_CurrentHeldItem()
{
	AttachThirdPersonHeldItem();
}

void UFZFHeldItemComponent::OnRep_CurrentHeldItemData()
{
	UpdateUpperBodyBlendWeight();
	UpdateHeldItemAnimation();
	RefreshLocalFirstPersonHeldItem();
}

void UFZFHeldItemComponent::AttachThirdPersonHeldItem()
{
	if (!IsValid(CurrentHeldItem))
	{
		return;
	}

	AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter)
	{
		return;
	}

	CurrentHeldItem->SetThirdPersonVisualMode();

	//if (OwnerCharacter->IsLocallyControlled())
	//{
	//	if (UStaticMeshComponent* MeshComp = CurrentHeldItem->GetItemMeshComponent())
	//	{
	//		MeshComp->SetOwnerNoSee(true);
	//		MeshComp->SetHiddenInGame(true);
	//	}
	//}

	CurrentHeldItem->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("hand_r_Socket")
	);
}

void UFZFHeldItemComponent::RefreshLocalFirstPersonHeldItem()
{
	AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter)
	{
		return;
	}

	// 내가 조종하는 캐릭터가 아니면 1인칭 아이템을 만들 필요 없음
	if (!OwnerCharacter->IsLocallyControlled())
	{
		DestroyLocalFirstPersonHeldItem();
		return;
	}

	DestroyLocalFirstPersonHeldItem();

	if (!CurrentHeldItemData)
	{
		return;
	}

	if (!CurrentHeldItemData->Mesh)
	{
		return;
	}

	if (!HeldItemClass)
	{
		return;
	}

	USkeletalMeshComponent* ArmMesh = OwnerCharacter->GetArmMesh();
	if (!ArmMesh)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	LocalFirstPersonHeldItem = GetWorld()->SpawnActor<AFZFHeldItemActor>(
		HeldItemClass,
		FTransform::Identity,
		SpawnParams
	);

	if (!LocalFirstPersonHeldItem)
	{
		return;
	}

	// 로컬 전용 아이템은 복제하지 않는다.
	LocalFirstPersonHeldItem->SetReplicates(false);
	LocalFirstPersonHeldItem->SetReplicateMovement(false);

	LocalFirstPersonHeldItem->SetHeldItemData(CurrentHeldItemData);
	LocalFirstPersonHeldItem->SetFirstPersonVisualMode();

	// [중요] 본인에게만 보여야 함 (확실하게 보이기 위해 HiddenInGame 해제)
	if (UStaticMeshComponent* MeshComp = LocalFirstPersonHeldItem->GetItemMeshComponent())
	{
		MeshComp->SetOnlyOwnerSee(true);
		MeshComp->SetHiddenInGame(false);
	}

	LocalFirstPersonHeldItem->AttachToComponent(
		ArmMesh,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("hand_r_Socket")
	);

	// 애니메이션 위치 및 회전 값 하드코딩 방식.
	if (CurrentHeldItemData->ItemType == EItemType::Equipment)
		OwnerCharacter->SetArmMeshTransform(FVector(45.0f, 20.0f, -194.0f), FRotator(0.0f, 0.0f, 0.0f));
}

void UFZFHeldItemComponent::DestroyLocalFirstPersonHeldItem()
{
	if (IsValid(LocalFirstPersonHeldItem))
	{
		LocalFirstPersonHeldItem->Destroy();
	}

	LocalFirstPersonHeldItem = nullptr;
}

void UFZFHeldItemComponent::UpdateUpperBodyBlendWeight()
{
	AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter)
	{
		return;
	}

	float BlendWeight = 0.0f;
	if (CurrentHeldItemData && CurrentHeldItemData->ItemType == EItemType::Equipment)
		BlendWeight = CurrentHeldItemData ? 1.0f : 0.0f;


	// 3인칭 Mesh AnimInstance
	if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
	{
		if (UFZFPlayerAnimInstance* AnimInstance = Cast<UFZFPlayerAnimInstance>(CharacterMesh->GetAnimInstance()))
		{
			AnimInstance->SetUpperBodyBlendWeight(BlendWeight);
		}
	}

	// 1인칭 ArmMesh AnimInstance
	if (USkeletalMeshComponent* ArmMesh = OwnerCharacter->GetArmMesh())
	{
		if (UFZFPlayerAnimInstance* AnimInstance = Cast<UFZFPlayerAnimInstance>(ArmMesh->GetAnimInstance()))
		{
			AnimInstance->SetUpperBodyBlendWeight(BlendWeight);
		}
	}
}

void UFZFHeldItemComponent::UpdateHeldItemAnimation()
{
	AFZFCharacterPlayer* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter)
	{
		return;
	}

	UAnimSequence* ThirdPersonIdle = nullptr;
	UAnimSequence* FirstPersonIdle = nullptr;

	if (CurrentHeldItemData && CurrentHeldItemData->AnimSet)
	{
		ThirdPersonIdle = CurrentHeldItemData->AnimSet->ThirdPersonIdle;
		FirstPersonIdle = CurrentHeldItemData->AnimSet->FirstPersonIdle;
	}

	// 3인칭 Mesh AnimInstance
	if (USkeletalMeshComponent* CharacterMesh = OwnerCharacter->GetMesh())
	{
		if (UFZFPlayerAnimInstance* AnimInstance = Cast<UFZFPlayerAnimInstance>(CharacterMesh->GetAnimInstance()))
		{
			AnimInstance->SetCurrentIdleAnim(ThirdPersonIdle);
		}
	}

	// 1인칭 ArmMesh AnimInstance
	if (USkeletalMeshComponent* ArmMesh = OwnerCharacter->GetArmMesh())
	{
		if (UFZFPlayerAnimInstance* AnimInstance = Cast<UFZFPlayerAnimInstance>(ArmMesh->GetAnimInstance()))
		{
			AnimInstance->SetCurrentIdleAnim(FirstPersonIdle);
		}
	}
}
