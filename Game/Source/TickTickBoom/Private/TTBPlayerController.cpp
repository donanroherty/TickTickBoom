

#include "TTBPlayerController.h"
#include "TTBCameraActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TTBHud.h"
#include "TTBGameState.h"

void ATTBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI input
	FInputActionBinding& togglePause =  InputComponent->BindAction("PauseGame", IE_Pressed, this, &ATTBPlayerController::TogglePauseGame);
	togglePause.bExecuteWhenPaused = true;
}

void ATTBPlayerController::BeginPlay()
{
	SpawnCamera();
}

void ATTBPlayerController::SpawnCamera()
{
	if (!CameraActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraActorClass not set"));
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CameraActor = GetWorld()->SpawnActor<ATTBCameraActor>(CameraActorClass, SpawnInfo);

	SetViewTargetWithBlend(CameraActor);
	
}

void ATTBPlayerController::SetCameraFocusTarget(AActor* TargetActor)
{
	CameraActor->SetFocusTarget(TargetActor);
}

void ATTBPlayerController::TogglePauseGame()
{
	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());

	if (GS) {
		GS->TogglePauseGame();
	}
}