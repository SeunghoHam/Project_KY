// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/CharacterBase.h"
#include "GameFramework/Character.h"

#include "TanjiroController.generated.h"

/**
 * 
 */
class UInputAction;
class UInputMappingContext;
UCLASS()
class KY_API ATanjiroController : public APlayerController
{
	GENERATED_BODY()
protected:
	
	virtual void SetupInputComponent() override;

public:
	/** Input Mapping Contexts */
	// 배열이이니까 Move Look 다넣음
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts; 
	//UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	//TArray<UInputMappingContext*> ExcludedMappingContexts;
};
