#include "Character/Player/FZFPlayerState.h"
#include "GAS/FZFAbilitySystemComponent.h"
#include "GAS/Attributes/FZFPlayerSet.h"
#include "Net/UnrealNetwork.h"


AFZFPlayerState::AFZFPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ASC = CreateDefaultSubobject<UFZFAbilitySystemComponent>("ASC");

	// ASC 자체가 네트워크를 통해 동기화되도록 켬
	ASC->SetIsReplicated(true);

	// 플레이어 컨트롤 캐릭터에 최적화된 혼합 모드 설정
	// 나 자신: 서버가 모든 데이터를 나에게 동기화
	// 다른 플레이어: 내 화면에 보이는 다른 사람의 데이터는 최소한의 정보만 동기화
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerSet = CreateDefaultSubobject<UFZFPlayerSet>("PlayerSet");

	// (선택) AttributeSet도 여기서 생성
	//AttributeSet = CreateDefaultSubobject<UFZFAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AFZFPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

UFZFPlayerSet* AFZFPlayerState::GetPlayerSet() const
{
	return PlayerSet;
}
