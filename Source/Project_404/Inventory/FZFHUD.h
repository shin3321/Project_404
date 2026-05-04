#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FZFHUD.generated.h"

class UTextBlock;

UCLASS()
class PROJECT_404_API UFZFHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // 아이템 이름 텍스트 설정
    UFUNCTION(BlueprintCallable)
    void SetItemName(const FText& InItemName);

    // 아이템 이름 텍스트 표시
    UFUNCTION(BlueprintCallable)
    void ShowItemName();

    // 아이템 이름 텍스트 숨김
    UFUNCTION(BlueprintCallable)
    void HideItemName();

protected:
    // 아이템 이름 표시용 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;
};