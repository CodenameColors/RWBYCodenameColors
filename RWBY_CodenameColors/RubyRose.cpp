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
	InputComponent->BindAction("Dodge", IE_Released, this, &ARubyRose::StopDodging);

	InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ARubyRose::StartAttack);
	InputComponent->BindAction("MeleeAttack", IE_Released, this, &ARubyRose::StopAttack);

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

	bMeleeAttacking = true;
	PerformAttack(true);
	GetCharacterMovement()->MaxWalkSpeed = 60.f;
}


bool ARubyRose::StartAttack_Validate() {
	return true;
}

void ARubyRose::StopAttack_Implementation(){

	bMeleeAttacking = false;
	PerformAttack(false);

	FTimerDelegate AttackState;
	

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
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 1);

		bool MeleeHitResult = GetWorld()->LineTraceSingle(MeleeAttackHitResult, StartLocation, EndLocation, ColliParams, Query);

		if (MeleeHitResult) {

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, 1);
			ARWBY_CodenameColorsCharacter* HitChar = Cast<ARWBY_CodenameColorsCharacter>(MeleeAttackHitResult.GetActor());

			if (HitChar) {
				DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, 1);

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

void ARubyRose::SetAttackingBool(bool NewBoolState) {

	bMeleeAttacking = NewBoolState;

}

void ARubyRose::OnRep_MeleeAttack() {
	
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {
		
	}
	else {
		GetMesh()->GetAnimInstance()->Montage_Play(Melee, 1);
	}

	OnAttack();
}

<<<<<<< HEAD

void ARubyRose::StartShooting() {

	PerformTask(ETask::Shooting);

}

void ARubyRose::StopShooting() {

	PerformTask(ETask::None);

	//GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ARubyRose::PerformTask(ETask::Type NewTask) {

	if (GetNetMode() == NM_Client) {
		ServerPerformTask_(NewTask);
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
	**/
	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (ThisPlayer) {

		if (Perspective == ECameraType::Third) {

			bool CamHitSuccess = GetWorld()->LineTraceSingle(CameraHit, CameraLocation, CameraLocation + (ForwardVector * 1000000), CamCollisionParams, CamObjectQueryParams);

			if (CamHitSuccess) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BUTTON PRESSED"));
				DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(0, 255, 0), true, 1);

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

						DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(255, 0, 0), true, 1);

						TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
						FDamageEvent DamageEvent(ValidDamageTypeClass);

						//Base Damage Dealer
						TestCharacter->GetShot(20, DamageEvent, ThisPlayer, this);



						//UE_LOG(LogClass, Warning, TEXT(" Hit:  %s "), *CameraHit.GetComponent()->GetName());
					}
				}

			}
			else {
				DrawDebugLine(GetWorld(), CameraLocation, CameraLocation + (ForwardVector * 100000), FColor(0, 0, 225), true, 1);
			}

			GetWorldTimerManager().SetTimer(TimerHandler_Task, this, &ARubyRose::OnFire, 1.f);
		}
		else if (Perspective == ECameraType::Side) {

			//FVector MoreTest2 = MoreTest

			GetMousePos_Implementation(CameraHit);

			//StartLocation + dir * 10000,

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shot in side Person"));
		}
	}
}
	


void ARubyRose::OnRep_Task() {

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

void ARubyRose::GetMousePos_Implementation(FHitResult Shoot2D) {
	
	AMyPlayerController * ThisPlayer = Cast<AMyPlayerController>(Controller);

	if (ThisPlayer) {

		FCollisionQueryParams CamCollisionParams;
		//cerates the object query parameters
		FCollisionObjectQueryParams CamObjectQueryParams;

		//makes it so the current actor cannot hit them self
		CamCollisionParams.AddIgnoredActor(this);


		FVector2D mousePos = FVector2D(0, 0);
		FVector worldpos; // = FVector(0, mousePos.X, mousePos.Y);
		FVector dir;
		ThisPlayer->GetMousePosition(mousePos.X, mousePos.Y);
		ThisPlayer->DeprojectMousePositionToWorld(worldpos, dir);

		FVector StartLocation = ThisPlayer->GetCharacter()->GetActorLocation();

		FVector EndTest1 = dir * 500;
		FVector EndTest2 = worldpos + EndTest1;
		FVector EndTest = FVector(ThisPlayer->GetCharacter()->GetActorLocation().X, EndTest2.Y, EndTest2.Z);

		MoreTest = EndTest - StartLocation;

		bool CamHitSuccess = GetWorld()->LineTraceSingle(Shoot2D, StartLocation, StartLocation + MoreTest * 100, CamCollisionParams, CamObjectQueryParams);
		DrawDebugLine(GetWorld(), StartLocation, StartLocation + MoreTest * 100, FColor::Blue, true, 5);

		if (CamHitSuccess) {
			DrawDebugLine(GetWorld(), StartLocation, StartLocation + MoreTest * 100, FColor::Green, true, 5);
			ARWBY_CodenameColorsCharacter* TestCharacter = Cast<ARWBY_CodenameColorsCharacter>(Shoot2D.GetActor());

			if (TestCharacter) {

				DrawDebugLine(GetWorld(), StartLocation, StartLocation + MoreTest * 100, FColor::Red, true, 5);

				TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				FDamageEvent DamageEvent(ValidDamageTypeClass);

				//Base Damage Dealer
				TestCharacter->GetShot(20, DamageEvent, ThisPlayer, this);
			}

		}
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shot in side Person"));
	}
}

bool ARubyRose::GetMousePos_Validate(FHitResult Shoot2D) {
	return true;
}


=======
>>>>>>> parent of cd08a69... Shooting now happens from the Ruby class
void ARubyRose::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARubyRose, bCanDealAttackDamage);
	DOREPLIFETIME(ARubyRose, Melee);
	
}
