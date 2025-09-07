// Fill out your copyright notice in the Description page of Project Settings.


#include "Tanjiro/Components/SoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "System/AudioManagerSubsystem.h"
// Sets default values for this component's properties
USoundComponent::USoundComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

FSoundHandle USoundComponent::PlayHandle(ESoundName Name, AActor* Owner, USceneComponent* AttachTo, FName Socket,
	float Volume, float Pitch, bool bAutoDestroy)
{
	if (auto* manager = GetAudioManager())
	{
		return manager->Play(Name, Owner, AttachTo, Socket, Volume, Pitch, bAutoDestroy);
	}
	return FSoundHandle();
}

FSoundHandle USoundComponent::Play2DHandle(ESoundName Name, float Volume, float Pitch, bool bAutoDestroy)
{
	if (auto* manager = GetAudioManager())
	{
		return manager->Play2D(Name, Volume, Pitch, bAutoDestroy);
	}
	return FSoundHandle();
}

FSoundHandle USoundComponent::PlayRandomHandle(const TArray<ESoundName>& Candidates, AActor* Owner,
	USceneComponent* AttachTo, FName SocketName, float Volume, FVector2D PitchRange, bool bAutoDestroy)
{
	if (auto* manager = GetAudioManager())
	{
		return manager->PlayRandom(Candidates, Owner, AttachTo, SocketName, Volume, PitchRange, bAutoDestroy);
	}
	return FSoundHandle();
}

FSoundHandle USoundComponent::PlayAtLocation(ESoundName Name, const FVector& Location, float Volume, float Pitch)
{
	if (auto* manager = GetAudioManager())
	{
		return manager->PlayAtLocation(Name,Location);
	}
	return FSoundHandle();
}

void USoundComponent::StopByHandle(FSoundHandle Handle, float FadeOutTime)
{
	if (auto* manager = GetAudioManager())
		manager->StopByHandle(Handle, FadeOutTime);
}

void USoundComponent::StopByName(ESoundName Name, float FadeOutTime)
{
	if (auto* manager = GetAudioManager())
		manager->StopByName(Name,FadeOutTime);
}

void USoundComponent::StopAll(float FadeOutTime)
{
	if (auto* manager = GetAudioManager())
		manager->StopAll();
}

UAudioManagerSubsystem* USoundComponent::GetAudioManager() const
{
	if (const UWorld* World =  GetWorld())
	{
		if (UGameInstance * GI = World->GetGameInstance())
		{
			return GI->GetSubsystem<UAudioManagerSubsystem>();
		}
	}
	return nullptr;
}

