#include "Inventory/FZFHUD.h"
#include "Components/TextBlock.h"

void UFZFHUD::SetItemName(const FText& InItemName)
{
    if (ItemNameText)
    {
        ItemNameText->SetText(InItemName);
    }
}

void UFZFHUD::ShowItemName()
{
    if (ItemNameText)
    {
        ItemNameText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFZFHUD::HideItemName()
{
    if (ItemNameText)
    {
        ItemNameText->SetVisibility(ESlateVisibility::Hidden);
    }
}