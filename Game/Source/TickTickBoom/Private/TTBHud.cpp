

#include "TTBHud.h"
#include "UserWidget.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"


ATTBHud::ATTBHud()
{
}

void ATTBHud::BeginPlay()
{
	if(ShortCircuitWidgetClass)
	{
		ShortCircuitWidget = CreateWidget<UUserWidget>(GetWorld(), ShortCircuitWidgetClass);
		if (ShortCircuitWidget)
			ShortCircuitWidget->AddToViewport();
	}

	if(HighScoreWidgetClass)
	{
		HighScoreWidget = CreateWidget<UUserWidget>(GetWorld(), HighScoreWidgetClass);
		if (HighScoreWidget)
			HighScoreWidget->AddToViewport();
	}
}

void ATTBHud::OpenPauseMenu()
{
	PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);

	if (PauseWidget)
		PauseWidget->AddToViewport();
}

void ATTBHud::ClosePauseMenu()
{
	if (PauseWidget)
		PauseWidget->RemoveFromParent();
}

void ATTBHud::ShowTutorial()
{
	if (TutorialWidgetClass)
	{
		TutorialWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialWidgetClass);
		if (TutorialWidget)
			TutorialWidget->AddToViewport();
	}
}

void ATTBHud::OnStageComplete()
{
	if (StageCompleteWidgetClass)
	{
		StageCompleteWidget = CreateWidget<UUserWidget>(GetWorld(), StageCompleteWidgetClass);
		if (StageCompleteWidget)
			StageCompleteWidget->AddToViewport();
	}
}

void ATTBHud::OnDeath()
{
	if (DeathMenuWidgetClass)
	{
		DeathMenuWidget = CreateWidget<UUserWidget>(GetWorld(), DeathMenuWidgetClass);
		if (DeathMenuWidget)
			DeathMenuWidget->AddToViewport();
	}
}

void ATTBHud::OnCycleComplete()
{
	if (DisarmWidgetClass)
	{
		DisarmWidget = CreateWidget<UUserWidget>(GetWorld(), DisarmWidgetClass);
		if (DisarmWidget)
			DisarmWidget->AddToViewport();
	}
}

void ATTBHud::OnGameComplete()
{
	if (GameCompleteWidgetClass)
	{
		GameCompleteWidget = CreateWidget<UUserWidget>(GetWorld(), GameCompleteWidgetClass);
		if (GameCompleteWidget)
			GameCompleteWidget->AddToViewport();
	}
}

void ATTBHud::BeginCountdown(int32 CountdownTime)
{
	if (CountdownWidgetClass)
	{
		CountdownWidget = CreateWidget<UUserWidget>(GetWorld(), CountdownWidgetClass);
		if (CountdownWidget)
			CountdownWidget->AddToViewport();
	}
}
