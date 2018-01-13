#pragma once

#include "CoreMinimal.h"
#include "TickTickBoom.h"
#include "GameFramework/Actor.h"
#include "TTBCameraActor.generated.h"

UCLASS()
class TICKTICKBOOM_API ATTBCameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATTBCameraActor();

	UPROPERTY()
	class USceneComponent* RootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	float CameraPitch;

	UPROPERTY(BlueprintReadOnly)
	AActor* TargetActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ZoomMargin;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateCameraDistanceAndLocation(FBox2D BBox, float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SetFocusTarget(AActor* NewTargetActor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool Get2DBoundingRectangle(AActor* InActor, FBox2D &OutBox);

	bool GetTargetSpringArmLength(APlayerController* PlayerController, FBox2D BBox, float &OutLength);
	bool GetTargetActorLocation(APlayerController* PlayerController, FBox2D BBox, FVector &OutLocation);

	

	/* Returns all points of the 3D bounding box */
	TArray<FVector> GetBoundingBoxPoints(FVector Origin, FVector Extents);
};
