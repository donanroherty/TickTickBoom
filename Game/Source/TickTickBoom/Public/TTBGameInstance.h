

#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "Engine/GameInstance.h"
#include "TTBGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHighScoreChanged);
/**
 * 
 */
UCLASS()
class TICKTICKBOOM_API UTTBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FHighScoreChanged OnHighScoreChanged;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bHideTutorial;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 HighScore;

	UFUNCTION(BlueprintCallable)
		void SetHighScore(int32 InScore);

	UFUNCTION(BlueprintCallable)
		void SetHideTutorial(bool bNewHideTutorial);
};
