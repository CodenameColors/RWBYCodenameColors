// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RWBY_CodenameColors.h"
#include "RWBY_CodenameColorsCharacter.h"

ARWBY_CodenameColorsCharacter::ARWBY_CodenameColorsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	CharMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharMeshTest"));
	CharMesh->SetOnlyOwnerSee(false);
	CharMesh->AttachParent = GetCapsuleComponent();
	CharMesh->bCastDynamicShadow = true;
	CharMesh->CastShadow = true;
	CharMesh->RelativeRotation = FRotator(0.f, 0.f, 0.f);
	CharMesh->RelativeLocation = FVector(0.f, 0.f, -98.f);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CMesh(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));

	if (CMesh.Succeeded()) {

		CharMesh->SetSkeletalMesh(CMesh.Object);

	}

	CharMesh->RelativeRotation = FRotator(0.f, -90.f, 0.f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
	CameraBoom->RelativeRotation = FRotator(0.f,180.f,0.f);

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera


	ThirdPersonBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPeresonCameraBoom"));
	ThirdPersonBoom->AttachTo(RootComponent);
	ThirdPersonBoom->bAbsoluteRotation = true;; // Rotation of the character should not affect rotation of boom
	ThirdPersonBoom->bDoCollisionTest = false;
	ThirdPersonBoom->TargetArmLength = 175;
	ThirdPersonBoom->SocketOffset = FVector(20.f, 75.f, 60.f);
	ThirdPersonBoom->RelativeRotation = FRotator(-10.f, -10, 0.f);


	// create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(ThirdPersonBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	Perspective = CameraType::Side;


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARWBY_CodenameColorsCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAxis("MoveRight", this, &ARWBY_CodenameColorsCharacter::MoveRight);

	InputComponent->BindAction("PerspectiveSwitch", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::SwitchCamera);

	//Allows the the character to the turn based on mouse 
	InputComponent->BindAxis("TurnRate", this, &ARWBY_CodenameColorsCharacter::TurnAtRate);
	//Allows the character to look up baed on the mouse
	InputComponent->BindAxis("LookUpRate", this, &ARWBY_CodenameColorsCharacter::LookUpAtRate);


	//Allows the Chracter to turn based on controler input (not working ATM)
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	//Allows the character to look up based on controller input (not working ATM)
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


}

void ARWBY_CodenameColorsCharacter::MoveRight(float Value)
{
	// add movement in that direction
	AddMovementInput(FVector(0.f,-1.f,0.f), Value);

}

//Turns the chracter based on MOUSE X input
void ARWBY_CodenameColorsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

// Turns Chracter based on the MOUSE Y input
void ARWBY_CodenameColorsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}






void ARWBY_CodenameColorsCharacter::SwitchCamera() {

	switch(Perspective){

		case(CameraType::None) :
			break;
		case(CameraType::Side):
			SideViewCameraComponent->Deactivate();
			FollowCamera->Activate();
			Perspective = CameraType::Third;
/*
			bUseControllerRotationPitch = false;
			bUseControllerRotationYaw = true;
			bUseControllerRotationRoll = false;
			*/

			break;
		case(CameraType::Third):
			FollowCamera->Deactivate();
			SideViewCameraComponent->Activate();
			Perspective = CameraType::Side;

		/*bUseControllerRotationPitch = false;
			bUseControllerRotationYaw = false;
			bUseControllerRotationRoll = false;
			*/

			break;
	}

}


