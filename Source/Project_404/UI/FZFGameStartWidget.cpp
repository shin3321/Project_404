// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FZFGameStartWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"

void UFZFGameStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HostStartButton)
	{
		HostStartButton->OnClicked.AddDynamic(this, &UFZFGameStartWidget::OnHostStartClicked);
	}

	if (GuestStartButton)
	{
		GuestStartButton->OnClicked.AddDynamic(this, &UFZFGameStartWidget::OnGuestStartClicked);
	}
}

void UFZFGameStartWidget::OnHostStartClicked()
{
	FString MyIP = GetLocalIPAddress();
	UE_LOG(LogTemp, Warning, TEXT("Host IP Address: %s"), *MyIP);

	// Todo 로비 레벨 띄우기
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("FZF_LobbyLevel"), true,
		TEXT("listen"));
}

void UFZFGameStartWidget::OnGuestStartClicked()
{
	if (IPAddressETB)
	{
		FString TargetIP = IPAddressETB->GetText().ToString();
		if (!TargetIP.IsEmpty())
		{
			// 입력된 IP로 접속 시도
			UGameplayStatics::OpenLevel(GetWorld(), FName(*TargetIP));
		}
	}
}

FString UFZFGameStartWidget::GetLocalIPAddress()
{
	bool bCanBind = false;
	TSharedPtr<class FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	if (LocalAddr.IsValid())
	{
		return LocalAddr->ToString(false);
	}
	return TEXT("127.0.0.1");
}
