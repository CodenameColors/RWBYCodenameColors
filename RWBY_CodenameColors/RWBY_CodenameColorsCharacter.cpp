// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "RWBY_CodenameColors.h"
#include "RWBY_CodenameColorsCharacter.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"
#include "Pickup.h"
#include "AmmoPickup.h"
#include "DustPickup.h"
#include "Kismet/KismetMathLibrary.h"

ARWBY_CodenameColorsCharacter::ARWBY_CodenameColorsCharacter(){

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
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1200.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;


	Perspective = ECameraType::Side;

	Health = 100;

	CurrentAmmo = 5;

	bCanPickupDust = true;
	bCanWallTrace = false;
	bCanClimb = false;
	bCanWallSlide = false;

	Dust = EDustType::None;
	CharacterState = ECharacterState::Normal;

	//GetSphereTracer()->OnComponentBeginOverlap.AddDynamic(this, &ARWBY_CodenameColorsCharacter::OnBeginOverlap);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARWBY_CodenameColorsCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	InputComponent->BindAction("Jump", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopJump);

	InputComponent->BindAxis("MoveRight", this, &ARWBY_CodenameColorsCharacter::MoveRight);
	InputComponent->BindAxis("MoveForward", this, &ARWBY_CodenameColorsCharacter::MoveForward);

	InputComponent->BindAction("PerspectiveSwitch", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::SwitchCamera);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartShooting);
	InputComponent->BindAction("Shoot", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopShooting);

	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartDodging);
	InputComponent->BindAction("Dodge", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopDodging);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::OnCrouchStart);
	InputComponent->BindAction("Crouch", IE_Released, this, &ARWBY_CodenameColorsCharacter::OnCrouchEnd);

	InputComponent->BindAction("Healing", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::StartHealing);
	InputComponent->BindAction("Healing", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopHealing);


	InputComponent->BindAction("Use Dust", IE_Pressed, this, &ARWBY_CodenameColorsCharacter::UseDust);

	//Allows the the character to the turn based on mouse 
	InputComponent->BindAxis("TurnRate", this, &ARWBY_CodenameColorsCharacter::TurnAtRate);
	//Allows the character to look up baed on the mouse
	InputComponent->BindAxis("LookUpRate", this, &ARWBY_CodenameColorsCharacter::LookUpAtRate);


	//Allows the Chracter to turn based on controler input (not working ATM)
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	//Allows the character to look up based on controller input (not working ATM)
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


}


void ARWBY_CodenameColorsCharacter::Tick(float DeltaSeconds){
	
	Super::Tick(DeltaSeconds);

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (ThisPlayer) {
	
		if (CharacterState != ECharacterState::Normal) {
			PerformElementalDamage(CharacterState, DeltaSeconds);
		}


		if(bCanWallTrace) {
			PerformLedgeTrace(bCanWallTrace);
		}

		if (bCanWallSlide) {
			PerformWallSlide(bCanWallSlide);
		}

		if (bCanClimb) {
			LedgeGrab();
		}

		if (bDoneClimbing) {

			bDoneClimbing = false;
			bCanClimb = false;
			bCanWallTrace = false;
			bClimbing = false;
			bHanging = false;

			//AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
			ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

			ThisPlayer->GetCharacter()->SetActorLocation(ClimbPosition, false, false);


			//CameraBoom->AttachTo(RootComponent);
		}
	}
}


//UNUSED AT THE MOMENT
void ARWBY_CodenameColorsCharacter::MoveCharacter( ){
	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	ThisPlayer->GetCharacter()->SetActorLocation(ClimbPosition, false, false);

	bCanClimb = false;
	bHanging = false;

	ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);


}


void ARWBY_CodenameColorsCharacter::StartJump(){
	


	if (bCanClimb) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Space Pressed"));

		SetClimbing(true);

		//bCanClimb = false;
		//bHanging = false;
		
	}
	else if (bSliding) {
		PerformWallJump(true);
	}
	else {
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}

}

