// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "RubyRose.h"
#include "UnrealNetwork.h"
#include "CharacterPowers/RosePetal.h"
#include "MyPlayerController.h"
#include "DrawDebugHelpers.h"

//Sets the default values of this class
ARubyRose::ARubyRose() {

	//Super::ARWBY_CodenameColorsCharacter();
	Super::SetMaxAmmo(25);

	CurrentAmmo = MaxAmmo / 5;

	SemblanceMultiplier = 1.f;

	//CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ARWBY_TestingCharacter::OnOverlap);
}


void ARubyRose::Tick(float DeltaSeconds) {

	Super::Tick(DeltaSeconds);

	if (WhileDodging && !EPoweredUpState::None ) {
		
		FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("Petals Socket Spawn"));

		//Spawn(SpawnLocation);
	}

	if (bCanDealAttackDamage) {
		OnAttack();
	}
	//OnAttack();

}

void ARubyRose::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARubyRose::StartDodging);
	//InputComponent->BindAction("Dodge", IE_Released, this, &ARubyRose::StopDodging);

	InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ARubyRose::StartAttack);
	InputComponent->BindAction("MeleeAttack", IE_Released, this, &ARubyRose::StopAttack);

	InputComponent->BindAction("Shoot", IE_Pressed, this, &ARubyRose::StartShooting);
	//InputComponent->BindAction("Shoot", IE_Released, this, &ARubyRose::StopShooting);

	InputComponent->BindAction("Semblance", IE_Pressed, this, &ARubyRose::UseSemblance);

}


// Function Used to the spawn the pickups
void ARubyRose::Spawn(FVector SpawnLocation){

	//Checks to see if there was a spawning mesh selected.
	if (RosePetal != NULL) {

		//Gets the world that the spawn point is in
		UWorld* const World = GetWorld();

		//Checks to see if the world is valid
		if (World) {

			// The parameters of which this should spawn
			FActorSpawnParameters SpawnParams;
			// Sets the owner to this spawn point
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			/*Uncomment if starting rotation is wanted
			*
			*
			//get a random rotation for the spawned item
			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.0f;
			SpawnRotation.Pitch = FMath::FRand() * 360.0f;
			SpawnRotation.Roll = FMath::FRand() * 360.0f;
			*/

			//Spawns with no default Rotation (this can be down here)
			FRotator SpawnRotation;
			SpawnRotation.Yaw = 0.0f;
			SpawnRotation.Pitch = 0.0f;
			SpawnRotation.Roll = 0.0f;

			//Checks to see if it should spawn
			
				//Spawns the pickup
				ARosePetal* const SpawnedPickup = World->SpawnActor<ARosePetal>(RosePetal, SpawnLocation, SpawnRotation);
				//Resets the Spawn delay timer
				//GetWorldTimerManager().SetTimer(PetalSpawnDelay, this, &ARubyRose::Spawn, .34f, false);


			

		}

	}
}

void ARubyRose::PerformDodge(bool bDodging){
	
	Super::PerformDodge(bDodging);
}

void ARubyRose::ServerPerformDodge_Implementation(bool bDodging) {
	PerformDodge(bDodging);
}

bool ARubyRose::ServerPerformDodge_Validate(bool bDodging) {
	return true;
}

void ARubyRose::StartDodging(){
	PerformDodge(true);

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	if (ThisPlayer) {
		FTimerHandle StopDodge;
		if (!ThisPlayer->GetCharacter()->GetCharacterMovement()->IsFalling()) {
			GetWorldTimerManager().SetTimer(StopDodge, this, &ARubyRose::StopDodging, .55, false);
		}
		else {
			GetWorldTimerManager().SetTimer(StopDodge, this, &ARubyRose::StopDodging, .4, false);
		}
	}

}

void ARubyRose::StopDodging(){
	PerformDodge(false);
}

