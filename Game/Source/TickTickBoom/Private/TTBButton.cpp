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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlinkTime = .3f;
	RetractedButtonZ = - 2.f;
	ButtonTargetHeight = 0.f;
	bIsPlaceholder = false;
	bIsActive = false;
	bIsPlaceholder = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	// Create geometry components
	Casing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing"));
	Casing->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
	Tube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tube"));
	Tube->AttachToComponent(Casing, FAttachmentTransformRules::KeepRelativeTransform);
	Button = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Button"));
	Button->AttachToComponent(Tube, FAttachmentTransformRules::KeepRelativeTransform);

	//Button->OnClicked.AddUniqueDynamic(this, &ATTBButton::OnButtonClicked);

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

	static ConstructorHelpers::FObjectFinder<UCurveFloat> ValleyCurve_Asset(TEXT("CurveFloat'/Game/Curves/Valley_Curve.Valley_Curve'"));
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
	ButtonMaterialInstance = UMaterialInstanceDynamic::Create(Button->GetMaterial(0), this);
	Button->SetMaterial(0, ButtonMaterialInstance);
}

void ATTBButton::SetActive(bool bNewActive)
{
	bIsActive = bNewActive;
}

void ATTBButton::RetractButton()
{
	HandleExtension(ETubeAction::TS_Retract);
}

void ATTBButton::ExtendButton()
{
	HandleExtension(ETubeAction::TS_Extend);
}

void ATTBButton::RetractImmediate()
{
	Tube->SetRelativeLocation(FVector(0.f, 0.f, RetractedButtonZ));
}

void ATTBButton::HandleExtension(ETubeAction NewTubeAction)
{
	ButtonTargetHeight = (NewTubeAction == ETubeAction::TS_Extend) ? 0.f : RetractedButtonZ;	// Set target height based on if this is a retraction or extension

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("ExtensionTLCallback") });
	TubeTimeline->AddInterpFloat(LinearCurve, tickCallback, FName{ TEXT("TubeAnimation") });
	TubeTimeline->SetPlayRate(Gameboard->GetGameboardTimeScale());

	TubeTimeline->PlayFromStart();
}

void ATTBButton::ExtensionTLCallback(float Val)
{
	float NewZ = FMath::Lerp(Tube->RelativeLocation.Z, ButtonTargetHeight, Val);
	Tube->SetRelativeLocation(FVector(0.f, 0.f, NewZ));
}

void ATTBButton::MoveToNewSlot(FTransform TargetTransform)
{
	PlaySound(CycleClickSound);
	MoveTargetXForm = TargetTransform;

	FOnTimelineEventStatic finishedCallback{};	// Called when the timeline completes
	finishedCallback.BindUFunction(this, FName{TEXT("MovementTLFinishedCallback")});
	MovementTimeline->SetTimelineFinishedFunc(finishedCallback);

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("MovementTLTickCallback") });

	MovementTimeline->AddInterpFloat(LinearCurve, tickCallback, FName{ TEXT("MovementAnimation") });
	MovementTimeline->SetPlayRate(Gameboard->GetGameboardTimeScale());
	MovementTimeline->PlayFromStart();
}

void ATTBButton::MovementTLTickCallback(float Val)
{
	FVector NewLoc = FMath::Lerp(GetActorLocation(), MoveTargetXForm.GetLocation(), Val);
	FRotator NewRot = FMath::Lerp(GetActorRotation(), MoveTargetXForm.GetRotation().Rotator(), Val);
	SetActorLocationAndRotation(NewLoc, NewRot);
}

void ATTBButton::MovementTLFinishedCallback()
{
	if (bIsPlaceholder)
		Destroy();
}

void ATTBButton::HandleColorTL(EColorFunction InColorFunction)
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
	tickCallback.BindUFunction(this, FName{ TEXT("SetColorTLCallback") });	
	ColorTimeline->AddInterpFloat(Curve, tickCallback, FName{ TEXT("ColorAnimation") });
	ColorTimeline->SetPlayRate(PlayRate);

	ColorTimeline->PlayFromStart();

	PlaySound(ClickSound);
}

void ATTBButton::SetColorTLCallback(float Val)
{
	ButtonMaterialInstance->SetScalarParameterValue(TEXT("ColorBlend"), Val);
}

void ATTBButton::HandlePressButtonTL()
{
	// Immediately deactivate all buttons so player cant click multiple buttons
	if (bIsActive)
		Gameboard->DeactivateButtons();

	FOnTimelineEventStatic finishedCallback{};	// Called when the timeline completes
	finishedCallback.BindUFunction(this, FName{ TEXT("OnPressButtonFinishedCallback") });
	MovementTimeline->SetTimelineFinishedFunc(finishedCallback);

	FOnTimelineFloat tickCallback{};	// Called on each tick of the timeline
	tickCallback.BindUFunction(this, FName{ TEXT("OnPressButtonTickCallback") });

	MovementTimeline->AddInterpFloat(ValleyCurve, tickCallback, FName{ TEXT("MovementAnimation") });
	MovementTimeline->PlayFromStart();
}

void ATTBButton::OnPressButtonTickCallback(float Val)
{
	float NewZ = FMath::Lerp(Button->RelativeLocation.Z, Val, 1);
	Button->SetRelativeLocation(FVector(0.f, 0.f, NewZ));
}

void ATTBButton::OnPressButtonFinishedCallback()
{
	Gameboard->ButtonClicked(this);
}

// void ATTBButton::OnButtonClicked(UPrimitiveComponent* pComponent, FKey ButtonPressed)
// {
// 	HandlePressButton();
// }

UAudioComponent* ATTBButton::PlaySound(USoundBase* Sound)
{
	UAudioComponent* AC = NULL;

	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, GetRootComponent());
	}

	return AC;
}