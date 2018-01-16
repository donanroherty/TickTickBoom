

#include "TTBBoardFactory.h"
#include "TTBTypes.h"
#include "Engine/DataTable.h"
#include "TTBGameBoard.h"
#include "TTBGameState.h"
#include "EngineUtils.h"
#include "Components/BillboardComponent.h"
#include "UObjectIterator.h"

// Sets default values
ATTBBoardFactory::ATTBBoardFactory()
{
	BoardSpacing = 20.f;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Marker = CreateDefaultSubobject<UBillboardComponent>(TEXT("Marker"));
	Marker->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Marker->bHiddenInGame = true;
}

void ATTBBoardFactory::OnConstruction(const FTransform& Transform)
{
	//return; // TODO: Remove.  Temp flag to disable construction script while I figure out construction issue in editor.  Using BeginPlay in the meantime.
	CreateBoards();
}

void ATTBBoardFactory::BeginPlay()
{
	//CreateBoards();

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());

	if (GS)
	{
		GS->OnBoardsGenerated();
	}
}

void ATTBBoardFactory::CreateBoards()
{
	//return;	//TODO: Remove this!  For debug only to disable construction script

	if (!GameStageData || !GameboardClass)
		return;

	// Destroy components created in last construction
	for (TObjectIterator<UChildActorComponent> Itr; Itr; ++Itr)
	{
		UChildActorComponent* CA = *Itr;

		if (CA->GetChildActorClass() == ATTBGameBoard::StaticClass())
		{
			CA->UnregisterComponent();
		}
	}

	int32 MaxStageCount = GameStageData->GetRowNames().Num();

	// Spawn a gameboard for each stage defined in GameboardDataTable
	for (int32 i = 0; i < MaxStageCount; i++)
	{
		// The data table rows are named by level number so we find the correct row for each gameboard we want to spawn here
		FGameboardData* Data = GameStageData->FindRow<FGameboardData>(FName(*FString::FromInt(i)), TEXT("LookUp GameStageData"));

		FString BoardName = "Board_" + FString::FromInt(i);
		UChildActorComponent* NewBoardComp = NewObject<UChildActorComponent>(this, FName(*BoardName));
		NewBoardComp->SetupAttachment(GetRootComponent());
		//NewBoardComp->RegisterComponent();
		NewBoardComp->SetChildActorClass(GameboardClass);
		GameboardComps.Add(NewBoardComp);
		NewBoardComp->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		RegisterAllComponents();

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