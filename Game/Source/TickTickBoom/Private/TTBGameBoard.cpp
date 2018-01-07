

#include "TTBGameBoard.h"

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
}

// Called when the game starts or when spawned
void ATTBGameBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTBGameBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

