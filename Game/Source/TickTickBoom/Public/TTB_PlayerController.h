

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTB_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ATTB_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	TSubclassOf<class ATTB_CameraActor> CameraActorClass;
	
	UPROPERTY(BlueprintReadWrite)
	class ATTB_CameraActor* CameraActor;

	void BeginPlay();

	//Spawns a new camera actor and sets it as the viewtarget
	void SpawnCamera();

	UFUNCTION(BlueprintCallable)
	void SetCameraFocusTarget(AActor* TargetActor);
};
