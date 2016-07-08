// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RWBY_CodenameColors.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"

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

	Perspective = ECameraType::Side;

	Health = 100;

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
	InputComponent->BindAxis("MoveForward", this, &ARWBY_CodenameColorsCharacter::MoveForward);

	InputComponent->BindAction("PerspectiveSwitch", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::SwitchCamera);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartShooting);
	InputComponent->BindAction("Shoot", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopShooting);

	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartDodging);
	InputComponent->BindAction("Dodge", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopDodging);

	//Allows the the character to the turn based on mouse 
	InputComponent->BindAxis("TurnRate", this, &ARWBY_CodenameColorsCharacter::TurnAtRate);
	//Allows the character to look up baed on the mouse
	InputComponent->BindAxis("LookUpRate", this, &ARWBY_CodenameColorsCharacter::LookUpAtRate);


	//Allows the Chracter to turn based on controler input (not working ATM)
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	//Allows the character to look up based on controller input (not working ATM)
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


}

// Moves the Charcter forward
void ARWBY_CodenameColorsCharacter::MoveForward(float Value)
{

	switch (Perspective) {
		case(ECameraType::None) :
			return;
			break;
		case(ECameraType::Side) :

			if ((Controller != NULL) && (Value != 0.0f))
			{
				//Sets the max walk speed higher when in third person
				GetCharacterMovement()->MaxWalkSpeed = 450.f;

			}
			break;
		case(ECameraType::Third) :

			if ((Controller != NULL) && (Value != 0.0f))
			{
				//Sets the max walk speed higher when in third person
				GetCharacterMovement()->MaxWalkSpeed = 600.f;

				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				AddMovementInput(Direction, Value);

			}

			break;

		}
}

void ARWBY_CodenameColorsCharacter::MoveRight(float Value)
{
	switch (Perspective){
		case(ECameraType::None) :
			return;
			
		case(ECameraType::Side):
			AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
			break;

		case(ECameraType::Third):
			//sets a new Movement speed
			GetCharacterMovement()->MaxWalkSpeed = 600.f;

			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);
	}

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

		case(ECameraType::None) :
			break;
		case(ECameraType::Side):
			SideViewCameraComponent->Deactivate();
			FollowCamera->Activate();
			Perspective = ECameraType::Third;
			break;
		case(ECameraType::Third):
			FollowCamera->Deactivate();
			SideViewCameraComponent->Activate();
			Perspective = ECameraType::Side;
			break;
	}

}

float ARWBY_CodenameColorsCharacter::TakeDamage(float DamageAmount, const FDamageEvent & DamageEvent, AController* EventInstigator, AActor * DamageCauser) {

	Health -= DamageAmount;
	if (Health < 0) {
		AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
		if (ThisPlayer) {
			ThisPlayer->OnKilled();
		}
		Destroy();
	}

	OnRep_Health();
	return DamageAmount;
}

void ARWBY_CodenameColorsCharacter::DealDamage(float Damage, FHitResult LineTrace) {

	APlayerController* MyController = Cast<APlayerController>(GetController());

	if (MyController != nullptr) {

		if (LineTrace.bBlockingHit) {

			ARWBY_CodenameColorsCharacter* TestCharacter = Cast<ARWBY_CodenameColorsCharacter>(LineTrace.GetActor());
			if (TestCharacter) {

				TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				FDamageEvent DamageEvent(ValidDamageTypeClass);

				TestCharacter->TakeDamage(Damage, DamageEvent, MyController, this);
			}
		}
	}
}

void ARWBY_CodenameColorsCharacter::StartShooting() {

	PerformTask(ETask::Shooting);
}

void ARWBY_CodenameColorsCharacter::StopShooting() {

	PerformTask(ETask::None);
}

void ARWBY_CodenameColorsCharacter::PerformTask(ETask::Type NewTask) {

	if (GetNetMode() == NM_Client) {
		ServerPerformTask(NewTask);
		return;
	}

	Task = NewTask;

	OnRep_Task();
}

