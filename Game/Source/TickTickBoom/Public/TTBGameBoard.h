

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

UCLASS()
class TICKTICKBOOM_API ATTBGameBoard : public AActor
{
	GENERATED_BODY()

public:
	/* Data used to build the gameboard and ruleset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn="true"))
	FGameboardData GameboardData;


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


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnShortCircuit();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ActivateBoard();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
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
};
