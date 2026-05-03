// Fill out your copyright notice in the Description page of Project Settings.


#include "FZFTimerWidget.h"
#include "Game/FZFGameState.h"

void UFZFTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AFZFGameState* GameState = GetWorld()->GetGameState<AFZFGameState>())
	{
		if (TimerText)
		{
			TimerText->SetText(FormatTime(GameState->RemainingRimeSeconds));
		}
	}
}
