

#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "GameFramework/HUD.h"
#include "TTBHud.generated.h"

/**
 * 
 */
UCLASS()
class ATTBHud : public AHUD
{
	GENERATED_BODY()

	ATTBHud();

	public:

	/* Widget classes */
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> PauseWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> ShortCircuitWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> HighScoreWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> TutorialWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> StageCompleteWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> DeathMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> CountdownWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> DisarmWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UUserWidget> GameCompleteWidgetClass;

	/* Spawned Widgets */
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* PauseWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* ShortCircuitWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* HighScoreWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* TutorialWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* StageCompleteWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* DeathMenuWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* CountdownWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* DisarmWidget;
	UPROPERTY(BlueprintReadOnly)
		class UUserWidget* GameCompleteWidget;

	void BeginPlay();

	/* Open pause menu */
	UFUNCTION(BlueprintCallable)
	void OpenPauseMenu();

	/* Close pause menu */
	UFUNCTION(BlueprintCallable)
	void ClosePauseMenu();
	
	/* Shows tutorial widget on screen */
	UFUNCTION(BlueprintCallable)
	void ShowTutorial();

	/* Shows stage complete widget on screen */
	UFUNCTION(BlueprintCallable)
	void OnStageComplete();

	/* Shows death widget on screen */
	UFUNCTION(BlueprintCallable)
	void OnDeath();

	/* Shows disarm widget on screen */
	UFUNCTION(BlueprintCallable)
	void OnCycleComplete();

	/* Shows game complete widget on screen */
	UFUNCTION(BlueprintCallable)
	void OnGameComplete();

	/* Shows countdown widget on screen and feeds it countdown time */
	UFUNCTION(BlueprintCallable)
	void BeginCountdown(int32 CountdownTime);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHighScoreWidget();
};
