// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal_Actor.generated.h"

UCLASS()
class EL_API APortal_Actor : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    // Sets default values for this actor's properties
    APortal_Actor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    //Status of the Portal (being visualized by the player or not)
    UFUNCTION(BlueprintPure, Category = "APortal_Actor|Portal")
        bool IsActive();

    UFUNCTION(BlueprintCallable, Category = "APortal_Actor|Portal")
        void SetActive(bool NewActive);

    //Render target to use to display the portal
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "APortal_Actor|Portal")
        void ClearRTT();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "APortal_Actor|Portal")
        void SetRTT(UTexture* RenderTexture);

    UFUNCTION(BlueprintNativeEvent, Category = "APortal_Actor|Portal")
        void ForceTick();

    //Target of where the portal is looking
    UFUNCTION(BlueprintPure, Category = "APortal_Actor|Portal")
        AActor* GetTarget();

    UFUNCTION(BlueprintCallable, Category = "APortal_Actor|Portal")
        void SetTarget(AActor* NewTarget);

    //Helpers
    UFUNCTION(BlueprintCallable, Category = "APortal_Actor|Portal")
        bool IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

    UFUNCTION(BlueprintCallable, Category = "APortal_Actor|Portal")
        bool IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal);

    UFUNCTION(BlueprintCallable, Category = "APortal_Actor|Portal")
        void TeleportActor(AActor* ActorToTeleport);

    FRotator ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference, AActor* TargetActor);

    FVector ConvertLocationToActorSpace(FVector Location, AActor* Reference, AActor* TargetActor);


protected:
    UPROPERTY(BlueprintReadOnly)
        USceneComponent* PortalRootComponent;

private:
    bool bIsActive;

    AActor* Target;

    //Used for Tracking movement of a point
    FVector LastPosition;
    bool    LastInFront;
};
