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

	bMeleeAttacking = true;
	PerformAttack(true);

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
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, true, .166f);

		bool MeleeHitResult = GetWorld()->LineTraceSingle(MeleeAttackHitResult, StartLocation, EndLocation, ColliParams, Query);

		if (MeleeHitResult) {

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, .166f);
			
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

		if (bMeleeAttacking && bCanDealAttackDamage) {
			GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Attack End"), Melee);
		}

	}

	OnAttack();
}



void ARubyRose::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARubyRose, bCanDealAttackDamage);
	DOREPLIFETIME(ARubyRose, Melee);

	
}
