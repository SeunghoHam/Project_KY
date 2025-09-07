// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "GameFramework/Character.h"



#include "Tanjiro.generated.h"

class UAnimationControlComponent;
class AOni;
class ATanjiroController;
class UCameraComponent;
class UCameraShakeBase;
class UInventoryComponent;
struct FInputActionValue;
class UInputAction;
class UInGameWidget;

class UTanjiroAnimMontageMap;
class UInputDataAsset;
class UItemDB;

class UParticleSystem;
class UParticleSystemComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UMaterialInstanceDynamic;

UENUM()
enum class EAnimState : uint8
{
	Parring,
	Idle
};
UCLASS()	
class KY_API ATanjiro : public ACharacterBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Charging;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* UnderSlash;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* TurnSlash;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* HitMontage;
	
	//UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Parring_Start;
	//UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Parring_Ing;
	//UPROPERTY(EditAnywhere,BlueprintReadOnly, Category=Setting) UAnimMontage* Parring_End;

	// Sets default values for this character's properties
	ATanjiro();
//	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;
	UInputAction* LookAction;
	UInputAction* JumpAction;
	UInputAction* ShiftAction;
	UInputAction* LMBAction;
	UInputAction* RMBAction;
	UInputAction* EAction;
	UInputAction* QAction;
	UInputAction* TabAction;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	/** First person camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* KatanaMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	TSubclassOf<AActor> BloodEffectActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	TSubclassOf<AActor> MoveAttackRoadEffectActor;
	
	UPROPERTY(EditDefaultsOnly, Category=Setting)
	UParticleSystem* FX_ParringAsset; // P_Sparks_F
	UParticleSystemComponent* FX_Parring;

	UPROPERTY(EditDefaultsOnly, Category=Setting)
	UNiagaraSystem* FX_TargetSelect;

	UPROPERTY(EditDefaultsOnly, Category=Setting)
	UNiagaraSystem* FX_TargetSelectBottom;
	
	//UPROPERTY(EditDefaultsOnly, Category=Setting)
	//UNiagaraComponent* FX_TargetSelect;
	
	UPROPERTY(EditDefaultsOnly, Category=Setting)
	UParticleSystem* SmokeFXClass;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	UParticleSystemComponent* SmokeFX;

	
	// 파티클 에셋을 에디터에서 지정할 수 있도록
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	TSubclassOf<UCameraShakeBase> DefaultCameraShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	TSubclassOf<UCameraShakeBase> ParringCameraShake;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category=Setting)
	UInputDataAsset* InputDataAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category=Setting)
	TObjectPtr<UItemDB> ItemDB;
	//UItemDB* ItemDB;

	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category=Setting)
	TSubclassOf<class UInGameWidget> InGameWidgetClass;
	class UInGameWidget* InGameWidget;

	//class APostProcessVolume* PPVolume;
	UPROPERTY()
	//TArray<TPair<UMaterialInstanceDynamic, float>> CachedPPMIDs;
	UMaterialInstanceDynamic* CachedPPMid;

	UPROPERTY(EditAnywhere,Category=Setting)
	UMaterialInterface* WhiteBlackMat;
	
	void CachedPPMaterialSetting(float value);
	APostProcessVolume* GetLevelProcessVolue();
	// 바인딩
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void OnShitPressed();
	void OnShiftReleased();
	void InputJumpPressed();
	void InputJumpReleased();
	void Input_LMB_Pressed();
	void Input_LMB_Release();
	void Input_RMB_Press();
	void Input_RMB_Release();
	void Input_Q_Press();
	void Input_Q_Release();
	void Input_E_Press();
	void Input_E_Release();
	void Input_Tab_Pressed();
	//void Input_E_Release();
	
	//void SetJumpHeightMeters(float Meters);

	//

	// BlueprintCall
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ParringCamera(bool _Enable);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ParringCameraMove(bool _Enable);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AttackMove(FVector originloc, FRotator originrot,
		FVector targetloc, FRotator targetrot);
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MoveAttack();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_AvoidMove();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ParringBackMove();

	
	virtual void CheckCurrentHP() override;
	
	
	// AnimNotify
	UFUNCTION(BlueprintCallable)
	void AN_AttackPoint();
	UFUNCTION(BlueprintCallable)
	void AN_FreezeAtEnd();
	void UnFreeze();


	//FX
	void PlayParringEffect();
	static void SpawnImpactFX(UObject* WorldContext, UNiagaraSystem* System,
						  const FVector& ImpactPoint, const FVector& Dir, const FVector& Scale = FVector(1.0f));//ImpactNormal);
	
	//virtual AActor* AttackPoint(FVector _halfSize, float _end) override;
	void JumpAttack();
	void MoveAttackTargetCheck();
	void MoveAttackSlow();
	void MoveAttack();
	void MoveAttackSubEffect();
	void MoveAttack2();

	void ParringMotion(bool _value);

	//UFUNCTION(BlueprintCallable)
	//void SetGameSpeed(float _speed);
	
	void localPlayMontage(UAnimMontage* montage);
	AActor* FindNearestEnemy(float _dist);
	AOni* LineTraceFront() const;
	
	AOni* TraceSphere(FVector _point) const;
	virtual void SuccessCounter() override;

	void TanjiroParring();//EAttackType _enemyAttackType);
	
	// 카타나의 소켓 위치 가져오기
	FTransform GetKatanaSocket() const;

	void SpawnBoodActor();
	void SpawnMoveAttackFireRoad(const FVector& _enemyLocation);
	
	void SetAnimState(EAnimState _state) { AnimState = _state;}
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EAnimState AnimState = EAnimState::Idle;


	
	// FOV
	UPROPERTY(EditAnywhere,BlueprintReadOnly) float NormalFov= 100.0f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly) float ParringFov = 110.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly) float FocusFov = 60.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly) FVector CameraStart = FVector(5.5f,12.f, 0.f);
	UPROPERTY(EditAnywhere,BlueprintReadOnly) FVector CameraLeftMove = FVector(5.5f,12.f, -10.f);
	UPROPERTY(EditAnywhere,BlueprintReadOnly) FVector CameraDownMove = FVector(-60.5f,40.f, 0.f);

	UFUNCTION(BlueprintCallable, BlueprintPure) FVector GetMoveAttackTargetPoint() {return MoveAttack_End;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetTanjiroIsAvoiding(){return bIsAvoiding;}
	UFUNCTION(BlueprintCallable)
	void SetTanjiroAvoide(bool _enable){bIsAvoiding = _enable;}

	// Parring
	void EndParringEffect();


	// CurrentOniState
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOniStateChange, FText, bool)
	FOnOniStateChange OnOniStateChange;

private:
	ATanjiroController* TanjiroController;
	AOni* CurrentTargetOni;

	// PlayEffect
	FTimerHandle ParringEffectTimerHandle;
	// Avoid
	bool bisShiftPressed = false;
	bool bIsAvoiding =false;
	
	// Parring
	bool bIsParringEnable;

	// MoveAttack
	float Ease01(float Alpha, float Exp) const;
	// Aim Setting
	bool bIsAimSetting= false;
	float  FocusingTime = 0.1f;
	float AimTime=0.f;

	

 	bool bIsMoveAttacking = false; // Q + LMB
	float ElapsedTime= 0.f;
	float MoveAttackTime = 0.2f;
	FVector MoveAttack_Start;
	FVector MoveAttack_End;
	FTimerHandle MoveAttackTimerHandle;
	FTimerHandle MoveAttackEffectTimerHandle;
	FTimerHandle FreezeTimerHandle;
	// JumpAttacking
	bool bIsJumpAttacking = false;
	float JumpAttackTime=0.0f;
	FVector JumpAttack_Start;
	FVector JumpAttack_End;
	FRotator Turn_Start;
	FRotator Turn_End;


	// Socket
	//FVector SocketLoc;
	
	//FRotator SocketRot;
	//FTransform SocketTransform;

	
	bool bJumpPressed = false;
	float MinJumpVelocity=  1000.f;
	/*
	float JumpTimer=0.0f;
	float JumpAttackDivide =1.f;
	float CurrentJumpVelocity = 500.f;
	float MaxJumpVelocity  = 1500.f;
*/
	bool bIsQPressed = false;
	bool bIsEPressed = false;


	int32 WidgetActiveNumber = 0;
public:
	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();
	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);
	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);
};
