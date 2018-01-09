

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TTBGameBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonGridUpdated);

USTRUCT(BlueprintType)
struct FGameboardData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Cols;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 Rows;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 CycleCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 SectionsMovePerCycle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TimeScale;
};

USTRUCT(BlueprintType)
struct FButtonGrid
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<class ATTBButton*> Rows;
};

UENUM(BlueprintType)
enum class EBoardState : uint8
{
	BS_PreCycle		UMETA(DisplayName = "PreCycle"),
	BS_Cycle 		UMETA(DisplayName = "Cycle"),
	BS_PostCycle	UMETA(DisplayName = "PostCycle")
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	MD_Forward		UMETA(DisplayName = "Forward"),
	MD_Backward		UMETA(DisplayName = "Backward"),
};

UENUM(BlueprintType)
enum class EGridSectionType : uint8
{
	GST_Column		UMETA(DisplayName = "Column"),
	GST_Row			UMETA(DisplayName = "Row"),
};

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

	UPROPERTY(BlueprintReadWrite)
		TArray<class ATTBButton*> Buttons;

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

	//UFUNCTION(BlueprintCallable, Category = Gameboard)
		//void CycleGridSection(int32 Idx, EDirection Dir, EGridSectionType SectionType);
};
