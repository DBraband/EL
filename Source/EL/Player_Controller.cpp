// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Controller.h"

void APlayer_Controller::BeginPlay()
{
	Super::BeginPlay();

    FActorSpawnParameters SpawnParams;

    PortalManager = nullptr;
    PortalManager = GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams);
    PortalManager->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
    PortalManager->SetControllerOwner(this);
    PortalManager->Init();
}

FMatrix APlayer_Controller::GetCameraProjectionMatrix()
{
    FMatrix ProjectionMatrix;

    if (GetLocalPlayer() != nullptr)
    {
        FSceneViewProjectionData PlayerProjectionData;

        GetLocalPlayer()->GetProjectionData(GetLocalPlayer()->ViewportClient->Viewport,
            EStereoscopicPass::eSSP_FULL,
            PlayerProjectionData);

        ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
    }

    return ProjectionMatrix;
}

APortalManager* APlayer_Controller::GetPortalManager()
{
    return PortalManager;
}
