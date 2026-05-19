// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "FZFGameLevelTeleport.generated.h"

UCLASS()
class PROJECT_404_API AFZFGameLevelTeleport : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFZFGameLevelTeleport();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TransferVolume;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnTeleportKeyPressed();
	
	UFUNCTION()
	void EnterLobbyLevel();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> TeleportWidgetClass;

	UPROPERTY()
	UUserWidget* TeleportWidgetInstance;

public:
	// 로딩 화면을 띄운 뒤 지정한 레벨로 이동하는 공통 함수
	void ShowLoadingAndTravel(const FString& LevelPath);

protected:

	// 로딩 화면 위젯 클래스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	// 생성된 로딩 화면 위젯 인스턴스
	UPROPERTY()
	UUserWidget* LoadingWidgetInstance;

	// 로딩 화면 대기 타이머
	FTimerHandle LevelTravelTimerHandle;

	// 타이머 후 이동할 레벨 경로 저장용
	FString PendingLevelPath;

	// 실제 레벨 이동 실행 함수
	UFUNCTION()
	void TravelToLobbyLevel();
};
