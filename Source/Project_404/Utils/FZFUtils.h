#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

template <typename T>
T* FZFFindComponentByName(AActor* OwnerActor, const FName& ComponentName)
{
	if (OwnerActor == nullptr)
	{
		return nullptr;
	}

	TArray<T*> Components;
	OwnerActor->GetComponents<T>(Components);

	for (T* Component : Components)
	{
		if (Component && Component->GetFName() == ComponentName)
		{
			return Component;
		}
	}

	return nullptr;
}