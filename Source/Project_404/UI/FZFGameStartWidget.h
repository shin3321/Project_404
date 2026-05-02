// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FZFGameStartWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_404_API UFZFGameStartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (WBP_FZFGameStart))
	class UButton* HostStartButton;

	UPROPERTY(meta = (WBP_FZFGameStart))
	class UButton* GuestStartButton;

	UPROPERTY(meta = (WBP_FZFGameStart))
	class UEditableText* IPAddressETB;

	UFUNCTION()
	void OnHostStartClicked();

	UFUNCTION()
	void OnGuestStartClicked();

	// 로컬 IP 주소를 가져오는 함수
	FString GetLocalIPAddress();
};
