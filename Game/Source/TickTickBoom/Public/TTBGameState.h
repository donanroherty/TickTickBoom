

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TTBGameState.generated.h"

/**
 * 
 */
UCLASS()
class ATTBGameState : public AGameStateBase
{
	GENERATED_BODY()

	ATTBGameState();

	public:

	bool bIsPaused;
	

	void TogglePauseGame();
};
