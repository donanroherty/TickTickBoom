

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTB_CameraActor.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTB_CameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATTB_CameraActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetFocusTarget(AActor* TargetActor);
};
