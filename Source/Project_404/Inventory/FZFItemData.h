#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "FZFItemData.generated.h"

//아이템 데이터 구조체

USTRUCT(BlueprintType)
struct PROJECT_404_API FFZFItemData
{


    GENERATED_BODY()

public:
    // 아이템 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ID = 0;

    // 아이템 아이콘 텍스처
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

};

