// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Enemy/Animation/OniAnimInstance.h"
#include "Oni.generated.h"

/**
 * 
 */
class ATanjiro;
class APickupItem;
// class UOniAnimInstance;
class UAnimationControlComponent;
class UEnemyRegistrySubsystem;


UCLASS()
class KY_API AOni : public ACharacterBase
{
	GENERATED_BODY()
AOni();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* SurpriseAttack;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* LeftAttack2;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* RightAttack2;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Kick;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* LeftComboCounter;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* RightComboCounter;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) TSubclassOf<APickupItem> DeathItemActor;

	UPROPERTY(EditAnywhere, Category=Setting)
	TSubclassOf<AActor> FX_SurpriseAttackActor;
	
	//virtual void GetCounter(EAttackType _type, const FTransform& _socketPoint) override;
	virtual void GetCounter(const FTransform& _socketPoint) override;
	
	void AttackMontage();
	virtual void CheckCurrentHP() override;
	virtual void Death() override;
	
	
	// BlueprintTask
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FVector GetTanjiroLastLocation();
	
	UFUNCTION(BlueprintCallable)
	void OniSurpriseAttack();
	FVector SurpriseHide();
	
	UFUNCTION(BlueprintCallable)
	void TaskComboAttack();
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool GetOniCanAttack(){ return bCanAttack; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTanjiroActor();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDistance();
	
	UFUNCTION(BlueprintCallable)
	void AN_ComboAttackEnd();
	UFUNCTION(BlueprintCallable)
	void AN_ComboAttackPoint();
	UFUNCTION(BlueprintCallable)
	void AN_CounterEnd();

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FVector GetAttackDir(){return AttackDir;}
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AttackDirMove(float amount);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool GetOniTreeActive(){ return bIsTreeActive; }

	void ChangePPOption(bool _enable);

	void DestroyOni();

	void RegisterEnemy();

	void OniAttack();
	
	void SetAnimType(EAnimType _type);

	void SetAttackDir();
	// EnemyAttakState에서 할거
	void OnRequestAttack(); // 이 객체 공격 활성화
	void OnReleaseAttack(); // 죽으면서 객체 공격권 넘기기
private:
	UEnemyRegistrySubsystem* RS;
	//ATanjiro* Tanjiro;
	AActor* Target;
	 bool bIsTreeActive= false;

	// Attack
	FVector AttackDir;
	bool bCanAttack = false;
	int32 ComboAttackCount = 0;
	FTimerHandle ComboAttackHandle;
	//SurpriseAttack
	FRotator SurpriseAttackRot;
	FVector SurpriseAttackLoc;
	
	UOniAnimInstance* AnimInstance;
	
	FTimerHandle AttackTimerHandle;
	bool bAttackLeft = false;
};
