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

protected:
    virtual void NativeConstruct() override;

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

    // AttributeSet 이벤트가 발생했을 때, 호출할 함수들
    // 스테미나 머터리얼 바 업데이트 함수
    UFUNCTION()
    void UpdateStaminaBar(float NewValue, float MaxValue);

    // HP 텍스트 % 업데이트 함수
    UFUNCTION()
    void UpdateHpText(float NewValue, float MaxValue);

    // 홀드 UI 표시
    UFUNCTION(BlueprintCallable)
    void ShowHoldProgress();

    // 홀드 UI 숨김
    UFUNCTION(BlueprintCallable)
    void HideHoldProgress();

    // 홀드 진행률 갱신
    UFUNCTION(BlueprintCallable)
    void UpdateHoldProgress(float InProgress);


protected:
    // 이름 표시용 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NameText;

    // 스테미나바 이미지
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> StaminaBar;

    // HP바 텍스트 블록
    UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

    // 화면 중앙 조준점 이미지
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CrosshairImage;

    // UMG에서 만든 DamageFade 애니메이션을 C++에서 참조
    // 이름이 UMG 애니메이션 이름과 정확히 같아야 함
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    TObjectPtr<UWidgetAnimation> DamageFade;

    // 홀드 진행률 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* HoldProgressImage;

    // 홀드 진행률용 머티리얼
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hold")
    TObjectPtr<UMaterialInterface> HoldProgressMaterial;

    // 홀드 진행률 Dynamic Material
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> HoldProgressDynamicMaterial;

private:
    // 생성된 다이내믹 머티리얼 인스턴스를 기억해둘 변수
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> StaminaDynamicMaterial;


};