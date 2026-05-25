// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Boss/FZFLaserActor.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/Player/FZFCharacterPlayer.h"

// Sets default values
AFZFLaserActor::AFZFLaserActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	CurrentMode = ELaserMode::Inactive;
	
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = RootComp;
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComp->SetGenerateOverlapEvents(true);

	//
	CollisionComp->SetBoxExtent(FVector(500.f, 10.f, 10.f));
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	LaserEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserEffectComp"));
	LaserEffectComp->SetupAttachment(CollisionComp); // 콜리전을 따라다니도록 설정
	LaserEffectComp->bAutoActivate = false;

	MoveDirection = FVector(1.0f, 1.0f, 1.0f);

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AFZFLaserActor::OnComponentBeginOverlap);
}

// Called when the game starts or when spawned
void AFZFLaserActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFZFLaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentMode == ELaserMode::Inactive)
		return;
	FVector NewLocation = GetActorLocation();

	if (CurrentType == ELaserType::Horizon && CurrentMode == ELaserMode::Moving)
	{
		NewLocation.Z += Speed * MoveDirection.Z * DeltaTime;
		if (NewLocation.Z < MinZ || NewLocation.Z > MaxZ)
		{
			MoveDirection.Z *= -1.0;
		}
	}
	if (CurrentType == ELaserType::Vertical)
	{
		NewLocation.X += Speed * MoveDirection.X * DeltaTime;
		if (NewLocation.X < 0.0f || NewLocation.X > MaxX)
			MoveDirection.X *= -1.0;
	}

	NewLocation.Y += Speed * MoveDirection.Y * DeltaTime;
	SetActorLocation(NewLocation);

	if (NewLocation.Y < MinY || NewLocation.Y > MaxY)
	{
		DeactivateLaser();
	}
}

void AFZFLaserActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AFZFLaserActor, CurrentMode);
	DOREPLIFETIME(AFZFLaserActor, CurrentType);
	DOREPLIFETIME(AFZFLaserActor, Speed);
	DOREPLIFETIME(AFZFLaserActor, MoveDirection);
}

void AFZFLaserActor::ActivateLaser(FVector StartLocation, ELaserMode Mode, ELaserType Type, float MoveSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("[Laser] Activate / Loc=%s Mode=%d Type=%d Speed=%f"),
		*StartLocation.ToString(),
		(int32)Mode,
		(int32)Type,
		MoveSpeed);

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorLocation(StartLocation);
	CurrentMode = Mode;
	Speed = MoveSpeed;
	CurrentType = Type;

	if (CurrentType == ELaserType::Horizon)
	{
		// 가로 방향 설정 로직
	}
	else if (CurrentType == ELaserType::Vertical)
	{
		// 세로 방향 설정 로직 (예: Pitch 90도 회전)
		SetActorRotation(FRotator(90.f, 0.f, 0.f));
	}

	// 콜리전 및 이펙트 활성화
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LaserEffectComp->Activate(true);
	LaserEffectComp->ReinitializeSystem();
	LaserEffectComp->SetVisibility(true);
}

void AFZFLaserActor::DeactivateLaser()
{
	OnLaserDeactive.Broadcast(this);
	CurrentMode = ELaserMode::Inactive;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserEffectComp->Deactivate();
	LaserEffectComp->SetVisibility(false);
}

ELaserMode AFZFLaserActor::GetLaserMode()
{
	return CurrentMode;
}

void AFZFLaserActor::OnRep_CurrentMode()
{
	if (CurrentMode == ELaserMode::Inactive)
	{
		// Deactivate 시각 로직 실행 (이펙트 끄기 등)
		LaserEffectComp->Deactivate();
		SetActorHiddenInGame(true);
	}
	else
	{
		// Activate 시각 로직 실행 (이펙트 켜기 등)
		LaserEffectComp->Activate();
		SetActorHiddenInGame(false);
	}
}

void AFZFLaserActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	// 1. 서버에서만 실행
	if (!HasAuthority()) return;
	if (!DamageGEClass) return;

	if (OtherActor && OtherActor->IsA(AFZFCharacterPlayer::StaticClass()))
	{
		// 2. 상대방이 ASC를 가지고 있는지 확인 (IAbilitySystemInterface 구현여부)
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
		if (ASI)
		{
			UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
			if (TargetASC && DamageGEClass)
			{
				// 3. 이펙트 컨텍스트 생성 (가해자 정보 설정)
				FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();

				// GetInsigator() 가 Null이면 나 자신(this, 레이저)을 가해자로 설정
				AActor* MyInstigator = GetInstigator() ? Cast<AActor>(GetInstigator()) : this;
				Context.AddInstigator(MyInstigator, this);

				// 4. 이펙트 스펙 생성 및 적용
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageGEClass, 1.0f, Context);
				if (SpecHandle.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}
}
