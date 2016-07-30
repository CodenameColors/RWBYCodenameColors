// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "RubyRose.h"
#include "UnrealNetwork.h"
#include "CharacterPowers/RosePetal.h"
#include "MyPlayerController.h"

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

}

void ARubyRose::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARubyRose::StartDodging);
	InputComponent->BindAction("Dodge", IE_Released, this, &ARubyRose::StopDodging);

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

void ARubyRose::StartAttack(){

}

void ARubyRose::StopAttack(){

}

void ARubyRose::PerformAttack(){

}

void ARubyRose::OnAttack(){

}


void ARubyRose::ServerPerformAttack_Implementation(){

}

bool ARubyRose::ServerPerformAttack_Validate(){

	return true;
}

void ARubyRose::OnRep_Attack() {

}

void ARubyRose::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
