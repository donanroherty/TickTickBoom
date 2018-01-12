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
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "ParticleDefinitions.h"

ATTBGameBoard::ATTBGameBoard()
{
	PrimaryActorTick.bCanEverTick = true;

	GameboardData.Cols = 3;
	GameboardData.Rows = 3;
	GameboardData.CycleCount = 10;
	GameboardData.SectionsMovePerCycle = 1;
	GameboardData.TimeScale = 1.0f;

	bBoardIsActive = true;
	ButtonSpacing = 10.f;
	ButtonHeight = 10.f;
	ButtonChoiceMaxIterations = 20;
	CountdownSeconds = 3;
	SafeButtonCycleBias = 2;
	BoardState = EBoardState::BS_PreCycle;
	bButtonsActive = false;

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

void ATTBGameBoard::BuildGameboard()
{
	// Create buttons
	for (int32 col = 0; col < GameboardData.Cols; col++)	// Columns
	{
		TArray<ATTBButton*> NewRow;

		for (int32 row = 0; row < GameboardData.Rows; row++)	// Rows
		{
			FVector ButtonLoc = FVector((row * ButtonSpacing) - (GetBoardLength() / 2), (col * ButtonSpacing) - (GetBoardWidth() / 2), ButtonHeight);

			if(ButtonClass)
			{
				FTransform SpawnXForm;
				ATTBButton* NewButt = GetWorld()->SpawnActorDeferred<ATTBButton>(ButtonClass, FTransform(FRotator::ZeroRotator, GetActorLocation()), this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (NewButt)
				{
					NewButt->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
					NewButt->SetActorRelativeLocation(ButtonLoc);
					NewButt->Gameboard = this;

					UGameplayStatics::FinishSpawningActor(NewButt, SpawnXForm);
					NewRow.AddUnique(NewButt);
				}
			}
		}

		FButtonGrid Row;
		Row.Rows = NewRow;
		ButtonsGrid.Add(Row);
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *(this->GetName() + ": Buttons created - Cols: " + FString::FromInt(ButtonsGrid.Num()) + " Rows: " + FString::FromInt(ButtonsGrid[0].Rows.Num())));

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

		/*
			Walls and gates
		*/
		float WallCount =
			i == 0 ? GameboardData.Rows		// Bottom left
			: i == 1 ? GameboardData.Cols	// Top left
			: i == 2 ? GameboardData.Rows	// Top right
			: GameboardData.Cols;			// Bottom right
	
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
			if (GateClass)
			{
				FTransform SpawnXForm;
				ATTBGate* NewGate = GetWorld()->SpawnActorDeferred<ATTBGate>(GateClass, SpawnXForm, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
				if (NewGate)
				{							
					NewGate->Gameboard = this;
					UGameplayStatics::FinishSpawningActor(NewGate, SpawnXForm);

					NewGate->AttachToComponent(NewWallComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Attach"));
					Gates.Add(NewGate);
				}
			}
		}
	}	
}

void ATTBGameBoard::OnShortCircuit()
{
	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());

	if (GS && GS->GetGameBoard() == this)
	{
		SetButtonsActive(false);
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
	SetButtonsActive(false);
}

void ATTBGameBoard::BeginPreCycle()
{
	BoardState = EBoardState::BS_PreCycle;
	ButtonChoiceIteration = 0;
	GetWorldTimerManager().SetTimer(ChooseSafeButtonTimerHandle, this, &ATTBGameBoard::ChooseSafeButton, .1f, true);
}

void ATTBGameBoard::ChooseSafeButton()
{
	ATTBButton* NewSafeButton = GetRandButton();
	while (NewSafeButton == SafeButton)		// Prevent randomly selecting the same safe button twice in a row
	{
		NewSafeButton = GetRandButton();
	}
	SafeButton = NewSafeButton;

	ButtonChoiceIteration++;

	if (ButtonChoiceIteration > ButtonChoiceMaxIterations)
	{
		GetWorldTimerManager().ClearTimer(ChooseSafeButtonTimerHandle);
		OnSafeButtonSet();
	}
	else
	{
		SafeButton->ChangeColor(EColorFunction::CC_Blink);
	}
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

TArray<class ATTBButton*> ATTBGameBoard::GetAllButtons()
{
	TArray<ATTBButton*> Selection;
	for (int32 i = 0; i< ButtonsGrid.Num(); i++)
	{
		for (int32 j = 0; j < ButtonsGrid.Num(); j++)
		{
			Selection.Add(ButtonsGrid[i].Rows[j]);
		}
	}

	return Selection;
}

void ATTBGameBoard::SetButtonsActive(bool bNewActive)
{
	bButtonsActive = bNewActive;
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

void ATTBGameBoard::SimulateCycle()
{
	/*
	* Randomly select either columns or rows as the cycle type
	* Select the actual buttons to move.  May be multiple sections in later stages
	* Check if the selected sections contain the safe button.  If not, repeat n times to increase the odds the safe button will be moved
	* For each button to move...
	* If the button is moving off board, spawn a proxy button in its place and move that off.  Then teleport the actual button to the far side of the board, ready to scroll in
	* Call movement timelines on all buttons in moving sections
	* Cycle the positions of buttons within the 2D ButtonsGrid array
	* Find the gates associated with the moving sections and play their animations
	*/

	/*
	* Select sections (columns or rows) to cycle
	*/	
	EGridSectionType CycleType = FMath::RandBool() == true ? EGridSectionType::GST_Column : EGridSectionType::GST_Row;	// Randomly choose to cycle columns or rows

	int32 safeBtnX, safeBtnY;
	GetButtonIndex(SafeButton, safeBtnX, safeBtnY);
	int32 SafeButtonSectionIdx = CycleType == EGridSectionType::GST_Column ? safeBtnX : safeBtnY;

	//Get grid section indices to move this cycle, biased towards adding a section with the safe button in it
	TArray<int32> SelectedSections;
	for (int32 i = 0; i < GameboardData.SectionsMovePerCycle; i++)
	{
		int32 rangeMax = CycleType == EGridSectionType::GST_Column ? GameboardData.Cols - 1 : GameboardData.Rows - 1;
		int32 NewSectionIndex = FMath::RandRange(0, rangeMax);

		if(SelectedSections.Contains(NewSectionIndex))	// Ensure we don't add the same section multiple times in one cycle
		{
			while (SelectedSections.Contains(NewSectionIndex))
				NewSectionIndex = FMath::RandRange(0, rangeMax);
		}	
		SelectedSections.Add(NewSectionIndex);
	}

	// Add a chance to forcibly set the first selected section to be the safe buttons section
	if (!SelectedSections.Contains(SafeButtonSectionIdx))
	{	
		int32 rangeMax = CycleType == EGridSectionType::GST_Column ? GameboardData.Cols - 1 : GameboardData.Rows - 1;
		if (rangeMax < SafeButtonCycleBias)
		{
			SelectedSections[0] = SafeButtonSectionIdx;
		}
	}

	/*
	* Cycle the selected sections
	*/
	for (int32 SectionIdx : SelectedSections)
	{
		// Get  random cycle direction
		EDirection CycleDirection = FMath::RandBool() == true ? EDirection::MD_Forward : EDirection::MD_Forward;
		// Get the buttons in the section
		TArray<ATTBButton*> CycleButtons = CycleType == EGridSectionType::GST_Column ? GetColumn(SectionIdx) : GetRow(SectionIdx);

		int32 Idx = 0;
		for (ATTBButton* b : CycleButtons)
		{
			/*
			* Find out if the button is the button traveling off the board and should wrap to the other side
			* If the button is the last in the array and is traveling forward...
			* If the button is first in the array and is traveling backward...
			*/
			bool bIsLeavingBoard = (CycleDirection == EDirection::MD_Forward && Idx == CycleButtons.Num() - 1)
				|| (CycleDirection == EDirection::MD_Backward && Idx == 0);
			
			/*
			* If the button is leaving the grid, we create a proxy in it's place to move off the grid.
			* The actual button is moved into position to scroll onto the board.
			*/
			if (bIsLeavingBoard)
			{
				/*
				* Spawn the proxy button.  This button scrolls off the grid instead of the actual button
				*/
				ATTBButton* Proxy = GetWorld()->SpawnActor<ATTBButton>(ButtonClass, b->GetActorTransform());
				Proxy->Gameboard = this;
				Proxy->bIsPlaceholder = true;
				Proxy->SetTubeExtension(ETubeAction::TS_Retract);

				FVector ProxyTargetLoc = GetNewButtonLoc(Proxy, CycleType, CycleDirection);
				ProxyTargetLoc.Z = 0.f;

				// Get target rotation based on movement direction
				FRotator TargetRot; // The rotation for a piece thats about to scroll onto the grid.
				if (CycleType == EGridSectionType::GST_Column)
					TargetRot = (CycleDirection == EDirection::MD_Forward) ? FRotator(-90.f, 0.f, 0.f) : FRotator(90.f, 0.f, 0.f);
				else
					TargetRot = (CycleDirection == EDirection::MD_Forward) ? FRotator(0.f, 0.f, 90.f) : FRotator(0.f, 0.f, -90.f);

				// Animate the proxy off the board
				Proxy->MoveButton(FTransform(TargetRot.Quaternion(), ProxyTargetLoc));

				/*
				* The button that is going off grid is moved immediately to the opposite side of the board and is moved onto the grid
				*/
				float Span = (CycleType == EGridSectionType::GST_Column) ? GetBoardLength() : GetBoardWidth();
				Span += ButtonSpacing;
				Span *= (CycleDirection == EDirection::MD_Forward) ? -1.f : 1.f;
				FVector SpanVect = (CycleType == EGridSectionType::GST_Column) ? FVector(Span, 0.f, 0.f) : FVector(0.f, Span, 0.f);
				FVector NewLoc = b->GetActorLocation() + SpanVect;
				NewLoc.Z = 0.f;	// Move the button below the gate

				// Move button to position on the opposite side of the grid in preparation to scroll onto the grid
				b->SetActorLocationAndRotation(NewLoc, (TargetRot * -1.f).Quaternion());
				b->RetractImmediate();	// Start in retracted state
				b->SetTubeExtension(ETubeAction::TS_Extend);	// Extend as it scrolls onto the grid
			}

			// Perform movement
			b->MoveButton(FTransform(FRotator::ZeroRotator, GetNewButtonLoc(b, CycleType, CycleDirection)));

			Idx++;	// Iterate loop index
		}

		// Create a temporary array of the selected col/row
		TArray<ATTBButton*> NewSection = CycleType == EGridSectionType::GST_Column ? GetColumn(SectionIdx) : GetRow(SectionIdx);

		/*
		* Cycle positions of buttons in the 2D grid array
		*/
		if (CycleDirection == EDirection::MD_Forward)
		{
			// Move the last item to the first position in the array
			ATTBButton* Item = NewSection[NewSection.Num() - 1];
			NewSection.RemoveAt(NewSection.Num() - 1);
			NewSection.Insert(Item, 0);
		}
		else
		{
			// Move the first item to the end of the array
			ATTBButton* Item = NewSection[0];
			NewSection.RemoveAt(0);
			NewSection.Add(Item);
		}

		// Assign the temp array back to the grid
		if (CycleType == EGridSectionType::GST_Column)
		{
			ButtonsGrid[SectionIdx].Rows = NewSection;
		}
		else
		{
			for (int32 i = 0; i < NewSection.Num(); i++)
			{
				TArray<ATTBButton*> ExistingRow = ButtonsGrid[i].Rows;
				ExistingRow[SectionIdx] = NewSection[i];
				ButtonsGrid[i].Rows = ExistingRow;
			}
		}

		/* 
			Play gate animations
			Gates are stored in a regular list so we do a little math to relate columns and rows to a linear list. 
		*/
		int32 Gate1Idx = (CycleType == EGridSectionType::GST_Column)
			? SectionIdx + GameboardData.Rows
			: SectionIdx;

		int32 Gate2Idx = (CycleType == EGridSectionType::GST_Column)
			? ((SectionIdx + (GameboardData.Rows * 2)) + ((GameboardData.Cols - SectionIdx) * 2)) - 1
			: ((SectionIdx + GameboardData.Cols) + (((GameboardData.Rows - 1) - SectionIdx) * 2)) + 1;

		Gates[Gate1Idx]->FlipGate();
		Gates[Gate2Idx]->FlipGate();
	}

	if (CycleIteration >= GameboardData.CycleCount)
	{
		GetWorldTimerManager().ClearTimer(CycleTimerHandle);
		OnCycleComplete();
	}
	CycleIteration++;
}

void ATTBGameBoard::OnSafeButtonSet()
{
	SafeButton->ChangeColor(EColorFunction::CC_BlinkAndHold);

	// Start machine noises
	if (MachineNoiseSound)
	{
		MachineNoiseAudioComp = PlaySound(MachineNoiseSound);
		MachineNoiseAudioComp->FadeIn(1.f);
	}

	ATTBHud* Hud = Cast<ATTBHud>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if(Hud)
		Hud->BeginCountdown(CountdownSeconds);

	GetWorldTimerManager().SetTimer(BeginCycleTimerHandle, this, &ATTBGameBoard::BeginCycle, CountdownSeconds);
}

void ATTBGameBoard::BeginCycle()
{
	BoardState = EBoardState::BS_Cycle;
	CycleIteration = 0;
	SafeButton->ChangeColor(EColorFunction::CC_SlowFadeOut);
	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &ATTBGameBoard::SimulateCycle, 1 / GetGameboardTimeScale(), true);
}

void ATTBGameBoard::OnCycleComplete()
{
	BoardState = EBoardState::BS_PostCycle;

	ATTBHud* Hud = Cast<ATTBHud>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if(Hud)
		Hud->OnCycleComplete();

	MachineNoiseAudioComp->FadeOut(1.f, 0.f);	// Stop machine noises

	SetButtonsActive(true);
}

FVector ATTBGameBoard::GetNewButtonLoc(ATTBButton * Button, EGridSectionType SectionType, EDirection Dir)
{
	// Get movement direction as a unit vector based on movement direction
	FVector MoveVect = (SectionType == EGridSectionType::GST_Column) ? GetActorForwardVector() : GetActorRightVector();
	MoveVect *= (Dir == EDirection::MD_Forward) ? 1.f : -1.f;

	// Get the final target location for the button
	FVector TargetLoc = Button->GetActorLocation() + (MoveVect * ButtonSpacing);
	TargetLoc.Z = ButtonHeight;

	return TargetLoc;
}

UAudioComponent* ATTBGameBoard::PlaySound(USoundBase* Sound)
{
	UAudioComponent* AC = NULL;

	if (Sound)
		AC = UGameplayStatics::SpawnSoundAttached(Sound, GetRootComponent());

	return AC;
}

void ATTBGameBoard::ButtonClicked(ATTBButton * ClickedButton)
{
	SetButtonsActive(false);

	if (ClickedButton == SafeButton)
	{
		SafeButton->ChangeColor(EColorFunction::CC_FadeIn);
		GetWorldTimerManager().SetTimer(DelayTimerHandle1, this, &ATTBGameBoard::CompleteStage, .5f);
	}
	else
	{
		FailStage();
	}
}

void ATTBGameBoard::CompleteStage()
{
	for (ATTBButton* b : GetAllButtons())
	{
		b->SetTubeExtension(ETubeAction::TS_Retract);
		b->ChangeColor(EColorFunction::CC_FadeIn);
	}
	PlaySound(MachineWindDownSound);

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());
	if (GS)
		GS->OnStageSucess();
}

void ATTBGameBoard::FailStage()
{
	SafeButton->ChangeColor(EColorFunction::CC_FadeIn);
	for (ATTBButton* b : GetAllButtons())
	{
		if (b != SafeButton)
			b->SetTubeExtension(ETubeAction::TS_Retract);
	}

	DeactivateBoard();
	GetWorldTimerManager().SetTimer(DelayTimerHandle1, this, &ATTBGameBoard::Explode, 1.f);

	ATTBGameState* GS = Cast<ATTBGameState>(GetWorld()->GetGameState());
	if (GS)
		GetWorldTimerManager().SetTimer(DelayTimerHandle2, GS, &ATTBGameState::OnStageFailed, 2.f);
}
