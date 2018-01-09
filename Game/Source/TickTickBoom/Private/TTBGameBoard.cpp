

#include "TTBGameBoard.h"
#include "TTBGameMode.h"
#include "TTBGameState.h"
#include "TTBButton.h"
#include "TTBGate.h"
#include "TTBHud.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SpotLightComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot light component"));
	SpotLight->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SpotLight->SetRelativeLocation(FVector(63.f, 0, 48.f));
	SpotLight->SetRelativeRotation(FRotator(-30.f, -180.f, 180.f));
	SpotLight->LightColor = FColor(255, 188, 127);
	SpotLight->Intensity = 100.f;
	SpotLight->OuterConeAngle = 36.f;
	SpotLight->AttenuationRadius = 300.f;
}

// Called when the game starts or when spawned
void ATTBGameBoard::BeginPlay()
{
	Super::BeginPlay();

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());
	if (GS)
		GS->OnShortCircuit.AddDynamic(this, &ATTBGameBoard::OnShortCircuit);
}

void ATTBGameBoard::BuildGameboard()
{
	// Build the gameboard

	// Create buttons
	for (int32 col = 0; col < GameboardData.Cols; col++)	// Columns
	{
		TArray<ATTBButton*> NewRow;

		for (int32 row = 0; row < GameboardData.Rows; row++)	// Rows
		{
			FVector ButtonLoc = FVector((row * ButtonSpacing) - (GetBoardLength() / 2), (col * ButtonSpacing) - (GetBoardWidth() / 2), ButtonHeight);

			UChildActorComponent* NewBtnComp = NewObject<UChildActorComponent>(this, FName(*("Button_" + FString::FromInt(col) + "," + FString::FromInt(row))));
			NewBtnComp->SetupAttachment(this->GetRootComponent());
			NewBtnComp->RegisterComponent();

			NewBtnComp->SetRelativeLocation(ButtonLoc);

			if (ButtonClass)
				NewBtnComp->SetChildActorClass(ButtonClass);

			ATTBButton* NewBtn = Cast<ATTBButton>(NewBtnComp->GetChildActor());

			NewBtn->Gameboard = this;
			NewRow.AddUnique(NewBtn);
			Buttons.Add(NewBtn);
		}

		FButtonGrid Row;
		Row.Rows = NewRow;
		ButtonsGrid.Add(Row);
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *(this->GetName() + ": Buttons created - Cols: " + FString::FromInt(ButtonsGrid.Num()) + " Rows: " + FString::FromInt(ButtonsGrid[0].Rows.Num())));

	OnButtonGridUpdated.Broadcast();

	for (int32 i = 0; i < 4; i++)
	{
		// Create corner component
		UStaticMeshComponent* NewCornerComp = NewObject<UStaticMeshComponent>(this, FName(*("Corner_" + FString::FromInt(i))));
		NewCornerComp->SetupAttachment(this->GetRootComponent());
		NewCornerComp->RegisterComponent();

		if (CornerMesh)
			NewCornerComp->SetStaticMesh(CornerMesh);

		// Offset for the corner pieces
		FVector LocOffset =
			i == 0 ? FVector(-1.f, -1.f, 0.f)	// Bottom left
			: i == 1 ? FVector(1.f, -1.f, 0.f)	// Top left
			: i == 2 ? FVector(1.f, 1.f, 0.f)	// Top right
			: FVector(-1.f, 1.f, 0.f);			// Bottom right

		FVector CornerLoc = ((FVector(GetBoardLength(), GetBoardWidth(), 0) / 2.f) + (FVector(ButtonSpacing, ButtonSpacing, ButtonSpacing) / 2.f)) * LocOffset;

		NewCornerComp->SetRelativeLocation(CornerLoc);
		NewCornerComp->SetRelativeRotation(FRotator(0.f, 90.f * i, 0.f));

		float WallCount =
			i == 0 ? GameboardData.Rows		// Bottom left
			: i == 1 ? GameboardData.Cols	// Top left
			: i == 2 ? GameboardData.Rows	// Top right
			: GameboardData.Cols;			// Bottom right


		//Walls and gates
		for (int32 j = 0; j < WallCount; j++)
		{
			// Make walls
			UStaticMeshComponent* NewWallComp = NewObject<UStaticMeshComponent>(this, FName(*("Wall_" + FString::FromInt(i) + "," + FString::FromInt(j))));
			NewWallComp->SetupAttachment(this->GetRootComponent());
			NewWallComp->RegisterComponent();

			if (WallMesh)
				NewWallComp->SetStaticMesh(WallMesh);

			FTransform CornerSocketXform = NewCornerComp->GetSocketTransform(TEXT("Attach"), ERelativeTransformSpace::RTS_World);
			FVector WallLoc = CornerSocketXform.GetLocation() + (FVector(CornerSocketXform.GetUnitAxis(EAxis::Y) * (ButtonSpacing * j)));

			NewWallComp->SetRelativeLocation(WallLoc);
			NewWallComp->SetRelativeRotation(CornerSocketXform.GetRotation());
			NewWallComp->SetRelativeScale3D(CornerSocketXform.GetScale3D());


			// Make gates
			UChildActorComponent* NewGateComp = NewObject<UChildActorComponent>(this, FName(*("Gate_" + FString::FromInt(i) + "," + FString::FromInt(j))));
			NewGateComp->SetupAttachment(this->GetRootComponent());
			NewGateComp->RegisterComponent();

			if (GateClass)
				NewGateComp->SetChildActorClass(GateClass);

			ATTBGate* NewGate = Cast<ATTBGate>(NewGateComp->GetChildActor());
			Gates.Add(NewGate);
			NewGate->Gameboard = this;
			NewGate->AttachToComponent(NewWallComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Attach"));
		}
	}
}

void ATTBGameBoard::OnShortCircuit()
{
	ATTBGameMode* GM = Cast<ATTBGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GM && GM->GetGameBoard() == this)
	{
		DeactivateButtons();
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
	DeactivateButtons();
}

void ATTBGameBoard::Explode()
{
	PlaySound(PlayerHurtSound);
	PlaySound(ExplosionSound);
	UGameplayStatics::SpawnEmitterAttached(ExplosionParticles, GetRootComponent(), TEXT("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	UGameplayStatics::SpawnEmitterAttached(SmokeParticles, GetRootComponent(), TEXT("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	UGameplayStatics::SpawnEmitterAttached(FireParticles, GetRootComponent(), TEXT("None"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
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

ATTBButton* ATTBGameBoard::GetRandButton()
{
	int32 randX = FMath::RandRange(0, GameboardData.Cols - 1);
	int32 randY = FMath::RandRange(0, GameboardData.Rows - 1);

	return GetButtonByIndex(randX, randY);
}

ATTBButton * ATTBGameBoard::GetButtonByIndex(int32 Col, int32 Row)
{
	return ButtonsGrid[Col].Rows[Row];
}

TArray<class ATTBButton*> ATTBGameBoard::GetColumn(int32 Index)
{
	return ButtonsGrid[Index].Rows;
}

TArray<class ATTBButton*> ATTBGameBoard::GetRow(int32 Index)
{
	TArray<ATTBButton*> Selection;

	for (int32 i = 0;  i < ButtonsGrid.Num(); i++)
	{
		Selection.Add(ButtonsGrid[i].Rows[Index]);
	}

	return Selection;
}

void ATTBGameBoard::ActivateButtons()
{
	for (ATTBButton* b : Buttons)
	{
		b->ActivateButton();
	}
}

void ATTBGameBoard::DeactivateButtons()
{
	for (ATTBButton* b : Buttons)
	{
		b->DeactivateButton();
	}
}

ATTBHud* ATTBGameBoard::GetHud()
{	
	return Cast<ATTBHud>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

bool ATTBGameBoard::GetButtonIndex(ATTBButton* Button, int32 &OutCol, int32 &OutRow)
{
	for (int32 i = 0; i < ButtonsGrid.Num(); i++)
	{
		for (int32 j = 0; j < ButtonsGrid.Num(); j++)
		{
			if (ButtonsGrid[i].Rows[j] == Button)
			{
				OutCol = i;
				OutRow = j;

				return true;
			}
		}
	}

	return false;
}

bool ATTBGameBoard::IsButtonTravelingOffBoard(TArray<ATTBButton*> ButtonArray, int32 Index, EDirection Dir)
{
	return (Dir == EDirection::MD_Forward && Index == ButtonArray.Num() - 1) // If the button is the last in the array and is traveling forward...
		|| (Dir == EDirection::MD_Backward && Index == 0); // If the button is first in the array and is traveling backward...
}

/*
void ATTBGameBoard::CycleButtons()
{
	// Randomly choose to cycle columns or rows
	EGridSectionType CycleType = FMath::RandBool() == true ? EGridSectionType::GST_Column : EGridSectionType::GST_Row;

	int32 safeBtnX;
	int32 safeBtnY;
	GetButtonIndex(SafeButton, safeBtnX, safeBtnY);

	int32 SafeButtonSectionIdx = CycleType == EGridSectionType::GST_Column ? safeBtnX : safeBtnY;

	TArray<int32> SelectedSections;

	//Get grid section indices to move this cycle, biased towards adding a section with the safe button in it
	for (int32 i = 0; i < GameboardData.SectionsMovePerCycle; i++)
	{
		int32 rangeMax = CycleType == EGridSectionType::GST_Column ? GameboardData.Cols - 1 : GameboardData.Rows - 1;
		int32 NewSectionIndex = FMath::RandRange(0, rangeMax);

		//We are cycling multiple columns or rows.If our random NewSectionIndex has already been selected, loop around and get a new random index
		if(SelectedSections.Contains(NewSectionIndex))
		{
			while (SelectedSections.Contains(NewSectionIndex))
			{
				NewSectionIndex = FMath::RandRange(0, rangeMax);
			}
		}
		
		SelectedSections.Add(NewSectionIndex);
	}

	// If selected sections do not contain a section with the safe button in it...
	if (!SelectedSections.Contains(SafeButtonSectionIdx))
	{
		// Add a chance to forcibly set the first selected section to be the safe buttons section
		int32 rangeMax = CycleType == EGridSectionType::GST_Column ? GameboardData.Cols - 1 : GameboardData.Rows - 1;
		if (rangeMax < SafeButtonCycleBias)
		{
			SelectedSections[0] = SafeButtonSectionIdx;
		}
	}

	// For each random section
	for (int32 i = 0;i < SelectedSections.Num(); i++)
	{
		// Randomly choose cycle direction
		EDirection CycleDirection = FMath::RandBool() == true ? EDirection::MD_Forward : EDirection::MD_Forward;

		TArray<ATTBButton*> CycleButtons = CycleType == EGridSectionType::GST_Column ? GetColumn(i) : GetRow(i);
		for (int32 j = 0; j < CycleButtons.Num(); j++)
		{

		}
	}
}

void ATTBGameBoard::CycleGridSection(int32 Idx, EDirection Dir, EGridSectionType SectionType)
{
	// Create a temporary array of the selected col/row
	TArray<ATTBButton*> NewSection = SectionType == EGridSectionType::GST_Column ? GetColumn(Idx) : GetRow(Idx);

	// Cycle array positions
	if (Dir == EDirection::MD_Forward)
	{
		// Move the last item to the first position in the array
		NewSection.Insert(NewSection[NewSection.Num() - 1], 0);
		NewSection.RemoveAt(NewSection.Num() - 1);
	}
	else
	{
		// Move the first item to the end of the array
		NewSection.Add(NewSection[0]);
		NewSection.RemoveAt(0);
	}

	// Assign the temp array back to the grid
	if (SectionType == EGridSectionType::GST_Column)
	{
		ButtonsGrid[Idx].Rows = NewSection;
	}
	else
	{
		for (int32 i = 0; i< NewSection.Num(); i++)
		{
			TArray<ATTBButton*> ExistingRow = ButtonsGrid[i].Rows;
			ExistingRow[Idx] = NewSection[i];
			ButtonsGrid[i].Rows = ExistingRow;
		}
	}
}
*/

void ATTBGameBoard::OnSafeButtonSet()
{
	SafeButton->FlashButton(false);

	// Start machine noises
	if (MachineNoiseSound)
	{
		MachineNoiseAudioComp = PlaySound(MachineNoiseSound);
		MachineNoiseAudioComp->FadeIn(1.f);
	}
	
	GetHud()->BeginCountdown(CountdownSeconds);
	GetWorldTimerManager().SetTimer(BeginCycleTimerHandle, this, &ATTBGameBoard::BeginCycle, CountdownSeconds);
}

void ATTBGameBoard::BeginCycle()
{
	BoardState = EBoardState::BS_Cycle;
	CycleOnTimer();
	SafeButton->SlowFadeOut();
}

void ATTBGameBoard::OnCycleComplete()
{
	BoardState = EBoardState::BS_PostCycle;
	GetHud()->OnCycleComplete();
	MachineNoiseAudioComp->FadeOut(1.f, 0.f);	// Stop machine noises

	for (ATTBButton* b : Buttons)
		ActivateButtons();
}

UAudioComponent* ATTBGameBoard::PlaySound(USoundBase* Sound)
{
	UAudioComponent* AC = NULL;

	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, GetRootComponent());
	}

	return AC;
}

void ATTBGameBoard::ButtonClicked(ATTBButton * ClickedButton)
{
	DeactivateButtons();

	if (ClickedButton == SafeButton)
	{
		SafeButton->FadeSafeIn();
		GetWorldTimerManager().SetTimer(DelayTimerHandle1, this, &ATTBGameBoard::OnLevelSuccess, .5f);
	}
	else
	{

		OnLevelFailure();
	}
}

void ATTBGameBoard::OnLevelSuccess()
{
	for (ATTBButton* b : Buttons)
	{
		b->RetractButton();
		b->FadeSafeIn();
	}
	PlaySound(MachineWindDownSound);

	ATTBGameMode* GM = Cast<ATTBGameMode>(GetWorld()->GetAuthGameMode());
	if(GM)
		GM->OnStageSucess();
}

void ATTBGameBoard::OnLevelFailure()
{
	SafeButton->FadeSafeIn();
	for (ATTBButton* b : Buttons)
	{
		if (b != SafeButton)
			b->RetractButton();
	}

	DeactivateBoard();
	GetWorldTimerManager().SetTimer(DelayTimerHandle1, this, &ATTBGameBoard::Explode, 1.f);

	ATTBGameMode* GM = Cast<ATTBGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
		GetWorldTimerManager().SetTimer(DelayTimerHandle2, GM, &ATTBGameMode::OnStageFailed, 2.f);
}