void ARubyRose::OnDodge(){

	Super::OnDodge();

	//AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	//Spawn(ThisPlayer->GetCharacter()->GetActorForwardVector()* -170);

	//Add in spawning of rose petals(Own Class)
	//also set those rose petals material using a method SetMaterial in the Rose petal class

}

void ARubyRose::StartAttack_Implementation(){

	if (bHanging || bSliding) {
		return;
	}
	SlowDown(120);

	SetAttackingBool(&bMeleeAttacking, true);
	PerformAttack(true);
	
}


bool ARubyRose::StartAttack_Validate() {
	return true;
}

void ARubyRose::StopAttack_Implementation(){

	bMeleeAttacking = false;
	PerformAttack(false);

	//FTimerDelegate AttackState;
	

	//AttackState.BindUFunction(this, FName("SetAttackingBool"), false);
	//GetWorldTimerManager().SetTimer(Attack, AttackState, 1.23f, false);
}

bool ARubyRose::StopAttack_Validate() {
	return true;
}


void ARubyRose::PerformAttack(bool ShouldAttack){

	if (GetNetMode() == NM_Client) {
		ServerPerformAttack(ShouldAttack);
		return;
	}

	bMeleeAttacking = ShouldAttack;

	OnRep_MeleeAttack();

}

void ARubyRose::OnAttack(){

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	//gets the socket 
	const USkeletalMeshSocket* MeleeSocket = GetMesh()->GetSocketByName(FName("AttackSocketEnd"));

	const FVector StartLocation = GetMesh()->GetSocketLocation(FName("AttackSocketBeginning"));
	const FVector EndLocation = GetMesh()->GetSocketLocation(FName("AttackSocketEnd"));

	

	if (MeleeSocket) {

		//Create Melee Attack Hit Result
		FHitResult MeleeAttackHitResult;
		//creates the Hit Parameters
		FCollisionQueryParams ColliParams;
		//create the object query 
		FCollisionObjectQueryParams Query;
		//you should not be allowed to hit your self
		ColliParams.AddIgnoredActor(this);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Attack Pressed"));

		//DEBUGING
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 1);

		bool MeleeHitResult = GetWorld()->LineTraceSingle(MeleeAttackHitResult, StartLocation, EndLocation, ColliParams, Query);

		if (MeleeHitResult) {

			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, 1);
			ARWBY_CodenameColorsCharacter* HitChar = Cast<ARWBY_CodenameColorsCharacter>(MeleeAttackHitResult.GetActor());

			if (HitChar) {
				//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, 1);

				if (LastHitActor != HitChar) {
					DealDamage(10, MeleeAttackHitResult);
					LastHitActor = HitChar;
					Semblance += 10 * 1.7;
				}
			}
		}
	}
}

void ARubyRose::ServerPerformAttack_Implementation(bool ShouldAttack){
	PerformAttack(ShouldAttack);
}

bool ARubyRose::ServerPerformAttack_Validate(bool ShouldAttack){
	return true;
}

void ARubyRose::SetAttackingBool(bool *OldBool, bool NewBoolState) {

	*OldBool = NewBoolState;

}

void ARubyRose::StartShooting() {

	PerformTask(ETask::Shooting);

	FTimerHandle StopAttack;
	GetWorldTimerManager().SetTimer(StopAttack, this, &ARubyRose::StopShooting, .6, false);
}

