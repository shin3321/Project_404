// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Boss/FZFBossBombActor.h"
#include "Components/BoxComponent.h"
#include "Character/Monster/Boss/FZFTestBoss.h"
#include "Character/Player/FZFCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AFZFBossBombActor::AFZFBossBombActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));

	BombBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
    
	BombBodyMesh->SetupAttachment(RootComponent);
	BombBodyMesh->SetCollisionProfileName(TEXT("NoCollision"));
    
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Assets/Crafting/EquipmentWorkbench/Meshes/Roshni_Sounder"));
	if (MeshAsset.Succeeded())
	{
		BombBodyMesh->SetStaticMesh(MeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void AFZFBossBombActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AFZFBossBombActor::OnComponentBeginOverlap);

	if (ExplosionFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
	}
}

// Called every frame
void AFZFBossBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFZFBossBombActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (OtherActor && OtherActor->IsA(AFZFTestBoss::StaticClass()))
	//{
	//	BossActor = Cast<AFZFTestBoss>(OtherActor);
	//	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AFZFBossBombActor::Explode, 2.0f, false);
	//}

	if (OtherActor && OtherActor->IsA(AFZFCharacterPlayer::StaticClass()))
	{
		AFZFCharacterPlayer* Player = Cast<AFZFCharacterPlayer>(OtherActor);
		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AFZFBossBombActor::Explode, 2.0f, false);
		//Explode();
	}
}

void AFZFBossBombActor::Explode()
{
	if (!HasAuthority()) return;

	// 서버에서 데미지 처리 등의 논리적 작업을 수행
	//if (BossActor)
	//{
	//	//Todo boss 데미지 설정
	//	UGameplayStatics::ApplyDamage(BossActor, 50.0f, GetInstigatorController(), this, UDamageType::StaticClass());
	//}	

	// 클라이언트들에게 이펙트 재생을 지시
	MulticastExplode();
	
	// 폭발물 자신 제거
	Destroy();
}

void AFZFBossBombActor::MulticastExplode_Implementation()
{
	// 이 함수는 서버의 지시를 받아 모든 클라이언트(및 Listen Server)에서 실행됩니다.
	if (ExplosionFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
	}
}

