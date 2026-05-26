// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameResultWidget.h"

void UGameResultWidget::UpdateGameResult(bool bIsClear)
{
	if (GameResult)
	{
		FText ResultText = bIsClear ? FText::FromString(TEXT("Clear")) : FText::FromString(TEXT("Fail"));
		GameResult->SetText(ResultText);
	}
}