void ARubyRose::StopShooting() {

	PerformTask(ETask::None);

	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ARubyRose::PerformTask(ETask::Type NewTask) {

	if (GetNetMode() == NM_Client) {
		ServerPerformTask(NewTask);
		return;
	}
	//GetCharacterMovement()->MaxWalkSpeed = 0.f;
	Task = NewTask;

	OnRep_Task();
}

void ARubyRose::ServerPerformTask__Implementation(ETask::Type NewTask) {

	PerformTask(NewTask);
}

bool ARubyRose::ServerPerformTask__Validate(ETask::Type NewTask) {
	return true;
}

void ARubyRose::OnFire() {


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
	*/
	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (!SideView) {

		bool CamHitSuccess = GetWorld()->LineTraceSingle(CameraHit, CameraLocation, CameraLocation + (ForwardVector * 1000000), CamCollisionParams, CamObjectQueryParams);
		//DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor::Blue, true, 1);
		if (CamHitSuccess) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BUTTON PRESSED"));
			DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor::Green, true, 1);

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

					DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor::Red, true, 1);

					TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
					FDamageEvent DamageEvent(ValidDamageTypeClass);

					APlayerController* MyController = Cast<APlayerController>(GetController());

					//Base Damage Dealer
					TestCharacter->GetShot(21, DamageEvent, MyController, this);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("RWBY Shot"));

				}
			}

		}
	}
	else if (SideView) {

		FVector StartLocation = GetMesh()->GetSocketLocation(FName("GunTip"));
		FVector EndLocation = StartLocation + ThisPlayer->GetCharacter()->GetActorForwardVector() * 1000;

		EndLocation = EndLocation.RotateAngleAxis(OutAngle, FVector(0, -1, 0));


		//bool CamHitSuccess = GetWorld()->LineTraceSingle(CameraHit, CameraLocation, CameraLocation + (ForwardVector * 1000000), CamCollisionParams, CamObjectQueryParams);
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 1);

	}
}

void ARubyRose::UseSemblance() {
	Super::UseSemblance();
}


void ARubyRose::PerformSemblance() {

	Super::PerformSemblance();

	if (Semblance == 100) {
		Semblance = 0;
		ServerSetValue(2.f);
	}

}

void ARubyRose::ServerPerformSemblance_Implementation() {

	PerformSemblance();
}

bool ARubyRose::ServerPerformSemblance_Validate() {
	return true;
}

void ARubyRose::ServerSetValue_Implementation(float NewFloatValue) {

	SemblanceMultiplier = NewFloatValue;

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	if (ThisPlayer) {
		ThisPlayer->GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = (ThisPlayer->GetCharacter()->GetCharacterMovement()->MaxWalkSpeed * SemblanceMultiplier);
	}

}

bool ARubyRose::ServerSetValue_Validate(float NewFloatValue){
	return true;
}


void ARubyRose::OnRep_Task() {

	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);
	if (ThisPlayer) {
		switch (Task) {

		case(ETask::None) :
			Shooting = false;

			if (Perspective == ECameraType::Side) {
				GetCharacterMovement()->MaxWalkSpeed = 450;
			}
			else if (Perspective == ECameraType::Third) {
				GetCharacterMovement()->MaxWalkSpeed = 600;
			}
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			ThisPlayer->GetCharacter()->EnableInput(ThisPlayer);
			break;
		case(ETask::Shooting) :
			if (CurrentAmmo > 0) {
				OnFire();
				Shooting = true;

				if (!ThisPlayer->GetCharacter()->GetCharacterMovement()->IsFalling()) {
					GetCharacterMovement()->MaxWalkSpeed = 0;
					GetCharacterMovement()->DisableMovement();
					ThisPlayer->GetCharacter()->DisableInput(ThisPlayer);
					//ThisPlayer->Mouse
				}
				else {
					//GetCharacterMovement()->DisableMovement();
					ThisPlayer->GetCharacter()->DisableInput(ThisPlayer);
				}
			}
							  break;
		}
	}
}


void ARubyRose::OnRep_MeleeAttack() {
	
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
		
	}
	else {
		GetMesh()->GetAnimInstance()->Montage_Play(Melee, SemblanceMultiplier);
	}

	OnAttack();
}

void ARubyRose::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARubyRose, bCanDealAttackDamage);
	DOREPLIFETIME(ARubyRose, Melee);
	DOREPLIFETIME(ARubyRose, SemblanceMultiplier);
	
}
