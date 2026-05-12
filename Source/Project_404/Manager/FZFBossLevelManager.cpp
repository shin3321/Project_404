// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/FZFBossLevelManager.h"
#include  "Character/Monster/Boss/FZFTestBoss.h"
#include  "Kismet/GameplayStatics.h"

// Sets default values
AFZFBossLevelManager::AFZFBossLevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFZFBossLevelManager::BeginPlay()
{
	Super::BeginPlay();
	AFZFTestBoss* Boss = Cast<AFZFTestBoss>(UGameplayStatics::GetActorOfClass(GetWorld(),AFZFTestBoss::StaticClass()));
	if (Boss != nullptr)
	{
		Boss->OnBossPhaseChanged.AddDynamic(this, &AFZFBossLevelManager::HandlePhaseChanged);
	}
}

// Called every frame
void AFZFBossLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFZFBossLevelManager::HandlePhaseChanged(EBossPhase NewPhase)
{
	switch (NewPhase)
	{
	case EBossPhase::Phase1:
		{
			
			break;
		}
		
	case EBossPhase::Phase2:
		{
			
			break;
		}
	case EBossPhase::Phase3:
		{
			
			break;
		}
	}
}

