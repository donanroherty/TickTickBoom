

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

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* ClickSound;
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* CycleClickSound;

public:
	UPROPERTY(BlueprintReadOnly)
	class ATTBGameBoard* Gameboard;
	UPROPERTY(BlueprintReadOnly)
	bool bIsPlaceholder;

protected:
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
	UFUNCTION(BlueprintImplementableEvent)
	void DoTubeExtension(ETubeAction NewTubeAction, float Rate);
	UFUNCTION(BlueprintCallable)
	void TubeExtensionCallback(float Val);

	/* Move button to new position on grid */
	void MoveButton(FTransform TargetTransform);
	UFUNCTION(BlueprintImplementableEvent)
	void DoMoveButton(FTransform TargetTransform, float Rate);
	UFUNCTION(BlueprintCallable)
	void MoveButtonTickCallback(float Val, FTransform TargetTransform);
	UFUNCTION(BlueprintCallable)
	void MoveButtonFinishedCallback();

	/* Animate the color of the button */
	UFUNCTION(BlueprintCallable)
	void ChangeColor(EColorFunction InColorFunction);
	UFUNCTION(BlueprintImplementableEvent)
	void DoChangeColor(EColorFunction InColorFunction);
	UFUNCTION(BlueprintCallable)
	void ChangeColorTLCallback(float Val);

	/* Animated button up and down */
	UFUNCTION(BlueprintCallable)
	void PressButton();
	UFUNCTION(BlueprintImplementableEvent)
	void DoPressButton();
	UFUNCTION(BlueprintCallable)
	void PressButtonTickCallback(float Val);
	UFUNCTION(BlueprintCallable)
	void PressButtonFinishedCallback();

	UFUNCTION()
	void OnButtonClicked(UPrimitiveComponent* pComponent, FKey ButtonPressed);

	UFUNCTION(BlueprintCallable)
		UAudioComponent* PlaySound(class USoundBase * Sound);
};
