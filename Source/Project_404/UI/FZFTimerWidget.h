// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "FZFTimerWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_404_API UFZFTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION(BlueprintPure, Category = "UI")FText FormatTime(int32 TotalSeconds) const
	{
		int32 Min = TotalSeconds / 60;
		int32 Sec = TotalSeconds % 60;
		return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Min, Sec));
	}

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimerText;
};
