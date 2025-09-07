// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EnemyRegistrySubsystem.h"
#include "Enemy/Oni.h"

void UEnemyRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("[Registry] Initialize GI=%s"),
	*GetGameInstance()->GetName());
}

void UEnemyRegistrySubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("[Registry] Deinitialize"));
	Super::Deinitialize();
}

void UEnemyRegistrySubsystem::RegisterEnemy(AOni* E)
{
	Enemies.Add(TWeakObjectPtr<AOni>(E));
	UE_LOG(LogTemp, Warning, TEXT("EnemiesCount : %d"), Enemies.Num());
	OnEnemyRegistered.Broadcast(E);
}

void UEnemyRegistrySubsystem::UnregisterEnemy(AOni* E)
{
	Enemies.Remove(TWeakObjectPtr<AOni>(E));
	OnEnemyUnregistered.Broadcast(E);
}
