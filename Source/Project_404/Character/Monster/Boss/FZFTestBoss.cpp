// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/Boss/FZFTestBoss.h"

// Sets default values
AFZFTestBoss::AFZFTestBoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFZFTestBoss::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFZFTestBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFZFTestBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFZFTestBoss::CheckBossPhase()
{
	EBossPhase NextBossPhase = EBossPhase::Phase1;
	if(보스의 약점이 꺠지면)
	NextBossPhase = EBossPhase::Phase2;
	
	if (NextBossPhase != CurrentBossPhase)
	{
		CurrentBossPhase = NextBossPhase;
		OnBossPhaseChanged.Broadcast(CurrentBossPhase);
	}
}

