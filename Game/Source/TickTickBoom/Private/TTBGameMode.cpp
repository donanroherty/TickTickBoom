

#include "TTBGameMode.h"



ATTBGameMode::ATTBGameMode()
{
	CurrentStage = 0;
	MaxStageCount = 10;
	BoardSpacing = 20.f;
	AvailableShortCircuits = 3;
}

ATTBGameBoard * ATTBGameMode::GetGameBoard()
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
