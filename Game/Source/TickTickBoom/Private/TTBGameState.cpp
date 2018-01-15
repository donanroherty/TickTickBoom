

#include "TTBGameState.h"
#include "TTBHud.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Runtime/Engine/Public/EngineGlobals.h"
#include "Engine/DataTable.h"
#include "TTBGameBoard.h"
#include "TTBHud.h"
#include "TTBPlayerController.h"
#include "TTBGameInstance.h"
#include "EngineUtils.h"
#include "TTBBoardFactory.h"

ATTBGameState::ATTBGameState()
{
	bIsPaused = false;
	CurrentStage = 0;
	BoardSpacing = 20.f;
	CountdownSeconds = 3;
	MaxShortCircuits = 3;
	AvailableShortCircuits = MaxShortCircuits;
}

void ATTBGameState::BeginPlay()
{
	if (!GameStageData)
		return;

	// Find the board factory in the level
	for (TActorIterator<ATTBBoardFactory> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (*ActorItr)
		{
			BoardFactory = *ActorItr;
		}
	}
	if (!BoardFactory)
		return;

	// Set camera focus to the gameboard
	ATTBPlayerController* PC = Cast<ATTBPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->Inititialize();
	}

	// Wait 3 seconds and start the first stage
	GetWorldTimerManager().SetTimer(StartGameTimer, this, &ATTBGameState::StartCurrentStage, 3.f, false);
}

void ATTBGameState::TogglePauseGame()
{
	bIsPaused = !bIsPaused;

	APlayerController* const PC = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	
	if (PC)
	{
		PC->SetPause(bIsPaused);

		ATTBHud* const Hud = Cast<ATTBHud>(PC->GetHUD());

		if (Hud) {
			if (bIsPaused)
			{
				Hud->OpenPauseMenu();
			}
			else
			{
				Hud->ClosePauseMenu();
			}
		}
	}
}

void ATTBGameState::StartCurrentStage()
{
	// Show tutorial
	UTTBGameInstance* GI = Cast<UTTBGameInstance>(GetGameInstance());
	if (CurrentStage == 0 && GI && !GI->bHideTutorial)
		GetHud()->ShowTutorial();

	// Set camera focus to the gameboard
	ATTBPlayerController* PC = Cast<ATTBPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
		PC->SetCameraFocusTarget(GetGameBoard());

	GetGameBoard()->ActivateBoard();

	// Wait 2 seconds before starting the gameboard precycle
	GetWorldTimerManager().SetTimer(StartGameTimer, GetGameBoard(), &ATTBGameBoard::BeginPreCycle, 2.f, false);
}

void ATTBGameState::OnStageCompleteDialogDismissed()
{
	//Deactivate previous board if it exists
	if (GetGameBoard())
		GetGameBoard()->DeactivateBoard();

	CurrentStage++;
	StartCurrentStage();
}

void ATTBGameState::OnStageSucess()
{
	// Wait 1 second before completing stage
	GetWorldTimerManager().SetTimer(StageSuccessTimer, this, &ATTBGameState::CompleteStage, 1.f, false);
}

void ATTBGameState::CompleteStage()
{
	UTTBGameInstance* GI = Cast<UTTBGameInstance>(GetGameInstance());
	if (GI)
		GI->SetHighScore(CurrentStage + 1);

	if (CurrentStage + 1 >= GetMaxStageCount())
	{
		GetWorldTimerManager().SetTimer(GameCompleteShowHudTimer, GetHud(), &ATTBHud::OnGameComplete, 2.f, false);	
	}
	else
	{
		GetHud()->OnStageComplete();
	}
}

void ATTBGameState::OnStageFailed()
{
	// Wait 3 seconds before failing game
	GetWorldTimerManager().SetTimer(StageFailedTimer, this, &ATTBGameState::FailStage, 3.f, false);
}

void ATTBGameState::FailStage()
{
	GetHud()->OnDeath();
}

void ATTBGameState::ShortCircuit()
{
	if (AvailableShortCircuits > 0 && GetGameBoard()->BoardState == EBoardState::BS_PostCycle)
	{
		GetGameBoard()->OnShortCircuit();
		OnShortCircuit.Broadcast();
	}
}

int32 ATTBGameState::GetMaxStageCount()
{
	return BoardFactory->GetMaxStageCount();
}

ATTBHud * ATTBGameState::GetHud()
{
	if (GEngine->GetFirstLocalPlayerController(GetWorld())
		&& GEngine->GetFirstLocalPlayerController(GetWorld())->GetHUD()
		&& Cast<ATTBHud>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetHUD()))
	{
		return Cast<ATTBHud>(GEngine->GetFirstLocalPlayerController(GetWorld())->GetHUD());
	}
	return nullptr;
}

ATTBGameBoard * ATTBGameState::GetGameBoard()
{
	return BoardFactory->GetGameBoard();
}
