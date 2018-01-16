

#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "GameFramework/GameStateBase.h"
#include "TTBGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShortCircuit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameFailed);
/**
 * 
 */
UCLASS()
class ATTBGameState : public AGameStateBase
{
	GENERATED_BODY()

	public:
	/* Manages pause state of the game */
	bool bIsPaused;
	
	/* Called when the player short circuits the board */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FShortCircuit OnShortCircuit;
	/* Called on game failure.  Used in level BP to dim lights */
	UPROPERTY(BlueprintAssignable)
	FGameFailed OnGameFailed;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBGameBoard> GameboardClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBBoardFactory> BoardFactoryClass;

	UPROPERTY(BlueprintReadWrite)
	class ATTBBoardFactory* BoardFactory;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentStage;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ATTBGameBoard*> Gameboards;

	/* The distance between gameboards */
	UPROPERTY(EditDefaultsOnly)
	float BoardSpacing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 CountdownSeconds;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxShortCircuits;
	UPROPERTY(BlueprintReadOnly)
	int32 AvailableShortCircuits;

	/* Timer handles */
	FTimerHandle StartGameTimer;
	FTimerHandle StartStageTimer;
	FTimerHandle StageSuccessTimer;
	FTimerHandle StageFailedTimer;
	FTimerHandle GameCompleteShowHudTimer;
	FTimerHandle GameCompleteTimer;

	/* Constructor */
	ATTBGameState();

	virtual void BeginPlay()override;

	void OnBoardsGenerated();

	/* Toggles game pause state */
	void TogglePauseGame();

	/* Starts the active stage */
	UFUNCTION(BlueprintCallable)
	void StartCurrentStage();

	/* Begin the next stage on stage complete */
	UFUNCTION(BlueprintCallable)
	void OnStageCompleteDialogDismissed();

	/* Called when the correct button is pressed */
	UFUNCTION(BlueprintCallable)
	void OnStageSucess();
	void CompleteStage();

	/* Called when the wrong button is pressed */
	UFUNCTION(BlueprintCallable)
	void OnStageFailed();
	void FailStage();

	UFUNCTION(BlueprintCallable)
	void ShortCircuit();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameboard")
	int32 GetMaxStageCount();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameboard")
	class ATTBHud* GetHud();

	/* Returns the gameboard for the active level */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Gameboard")
	class ATTBGameBoard* GetGameBoard();
};
