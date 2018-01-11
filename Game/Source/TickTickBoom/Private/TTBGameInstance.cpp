

#include "TTBGameInstance.h"

void UTTBGameInstance::SetHighScore(int32 InScore)
{
	bHideTutorial = true;

	if (InScore > HighScore)
	{
		HighScore = InScore;
		OnHighScoreChanged.Broadcast();
	}
}

void UTTBGameInstance::SetHideTutorial(bool bNewHideTutorial)
{
	bHideTutorial = bNewHideTutorial;
}
