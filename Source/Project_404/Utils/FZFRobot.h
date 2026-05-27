// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FZFRobot.generated.h"

UCLASS()
class PROJECT_404_API AFZFRobot : public AActor
{
	GENERATED_BODY()
	
public:	
	AFZFRobot();

protected:
	virtual void BeginPlay() override;

};
