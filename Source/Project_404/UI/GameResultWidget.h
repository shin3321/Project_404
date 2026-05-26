// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* GameResult;
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateGameResult(bool bIsClear);
};
