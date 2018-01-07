

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
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentStage;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaxStageCount;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ATTBGameBoard*> Gameboards;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BoardSpacing;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 AvailableShortCircuits;

	/* Returns the gameboard for the active level */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Gameboard")
	class ATTBGameBoard* GetGameBoard();
};
