

#include "TTBGameState.h"
#include "TTBHud.h"
#include "Runtime/Engine/Public/EngineGlobals.h"

ATTBGameState::ATTBGameState()
{
	bIsPaused = false;
}

void ATTBGameState::TogglePauseGame()
{
	bIsPaused = !bIsPaused;

	APlayerController* const PC = Cast<APlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()));
	
	if (PC)
	{
		PC->SetPause(bIsPaused);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Got here!!!"));

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