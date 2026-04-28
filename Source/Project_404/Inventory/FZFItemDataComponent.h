#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/FZFItemData.h"
#include "FZFItemDataComponent.generated.h"

// 월드 아이템 액터의 아이템 데이터를 보관하는 컴포넌트 클래스
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECT_404_API UFZFItemDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 아이템 데이터 컴포넌트 생성자
	UFZFItemDataComponent();

public:
	// 아이템 ID와 아이콘 정보를 담는 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UFZFItemData> ItemData;
};