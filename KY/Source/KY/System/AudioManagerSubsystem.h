// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioManagerSubsystem.generated.h"

/**
 * 
 */
// 사운드 식별용 핸들 (여러 소리를 동시에 재생할 때 구분하기 위해 사용)
USTRUCT(BlueprintType)
struct FSoundHandle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGuid Id;

	FSoundHandle() { Id.Invalidate(); }
	explicit FSoundHandle(const FGuid& In) : Id(In) {}
	
	//UFUNCTION(BlueprintPure)
	bool IsValueValid() const { return Id.IsValid(); }
};

UENUM()
enum class ESoundType : uint8
{
	Effect,
	Vocal
};
/** 재생할 사운드 이름(카테고리/클립) */
UENUM(BlueprintType)
enum class ESoundName : uint8
{
	effect_Swing UMETA(DisplayName="Swing1"),
	effect_Hit UMETA(DisplayName="Hit"),
	effect_Avoid UMETA(DisplayName="Avoid"),
	effect_Parring UMETA(DisplayName="Parring"),
	//effect_ZugglingAttack UMETA(DisplayName="Z_Attack"),
	//effect_ZugglingDead UMETA(DisplayName="Z_Dead"),
	//effect_TankDestroy UMETA(DisplayName="TankDestroy"),
	//effect_Shoot UMETA(DisplayName="DefaultShoot"),
	//effect_Shoot UMETA(DisplayName="DefaultShoot"),
	/*
	effect_Slash1      UMETA(DisplayName="Katana01"),
	effect_Slash2      UMETA(DisplayName="katana02"),
	effect_Groggy		UMETA(DisplayName="Groggy"),
	vocal1	UMETA(DisplayName="Uzuha1"),
	*/
};

UCLASS()
class KY_API UAudioManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& C) override;
	//virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	/** 사운드 로드/등록 */
	void RegisterSound(ESoundName Name, const TCHAR* Path);
	// 경로 빌더(디렉터리 + 에셋명 → “/Game/Dir/Name.Name”)
	static FString MakeAssetRef(const FString& Dir, const ESoundType& Type, const FString& AssetName);

	FSoundHandle Play(ESoundName Name, AActor* Owner, USceneComponent* AttachTo = nullptr, FName Socket = NAME_None, float Volume = 1.f, float Pitch = 1.f, bool bAutoDestroy = true);
	FSoundHandle Play2D(ESoundName Name, float Volume = 1.f, float Pitch = 1.f, bool bAutoDestroy = true);

	FSoundHandle PlayAtLocation(ESoundName Name, const FVector& Location, float Volume = 1.f, float Pitch = 1.f);

	/** 2D(Non-spatialized) 재생 */

	/** 여러 후보 중 랜덤 재생(직전 반복 회피) */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	FSoundHandle PlayRandom(const TArray<ESoundName>& Candidates,
		AActor* Owner,
		USceneComponent* AttachTo = nullptr,
		FName SocketName = NAME_None,
		float Volume = 1.f,
		FVector2D PitchRange = FVector2D(0.95f, 1.05f),
		bool bAutoDestroy = true);
	/** 핸들로 정지 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopByHandle(FSoundHandle Handle, float FadeOutTime = 0.f);

	/** 이름으로 재생 중인 모든 소리 정지 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopByName(ESoundName Name, float FadeOutTime = 0.f);

	/** 전부 정지 */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopAll(float FadeOutTime = 0.f);


	/** 기본 감쇠/동시성 옵션 (원하면 에디터에서 지정) */
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundAttenuation* DefaultAttenuation = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundConcurrency* DefaultConcurrency = nullptr;

private:

	/** 이름 → 사운드 */
	// CDO(Class Default Object)에서 한번만 돌아가도록 - 생성자에서 한번만 실행되게
	UPROPERTY()
	TMap<ESoundName, TObjectPtr<USoundBase>> SoundMap; // or TsoftObjectPtr

	/** 재생ID → 컴포넌트 */
	TMap<FGuid, TWeakObjectPtr<UAudioComponent>> ActiveById;

	/** 컴포넌트 → 재생ID (역방향 탐색용) */
	TMap<TWeakObjectPtr<UAudioComponent>, FGuid> IdByComp;

	/** 이름 → 재생ID들(동시에 여러 개 가능) */
	TMultiMap<ESoundName, FGuid> NameToIds;

	/** 이름별 마지막 인덱스(랜덤 반복 회피용) */
	TMap<ESoundName, int32> LastRandomIndex;

	UFUNCTION() void OnAudioFinished(UAudioComponent* FinishedComp);
};
