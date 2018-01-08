

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTBGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ATTBGameMode : public AGameModeBase
{
	GENERATED_BODY()
		ATTBGameMode();
public:
	class UDataTable* GameboardData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBGameBoard> GameboardClass;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ATTBGameBoard*> Gameboards;

	/* The distance between gameboards */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BoardSpacing;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 AvailableShortCircuits;

	FTimerHandle StartGameTimer;
	FTimerHandle StartStageTimer;
	FTimerHandle StageSuccessTimer;
	FTimerHandle StageFailedTimer;

	void BeginPlay();

	/* Starts the active stage */
	UFUNCTION(BlueprintCallable)
	void StartCurrentStage();

	/* Spawns all gameboards in a row from left to right */
	UFUNCTION(BlueprintCallable)
	void SpawnGameboards();

	/* Begin the next stage on stage complete */
	UFUNCTION(BlueprintCallable)
	void IncrementStage();

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
