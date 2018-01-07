

#include "TTBGameBoard.h"
#include "TTBGameMode.h"
#include "TTBGameState.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
ATTBGameBoard::ATTBGameBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameboardData.Cols = 3;
	GameboardData.Rows = 3;
	GameboardData.CycleCount = 10;
	GameboardData.SectionsMovePerCycle = 1;
	GameboardData.TimeScale = 1.0f;

	bBoardIsActive = true;
	ButtonSpacing = 10.f;
	ButtonHeight = 10.f;
	SafeButtonChoiceIterations = 20;
	CountdownSeconds = 3;
	SafeButtonCycleBias = 2;
	BoardState = EBoardState::BS_PreCycle;
}

// Called when the game starts or when spawned
void ATTBGameBoard::BeginPlay()
{
	Super::BeginPlay();

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->OnShortCircuit.AddDynamic(this, &ATTBGameBoard::OnShortCircuit);
	}
}

// Called every frame
void ATTBGameBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATTBGameBoard::OnShortCircuit()
{
	ATTBGameMode* GM = Cast<ATTBGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GM && GM->GetGameBoard() == this)
	{
		OnButtonsDeactivated.Broadcast();
		BeginPreCycle();
	}
}


void ATTBGameBoard::ActivateBoard()
{
	bBoardIsActive = true;
}

void ATTBGameBoard::DeactivateBoard()
{
	bBoardIsActive = false;
	OnButtonsDeactivated.Broadcast();
}

float ATTBGameBoard::GetBoardWidth()
{
	return (GameboardData.Cols - 1) * ButtonSpacing;
}

float ATTBGameBoard::GetBoardLength()
{
	return (GameboardData.Rows - 1) * ButtonSpacing;
}

float ATTBGameBoard::GetGameboardTimeScale()
{
	return GameboardData.TimeScale;
}
