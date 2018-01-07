

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TTBGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShortCircuit);

/**
 * 
 */
UCLASS()
class ATTBGameState : public AGameStateBase
{
	GENERATED_BODY()

	ATTBGameState();

	public:

	/* Manages pause state of the game */
	bool bIsPaused;
	
	/* Called when the player short circuits the board */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
		FShortCircuit OnShortCircuit;

	/* Toggles game pause state */
	void TogglePauseGame();
};
