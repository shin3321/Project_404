#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FZFHUD.generated.h"

class UTextBlock;
class UImage;

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

    // 조준점 기본 상태로 변경
    UFUNCTION(BlueprintCallable)
    void SetCrosshairNormal();

    // 조준점 강조 상태로 변경
    UFUNCTION(BlueprintCallable)
    void SetCrosshairHighlight();

protected:
    // 아이템 이름 표시용 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;

    // 화면 중앙 조준점 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* CrosshairImage;
};