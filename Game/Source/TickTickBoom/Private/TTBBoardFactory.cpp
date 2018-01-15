

#include "TTBBoardFactory.h"
#include "TTBTypes.h"
#include "Engine/DataTable.h"
#include "TTBGameBoard.h"
#include "TTBGameState.h"
#include "EngineUtils.h"

// Sets default values
ATTBBoardFactory::ATTBBoardFactory()
{
	BoardSpacing = 20.f;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
}

void ATTBBoardFactory::OnConstruction(const FTransform& Transform)
{
	bool bDebugBuildBoard = true;	//TODO: Remove this!

	if (!GameStageData || !GameboardClass || !bDebugBuildBoard)
		return;

	// Destroy previously constructed components
	for (UChildActorComponent* ca : GameboardComps)
	{
		ca->UnregisterComponent();
	}

	int32 MaxStageCount = GameStageData->GetRowNames().Num();

	// Spawn a gameboard for each stage defined in GameboardDataTable
	for (int32 i = 0; i < MaxStageCount; i++)
	{
		// The data table rows are named by level number so we find the correct row for each gameboard we want to spawn here
		FGameboardData* Data = GameStageData->FindRow<FGameboardData>(FName(*FString::FromInt(i)), TEXT("LookUp GameStageData"));

		UChildActorComponent* NewBoardComp = NewObject<UChildActorComponent>(this);
		NewBoardComp->SetupAttachment(GetRootComponent());
		NewBoardComp->RegisterComponent();
		NewBoardComp->SetChildActorClass(GameboardClass);
		GameboardComps.Add(NewBoardComp);

		ATTBGameBoard* NewBoardActor = Cast<ATTBGameBoard>(NewBoardComp->GetChildActor());
		NewBoardActor->GameboardData = *Data;
		NewBoardActor->Factory = this;
		NewBoardActor->GenerateBoard();

		Gameboards.Add(NewBoardActor);

 		if (i > 0)
 		{
 			UChildActorComponent* LastBoardComp = GameboardComps[GameboardComps.Num() - 2];
 			ATTBGameBoard* LastBoardActor = Cast<ATTBGameBoard>(LastBoardComp->GetChildActor());
 
 			float LastWidth = (LastBoardActor->GetBoardWidth() + LastBoardActor->ButtonSpacing + 20.f) / 2;
 			float NewWidth = (NewBoardActor->GetBoardWidth() + NewBoardActor->ButtonSpacing + 20.f) / 2;
 
 			// Offset the location of the new board by the bounds of each board + BoardSpacing
 			FVector Offset = FVector(0.f, LastWidth + NewWidth + BoardSpacing, 0.f);
 			FVector NewLoc = LastBoardComp->RelativeLocation + Offset;
 			NewBoardComp->SetRelativeLocation(NewLoc);
 		}
 	}
}

ATTBGameBoard * ATTBBoardFactory::GetGameBoard()
{
	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());

	if (GS && Gameboards.Num() > GS->CurrentStage)
	{
		return Gameboards[GS->CurrentStage];
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentStage exceeds Gameboards array Num"));
		return nullptr;
	}
}

int32 ATTBBoardFactory::GetMaxStageCount()
{
	return GameStageData->GetRowNames().Num();
}