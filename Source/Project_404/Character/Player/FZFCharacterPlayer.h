#pragma once

#include "CoreMinimal.h"
#include "Character/FZFCharacterBase.h"
#include "FZFCharacterPlayer.generated.h"

UCLASS()
class PROJECT_404_API AFZFCharacterPlayer : public AFZFCharacterBase
{
	GENERATED_BODY()
	
public:
	AFZFCharacterPlayer();

protected:
	// 카메라 컴포넌트 구성
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;


};
