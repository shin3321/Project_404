#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Animation/WidgetAnimation.h"

#include "FZFHUD.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class PROJECT_404_API UFZFHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    // 타겟 이름 텍스트 설정
    UFUNCTION(BlueprintCallable)
    void SetTargetName(const FText& InTargetName);

    // 타겟 이름 텍스트 표시
    UFUNCTION(BlueprintCallable)
    void ShowWidget();

    // 타겟 이름 텍스트 숨김
    UFUNCTION(BlueprintCallable)
    void HideWidget();

    // 조준점 기본 상태로 변경
    UFUNCTION(BlueprintCallable)
    void SetCrosshairNormal();

    // 조준점 강조 상태로 변경
    UFUNCTION(BlueprintCallable)
    void SetCrosshairHighlight();

    // 피격 HUD 효과를 실행하는 함수
    UFUNCTION(BlueprintCallable)
    void PlayDamageEffect();
    //데미지 받을 때
    //HUDWidget->PlayDamageEffect(); 호출 하면됨

protected:
    //이름 표시용 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NameText;

    // 화면 중앙 조준점 이미지
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CrosshairImage;

    // UMG에서 만든 DamageFade 애니메이션을 C++에서 참조
    // 이름이 UMG 애니메이션 이름과 정확히 같아야 함
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    TObjectPtr<UWidgetAnimation> DamageFade;


};