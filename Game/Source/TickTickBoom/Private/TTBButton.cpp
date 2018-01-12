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
	BlinkTime = .3f;
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

	/* Load curves */
	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearCurve_Asset(TEXT("CurveFloat'/Game/Curves/Linear_Curve.Linear_Curve'"));
	check(LinearCurve_Asset.Succeeded());
	LinearCurve = LinearCurve_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> LinearReverseCurve_Asset(TEXT("CurveFloat'/Game/Curves/LinearReverse_Curve.LinearReverse_Curve'"));
	check(LinearReverseCurve_Asset.Succeeded());
	LinearReverseCurve = LinearReverseCurve_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> BlinkCurve_Asset(TEXT("CurveFloat'/Game/Curves/Blink_Curve.Blink_Curve'"));
	check(BlinkCurve_Asset.Succeeded());
	BlinkCurve = BlinkCurve_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> BlinkAndHoldCurve_Asset(TEXT("CurveFloat'/Game/Curves/BlinkAndHold_Curve.BlinkAndHold_Curve'"));
	check(BlinkAndHoldCurve_Asset.Succeeded());
	BlinkAndHoldCurve = BlinkAndHoldCurve_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> SlowFadeOutCurve_Asset(TEXT("CurveFloat'/Game/Curves/SlowFadeOut_Curve.SlowFadeOut_Curve'"));
	check(SlowFadeOutCurve_Asset.Succeeded());
	SlowFadeOutCurve = SlowFadeOutCurve_Asset.Object;

	static ConstructorHelpers::FObjectFinder<UCurveFloat> ValleyCurve_Asset(TEXT("CurveFloat'/Game/Curves/FlipGate_Curve.FlipGate_Curve'"));
	check(ValleyCurve_Asset.Succeeded());
	ValleyCurve = ValleyCurve_Asset.Object;

	// Load sound assets.
	static ConstructorHelpers::FObjectFinder<USoundCue> ClickSound_Asset(TEXT("SoundCue'/Game/Sounds/SC_Click.SC_Click'"));
	check(ClickSound_Asset.Succeeded())
	ClickSound = ClickSound_Asset.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CycleClickSound_Asset(TEXT("SoundCue'/Game/Sounds/SC_CycleClicks.SC_CycleClicks'"));
	check(CycleClickSound_Asset.Succeeded())
	CycleClickSound = ClickSound_Asset.Object;

	// Create timelines
	MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MovementTimeline"));
	TubeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TubeTimeline"));
	ColorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ColorTimeline"));
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

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("SetTubeExtensionTLCallback") });
	TubeTimeline->AddInterpFloat(LinearCurve, tickCallback, FName{ TEXT("TubeAnimation") });
	TubeTimeline->SetPlayRate(Gameboard->GetGameboardTimeScale());

	TubeTimeline->PlayFromStart();
}

void ATTBButton::SetTubeExtensionTLCallback(float Val)
{
	float NewZ = FMath::Lerp(TubeMesh->RelativeLocation.Z, ButtonTargetHeight, Val);
	TubeMesh->SetRelativeLocation(FVector(0.f, 0.f, NewZ));
}

void ATTBButton::MoveButton(FTransform TargetTransform)
{
	PlaySound(CycleClickSound);
	MoveTargetXForm = TargetTransform;

	FOnTimelineEventStatic finishedCallback{};	// Called when the timeline completes
	finishedCallback.BindUFunction(this, FName{TEXT("MoveButtonFinishedCallback")});
	MovementTimeline->SetTimelineFinishedFunc(finishedCallback);

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("MoveButtonTickCallback") });

	MovementTimeline->AddInterpFloat(LinearCurve, tickCallback, FName{ TEXT("MovementAnimation") });
	MovementTimeline->SetPlayRate(Gameboard->GetGameboardTimeScale());
	MovementTimeline->PlayFromStart();
}

void ATTBButton::MoveButtonTickCallback(float Val)
{
	FVector NewLoc = FMath::Lerp(GetActorLocation(), MoveTargetXForm.GetLocation(), Val);
	FRotator NewRot = FMath::Lerp(GetActorRotation(), MoveTargetXForm.GetRotation().Rotator(), Val);
	SetActorLocationAndRotation(NewLoc, NewRot);
}

void ATTBButton::MoveButtonFinishedCallback()
{
	if (bIsPlaceholder)
		Destroy();
}

void ATTBButton::ChangeColor(EColorFunction InColorFunction)
{
	UCurveFloat* Curve = BlinkCurve;

	Curve =
		(InColorFunction == EColorFunction::CC_Blink) ? BlinkCurve
		: (InColorFunction == EColorFunction::CC_BlinkAndHold) ? BlinkAndHoldCurve
		: (InColorFunction == EColorFunction::CC_SlowFadeOut) ? SlowFadeOutCurve
		: (InColorFunction == EColorFunction::CC_FadeOut) ? LinearReverseCurve
		: LinearCurve;

	float PlayRate = (InColorFunction == EColorFunction::CC_SlowFadeOut) 
		? 1.f / 3.f 
		: 1.f / 0.3f;
	
	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("ChangeColorTLCallback") });	
	ColorTimeline->AddInterpFloat(Curve, tickCallback, FName{ TEXT("ColorAnimation") });
	ColorTimeline->SetPlayRate(PlayRate);

	ColorTimeline->PlayFromStart();

	PlaySound(ClickSound);
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

	FOnTimelineEventStatic finishedCallback{};	// Called when the timeline completes
	finishedCallback.BindUFunction(this, FName{ TEXT("PressButtonFinishedCallback") });
	MovementTimeline->SetTimelineFinishedFunc(finishedCallback);

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("PressButtonTickCallback") });

	MovementTimeline->AddInterpFloat(ValleyCurve, tickCallback, FName{ TEXT("MovementAnimation") });
	MovementTimeline->PlayFromStart();
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