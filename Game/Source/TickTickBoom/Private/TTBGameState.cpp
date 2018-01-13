

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

	// Spawn all gameboards
	SpawnGameboards();

	// Set camera focus to the gameboard
	ATTBPlayerController* PC = Cast<ATTBPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
		PC->SetCameraFocusTarget(GetGameBoard());

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
	// Show tutorial?
	UTTBGameInstance* GI = Cast<UTTBGameInstance>(GetGameInstance());
	//if (CurrentStage == 0 && GI && !GI->bHideTutorial)
	//GetHud()->ShowTutorial();

	// Set camera focus to the gameboard
	ATTBPlayerController* PC = Cast<ATTBPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
		PC->SetCameraFocusTarget(GetGameBoard());

	GetGameBoard()->ActivateBoard();

	// Wait 2 seconds before starting the gameboard precycle
	GetWorldTimerManager().SetTimer(StartGameTimer, GetGameBoard(), &ATTBGameBoard::BeginPreCycle, 2.f, false);
}

void ATTBGameState::SpawnGameboards()
{
	// Spawn a gameboard for each stage defined in GameboardDataTable
	for (int32 i = 0; i < GetMaxStageCount(); i++)
	{
		// The data table rows are named by level number so we find the correct row for each gameboard we want to spawn here
		FGameboardData* Data = GameStageData->FindRow<FGameboardData>(FName(*FString::FromInt(i)), TEXT("LookUp Operation"));

		if (GameboardClass)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ATTBGameBoard* NewBoard = GetWorld()->SpawnActor<ATTBGameBoard>(GameboardClass, SpawnInfo);

			if (NewBoard)
			{
				NewBoard->GameboardData = *Data;
				NewBoard->BuildGameboard();
				Gameboards.Add(NewBoard);

				if (i > 0)
				{
					ATTBGameBoard* LastBoard = Gameboards[Gameboards.Num() - 2];

					FVector LastOrigin, LastExtent, NewOrigin, NewExtent;
					LastBoard->GetActorBounds(false, LastOrigin, LastExtent);
					NewBoard->GetActorBounds(false, NewOrigin, NewExtent);

					// Offset the location of the new board by the bounds of each board + BoardSpacing
					FVector NewLoc = LastBoard->GetActorLocation() + FVector(0, LastExtent.Y + NewExtent.Y + BoardSpacing, 0);
					NewBoard->SetActorLocation(NewLoc);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GameboardClass not set in gamemode"));
		}
	}
}

void ATTBGameState::IncrementStage()
{
	//Deactivate previous board if it exists
	if (GetGameBoard())
		GetGameBoard()->DeactivateBoard();

	CurrentStage++;
	StartCurrentStage();
}

void ATTBGameState::OnStageCompleteDialogDismissed()
{
	IncrementStage();
}

void ATTBGameState::OnStageSucess()
{
	// Wait 1 second before completing stage
	GetWorldTimerManager().SetTimer(StartGameTimer, this, &ATTBGameState::CompleteStage, 1.f, false);
}

void ATTBGameState::CompleteStage()
{
	UTTBGameInstance* GI = Cast<UTTBGameInstance>(GetGameInstance());
	if (GI)
		GI->SetHighScore(CurrentStage + 1);

	if (CurrentStage + 1 > GetMaxStageCount())
		GetHud()->OnGameComplete();
	else
		GetHud()->OnStageComplete();
}

void ATTBGameState::OnStageFailed()
{
	// Wait 3 seconds before failing game
	GetWorldTimerManager().SetTimer(StartGameTimer, this, &ATTBGameState::FailStage, 3.f, false);
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
	return GameStageData->GetRowNames().Num();
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
	if (Gameboards.Num() > CurrentStage)
	{
		return Gameboards[CurrentStage];
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentStage exceeds Gameboards array Num"));
		return nullptr;
	}
}
