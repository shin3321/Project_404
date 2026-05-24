// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FZFInteractableInterface.generated.h"

class AFZFCharacterPlayer;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFZFInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT_404_API IFZFInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void Interact(AFZFCharacterPlayer* Interactor, UPrimitiveComponent* HitComponent) = 0;

	virtual FText GetInteractableName(UPrimitiveComponent* HitComponent) const = 0;
};
