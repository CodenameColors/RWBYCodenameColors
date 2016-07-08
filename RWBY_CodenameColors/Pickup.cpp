// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "Pickup.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsActive = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;


}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//Creates a FRotator of the type APickup
	FRotator ActorRotation = APickup::GetActorRotation();

	//adds 1.5 to the FRotator every frame
	ActorRotation.Yaw += 1.5;

	//Takes the ActorRotation FRotator and then sets it to be used on the current actor/ All of the actor's components
	APickup::SetActorRotation(ActorRotation);


}

bool APickup::IsActive() {
	return bIsActive;
}

void APickup::SetActive(bool NewActiveState) {
	bIsActive = NewActiveState;
}

void APickup::WasCollected_Implementation() {

	FString PickupDebugString = GetName();
	UE_LOG(LogClass, Log, TEXT("You Have Collected %s"), *PickupDebugString);
}

bool APickup::WasCollected_Validate() {

	return true;
}


void APickup::SetMesh(class UStaticMeshComponent* NewMesh) {
	PickupMesh = NewMesh;
}
