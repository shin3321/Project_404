#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "FZFPlayerState.generated.h"

class UFZFPlayerSet;

UCLASS()
class PROJECT_404_API AFZFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	/**
	 * @param ObjectInitializer
	 * 언리얼 오브젝트 생성 시 초기화 옵션을 제어하는 객체입니다.
	 * 1. 부모 클래스의 기본 컴포넌트 설정을 자식에서 변경(Override)할 때 사용합니다.
	 * 2. 특히 GAS처럼 엔진 내부 프레임워크와 깊게 연동되는 클래스에서 안전한 초기화를 보장하기 위해 사용합니다.
	 * 3. 최신 엔진에서는 생략 가능하나, 확장성과 상속 시 컴포넌트 제어를 위해 관례적으로 명시합니다.
	 */
	AFZFPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// ASC 인터페이스 상속 함수
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	// PlayerSet을 가져오는 함수
	UFZFPlayerSet* GetPlayerSet() const;

protected:
	// 읽기 가능, 수정 불가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// AbilitySystemComponent
	TObjectPtr<class UFZFAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFZFPlayerSet> PlayerSet;
};
