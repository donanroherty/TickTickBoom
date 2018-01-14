

#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "GameFramework/Actor.h"
#include "TTBTypes.h"
#include "TTBGameBoard.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBGameBoard : public AActor
{
	GENERATED_BODY()

public:
	// Components
	UPROPERTY()
	class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* MachineNoiseAudioComp;
	UPROPERTY(VisibleAnywhere)
	class USpotLightComponent* SpotLight;

	/* Data used to build the gameboard */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameboardData GameboardData;

	/* The safe button the player should track and press */
	class ATTBButton* SafeButton;
	/* The gates surround the button grid */
	TArray<class ATTBGate*> Gates;
	/* The grid of buttons on the gameboard */
	TArray<FButtonGrid> ButtonsGrid;

	/* Distance between buttons.  Should equate to the dimensions of the button mesh */
	UPROPERTY(EditDefaultsOnly)
	float ButtonSpacing;
	/* Z position of a button when on the grid */
	UPROPERTY(EditDefaultsOnly)
	float ButtonHeight;

	/* The number of loops when selecting a random safe button */
	UPROPERTY(BlueprintReadWrite)
	int32 ButtonChoiceMaxIterations;
	/* How many attempts to try select a section with the safe button in it */
	UPROPERTY(EditDefaultsOnly)
	int32 SafeButtonCycleBias;

	// Geometry
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBButton> ButtonClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBGate> GateClass;
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* CornerMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* WallMesh;

	// Sounds
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* MachineNoiseSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* PlayerHurtSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* MachineWindDownSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* ShortCircuitSound;

	// Effects
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ExplosionParticles;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* SmokeParticles;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* FireParticles;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* SparksParticles;

public:
	bool bButtonsActive;
	EBoardState BoardState;

private:
	bool bBoardIsActive;
	int32 ButtonChoiceIteration;
	int32 CycleIteration;

	// Timer handles
	FTimerHandle BeginCycleTimerHandle;
	FTimerHandle DelayTimerHandle1;
	FTimerHandle DelayTimerHandle2;
	FTimerHandle ChooseSafeButtonTimerHandle;
	FTimerHandle CycleTimerHandle;

public:	
	// Sets default values for this actor's properties
	ATTBGameBoard();

public:
	/* Build the gameboard to GameboardData spec */
	void BuildGameboard();

	/* Called when player tries to short circuit the board */
	UFUNCTION()
	void OnShortCircuit();

	/* Set board active */
	void ActivateBoard();

	/* Set board and buttons inactive */
	void DeactivateBoard();

	/* Initiates safe button selection */
	void BeginPreCycle();

	/* Gets a random button to be the safe button */
	void ChooseSafeButton();

	/* Wraps up safe button selection and calls BeginCycle()  */
	void OnSafeButtonSet();

	/* Prepares board to cycle and calls the cycle simulation in a loop */
	void BeginCycle();

	/* Called when the board is finished cycling */
	void OnCycleComplete();

	/* Get the position a button should cycle to */
	FVector GetNewButtonLoc(class ATTBButton* Button, EGridSectionType SectionType, EDirection Dir);

	/* Play audio */
	UAudioComponent* PlaySound(USoundBase* Sound);
	
	/* Called when the player presses a button */
	void ButtonClicked(class ATTBButton* ClickedButton);
	
	/* Called when the player completes a stage */
	void CompleteStage();
	/* Called when a player fails a stage */
	void FailStage();

	/* Plays sounds and effects when the gameboard explodes */
	void Explode();

	/* Get total width of the board */
	float GetBoardWidth();

	/* Get total length of the board */
	float GetBoardLength();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	float GetGameboardTimeScale();

	/* Get a random button in the grid */
	class ATTBButton* GetRandButton();

	/* Get all buttons at a 2d grid index */
	class ATTBButton* GetButtonByIndex(int32 Col, int32 Row);

	/* Gets all buttons in a grid column */
	TArray<class ATTBButton*> GetColumn(int32 Index);
	/* Gets all buttons in a grid row */
	TArray<class ATTBButton*> GetRow(int32 Index);
	/* Get all buttons on the board */
	TArray<class ATTBButton*> GetAllButtons();

	/* Set active state of all buttons on the board */
	void SetButtonsActive(bool bNewActive);

	/* Returns a button by grid index */
	bool GetButtonIndex(ATTBButton* Button, int32 &OutCol, int32 &OutRow);

	/* Performs a single cycle, playing all animations and updating the grid */
	void SimulateCycle();
};
