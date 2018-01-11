

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBTypes.h"
#include "TTBGameBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonGridUpdated);

UCLASS()
class TICKTICKBOOM_API ATTBGameBoard : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USceneComponent* RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UAudioComponent* MachineNoiseAudioComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USpotLightComponent* SpotLight;

	/* Data used to build the gameboard and ruleset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	FGameboardData GameboardData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBButton> ButtonClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ATTBGate> GateClass;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* CornerMesh;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* WallMesh;

	/* The safe button the player should track and press */
	UPROPERTY(BlueprintReadWrite)
	class ATTBButton* SafeButton;

	UPROPERTY(BlueprintReadWrite)
	bool bBoardIsActive;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float ButtonSpacing;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float ButtonHeight;

	UPROPERTY(BlueprintReadWrite)
	int32 SafeButtonChoiceIterations;

	/* Iterated each loop of ChooseSafeButton() to control max iterations */
	int32 SafeButtonCurrentIteration;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 CountdownSeconds;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 SafeButtonCycleBias;

	UPROPERTY(BlueprintReadWrite)
	EBoardState BoardState;

	UPROPERTY(BlueprintReadWrite)
	TArray<class ATTBGate*> Gates;

	UPROPERTY(BlueprintReadWrite)
	TArray<FButtonGrid> ButtonsGrid;

	/* Delegates */
	/* Called when the button grid is updated */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonGridUpdated OnButtonGridUpdated;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* MachineNoiseSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* PlayerHurtSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundBase* MachineWindDownSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UParticleSystem* ExplosionParticles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UParticleSystem* SmokeParticles;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UParticleSystem* FireParticles;

	FTimerHandle BeginCycleTimerHandle;
	FTimerHandle DelayTimerHandle1;
	FTimerHandle DelayTimerHandle2;
	FTimerHandle ChooseSafeButtonTimerHandle;

public:	
	// Sets default values for this actor's properties
	ATTBGameBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void BuildGameboard();

	UFUNCTION(BlueprintCallable)
	void OnShortCircuit();

	UFUNCTION(BlueprintCallable)
	void ActivateBoard();

	UFUNCTION(BlueprintCallable)
	void DeactivateBoard();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeginPreCycle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChooseSafeButton();

	UFUNCTION(BlueprintCallable)
	void OnSafeButtonSet();

	UFUNCTION(BlueprintCallable)
	void BeginCycle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CycleOnTimer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Cycle();

	UFUNCTION(BlueprintCallable)
	void OnCycleComplete();

	UFUNCTION(BlueprintCallable)
	void PrepCycle(class ATTBButton* Button, EGridSectionType SectionType, EDirection Dir, bool bIsLeavingGrid);

	FVector GetNewButtonLoc(class ATTBButton* Button, EGridSectionType SectionType, EDirection Dir);

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySound(USoundBase* Sound);
	
	/* Called when the player presses a button */
	UFUNCTION(BlueprintCallable)
	void ButtonClicked(class ATTBButton* ClickedButton);

	UFUNCTION(BlueprintCallable)
	void OnLevelSuccess();

	UFUNCTION(BlueprintCallable)
	void OnLevelFailure();

	/* Plays sounds and effects when the gameboard explodes */
	UFUNCTION(BlueprintCallable)
	void Explode();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	float GetBoardWidth();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	float GetBoardLength();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	float GetGameboardTimeScale();

	UFUNCTION(BlueprintCallable, Category = Gameboard)
	class ATTBButton* GetRandButton();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	class ATTBButton* GetButtonByIndex(int32 Col, int32 Row);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	TArray<class ATTBButton*> GetColumn(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	TArray<class ATTBButton*> GetRow(int32 Index);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	TArray<class ATTBButton*> GetAllButtons();

	UFUNCTION(BlueprintCallable, Category = Buttons)
	void ActivateButtons();

	UFUNCTION(BlueprintCallable, Category = Buttons)
	void DeactivateButtons();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	class ATTBHud* GetHud();

	/* Returns a button by grid index */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	bool GetButtonIndex(ATTBButton* Button, int32 &OutCol, int32 &OutRow);

	// Find out if the button at index is the button traveling off the board and should wrap to the other side
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
	bool IsButtonTravelingOffBoard(TArray<ATTBButton*> ButtonArray, int32 Index, EDirection Dir);

	//UFUNCTION(BlueprintCallable, Category = Gameboard)
		//void CycleButtons();

	UFUNCTION(BlueprintCallable, Category = Gameboard)
		void CycleGridSection(int32 Idx, EDirection Dir, EGridSectionType SectionType);
};
