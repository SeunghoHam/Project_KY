// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundConcurrency.h"
#include "Components/ActorComponent.h"
#include "SoundComponent.generated.h"


enum class ESoundName : uint8;
class UAudioManagerSubsystem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KY_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USoundComponent();
	
	/** 소유 액터의 루트(또는 지정 컴포넌트)에 부착 재생 */
	UFUNCTION(BlueprintCallable) FSoundHandle PlayHandle(ESoundName Name, AActor* Owner,USceneComponent* AttachTo=nullptr,
		FName Socket=NAME_None,float Volume=1.f, float Pitch=1.f, bool bAutoDestroy=true);
	UFUNCTION(BlueprintCallable) FSoundHandle Play2DHandle(ESoundName Name, float Volume=1.f, float Pitch=1.f, bool bAutoDestroy=true);
	UFUNCTION(BlueprintCallable) FSoundHandle PlayRandomHandle(const TArray<ESoundName>& Candidates,
							AActor* Owner,
							USceneComponent* AttachTo = nullptr,
							FName SocketName = NAME_None,
							float Volume = 1.f,
							FVector2D PitchRange = FVector2D(0.95f, 1.05f),
							bool bAutoDestroy = true);
	// 월드 위치에서 3D 재생(일회성)
	UFUNCTION(BlueprintCallable) FSoundHandle PlayAtLocation(ESoundName Name, const FVector& Location, float Volume = 1.f, float Pitch = 1.f);
	
	UFUNCTION(BlueprintCallable) void StopByHandle(FSoundHandle Handle, float FadeOutTime=0.f);
	UFUNCTION(BlueprintCallable) void StopByName(ESoundName Name, float FadeOutTime=0.f);
	UFUNCTION(BlueprintCallable) void StopAll(float FadeOutTime=0.f);

private:
	UAudioManagerSubsystem* GetAudioManager() const;
		
};
