// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "SpawnPoint.h"
#include "Pickup.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Creates the Box collision/ spawn area
	PointToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnPoint"));
	RootComponent = PointToSpawn;

	//Set the default spawn delay (can be changed in the editor)
	SpawnDelayHigh = 2.0f;
	SpawnDelayLow = .75f;

	//Can spawn when instatiated
	bShouldSpawn = true;
}
//
// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	//Calls the inheirted begin play function
	Super::BeginPlay();

	//Sets up a timer for the spawn points. After the time delay finishes then the spawn function is delegated to.
	SpawnDelay = FMath::FRandRange(SpawnDelayLow, SpawnDelayHigh);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnPoint::Spawn, SpawnDelay, false);

}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	//inheirted function call
	Super::Tick(DeltaTime);

}

void ASpawnPoint::SetSpawningActive(bool bShouldSpawn)
{
	//if we should spawn a pickup the following will happen
	if (bShouldSpawn) {
		//sets the timer on the spawn pickup
		SpawnDelay = FMath::FRandRange(SpawnDelayLow, SpawnDelayHigh);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnPoint::Spawn, SpawnDelay, false);
	}
	//if we should not spawn a pickup at the time then
	else {
		//clear the timer on the pickup
		GetWorldTimerManager().ClearTimer(SpawnTimer);
	}
}

// Use this Function if a random point in the bounding box is wanted to spawn
FVector ASpawnPoint::GetSpawnPoint()
{
	//Finds the Origin of the box
	FVector SpawnOrigin = PointToSpawn->Bounds.Origin;
	//finds the bounds of the box
	FVector SpawnExtent = PointToSpawn->Bounds.BoxExtent;

	//returns a random vector in between the two
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);


}

// Function Used to the spawn the pickups
void ASpawnPoint::Spawn()
{
	//Checks to see if there was a spawning mesh selected.
	if (SpawningMesh != NULL) {

		//Gets the world that the spawn point is in
		UWorld* const World = GetWorld();

		//Checks to see if the world is valid
		if (World) {

			// The parameters of which this should spawn
			FActorSpawnParameters SpawnParams;
			// Sets the owner to this spawn point
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			//Gets the orgin of the box, which is the place the pickup will spawn
			FVector SpawnLocation = PointToSpawn->Bounds.Origin;

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
			if (GetShouldSpawn()) {
				//Spawns the pickup
				APickup* const SpawnedPickup = World->SpawnActor<APickup>(SpawningMesh, SpawnLocation, SpawnRotation);
				//Resets the Spawn delay timer
				SpawnDelay = FMath::FRandRange(SpawnDelayLow, SpawnDelayHigh);
				GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnPoint::Spawn, SpawnDelay, false);

			}
			else {
				//If False Try Again
				SetSpawningActive(true);
			}

		}

	}
}

// Returns a boolean that decides whether the pickup should spawn something at that time
bool ASpawnPoint::GetShouldSpawn() {

	//Create an Array to hold all the overlapping Actors in
	TArray<AActor*> CollectedActors;
	//Puts all the overlaping Actors in said array
	PointToSpawn->GetOverlappingActors(CollectedActors);

	// For each Actor we collected
	for (int32 iCollected = 0; iCollected < CollectedActors.Num(); iCollected++)
	{
		// Cast the actor to APickup
		APickup* const TestPickup = Cast<APickup>(CollectedActors[iCollected]);
		// Cast the actor to ACharacter
		AActor* const isCharacter = Cast<ACharacter>(CollectedActors[iCollected]);

		// If the pickup is valid/ and already spawned, AND if there is a character in box then DON'T SPAWN
		if (TestPickup && !TestPickup->IsPendingKill() && TestPickup->IsActive() && isCharacter)
		{
			UE_LOG(LogClass, Log, TEXT("Spawn = false "));
			return false;

		}
		// If there is NOT a character in the box, AND if there is a Pickup spawned, DON'T SPAWN
		else if (!isCharacter && TestPickup) {
			UE_LOG(LogClass, Log, TEXT("Spawn = false2"));

			return false;
		}
		// if there is character in the box, AND if there is not pickup spawned there SPAWN
		else if (isCharacter && !TestPickup) {
			UE_LOG(LogClass, Log, TEXT("Spawn = true"));

			return true;
		}
		// if there is a NOT a Character in the box, AND if there is no valid pickup spawned already then SPAWN
		else if (!isCharacter && !TestPickup) {
			UE_LOG(LogClass, Log, TEXT("Spawn = true2"));

			return true;
		}
		//Don't Spawn if there is nothing in the array
		return false;
	}
	//spawn if everything fails (Fail safe)
	return true;
}

// forces spawns
void ASpawnPoint::SetShouldSpawn(bool NewSpawnState) {

	bShouldSpawn = NewSpawnState;

}