void ARWBY_CodenameColorsCharacter::ServerPerformTask_Implementation(ETask::Type NewTask) {

	PerformTask(NewTask);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformTask_Validate(ETask::Type NewTask) {
	return true;
}

void ARWBY_CodenameColorsCharacter::OnFire() {


	if (Task != ETask::Shooting) {
		return;
	}

	const USkeletalMeshSocket* MeleeSocket = GetMesh()->GetSocketByName(FName("AttackSocket 2"));
	//gets the beginning socket
	FVector CameraLocation = GetFollowCamera()->GetComponentLocation();
	FVector CameraForVector = GetFollowCamera()->GetForwardVector();

	FVector ForwardVector;
	float Length;

	CameraForVector.ToDirectionAndLength(ForwardVector, Length);

	FHitResult CameraHit;
	//creates the hit parameters
	FCollisionQueryParams CamCollisionParams;
	//cerates the object query parameters
	FCollisionObjectQueryParams CamObjectQueryParams;

	//makes it so the current actor cannot hit them self
	CamCollisionParams.AddIgnoredActor(this);

	/*creates a bool that checks if the ray trace hit something
	*@PARAM Needs a FHitresult
	*@PARAM find the beginning location with the location of a socket
	*@PARAM Find the ending location with the location of another socket
	*@PARAM collisonparams the parameters the hit trace must follow
	*@PARAM ObjectQueryParams the parameters the hit trace must follow
	* Takes the beginnig location, and the end of location and draws a line between them,
	* Makes the line be drawn on the normal, and not a define vector GREAT for attaking animations
	**/
	bool CamHitSuccess = GetWorld()->LineTraceSingle(CameraHit, CameraLocation, CameraLocation + (ForwardVector * 1000000), CamCollisionParams, CamObjectQueryParams);
	

	if (CamHitSuccess) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BUTTON PRESSED"));
		DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(0, 255, 0), true);

		UCapsuleComponent* SkeletalTest = Cast<UCapsuleComponent>(CameraHit.GetComponent());

		//if what was hit, was indeed a capsule component
		if (SkeletalTest) {
			//debuging...
			//UE_LOG(LogClass, Warning, TEXT(" Hit:  %s "), *HitResult.GetComponent()->GetName());
			//UE_LOG(LogClass, Log, TEXT(" Skeletal Mesh Hit:  %s "), *HitResult.GetComponent()->GetName());

			//Cast the Hit result to the ARWBY_TestingCharacter class to test
			ARWBY_CodenameColorsCharacter* TestCharacter = Cast<ARWBY_CodenameColorsCharacter>(CameraHit.GetActor());
			//if what was hit is part of the ARWBY_testingCharacter Testing THEN...
			if (TestCharacter) {

				DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(255, 0, 0), true);

				TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				FDamageEvent DamageEvent(ValidDamageTypeClass);

				APlayerController* MyController = Cast<APlayerController>(GetController());

				TestCharacter->TakeDamage(100, DamageEvent, MyController, this);
				

			}
		}

	}
	else {
		DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(0, 0, 225), true);
	}

	GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARWBY_CodenameColorsCharacter::OnFire, 1.f);

}

void ARWBY_CodenameColorsCharacter::OnDodge() {

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (!(ThisPlayer->GetCharacter()->GetCharacterMovement()->IsFalling())) {
		ThisPlayer->GetCharacter()->GetCharacterMovement()->AddImpulse(ThisPlayer->GetCharacter()->GetActorForwardVector() * 300000, false);
	}
	else {
		ThisPlayer->GetCharacter()->GetCharacterMovement()->AddImpulse(ThisPlayer->GetCharacter()->GetActorForwardVector() * 50000, false);
	}
		//ThisPlayer->GetCharacter()->GetCharacterMovement()->AddImpulse(ThisPlayer->GetCharacter()->GetActorForwardVector() * 300000, false);
		//MyController->GetCharacter()->GetCharacterMovement()->AddImpulse(MyController->GetCharacter()->GetActorUpVector() * 20000, false);
		//MyController->GetCharacter()->GetCharacterMovement()->Velocity += FVector(MyController->GetCharacter()->GetActorForwardVector().X,MyController->GetCharacter()->GetActorForwardVector().Y, 0) ;
}

void ARWBY_CodenameColorsCharacter::StartDodging() {

	PerformDodge(true);
}

void ARWBY_CodenameColorsCharacter::StopDodging() {

	PerformDodge(false);
}

void ARWBY_CodenameColorsCharacter::PerformDodge(bool bDodging) {

	if (GetNetMode() == NM_Client) {
		ServerPerformDodge(bDodging);
	}
	
	isDodging = bDodging;

	OnRep_Dodge();
}

void ARWBY_CodenameColorsCharacter::ServerPerformDodge_Implementation(bool bDodging) {

	PerformDodge(bDodging);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformDodge_Validate(bool bDodging) {
	return true; 
}

/*On_Rep methods
*
*
*/
void ARWBY_CodenameColorsCharacter::OnRep_Task() {

	switch (Task) {

	case(ETask::None) :
		break;
	case(ETask::Shooting) :
		OnFire();
		break;
	}
}

void ARWBY_CodenameColorsCharacter::OnRep_Health() {

	FollowCamera->PostProcessSettings.SceneFringeIntensity = 100.f - Health * 0.5f;

	SideViewCameraComponent->PostProcessSettings.SceneFringeIntensity = 100.f - Health * 0.5f;
}

void ARWBY_CodenameColorsCharacter::OnRep_Dodge() {

	if (isDodging) {
		OnDodge();
	}
	else {
	}
}

/* Replication Method (Properties)
*
*
*/

void ARWBY_CodenameColorsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, Task);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, Health);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, isDodging);

}

