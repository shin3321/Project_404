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

	void SetCurrentIdleAnim(UAnimSequence* IdleAnim) { CurrentIdleAnim = IdleAnim; }

	void SetUpperBodyBlendWeight(float NewWeight);

protected:
	// 애니메이션 초기화 될 때 호출되는 함수.
	virtual void NativeInitializeAnimation() override;

	// 애니메이션을 업데이트 할 때마다(틱 마다) 실행되는 함수.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:

	// 상, 하체 분리 애니메이션에서 상체에 해당하는 Weight값. 
	// 0이면 기본 애니메이션 재생.
	// 1이면 무기 타입에 대한 상체 애니메이션 재생.
	UPROPERTY(BlueprintReadOnly, Category = "LayeredBlend")
	float UpperBodyBlendWeight = 0.0f;

protected:
	// 걷는지 뛰는지 확인하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsRunning : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimSequence> CurrentIdleAnim;
};
