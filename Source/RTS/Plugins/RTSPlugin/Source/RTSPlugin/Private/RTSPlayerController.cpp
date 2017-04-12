// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "RTSPlayerController.h"

#include "RTSSelectableComponent.h"
#include "RTSCameraBoundsVolume.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"
#include "EngineUtils.h"


void ARTSPlayerController::BeginPlay()
{
    APlayerController::BeginPlay();

    // Enable mouse input.
    APlayerController::bShowMouseCursor = true;
    APlayerController::bEnableClickEvents = true;
    APlayerController::bEnableMouseOverEvents = true;

    // Bind actions.
    InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::OnLeftMouseButtonReleased);
    InputComponent->BindAxis("MoveCameraLeftRight", this, &ARTSPlayerController::OnMoveCameraLeftRight);
    InputComponent->BindAxis("MoveCameraUpDown", this, &ARTSPlayerController::OnMoveCameraUpDown);

    // Get camera bounds.
    for (TActorIterator<ARTSCameraBoundsVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        CameraBoundsVolume = *ActorItr;
        break;
    }
}

void ARTSPlayerController::OnLeftMouseButtonReleased()
{
    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    // Get local player viewport.
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

    if (!LocalPlayer || !LocalPlayer->ViewportClient)
    {
        return;
    }

    // Get mouse position.
    FVector2D MousePosition;
    if (!LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
    {
        return;
    }

    // Get ray.
    FVector WorldOrigin;
    FVector WorldDirection;
    if (!UGameplayStatics::DeprojectScreenToWorld(this, MousePosition, WorldOrigin, WorldDirection))
    {
        return;
    }

    // Cast ray.
    FCollisionObjectQueryParams Params(FCollisionObjectQueryParams::InitType::AllObjects);
    TArray<FHitResult> HitResults;

    World->LineTraceMultiByObjectType(
        HitResults,
        WorldOrigin,
        WorldOrigin + WorldDirection * HitResultTraceDistance,
        Params);

    // Check results.
    SelectedActors.Empty();

    for (auto& HitResult : HitResults)
    {
        // Check if hit any actor.
        if (HitResult.Actor == nullptr)
        {
            continue;
        }

        // Check if hit selectable actor.
        auto SelectableComponent = HitResult.Actor->FindComponentByClass<URTSSelectableComponent>();

        if (!SelectableComponent)
        {
            continue;
        }

        // Select single actor.
        SelectedActors.Add(HitResult.Actor.Get());
        break;
    }

    // Notify listeners.
    NotifyOnSelectionChanged(SelectedActors);
}

void ARTSPlayerController::OnMoveCameraLeftRight(float Value)
{
    CameraLeftRightAxisValue = Value;
}

void ARTSPlayerController::OnMoveCameraUpDown(float Value)
{
    CameraUpDownAxisValue = Value;
}

void ARTSPlayerController::NotifyOnSelectionChanged(const TArray<AActor*>& Selection)
{
    ReceiveOnSelectionChanged(Selection);
}

void ARTSPlayerController::PlayerTick(float DeltaTime)
{
    APlayerController::PlayerTick(DeltaTime);

    APawn* PlayerPawn = GetPawn();

    if (!PlayerPawn)
    {
        return;
    }

    // Get mouse input.
    float MouseX;
    float MouseY;

    const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

    const float ScrollBorderRight = ViewportSize.X - CameraScrollThreshold;
    const float ScrollBorderTop = ViewportSize.Y - CameraScrollThreshold;

    if (GetMousePosition(MouseX, MouseY))
    {
        if (MouseX <= CameraScrollThreshold)
        {
            CameraLeftRightAxisValue -= 1 - (MouseX / CameraScrollThreshold);
        }
        else if (MouseX >= ScrollBorderRight)
        {
            CameraLeftRightAxisValue += (MouseX - ScrollBorderRight) / CameraScrollThreshold;
        }

        if (MouseY <= CameraScrollThreshold)
        {
            CameraUpDownAxisValue += 1 - (MouseY / CameraScrollThreshold);
        }
        else if (MouseY >= ScrollBorderTop)
        {
            CameraUpDownAxisValue -= (MouseY - ScrollBorderTop) / CameraScrollThreshold;
        }
    }

    // Apply input.
    CameraLeftRightAxisValue = FMath::Clamp(CameraLeftRightAxisValue, -1.0f, +1.0f);
    CameraUpDownAxisValue = FMath::Clamp(CameraUpDownAxisValue, -1.0f, +1.0f);
    
    FVector Location = PlayerPawn->GetActorLocation();
    Location += FVector::RightVector * CameraSpeed * CameraLeftRightAxisValue;
    Location += FVector::ForwardVector * CameraSpeed * CameraUpDownAxisValue;

    // Enforce camera bounds.
    if (!CameraBoundsVolume || CameraBoundsVolume->EncompassesPoint(Location))
    {
        PlayerPawn->SetActorLocation(Location);
    }
}