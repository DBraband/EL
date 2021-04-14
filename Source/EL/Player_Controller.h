// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalManager.h"
#include "Portal_Actor.h"
#include "Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class EL_API APlayer_Controller : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	APortalManager* PortalManager;
	
	FMatrix GetCameraProjectionMatrix();

	APortalManager* GetPortalManager();

};
