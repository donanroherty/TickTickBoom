

#include "TTBPlayerController.h"
#include "TTBCameraActor.h"
#include "TTBHud.h"
#include "TTBGameState.h"
#include "TTBGameBoard.h"
#include "TTBButton.h"
#include "Runtime/Engine/Classes/Engine/World.h"

void ATTBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI input
	FInputActionBinding& togglePause =  InputComponent->BindAction("PauseGame", IE_Pressed, this, &ATTBPlayerController::TogglePauseGame);
	FInputActionBinding& clearStage = InputComponent->BindAction("Debug_ClearStage", IE_Pressed, this, &ATTBPlayerController::DebugClearStage);
	togglePause.bExecuteWhenPaused = true;
}

void ATTBPlayerController::Inititialize()
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

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		SetCameraFocusTarget(GS->GetGameBoard());
	}
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

void ATTBPlayerController::DebugClearStage()
{
	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());

	if (GS) {
		GS->GetGameBoard()->SafeButton->PressButton();
	}
}
