// Copyright Epic Games, Inc. All Rights Reserved.

#include "KYGameMode.h"
#include "System/EnemyAttackState.h"
AKYGameMode::AKYGameMode()
{
	// stu
	GameStateClass = AEnemyAttackState::StaticClass();
}
