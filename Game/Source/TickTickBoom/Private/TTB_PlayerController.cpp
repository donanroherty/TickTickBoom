

#include "TTB_PlayerController.h"
#include "TTB_CameraActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"

void ATTB_PlayerController::BeginPlay()
{
	SpawnCamera();
}

void ATTB_PlayerController::SpawnCamera()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CameraActor = GetWorld()->SpawnActor<ATTB_CameraActor>(CameraActorClass, SpawnInfo);

	SetViewTargetWithBlend(CameraActor);
}

void ATTB_PlayerController::SetCameraFocusTarget(AActor * TargetActor)
{
	CameraActor->SetFocusTarget(TargetActor);
}
