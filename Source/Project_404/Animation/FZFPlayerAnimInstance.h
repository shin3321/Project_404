// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FZFAnimInstance.h"
#include "FZFPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_404_API UFZFPlayerAnimInstance : public UFZFAnimInstance
{
	GENERATED_BODY()

public:
	UFZFPlayerAnimInstance();

protected:
	// 애니메이션 초기화 될 때 호출되는 함수.
	virtual void NativeInitializeAnimation() override;

	// 애니메이션을 업데이트 할 때마다(틱 마다) 실행되는 함수.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
