// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DataAsset/TanjiroAnimMontageMap.h"
#include "CharacterBase.generated.h"

//class UAnimationControlComponent;
class USoundComponent;
UENUM()
enum class EAttackType : uint8
{
	Center,
	Right,
	Left,
	LeftEnemyCombo,
	RightEnemyCombo,
	LeftParring,
	RightParring
};
UCLASS()
class KY_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	USoundComponent* SoundComponent;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* LeftAttack;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* RightAttack;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* LeftCounter;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* RightCounter;
	//UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Hit;
	//UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* DeathAnim;

	
	void PlayAnimationMontage(UAnimMontage* montage);

	
	void OpenCounterWindow();
	void CloseCounterWindow();

	virtual AActor* AttackPoint(FVector _dir, FVector _halfSize, float _end);
	AActor* CreateCheckCollision(FVector _dir, FVector _halfSize, float _end);
	bool CounterWindowIsOpen() const { return bIsCanCounter; }
	UAnimMontage* GetCurrentMontage() const { return CurrentPlayMontage; }
	//virtual void TryCounter();
	virtual void SuccessCounter(); // 내가 시도한 공격이 먹힘 -> 몽타주 멈추고  Idle or 자동공격
	virtual void GetCounter(const FTransform& _socketPoint){}; // 카운터 공격을 받음 -> 몽타주 멈추고, 그로기 올림
	FVector Hand_L_Point;
	FVector Hand_R_Point;
	
	virtual void CheckCurrentHP();

	EAttackType GetAttackType() const {return AttackType;}
	void ChangeAttackType(EAttackType _type) { AttackType = _type; }

	void StatusInitialize(float maxHp, float maxMp);

	virtual void Death();
protected:

	// Time Speed
	UFUNCTION(BlueprintCallable)
	void SetGameSpeed(float _speed);
	UAnimMontage* CurrentPlayMontage = nullptr;
	FName TargetTag;
	EAttackType AttackType;

	float MaxHP;
	float CurrentHP;
	float MaxMP;
	float CurrentMP;


	// AnimMontage
	FName key_RightAttack = FName("RightAttack");
	FName key_LeftAttack = FName("LeftAttack");
	FName AN_RightCounter = FName("RightCounter");
	FName AN_LeftCounter  = FName("LeftCounter");
	
private:
	bool bIsCanCounter = false;
	float GetDamage =0.f;
};
