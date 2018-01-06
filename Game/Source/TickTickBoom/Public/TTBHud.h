

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TTBHud.generated.h"

/**
 * 
 */
UCLASS()
class ATTBHud : public AHUD
{
	GENERATED_BODY()

	public:

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
	
	UFUNCTION(BlueprintCallable)
		void ShowTutorial();

	UFUNCTION(BlueprintCallable)
		void OnStageComplete();

	UFUNCTION(BlueprintCallable)
		void OnDeath();

	UFUNCTION(BlueprintCallable)
		void OnCycleComplete();

	UFUNCTION(BlueprintCallable)
		void OnGameComplete();

	UFUNCTION(BlueprintCallable)
		void BeginCountdown(int32 CountdownTime);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateHighScoreWidget();
};
