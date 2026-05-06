#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FZFPlayerController.generated.h"

// TODO
// 1. 상호 작용 로직
// 2. 입력 상태 제어
// 3. RPC - Remote Procedure Call
// 4. UI 및 시각 효과 관리 (HUD)

UCLASS()
class PROJECT_404_API AFZFPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AFZFPlayerController();

	// 변수들을 네트워크를 통해 동기화하기 위해 반드시 오버라이드 해야 하는 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/* ==========================================
	   [서버 RPC (클라이언트 -> 서버 요청)]
	   ========================================== */
	// 위젯의 구매 버튼을 누르면 클라이언트가 이 함수를 호출합니다.
	// Reliable: 패킷 유실을 막아 반드시 도착하게 합니다.
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RequestPurchase(class AFZFStore* TargetStore, FName ItemId, float ItemCost);

protected:
	// 실제로 서버로 넘어가서 실행될 RPC 함수
	// Reliable: 반드시 도착하도록 보장 (결제 같은 중요 로직에 필수)
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestPurchase(class AFZFStore* TargetStore, FName ItemId, float ItemCost);
};
