#include "TTBButton.h"
#include "TTBGameMode.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "TTBGameBoard.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/TimelineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ATTBButton::ATTBButton()
{
	RetractedButtonZ = - 2.f;
	ButtonTargetHeight = 0.f;
	bIsPlaceholder = false;
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	/* Create geometry components */
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing"));
	CasingMesh->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
	TubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tube"));
	TubeMesh->AttachToComponent(CasingMesh, FAttachmentTransformRules::KeepRelativeTransform);
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	ButtonMesh->AttachToComponent(TubeMesh, FAttachmentTransformRules::KeepRelativeTransform);

	/* Setup click event for button */
	ButtonMesh->OnClicked.AddUniqueDynamic(this, &ATTBButton::OnButtonClicked);
}

void ATTBButton::BeginPlay()
{
	Super::BeginPlay();
	// Create a material instance to animate button color
	ButtonMaterialInstance = UMaterialInstanceDynamic::Create(ButtonMesh->GetMaterial(0), this);
	ButtonMesh->SetMaterial(0, ButtonMaterialInstance);
}

void ATTBButton::RetractImmediate()
{
	TubeMesh->SetRelativeLocation(FVector(0.f, 0.f, RetractedButtonZ));
}

void ATTBButton::SetTubeExtension(ETubeAction NewTubeAction)
{
	ButtonTargetHeight = (NewTubeAction == ETubeAction::TS_Extend) ? 0.f : RetractedButtonZ;	// Set target height based on if this is a retraction or extension
	DoTubeExtension(NewTubeAction, Gameboard->GetGameboardTimeScale());
}

void ATTBButton::TubeExtensionCallback(float Val)
{
	float NewZ = FMath::Lerp(TubeMesh->RelativeLocation.Z, ButtonTargetHeight, Val);
	TubeMesh->SetRelativeLocation(FVector(0.f, 0.f, NewZ));
}

void ATTBButton::MoveButton(FTransform TargetTransform)
{
	PlaySound(CycleClickSound);
	DoMoveButton(TargetTransform, Gameboard->GetGameboardTimeScale());
}

void ATTBButton::MoveButtonTickCallback(float Val, FTransform TargetTransform)
{
	FVector NewLoc = FMath::Lerp(GetActorLocation(), TargetTransform.GetLocation(), Val);
	FRotator NewRot = FMath::Lerp(GetActorRotation(), TargetTransform.GetRotation().Rotator(), Val);
	SetActorLocationAndRotation(NewLoc, NewRot);
}

void ATTBButton::MoveButtonFinishedCallback()
{
	if (bIsPlaceholder)
		Destroy();
}

void ATTBButton::ChangeColor(EColorFunction InColorFunction)
{
	PlaySound(ClickSound);
	DoChangeColor(InColorFunction);
	return;
}

void ATTBButton::ChangeColorTLCallback(float Val)
{
	ButtonMaterialInstance->SetScalarParameterValue(TEXT("ColorBlend"), Val);
}

void ATTBButton::PressButton()
{	
	if (!Gameboard->bButtonsActive)
		return;

	// Immediately deactivate all buttons so player cant click multiple buttons	
	Gameboard->SetButtonsActive(false);

	DoPressButton();
}

void ATTBButton::PressButtonTickCallback(float Val)
{
	float NewZ = FMath::Lerp(ButtonMesh->RelativeLocation.Z, Val, 1);
	ButtonMesh->SetRelativeLocation(FVector(0.f, 0.f, NewZ));
}

void ATTBButton::PressButtonFinishedCallback()
{
	Gameboard->ButtonClicked(this);
}

void ATTBButton::OnButtonClicked(UPrimitiveComponent* pComponent, FKey ButtonPressed)
{
	PressButton();
}

UAudioComponent* ATTBButton::PlaySound(USoundBase* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, GetRootComponent());
	}
	return AC;
}