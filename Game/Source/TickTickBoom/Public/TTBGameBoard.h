

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "TTBGameBoard.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonGridUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonsDeactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FButtonsActivated);

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

UCLASS()
class TICKTICKBOOM_API ATTBGameBoard : public AActor
{
	GENERATED_BODY()

public:
	/* Data used to build the gameboard and ruleset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	FGameboardData GameboardData;

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


	/* Called when the button grid is updated */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonGridUpdated OnButtonGridUpdated;

	/* Called when the buttons are deactivated */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonsDeactivated OnButtonsDeactivated;

	/* Called when the buttons are activated */
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonsActivated OnButtonsActivated;

public:	
	// Sets default values for this actor's properties
	ATTBGameBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnSafeButtonSet();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void BeginCycle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void CycleOnTimer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnCycleComplete();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ButtonClicked(class ATTBButton* ClickedButton);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnLevelSuccess();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnLevelFailure();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void Explode();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
		float GetBoardWidth();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
		float GetBoardLength();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Gameboard)
		float GetGameboardTimeScale();
};
