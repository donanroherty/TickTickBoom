

#include "TTBGameBoard.h"


// Sets default values
ATTBGameBoard::ATTBGameBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

