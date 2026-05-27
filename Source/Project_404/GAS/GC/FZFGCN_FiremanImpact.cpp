// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GC/FZFGCN_FiremanImpact.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"

AFZFGCN_FiremanImpact::AFZFGCN_FiremanImpact()
{
    PrimaryActorTick.bCanEverTick = false;

    // 캐스케이드 파티클 컴포넌트 생성 및 루트 설정
    FireFXComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireFXComponent"));
    SetRootComponent(FireFXComponent);

    // GameplayCue 관리 옵션 (큐가 해제되면 이 액터 자동 파괴)
    bAutoDestroyOnRemove = true;
}

bool AFZFGCN_FiremanImpact::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    Super::OnActive_Implementation(MyTarget, Parameters);

    if (MyTarget)
    {
        // 플레이어 액터에 이펙트 액터 부착
        FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

        // 플레이어 캐릭터의 메쉬 컴포넌트나 루트에 부착합니다. 
        AttachToActor(MyTarget, AttachmentRules, FName("Mesh"));

        // 캐스케이드 파티클 활성화
        if (FireFXComponent)
        {
            FireFXComponent->Activate(true);
        }
    }

    return true;
}

bool AFZFGCN_FiremanImpact::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    // 디스포즈되기 전 파티클을 안전하게 끕니다.
    if (FireFXComponent)
    {
        FireFXComponent->Deactivate();
    }

    Super::OnRemove_Implementation(MyTarget, Parameters);
    return true;
}
