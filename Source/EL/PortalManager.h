// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneComponent.h"
#include "PortalManager.generated.h"

//Forward declaration
class APlayer_Controller;
class APortal_Actor;

UCLASS()
class EL_API APortalManager : public AActor
{
    GENERATED_UCLASS_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalManager();

    // Called by a Portal actor when wanting to teleport something
    UFUNCTION(BlueprintCallable, Category = "Portal")
        void RequestTeleportByPortal(APortal_Actor* Portal, AActor* TargetToTeleport);

    // Save a reference to the PlayerControler
    void SetControllerOwner(APlayer_Controller* NewOwner);

    // Various setup that happens during spawn
    void Init();

    // Manual Tick
    void Update(float DeltaTime);

    // Find all the portals in world and update them
    // returns the most valid/usable one for the Player
    APortal_Actor* UpdatePortalsInWorld();

    // Update SceneCapture
    void UpdateCapture(APortal_Actor* Portal);

private:
    //Function to create the Portal render target
    void GeneratePortalTexture();

    UPROPERTY()
        USceneCaptureComponent2D* SceneCapture;

    UPROPERTY(transient)
        UTextureRenderTarget2D* PortalTexture;

    UPROPERTY()
        APlayer_Controller* ControllerOwner;

    int32 PreviousScreenSizeX;
    int32 PreviousScreenSizeY;

    float UpdateDelay;

};
