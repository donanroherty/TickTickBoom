

#include "TTBCameraActor.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ATTBCameraActor::ATTBCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ZoomMargin = 50.f;
	CameraPitch = -60.f;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SpringArm->SetRelativeRotation(FRotator(CameraPitch, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->TargetArmLength = 200.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepRelativeTransform);
	Camera->SetFieldOfView(50.f);
}

// Called when the game starts or when spawned
void ATTBCameraActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATTBCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FBox2D BBox;
	if (Get2DBoundingRectangle(TargetActor, BBox))
	{
		UpdateCameraDistanceAndLocation(BBox, DeltaTime);
	}	
}

void ATTBCameraActor::UpdateCameraDistanceAndLocation(FBox2D BBox, float DeltaTime)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return;
	
	float TargetArmLength;
	FVector TargetLoc;

	GetTargetSpringArmLength(PC, BBox, TargetArmLength);
	GetTargetActorLocation(PC, BBox, TargetLoc);

	float NewDist = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, 2.f);
	FVector NewLoc = FMath::VInterpTo(GetActorLocation(), TargetLoc, DeltaTime, 20.f);

	/* Set distance and location */
	SpringArm->TargetArmLength = NewDist;
	SetActorLocation(NewLoc);
}

void ATTBCameraActor::SetFocusTarget(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

bool ATTBCameraActor::Get2DBoundingRectangle(AActor* InActor, FBox2D &OutBox)
{
	if (!InActor)
		return false;

	// Get the 3D BB points of the actor
	FVector Origin, Extent;
	InActor->GetActorBounds(false, Origin, Extent);
	TArray<FVector> BBPoints = GetBoundingBoxPoints(Origin, Extent);

	TArray<FVector2D> SSPoints;
	FVector2D Total;

	// Get the screen space positions of each BBPoint
	for (FVector BBLoc : BBPoints)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		FVector2D ScreenLoc;

		if (PC && PC->ProjectWorldLocationToScreen(BBLoc, ScreenLoc, false))
		{
			SSPoints.Add(ScreenLoc);
			Total += ScreenLoc;
		}
		else
		{
			return false;
		}
	}
	// Box2d Min needs an initial value as no new value will be smaller that the default (0, 0) value
	FBox2D Box;
	Box.Min = SSPoints[0];
	Box.Max = FVector2D::ZeroVector;

	// Of all 2d SS points, get the top-left-most point and bottom-right-most point
	for (FVector2D BBLoc : SSPoints)
	{
		FVector2D Min = FVector2D(FMath::Min(Box.Min.X, BBLoc.X), FMath::Min(Box.Min.Y, BBLoc.Y));	// top-left
		FVector2D Max = FVector2D(FMath::Max(Box.Max.X, BBLoc.X), FMath::Max(Box.Max.Y, BBLoc.Y));	// bottom-right
		Box = FBox2D(Min, Max);
	}

	OutBox = Box;
	return true;
}

bool ATTBCameraActor::GetTargetSpringArmLength(APlayerController* PlayerController, FBox2D BBox, float &OutLength)
{
	if (!PlayerController)
		return false;

	/* Get new distance */
	FVector2D diff = BBox.Max - BBox.Min;
	diff.X += ZoomMargin;
	diff.Y += ZoomMargin;

	int32 VPSizeX, VPSizeY;
	PlayerController->GetViewportSize(VPSizeX, VPSizeY);

	// Choose the axis with the largest overlap to be a multiplier for the camera distance.  This ensures the object fits in X and Y screen space.
	OutLength = FMath::Max(diff.X / VPSizeX, diff.Y / VPSizeY) * SpringArm->TargetArmLength;

	return true;
}

bool ATTBCameraActor::GetTargetActorLocation(APlayerController* PlayerController, FBox2D BBox, FVector &OutLocation)
{
	int32 VPSizeX, VPSizeY;
	PlayerController->GetViewportSize(VPSizeX, VPSizeY);

	FVector2D BBScreenCenter = (BBox.Min + BBox.Max) / 2;
	FVector BBWorldCenter, BBWorldDir;
	if (!PlayerController->DeprojectScreenPositionToWorld(BBScreenCenter.X, BBScreenCenter.Y, BBWorldCenter, BBWorldDir))
		return false;

	FVector2D VPScreenCenter = FVector2D(VPSizeX, VPSizeY) / 2;
	FVector VPWorldCenter, VPWorldDir;
	if(!PlayerController->DeprojectScreenPositionToWorld(VPScreenCenter.X, VPScreenCenter.Y, VPWorldCenter, VPWorldDir))
		return false;

	FVector Diff = BBWorldCenter - VPWorldCenter;
	OutLocation = GetActorLocation() + Diff;

	return true;
}

TArray<FVector> ATTBCameraActor::GetBoundingBoxPoints(FVector Origin, FVector Extents)
{
	TArray<FVector> Points;
	Points.Add(FVector(1.f, 1.f, 1.f) * Extents + Origin);
	Points.Add(FVector(-1.f, 1.f, 1.f) * Extents + Origin);
	Points.Add(FVector(-1.f, -1.f, 1.f) * Extents + Origin);
	Points.Add(FVector(1.f, -1.f, 1.f) * Extents + Origin);
	Points.Add(FVector(1.f, 1.f, -1.f) * Extents + Origin);
	Points.Add(FVector(-1.f, 1.f, -1.f) * Extents + Origin);
	Points.Add(FVector(-1.f, -1.f, -1.f) * Extents + Origin);
	Points.Add(FVector(1.f, -1.f, -1.f) * Extents + Origin);

	return Points;
}