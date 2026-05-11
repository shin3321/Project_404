#include "Inventory/FZFHUD.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

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