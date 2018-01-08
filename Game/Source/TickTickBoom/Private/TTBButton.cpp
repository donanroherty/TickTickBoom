

#include "TTBButton.h"
#include "TTBGameMode.h"
#include "TTBGameBoard.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
ATTBButton::ATTBButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonExtendedHeight = 2.f;
	ButtonTargetHeight = 0.f;
	bIsPlaceholder = false;
	bIsActive = false;
	bIsPlaceholder = false;
}

// Called when the game starts or when spawned
void ATTBButton::BeginPlay()
{
	Super::BeginPlay();
	
// 	ATTBGameMode* GM = Cast<ATTBGameMode>(GetWorld()->GetAuthGameMode());
// 
// 	if (GM)
// 	{
// 		ATTBGameBoard* Gameboard = GM->GetGameBoard();
// 	}

	if (Gameboard)
	{
		Gameboard->OnButtonsActivated.AddDynamic(this, &ATTBButton::OnButtonsActivated);
		Gameboard->OnButtonsDeactivated.AddDynamic(this, &ATTBButton::OnButtonsDeactivated);
		Gameboard->OnButtonGridUpdated.AddDynamic(this, &ATTBButton::UpdateDebug);
	}

	// Run debug update manually the first time
	UpdateDebug();
}

// Called every frame
void ATTBButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATTBButton::OnButtonsActivated()
{
	bIsActive = true;
}

void ATTBButton::OnButtonsDeactivated()
{
	bIsActive = false;
}

