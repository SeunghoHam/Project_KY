// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OniAnimInstance.generated.h"

/**
 * 
 */

UENUM()
enum class EAnimType : uint8
{
	Idle,
	Die,
	Punch1,
	Punch2,
	Kick,
};
UCLASS()
class KY_API UOniAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float IK_L_Alpha = 0.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float IK_R_Alpha = 0.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector EffectorLocation; // 월드상에 손이 위치할 공간
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector JointTargetLocation; // 월드상에 팔꿈치 위치할 공간(캐릭터랑 Effector위치 기반으로 중간정도?
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FTransform EffectorTransform;


	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EAnimType AnimType = EAnimType::Idle;
};
