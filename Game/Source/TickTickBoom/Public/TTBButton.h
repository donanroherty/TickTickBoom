

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBTypes.h"
#include "TTBButton.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBButton : public AActor
{
	GENERATED_BODY()
	
protected:	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* CasingMesh;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* TubeMesh;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ButtonMesh;

	class UTimelineComponent* MovementTimeline;
	class UTimelineComponent* TubeTimeline;
	class UTimelineComponent* ColorTimeline;
	class UTimelineComponent* PressButtonTimeline;

	/* Timeline Curves */
	UPROPERTY()
	class UCurveFloat* LinearCurve;
	UPROPERTY()
	class UCurveFloat* BlinkCurve;
	UPROPERTY()
	class UCurveFloat* BlinkAndHoldCurve;
	UPROPERTY()
	class UCurveFloat* LinearReverseCurve;
	UPROPERTY()
	class UCurveFloat* SlowFadeOutCurve;
	UPROPERTY()
	class UCurveFloat* ValleyCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class USoundBase* ClickSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class USoundBase* CycleClickSound;

public:
	class ATTBGameBoard* Gameboard;
	bool bIsPlaceholder;

protected:
	UPROPERTY(EditDefaultsOnly)
	float BlinkTime;
	UPROPERTY(EditDefaultsOnly)
	float RetractedButtonZ;

	UMaterialInstanceDynamic* ButtonMaterialInstance;
	float ButtonTargetHeight;
	FTransform MoveTargetXForm;

public:
	ATTBButton();

	virtual void BeginPlay()override;
	
	/* Put button into retracted state.  Does not animate */
	UFUNCTION(BlueprintCallable)
	void RetractImmediate();

	/* Extend the tube up and down */
	UFUNCTION(BlueprintCallable)
	void SetTubeExtension(ETubeAction NewTubeAction);
	UFUNCTION()
	void SetTubeExtensionTLCallback(float Val);

	/* Move button to new position on grid */
	UFUNCTION(BlueprintCallable)
	void MoveButton(FTransform TargetTransform);
	UFUNCTION()
	void MoveButtonTickCallback(float Val);
	UFUNCTION()
	void MoveButtonFinishedCallback();

	/* Animate the color of the button */
	UFUNCTION(BlueprintCallable)
	void ChangeColor(EColorFunction InColorFunction);
	UFUNCTION()
	void ChangeColorTLCallback(float Val);

	/* Animated button up and down */
	UFUNCTION(BlueprintCallable)
	void PressButton();
	UFUNCTION()
	void PressButtonTickCallback(float Val);
	UFUNCTION()
	void PressButtonFinishedCallback();

	UFUNCTION()
	void OnButtonClicked(UPrimitiveComponent* pComponent, FKey ButtonPressed);

	UFUNCTION(BlueprintCallable)
		UAudioComponent* PlaySound(class USoundBase * Sound);
};
