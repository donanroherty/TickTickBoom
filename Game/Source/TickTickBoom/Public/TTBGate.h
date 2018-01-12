#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTBGate.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBGate : public AActor
{
	GENERATED_BODY()

public:
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* GateMesh;

	UPROPERTY(BlueprintReadWrite)
	class ATTBGameBoard* Gameboard;

	class UTimelineComponent* FlipGateTimeline;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* FlipGateCurve;

	// Sets default values for this actor's properties
	ATTBGate();

public:
	/* Animate the gate opening and closing */
	void FlipGate();
	UFUNCTION()
		void FlipGateTLCallback(float Val);
};
