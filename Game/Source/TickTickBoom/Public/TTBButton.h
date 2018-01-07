

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBButton.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATTBButton();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
	class ATTBGameBoard* Gameboard;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float ButtonExtendedHeight;

	UPROPERTY(BlueprintReadWrite)
		float ButtonTargetHeight;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
		bool bIsPlaceholder;

	UPROPERTY(BlueprintReadWrite)
		bool bIsActive;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool bDrawDebug;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UMaterialInstanceDynamic* ButtonMaterialInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnButtonsActivated();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnButtonsDeactivated();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateDebug();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void RetractButton();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ExtendButton();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void RetractImmediate();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void MoveToNewSlot(FTransform TargetTransform);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FlashButton(bool bFlashOnOff);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FadeSafeIn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void FadeSafeOut();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SlowFadeOut();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnButtonClicked();
};