void ARWBY_CodenameColorsCharacter::SetClimbing_Implementation(bool NewState){
	bClimbing = NewState;
}

bool ARWBY_CodenameColorsCharacter::SetClimbing_Validate(bool NewState){
	return true;
}

void ARWBY_CodenameColorsCharacter::StopJump() {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Space Released"));
	bPressedJump = false;
	bClimbing = false;
	JumpKeyHoldTime = 0.0f;
}


void ARWBY_CodenameColorsCharacter::OnCrouchStart_Implementation(){

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (bHanging) {
		ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		bHanging = false;
		bCanClimb = false;
		bCanWallTrace = false;
		bClimbing = false;
	}
	

}

bool ARWBY_CodenameColorsCharacter::OnCrouchStart_Validate() {
	return true;
}

void ARWBY_CodenameColorsCharacter::OnCrouchEnd(){

	
	GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARWBY_CodenameColorsCharacter::OnRep_Slide, .15f);
}

// Moves the Charcter forward
void ARWBY_CodenameColorsCharacter::MoveForward(float Value)
{

	if (bHanging || bWallJumping) {
		return;
	}

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

	if (bHanging || bWallJumping) {
		return;
	}

	switch (Perspective){
		case(ECameraType::None) :
			return;
			
		case(ECameraType::Side):
			AddMovementInput(FVector(0.f, -1.f, 0.f), Value);
			break;

		case(ECameraType::Third):

			if (true) {

			}
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

void ARWBY_CodenameColorsCharacter::PerformWallSlide(bool CanSlide) {

	if (GetNetMode() == NM_Client) {
		ServerPerformWallSlide(CanSlide);
	}

	//bSliding = true;

	OnRep_Slide();
}

void ARWBY_CodenameColorsCharacter::ServerPerformWallSlide_Implementation(bool CanSlide) {
	PerformWallSlide(CanSlide);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformWallSlide_Validate(bool CanSlide) {
	return true;
}

void ARWBY_CodenameColorsCharacter::OnWallSlide() {

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (ThisPlayer) {
		if (!bHanging && !bCanClimb && !bCanClimb) {
			if (ThisPlayer->GetCharacter()->GetCharacterMovement()->Velocity.Z < -50) {
				bSliding = true;
				ThisPlayer->GetCharacter()->GetCharacterMovement()->Velocity.Z = -200;
				//ThisPlayer->GetCharacter()->GetCharacterMovement()->Velocity.Y = 0;
				ThisPlayer->GetCharacter()->GetCharacterMovement()->Velocity.X = 0;

				//ThisPlayer->GetCharacter()->SetActorRotation(ThisPlayer->GetCharacter()->GetActorRotation() * -1);

			}
		}
	}

}

void ARWBY_CodenameColorsCharacter::PerformLedgeTrace(bool CanTrace) {

	if (GetNetMode() == NM_Client) {
		ServerPerformLedgeTrace(CanTrace);
	}

	bCanWallTrace = true;

	OnRep_Ledge();
}

void ARWBY_CodenameColorsCharacter::ServerPerformLedgeTrace_Implementation(bool CanTrace) {

	PerformLedgeTrace(CanTrace);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformLedgeTrace_Validate(bool CanTrace) {
	return true;
}


void ARWBY_CodenameColorsCharacter::OnLedgeTrace() {


	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (!ThisPlayer) {
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, TEXT("WALL DETECTED"));

	const FVector Location = GetMesh()->GetSocketLocation(FName("Hip"));

		AActor* ActorToIgnore = this;
		const FVector Start = GetMesh()->GetSocketLocation(FName("Hip"));
		const FVector End = Location + ThisPlayer->GetCharacter()->GetActorForwardVector() * 150;
		const float Radius = 20;

		//const USkeletalMeshSocket* MeleeSocket = GetMesh()->GetSocketByName(FName("test"));
		//gets the beginning socket
		
		FHitResult HitOut;
		ECollisionChannel TraceChannel = ECC_EngineTraceChannel1;


		FCollisionQueryParams TraceParams(FName(TEXT("LedgeTrace")), true, ActorToIgnore);
		TraceParams.bTraceComplex = true;
		//TraceParams.bTraceAsyncScene = true;
		TraceParams.bReturnPhysicalMaterial = false;

		//Ignore Actors
		TraceParams.AddIgnoredActor(ActorToIgnore);

		//Re-initialize hit info
		HitOut = FHitResult(ForceInit);

		//Get World Source
		TObjectIterator<AMyPlayerController> ThePC;
		if (ThePC) {

			bool WallHit = ThePC->GetWorld()->SweepSingle(HitOut, Start, End, FQuat(), TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);
			DrawDebugSphere(GetWorld(), End, Radius, 10, FColor::Red, false, .01666);
			//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, .01666);

			if (WallHit) {
				bCanWallSlide = true;
				FVector Location = Location + HitOut.Location;

				DrawDebugSphere(GetWorld(), Location, Radius, 10, FColor::Red, false, .01666);
				//DrawDebugLine(GetWorld(), Start, Location, FColor::Red, false, .01666);

			}
			else {
				bCanWallSlide = false;
			}
		}

		//TRACE DOWN

		
		const FVector LocationDown = GetMesh()->GetSocketLocation(FName("Hip1"));

		const FVector DownStart = ThisPlayer->GetCharacter()->GetActorUpVector() *150+ LocationDown;
		const FVector EndDown = Location;

		//const USkeletalMeshSocket* MeleeSocket = GetMesh()->GetSocketByName(FName("test"));
		//gets the beginning socket

		FHitResult HitDown;
		FCollisionObjectQueryParams CamObjectQueryParams;

		//Re-initialize hit info
		HitDown = FHitResult(ForceInit);

		if (ThePC) {

			bool WallHit = ThePC->GetWorld()->LineTraceSingle(HitDown, DownStart, LocationDown, TraceParams, CamObjectQueryParams);
			DrawDebugSphere(GetWorld(), DownStart, Radius, 10, FColor::Red, false, .01666);
			DrawDebugLine(GetWorld(), DownStart, LocationDown, FColor::Red, false, .01666);

			if (WallHit) {

				bCanWallSlide = false;
				FVector DownStart = DownStart + (HitDown.Location + ThisPlayer->GetCharacter()->GetActorUpVector() * 20);
				if (HitDown.Distance < 90 && ThisPlayer->GetCharacter()->GetMovementComponent()->IsFalling()) {



					DrawDebugSphere(GetWorld(), DownStart, Radius, 10, FColor::Green, true, .01666);
					DrawDebugLine(GetWorld(), DownStart, LocationDown, FColor::Green, false, .01666);
					bCanClimb = true;
					bCanWallTrace = false;
					FVector LocationOffset = FVector(DownStart.X, DownStart.Y - ThisPlayer->GetCharacter()->GetActorForwardVector().Y,
						DownStart.Z - ThisPlayer->GetCharacter()->GetActorUpVector().Z * 130);

					LocationOffset -= ThisPlayer->GetCharacter()->GetActorForwardVector() * 50;

					//LocationOffset.Z = LocationOffset.Z -130;

					ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

					FRotator Forward = ThisPlayer->GetCharacter()->GetActorRotation();

					ThisPlayer->GetCharacter()->SetActorLocation(LocationOffset, false, false);

					//GetCapsuleComponent()->MoveComponent(FVector(1,0,0), FQuat(), false, false, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics );
					ThisPlayer->GetCharacter()->GetCharacterMovement()->StopMovementImmediately();

					bHanging = true;
					bCanClimb = true;
					ClimbPosition = DownStart + ThisPlayer->GetCharacter()->GetActorUpVector() * 90;

					//bClimbing = true;
					;
				}

				else if (HitDown.Distance > 60 && ThisPlayer->GetCharacter()->GetMovementComponent()->IsFalling()) {

				}
				else {
					//bCanWallSlide = true;
				}

					//if (!bHanging && !bCanClimb)
						//PerformWallSlide(bCanWallSlide);
					/*
					if(ThisPlayer->GetCharacter()->GetMovementComponent()->IsFalling()){

					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Ledge Trip Detected"));

					ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					FRotator Forward = ThisPlayer->GetCharacter()->GetActorRotation();
					ThisPlayer->GetCharacter()->GetCharacterMovement()->StopMovementImmediately();

					FVector LocationOffset = DownStart;
					LocationOffset += ThisPlayer->GetCharacter()->GetActorUpVector() * 100;

					ThisPlayer->GetCharacter()->SetActorLocation(LocationOffset, false, false);
					ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

					//bDoneClimbing = false;
					//bCanClimb = false;
					//bCanWallTrace = false;
					//bClimbing = false;
					//bHanging = false;
					bLedgeTrip = true;
					OnRep_Trip();
					}
					*/
					//ThisPlayer->GetCharacter()->GetCharacterMovement()->StopMovementImmediately();
				
			
				
				//ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			}
		}
}

void ARWBY_CodenameColorsCharacter::LedgeGrab(){

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (!ThisPlayer) {
		return;
	}

	//ThisPlayer->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

}

void ARWBY_CodenameColorsCharacter::PerformWallJump(bool CanJump) {

	if (GetNetMode() == NM_Client) {
		ServerPerformWallJump(CanJump);
	}

	OnWallJump();
}

void ARWBY_CodenameColorsCharacter::ServerPerformWallJump_Implementation(bool CanJump){
	PerformWallJump(CanJump);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformWallJump_Validate(bool CanJump){
	return true; 
}

void ARWBY_CodenameColorsCharacter::OnWallJump() {

	bWallJumping = true;
	

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	if (ThisPlayer) {
		ThisPlayer->GetCharacter()->GetMovementComponent()->Velocity.Z = -1;
	}
	
	
	GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARWBY_CodenameColorsCharacter::ServerAddVelocity, .4367f);
}

void ARWBY_CodenameColorsCharacter::ServerAddVelocity_Implementation() {
	
	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	if (ThisPlayer) {
		if (bWallJumping) {
			ThisPlayer->GetCharacter()->GetCharacterMovement()->Velocity.operator+=(FVector(0, ThisPlayer->GetCharacter()->GetActorForwardVector().Y, -2) * (750 * -1));
		}
		bWallJumping = false;
	}
}

bool ARWBY_CodenameColorsCharacter::ServerAddVelocity_Validate() {
	return true;
}

float ARWBY_CodenameColorsCharacter::TakeDamage(float DamageAmount, const FDamageEvent & DamageEvent, AController* EventInstigator, AActor * DamageCauser) {

	Health -= DamageAmount;  
	if (Health <= 0) {
		AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
		if (ThisPlayer) {
			ThisPlayer->OnKilled();
		}
		Destroy();
	}

	OnRep_Health();
	return DamageAmount;
}
float ARWBY_CodenameColorsCharacter::GetShot(float DamageAmount, const FDamageEvent & DamageEvent, AController * EventInstigator, AActor * DamageCauser) {

	Health -= DamageAmount;
	if (Health <= 0) {
		AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
		if (ThisPlayer) {
			ThisPlayer->OnKilled();
		}
		Destroy();
	}


	ARWBY_CodenameColorsCharacter * DamageDealer = Cast<ARWBY_CodenameColorsCharacter>(DamageCauser);

	if (DamageDealer) {

		switch (DamageDealer->PoweredUpState) {

		case(EPoweredUpState::FiredUp) :
			CharacterState = ECharacterState::OnFire;


			GetWorldTimerManager().ClearTimer(FireLength);
			break;
		case(EPoweredUpState::GravityUp) :
			CharacterState = ECharacterState::Normal;
			break;
		case(EPoweredUpState::IcedUp) :
			CharacterState = ECharacterState::Freezing;
			break;
		case(EPoweredUpState::ShockedUp) :
			CharacterState = ECharacterState::Shocked;
			GetWorldTimerManager().ClearTimer(ShockLength);
			break;
		case(EPoweredUpState::WateredUp) :
			CharacterState = ECharacterState::Wet;
			break;
		case(EPoweredUpState::None) :
			CharacterState = ECharacterState::Normal;
			break;

		}
	}

	if (CharacterState == ECharacterState::OnFire) {
	GetWorldTimerManager().SetTimer(FireLength, this, &ARWBY_CodenameColorsCharacter::ServerResetCharacterState, 5);
}
	else if (CharacterState == ECharacterState::Shocked) {
		GetWorldTimerManager().SetTimer(ShockLength, this, &ARWBY_CodenameColorsCharacter::ServerResetCharacterState, 3.5);
	}
	else {
	}
	
	//GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARWBY_CodenameColorsCharacter::OnFire, 1.f);
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

void ARWBY_CodenameColorsCharacter::StartHealing() {

	PerformHealing(true);
}

void ARWBY_CodenameColorsCharacter::StopHealing() {

	PerformHealing(false);
}

void ARWBY_CodenameColorsCharacter::PerformHealing(bool Healing) {

	if (GetNetMode() == NM_Client) {
		ServerPerformHealing(Healing);
	}

	bCanHeal = Healing;
	OnRep_Health();
}

void ARWBY_CodenameColorsCharacter::ServerPerformHealing_Implementation(bool Healing) {
	PerformHealing(Healing);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformHealing_Validate(bool Healing) {
	return true;
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

void ARWBY_CodenameColorsCharacter::PerformElementalDamage(ECharacterState::Type CurrentState, float DeltaSeconds) {

	if (GetNetMode() == NM_Client) {
		ServerPerformElementalDamage(CurrentState, DeltaSeconds);
	}

	OnElementalDamage(CurrentState, DeltaSeconds);
}

void ARWBY_CodenameColorsCharacter::ServerPerformElementalDamage_Implementation(ECharacterState::Type CurrentState, float DeltaSeconds) {
	PerformElementalDamage(CurrentState, DeltaSeconds);
}

bool ARWBY_CodenameColorsCharacter::ServerPerformElementalDamage_Validate(ECharacterState::Type CurrentState, float DeltaSeconds) {
	return true;
}

void ARWBY_CodenameColorsCharacter::OnElementalDamage(ECharacterState::Type CurrentState, float DeltaSeconds) {




	switch (CurrentState) {
		case(ECharacterState::OnFire) :
			Health -= DeltaSeconds *  UKismetMathLibrary::RandomIntegerInRange(2, 5);  
				OnRep_Health();
				break;
		case(ECharacterState::Shocked):
			Health -= DeltaSeconds *  UKismetMathLibrary::RandomIntegerInRange(0, 3);   
			OnRep_Health();
			break;

	}

	if (Health <= 0) {
		AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
		if (ThisPlayer) {
			ThisPlayer->OnKilled();
		}
		Destroy();
	}

}

void ARWBY_CodenameColorsCharacter::OnElementalDamage(ECharacterState::Type CurrentState, float DeltaSeconds, int FrozenPercent) {

	switch (CurrentState) {

		case(ECharacterState::Freezing) :

			if (Perspective == ECameraType::Side) {
				GetCharacterMovement()->MaxWalkSpeed = 450 - (450 * (FrozenPercent / 100));
			}
			else {
				GetCharacterMovement()->MaxWalkSpeed = 600 - (600 * (FrozenPercent / 100));
			}
			break;

		case(ECharacterState::Frozen) :

			GetCharacterMovement()->MaxWalkSpeed = 0;
			break;
	}
}



void ARWBY_CodenameColorsCharacter::ServerResetCharacterState_Implementation() {
	CharacterState = ECharacterState::Normal;
}

bool ARWBY_CodenameColorsCharacter::ServerResetCharacterState_Validate() {
	return true;
}

void ARWBY_CodenameColorsCharacter::OnHeal() {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("HEALING"));
	if (Health < 100) {
		Health += 10;
		PerformHealing(bCanHeal);
	}	
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

				if (TestCharacter->CharacterState == ECharacterState::Wet) {
					if (PoweredUpState == EPoweredUpState::FiredUp){

						TestCharacter->GetShot(20 * .63, DamageEvent, MyController, this);
						TestCharacter->ServerResetCharacterState();
					}
					else if (PoweredUpState == EPoweredUpState::ShockedUp) {

						TestCharacter->GetShot(20 * 1.44, DamageEvent, MyController, this);
						TestCharacter->ServerResetCharacterState();
					}
				}

				else if (TestCharacter->CharacterState == ECharacterState::Shocked) {

					if (PoweredUpState == EPoweredUpState::WateredUp) {
						TestCharacter->GetShot(20 * 1.44, DamageEvent, MyController, this);
						TestCharacter->ServerResetCharacterState();
					}
					 
				}

				else {
					TestCharacter->GetShot(17, DamageEvent, MyController, this);
				}


				//UE_LOG(LogClass, Warning, TEXT(" Hit:  %s "), *CameraHit.GetComponent()->GetName());
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

	if (bHanging) {
		return;
	}

	
	if (ThisPlayer) {
		
		if (!(ThisPlayer->GetCharacter()->GetCharacterMovement()->IsFalling())) {
			ThisPlayer->GetCharacter()->GetCharacterMovement()->AddImpulse(ThisPlayer->GetCharacter()->GetActorForwardVector() * 300000, false);
		}
		else {
			ThisPlayer->GetCharacter()->GetCharacterMovement()->AddImpulse(ThisPlayer->GetCharacter()->GetActorForwardVector() * 50000, false);
		}
		
	
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

void ARWBY_CodenameColorsCharacter::UseDust() {
	
	PerformUseDust();
}

void ARWBY_CodenameColorsCharacter::PerformUseDust() {

	if (GetNetMode() == NM_Client) {
		ServerPerformUseDust();
	}

	OnRep_Dust();
}

void ARWBY_CodenameColorsCharacter::ServerPerformUseDust_Implementation() {

	PerformUseDust();
}

bool ARWBY_CodenameColorsCharacter::ServerPerformUseDust_Validate() {
	return true;
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

void ARWBY_CodenameColorsCharacter::SetMaxAmmo(float NewMaxAmmo) {

	MaxAmmo = NewMaxAmmo;
}

bool ARWBY_CodenameColorsCharacter::IsPoweredUp() {

	return bIsPoweredUp;
}



void ARWBY_CodenameColorsCharacter::Collect()
{

	if (CurrentAmmo < MaxAmmo) {

		//Gets all the current overlapping actors
		TArray<AActor*> CollectedActors;

		GetCapsuleComponent()->GetOverlappingActors(CollectedActors);

		for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected) {

			APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);

			if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive()) {

				AAmmoPickup* TestAmmoPickup = Cast<AAmmoPickup>(TestPickup);

				if (TestAmmoPickup) {

						CurrentAmmo += MaxAmmo / 5;

						TestAmmoPickup->WasCollected();

						if (GetNetMode() == NM_Client) {
							TestAmmoPickup->WasCollected_Implementation();
						}

						TestAmmoPickup->SetActive(false);

						OnRep_Ammo();

					

				}

			}
		}
	}


	if (bCanPickupDust) {
		//Gets all the current overlapping actors
		TArray<AActor*> CollectedActors;

		GetCapsuleComponent()->GetOverlappingActors(CollectedActors);

		for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected) {

			APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);

			if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive()) {

				ADustPickup* TestDustPickup = Cast<ADustPickup>(TestPickup);

				if (TestDustPickup) {

						TestDustPickup->WasCollected();
					
						if (GetNetMode() == NM_Client) {
							TestDustPickup->WasCollected_Implementation();
							
						}

						if(TestDustPickup->GetMesh()->GetMaterial(0)->GetName() == "Electricty"){
							Dust = EDustType::Electic;
						}
						else if (TestDustPickup->GetMesh()->GetMaterial(0)->GetName() == "Fire") {
							Dust = EDustType::Fire;
						}
						else if (TestDustPickup->GetMesh()->GetMaterial(0)->GetName() == "Ground") {
							Dust = EDustType::Gravity;
						}
						else if (TestDustPickup->GetMesh()->GetMaterial(0)->GetName() == "Ice") {
							Dust = EDustType::Ice;
						}
						else if(TestDustPickup->GetMesh()->GetMaterial(0)->GetName() == " Water") {
							Dust = EDustType::Water;
						}
						else {
							return;
						}
						OnRep_Dust();
						bCanPickupDust = false;

				}

			}
		}

	}

}



void ARWBY_CodenameColorsCharacter::ResetDust() {

	bIsPoweredUp = false;
	bCanPickupDust = true;
	Dust = EDustType::None;
	PoweredUpState = EPoweredUpState::None;
}

/*On_Rep methods
*
*
*/
void ARWBY_CodenameColorsCharacter::OnRep_Task() {

	switch (Task) {

	case(ETask::None) :
		Shooting = false;
		break;
	case(ETask::Shooting) :
		if (CurrentAmmo > 0) {
			OnFire();
			Shooting = true;
		}
		
		break;
	}
}

void ARWBY_CodenameColorsCharacter::OnRep_Health() {

	FollowCamera->PostProcessSettings.SceneFringeIntensity = 100.f - Health * 0.5f;

	SideViewCameraComponent->PostProcessSettings.SceneFringeIntensity = 100.f - Health * 0.5f;

	if (bCanHeal) {
		GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARWBY_CodenameColorsCharacter::OnHeal, 1.5f);
	}

}

void ARWBY_CodenameColorsCharacter::OnRep_Dodge() {

	if (isDodging) {
		GetWorldTimerManager().SetTimer(DodgeDelay, this, &ARWBY_CodenameColorsCharacter::OnDodge, .324f);
		//OnDodge();
	}
	else {
	}
}

void ARWBY_CodenameColorsCharacter::OnRep_Ammo() {
	//CurrentAmmo -= 1;

	FString PickupDebugString = FString::SanitizeFloat(CurrentAmmo);
	UE_LOG(LogClass, Log, TEXT("You Have Collected %s"), *PickupDebugString);

}

void ARWBY_CodenameColorsCharacter::OnRep_Dust() {

	if (!bCanPickupDust) {
		bIsPoweredUp = true;
		
		switch (Dust) {
			case(EDustType::Fire) :
				PoweredUpState = EPoweredUpState::FiredUp;
				break;
			case(EDustType::Electic) :
				PoweredUpState = EPoweredUpState::ShockedUp;
				break;
			case(EDustType::Gravity) :
				PoweredUpState = EPoweredUpState::GravityUp;
				break;
			case(EDustType::Ice) :
				PoweredUpState = EPoweredUpState::IcedUp;
				break;
			case(EDustType::Water) :
				PoweredUpState = EPoweredUpState::WateredUp;
				break;
			case(EDustType::None) :
				return;
		}
		
		GetWorldTimerManager().SetTimer(PoweredUp, this, &ARWBY_CodenameColorsCharacter::ResetDust, 5.f);
	}

}

void ARWBY_CodenameColorsCharacter::OnRep_Ledge() {
	
	if (!bCanClimb) {
		OnLedgeTrace();
	}
}

void ARWBY_CodenameColorsCharacter::OnRep_Trip(){


}

void ARWBY_CodenameColorsCharacter::OnRep_Slide() {

	if (!bHanging) {
		bCanWallTrace = true;
		//bCanWallSlide = true;
		
		OnWallSlide();
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
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, MaxAmmo);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, CurrentAmmo);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, Shooting);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bIsPoweredUp);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bCanPickupDust);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bCanWallTrace);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bCanClimb);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bHanging);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, bClimbing);
	DOREPLIFETIME(ARWBY_CodenameColorsCharacter, ClimbPosition);
}

