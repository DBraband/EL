// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal_Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player_Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player_Controller.h"

APortal_Actor::APortal_Actor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    bIsActive = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent->Mobility = EComponentMobility::Static;

    PortalRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PortalRootComponent"));
    PortalRootComponent->SetupAttachment(GetRootComponent());
    PortalRootComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    PortalRootComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    PortalRootComponent->Mobility = EComponentMobility::Movable;
}

// Called when the game starts or when spawned
void APortal_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortal_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APortal_Actor::IsActive()
{
    return bIsActive;
}

void APortal_Actor::SetActive(bool NewActive)
{
    bIsActive = NewActive;
}

void APortal_Actor::ClearRTT_Implementation()
{

}

void APortal_Actor::SetRTT_Implementation(UTexture* RenderTexture)
{

}

void APortal_Actor::ForceTick_Implementation()
{

}

AActor* APortal_Actor::GetTarget()
{
    return Target;
}

void APortal_Actor::SetTarget(AActor* NewTarget)
{
    Target = NewTarget;
}

bool APortal_Actor::IsPointInFrontOfPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
    FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
    float PortalDot = PortalPlane.PlaneDot(Point);

    //If < 0 means we are behind the Plane
    //See : http://api.unrealengine.com/INT/API/Runtime/Core/Math/FPlane/PlaneDot/index.html
    return (PortalDot >= 0);
}

bool APortal_Actor::IsPointCrossingPortal(FVector Point, FVector PortalLocation, FVector PortalNormal)
{
    FVector IntersectionPoint;
    FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
    float PortalDot = PortalPlane.PlaneDot(Point);
    bool IsCrossing = false;
    bool IsInFront = PortalDot >= 0;

    bool IsIntersect = FMath::SegmentPlaneIntersection(LastPosition,
        Point,
        PortalPlane,
        IntersectionPoint);

    // Did we intersect the portal since last Location ?
    // If yes, check the direction : crossing forward means we were in front and now at the back
    // If we crossed backward, ignore it (similar to Prey 2006)
    if (IsIntersect && !IsInFront && LastInFront)
    {
        IsCrossing = true;
    }

    // Store values for next check
    LastInFront = IsInFront;
    LastPosition = Point;

    return IsCrossing;
}

void APortal_Actor::TeleportActor(AActor* ActorToTeleport)
{
    {
        if (ActorToTeleport == nullptr || Target == nullptr)
        {
            return;
        }

        //-------------------------------
        //Retrieve and save Player Velocity
        //(from the Movement Component)
        //-------------------------------
        FVector SavedVelocity = FVector::ZeroVector;
        APlayer_Character* EC = nullptr;

        if (ActorToTeleport->IsA(APlayer_Character::StaticClass()))
        {
            EC = Cast<APlayer_Character>(ActorToTeleport);

            SavedVelocity = EC->GetVelocity();

        }

        //-------------------------------
        //Compute and apply new location
        //-------------------------------
        FHitResult HitResult;
        FVector NewLocation = ConvertLocationToActorSpace(ActorToTeleport->GetActorLocation(), this);

        ActorToTeleport->SetActorLocation(NewLocation,
            false,
            &HitResult,
            ETeleportType::TeleportPhysics);

        //-------------------------------
        //Compute and apply new rotation
        //-------------------------------
        FRotator NewRotation = ConvertRotationToActorSpace(ActorToTeleport->GetActorRotation(), this);
        
        

        //-------------------------------
        //If we are teleporting a character we need to
        //update its controller as well and reapply its velocity
        //-------------------------------
        if (ActorToTeleport->IsA(APlayer_Character::StaticClass()))
        {
            //Update Controller
            APlayer_Controller* EPC = Cast<APlayer_Controller>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

            if (EPC != nullptr)
            {
                NewRotation = ConvertRotationToActorSpace(EPC->GetControlRotation(), this);
                NewRotation.Yaw += 180.f;
                EPC->SetControlRotation(NewRotation);
            }

            //Reapply Velocity (Need to reorient direction into local space of Portal)
            {
                FVector Dots;
                Dots.X = FVector::DotProduct(SavedVelocity, GetActorForwardVector());
                Dots.Y = FVector::DotProduct(SavedVelocity, GetActorRightVector());
                Dots.Z = FVector::DotProduct(SavedVelocity, GetActorUpVector());

                FVector NewVelocity = Dots.X * Target->GetActorForwardVector()
                    + Dots.Y * Target->GetActorRightVector()
                    + Dots.Z * Target->GetActorUpVector();

                EC->GetVelocity() = NewVelocity;
            }
        }

        //Cleanup Teleport
        LastPosition = NewLocation;
    }
}


FVector APortal_Actor::ConvertLocationToActorSpace(FVector Location, AActor * Reference)
    {
        if (Reference == nullptr || Target == nullptr)
        {
            return FVector::ZeroVector;
        }

        FVector Direction = Location - Reference->GetActorLocation();
        FVector TargetLocation = Target->GetActorLocation();

        FVector Dots;
        Dots.X = FVector::DotProduct(Direction, Reference->GetActorForwardVector());
        Dots.Y = FVector::DotProduct(Direction, Reference->GetActorRightVector());
        Dots.Z = FVector::DotProduct(Direction, Reference->GetActorUpVector());

        FVector NewDirection = Dots.X * Target->GetActorForwardVector()
            + Dots.Y * Target->GetActorRightVector()
            + Dots.Z * Target->GetActorUpVector();

        return TargetLocation + NewDirection;
    }

FRotator APortal_Actor::ConvertRotationToActorSpace(FRotator Rotation, AActor* Reference)
{

    if (Reference == nullptr || Target == nullptr)
    {
        return FRotator::ZeroRotator;
    }

    FTransform SourceTransform = Reference->GetActorTransform();
    FTransform TargetTransform = Target->GetActorTransform();
    FQuat QuatRotation = FQuat(Rotation);

    FQuat LocalQuat = SourceTransform.GetRotation().Inverse() * QuatRotation;
    FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;

    return NewWorldQuat.Rotator();
}

bool APortal_Actor::IsPointInsideBox(FVector Point, UBoxComponent* Box)
{
    if (Box != nullptr)
    {
        // From :
        // https://stackoverflow.com/questions/52673935/check-if-3d-point-inside-a-box/52674010

        FVector Center = Box->GetComponentLocation();
        FVector Half = Box->GetScaledBoxExtent();
        FVector DirectionX = Box->GetForwardVector();
        FVector DirectionY = Box->GetRightVector();
        FVector DirectionZ = Box->GetUpVector();

        FVector Direction = Point - Center;

        bool IsInside = FMath::Abs(FVector::DotProduct(Direction, DirectionX)) <= Half.X &&
            FMath::Abs(FVector::DotProduct(Direction, DirectionY)) <= Half.Y &&
            FMath::Abs(FVector::DotProduct(Direction, DirectionZ)) <= Half.Z;

        return IsInside;
    }
    else
    {
        return false;
    }
}

APortalManager* APortal_Actor::GetPortalManager(AActor* Context)
{
    APortalManager* Manager = nullptr;

    //Retrieve the World from the Context actor
    if (Context != nullptr && Context->GetWorld() != nullptr)
    {
        //Find PlayerController
        APlayer_Controller* EPC = Cast<APlayer_Controller>(Context->GetWorld()->GetFirstPlayerController());

        //Retrieve the Portal Manager
        if (EPC != nullptr && EPC->GetPortalManager() != nullptr)
        {
            Manager = EPC->GetPortalManager();
        }
    }

    return Manager;
}