#include "Character/FZFCharacterBase.h"
#include "GAS/FZFAbilitySystemComponent.h"

AFZFCharacterBase::AFZFCharacterBase()
{
    // 네트워크 설정
    bReplicates = true;
    //SetReplicates(true);
}

UAbilitySystemComponent* AFZFCharacterBase::GetAbilitySystemComponent() const
{
    return ASC;
}

void AFZFCharacterBase::InitAbilitySystem()
{
    // 이 함수 내부에서 InitAbilityActorInfo를 실행하도록 로직 구현
    // Beginplay()에서 함수 호출
}

