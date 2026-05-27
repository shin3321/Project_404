#include "Inventory/FZFHUD.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Game/FZFGameState.h"

void UFZFHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (StaminaBar)
    {
        StaminaDynamicMaterial = StaminaBar->GetDynamicMaterial();
    }

    // 홀드 진행률 이미지에 들어있는 Brush Material을 Dynamic Material로 변환
    if (HoldProgressImage)
    {
        HoldProgressDynamicMaterial = HoldProgressImage->GetDynamicMaterial();

        if (HoldProgressDynamicMaterial)
        {
            // 처음에는 0%로 시작
            HoldProgressDynamicMaterial->SetScalarParameterValue(TEXT("Progress"), 0.0f);
        }

        // 시작 시 홀드 UI 숨김
        HoldProgressImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UFZFHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // GameState에서 남은 시간을 가져와 타이머 텍스트 업데이트
    if (TimerText)
    {
        // 특정 레벨(FZFGameLevel)에서만 타이머 표시
        if (GetWorld()->GetMapName().Contains(TEXT("FZFGameLevel")))
        {
            TimerText->SetVisibility(ESlateVisibility::Visible);

            if (AFZFGameState* GS = GetWorld()->GetGameState<AFZFGameState>())
            {
                int32 TotalSeconds = GS->RemainingTimeSeconds;
                int32 Minutes = TotalSeconds / 60;
                int32 Seconds = TotalSeconds % 60;

                FString TimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
                TimerText->SetText(FText::FromString(TimerString));
            }
        }
        else
        {
            // 그 외 레벨에서는 타이머 숨김
            TimerText->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

// 아이템 이름 텍스트 설정
void UFZFHUD::SetTargetName(const FText& InTargetName)
{
    if (NameText)
    {
        NameText->SetText(InTargetName);
    }
}

// 아이템 이름 텍스트 표시
void UFZFHUD::ShowWidget()
{
    if (NameText)
    {
        NameText->SetVisibility(ESlateVisibility::Visible);
    }
}

// 아이템 이름 텍스트 숨김
void UFZFHUD::HideWidget()
{
    if (NameText)
    {
        NameText->SetVisibility(ESlateVisibility::Hidden);
    }
}

// 조준점을 기본 상태로 변경
void UFZFHUD::SetCrosshairNormal()
{
    if (CrosshairImage)
    {
        // 평소에는 조금 연하게 보이도록 설정
        CrosshairImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.1f));
    }
}

// 조준점을 강조 상태로 변경
void UFZFHUD::SetCrosshairHighlight()
{
    if (CrosshairImage)
    {
        // 상호작용 가능한 액터를 바라보면 더 진하게 표시
        CrosshairImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
    }
}

void UFZFHUD::PlayDamageEffect()
{
    // DamageFade 애니메이션이 존재하면 실행
    if (DamageFade)
    {
        PlayAnimation(DamageFade);
    }
}

void UFZFHUD::UpdateStaminaBar(float NewValue, float MaxValue)
{
    if (StaminaDynamicMaterial && MaxValue > 0.0f)
    {
        const float Percent = NewValue / MaxValue;
        StaminaDynamicMaterial->SetScalarParameterValue(TEXT("Progress"), Percent);
    }
}

void UFZFHUD::UpdateHpText(float NewValue, float MaxValue)
{
    if (HPText && MaxValue > 0.0f)
    {
        // 현재 체력과 최대 체력 비율 계산 후 100을 곱해 % 수치로 만듦
        const float Percent = (NewValue / MaxValue) * 100.0f;

        // FString::Printf를 이용해 정수형태(또는 %.1f로 소수점 한자리) 포맷을 만듦
        // %는 C++ 포맷팅 규칙 상 %% 로 적어야 실제 문자열에 '%' 하나가 출력
        FString HpPercentString = FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Percent));

        // 텍스트 컴포넌트에 적용 (FString을 FText로 변환하여 세팅)
        HPText->SetText(FText::FromString(HpPercentString));
    }
}

// 홀드 UI 표시
void UFZFHUD::ShowHoldProgress()
{
    if (HoldProgressImage)
    {
        HoldProgressImage->SetVisibility(ESlateVisibility::Visible);
    }
}

// 홀드 UI 숨김
void UFZFHUD::HideHoldProgress()
{
    if (HoldProgressImage)
    {
        HoldProgressImage->SetVisibility(ESlateVisibility::Hidden);
    }

    // 숨길 때 진행률도 초기화
    UpdateHoldProgress(0.0f);
}

// 홀드 진행률 갱신
void UFZFHUD::UpdateHoldProgress(float InProgress)
{
    if (HoldProgressDynamicMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUD UpdateHoldProgress = %f"), InProgress);
        HoldProgressDynamicMaterial->SetScalarParameterValue(
            TEXT("Progress"),
            FMath::Clamp(InProgress, 0.0f, 1.0f)
        );
    }
}
