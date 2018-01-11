

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBTypes.h"
#include "TTBButton.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBButton : public AActor
{
	GENERATED_BODY()
	
public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* Casing;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* Tube;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* Button;

	UPROPERTY(VisibleAnywhere)
		class UTimelineComponent* MovementTimeline;
	UPROPERTY(VisibleAnywhere)
		class UTimelineComponent* TubeTimeline;
	UPROPERTY(VisibleAnywhere)
		class UTimelineComponent* ColorTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class USoundBase* ClickSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class USoundBase* CycleClickSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = "true"))
	class ATTBGameBoard* Gameboard;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float BlinkTime;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float RetractedButtonZ;

	UPROPERTY(BlueprintReadWrite)
		float ButtonTargetHeight;

	UPROPERTY(BlueprintReadWrite)
		FTransform MoveTargetXForm;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
		bool bIsPlaceholder;

	UPROPERTY(BlueprintReadWrite)
		bool bIsActive;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		bool bDrawDebug;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		UMaterialInstanceDynamic* ButtonMaterialInstance;

	/* One second, linear curve.  Scaled and used for various timelines */
	UPROPERTY()
	class UCurveFloat* LinearCurve;
	/* 0.3 second curve for quick On/Off animation */
	UPROPERTY()
		class UCurveFloat* BlinkCurve;
	UPROPERTY()
		class UCurveFloat* BlinkAndHoldCurve;
	UPROPERTY()
		class UCurveFloat* LinearReverseCurve;
	UPROPERTY()
		class UCurveFloat* SlowFadeOutCurve;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	ATTBButton();

	UFUNCTION(BlueprintCallable)
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateDebug();

	UFUNCTION(BlueprintCallable)
	void RetractButton();
	UFUNCTION(BlueprintCallable)
	void ExtendButton();
	
	UFUNCTION(BlueprintCallable)
	void RetractImmediate();

	UFUNCTION(BlueprintCallable)
	void HandleExtension(ETubeAction NewTubeAction);
	UFUNCTION()
	void ExtensionTLCallback(float Val);

	UFUNCTION(BlueprintCallable)
	void MoveToNewSlot(FTransform TargetTransform);
	UFUNCTION()
	void MovementTLTickCallback(float Val);
	UFUNCTION()
	void MovementTLFinishedCallback();

	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySound(class USoundBase * Sound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnButtonClicked();

	UFUNCTION(BlueprintCallable)
	void HandleColorTimeline(EColorFunction InColorFunction);
	UFUNCTION()
	void SetColorTimelineCallback(float Val);
};
