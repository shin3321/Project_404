// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA/FZFGA_Interact.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Item/FZFItemBase.h"
#include "Inventory/FZFInventoryComponent.h"
#include "Camera/CameraComponent.h"

UFZFGA_Interact::UFZFGA_Interact()
{
	// 인스턴싱 정책: 어빌리티가 실행될 때마다 인스턴스를 생성 (데이터 관리가 편함)
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFZFGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 연타 방지 확장 설계 (짧은 딜레이 주기)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 로직 실행
	PerformTraceAndPickup();

	// 실행 후 즉시 어빌리티 종료
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UFZFGA_Interact::PerformTraceAndPickup()
{
	// 캐릭터 정보 가져오기
	AFZFCharacterPlayer* Player = Cast<AFZFCharacterPlayer>(GetAvatarActorFromActorInfo());
	if (!Player)
	{
		return;
	}

	UCameraComponent* CameraComp = Player->GetCamera();

	// 카메라 위치와 방향 참조
	if (!CameraComp)
	{
		return;

	}
	// 라인트레이스 시작 위치를 카메라 위치로 설정
	FVector Start = CameraComp->GetComponentLocation();

	// 카메라가 바라보는 방향으로 500.f 거리만큼 끝 위치 설정
	FVector End = Start + (CameraComp->GetForwardVector() * TraceDistance);

	// 라인트레이스 충돌 결과를 저장할 변수
	FHitResult Hit;

	// 라인트레이스 옵션 설정
	FCollisionQueryParams Params;

	// 자기 자신은 라인트레이스 충돌 대상에서 제외
	Params.AddIgnoredActor(Player);

	// Start부터 End까지 Visibility 채널로 라인트레이스 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// 라인트레이스 디버그 선 표시
	// 맞은 액터가 있으면 초록색, 없으면 빨간색
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.f, 0, 1.f);
	
	if (bHit && Hit.GetActor())
	{
		// 아이템 베이스인지 확인
		AFZFItemBase* ItemActor = Cast<AFZFItemBase>(Hit.GetActor());
		if (ItemActor)
		{
			UFZFInventoryComponent* Inventory = Player->GetInventoryComponent();
			if (Inventory)
			{
				// 아이템 데이터 획득 및 인벤토리 추가
				if (Inventory->AddItem(ItemActor->GetItemData()))
				{
					// 성공 시 액터 파괴
					ItemActor->Destroy();
				}
			}
		}
	}
}
