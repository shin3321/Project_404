// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/FZFBarrierSkill.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Components/PrimitiveComponent.h"
#include "GameplayTagContainer.h"

AFZFBarrierSkill::AFZFBarrierSkill()
{
	PrimaryActorTick.bCanEverTick = false;

	BarrierCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BarrierCollision"));
	BarrierCollision->SetupAttachment(Root);
	BarrierCollision->SetSphereRadius(BarrierRadius);
	BarrierCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BarrierCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BarrierCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BarrierCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BarrierCollision->SetGenerateOverlapEvents(true);

	BarrierMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrierMesh"));
	BarrierMesh->SetupAttachment(Root);
}

void AFZFBarrierSkill::BeginPlay()
{
	Super::BeginPlay();

	UpdateBarrierSize();

	if (!IsServer())
	{
		return;
	}

	BarrierCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AFZFBarrierSkill::OnBarrierBeginOverlap
	);

	BarrierCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&AFZFBarrierSkill::OnBarrierEndOverlap
	);

	TArray<AActor*> OverlappingActors;
	BarrierCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		ApplyInvincibleEffect(Actor);
	}
}

void AFZFBarrierSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsServer())
	{
		RemoveAllAppliedEffects();
	}

	Super::EndPlay(EndPlayReason);
}

void AFZFBarrierSkill::UpdateBarrierSize()
{
	if (BarrierCollision)
	{
		BarrierCollision->SetSphereRadius(BarrierRadius);
	}

	if (!BarrierMesh || !BarrierMesh->GetStaticMesh())
	{
		return;
	}

	const FBoxSphereBounds MeshBounds = BarrierMesh->GetStaticMesh()->GetBounds();

	const float MeshRadius = MeshBounds.SphereRadius;

	if (MeshRadius <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const float Scale = BarrierRadius / MeshRadius;

	BarrierMesh->SetRelativeScale3D(FVector(Scale));
}

void AFZFBarrierSkill::OnBarrierBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsServer())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] BeginOverlap: %s / Component: %s"),
		*GetNameSafe(OtherActor),
		*GetNameSafe(OtherComp)
	);


	ApplyInvincibleEffect(OtherActor);
}

void AFZFBarrierSkill::OnBarrierEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsServer())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] EndOverlap: %s"),
		*GetNameSafe(OtherActor)
	);

	RemoveInvincibleEffect(OtherActor);
}

void AFZFBarrierSkill::ApplyInvincibleEffect(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Apply Failed: TargetActor invalid"));
		return;
	}

	if (!InvincibleEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Apply Failed: InvincibleEffectClass is null"));
		return;
	}

	UAbilitySystemComponent* TargetASC = GetASCFromActor(TargetActor);
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Apply Failed: TargetASC is null / Target: %s"),
			*GetNameSafe(TargetActor)
		);
		return;
	}

	if (AppliedEffectHandles.Contains(TargetASC))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Already Applied: %s"),
			*GetNameSafe(TargetActor)
		);
		return;
	}

	UAbilitySystemComponent* InstigatorASC = SourceASC ? SourceASC.Get() : TargetASC;

	FGameplayEffectContextHandle EffectContext = InstigatorASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		InstigatorASC->MakeOutgoingSpec(InvincibleEffectClass, EffectLevel, EffectContext);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Apply Failed: SpecHandle invalid"));
		return;
	}

	FActiveGameplayEffectHandle ActiveHandle =
		InstigatorASC->ApplyGameplayEffectSpecToTarget(
			*SpecHandle.Data.Get(),
			TargetASC
		);

	if (ActiveHandle.IsValid())
	{
		AppliedEffectHandles.Add(TargetASC, ActiveHandle);

		const bool bHasInvincibleTag = TargetASC->HasMatchingGameplayTag(
			FGameplayTag::RequestGameplayTag(TEXT("State.Buff.Invincible"))
		);

		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Invincible Applied to %s / HasTag: %d / Effect: %s"),
			*GetNameSafe(TargetActor),
			bHasInvincibleTag,
			*GetNameSafe(InvincibleEffectClass)
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Apply Failed: ActiveHandle invalid"));
	}
}

void AFZFBarrierSkill::RemoveInvincibleEffect(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = GetASCFromActor(TargetActor);
	if (!TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Remove Failed: TargetASC is null / Target: %s"),
			*GetNameSafe(TargetActor));
		return;
	}

	FActiveGameplayEffectHandle* Handle = AppliedEffectHandles.Find(TargetASC);
	if (!Handle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Remove Failed: No Handle / Target: %s / HasTag: %d"),
			*GetNameSafe(TargetActor),
			TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Buff.Invincible")))
		);
		return;
	}

	TargetASC->RemoveActiveGameplayEffect(*Handle);
	AppliedEffectHandles.Remove(TargetASC);

	const bool bHasInvincibleTag = TargetASC->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(TEXT("State.Buff.Invincible"))
	);

	UE_LOG(LogTemp, Warning, TEXT("[BarrierTest] Invincible Removed from %s / HasTag: %d"),
		*GetNameSafe(TargetActor),
		bHasInvincibleTag
	);
}

void AFZFBarrierSkill::RemoveAllAppliedEffects()
{
	for (TPair<TObjectPtr<UAbilitySystemComponent>, FActiveGameplayEffectHandle>& Pair : AppliedEffectHandles)
	{
		UAbilitySystemComponent* ASC = Pair.Key.Get();
		if (ASC && Pair.Value.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Pair.Value);
		}
	}

	AppliedEffectHandles.Empty();
}