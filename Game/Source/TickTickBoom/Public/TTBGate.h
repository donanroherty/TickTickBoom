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
	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* InteriorLight;

	UPROPERTY(BlueprintReadWrite)
	class ATTBGameBoard* Gameboard;

	ATTBGate();

public:
	/* Animate the gate opening and closing */
	void FlipGate();
	UFUNCTION(BlueprintImplementableEvent)
	void DoFlipGate(float Rate);
	UFUNCTION(BlueprintCallable)
	void FlipGateTLCallback(float Val);
};
