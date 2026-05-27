#include "Utils/Boss/FZFBossIntroTrigger.h"

#include "Components/BoxComponent.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Manager/FZFBossLevelManager.h"

AFZFBossIntroTrigger::AFZFBossIntroTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetGenerateOverlapEvents(true);
}

void AFZFBossIntroTrigger::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(
		this,
		&AFZFBossIntroTrigger::OnOverlapBegin
	);
}

void AFZFBossIntroTrigger::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bTriggered)
	{
		return;
	}

	if (!Cast<AFZFCharacterPlayer>(OtherActor))
	{
		return;
	}

	bTriggered = true;

	if (BossLevelManager)
	{
		BossLevelManager->StartBossIntro();
	}
}