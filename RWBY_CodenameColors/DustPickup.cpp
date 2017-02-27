// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "DustPickup.h"
#include "RWBY_CodenameColorsCharacter.h"


// Sets default values
ADustPickup::ADustPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsDustActive = false;

	bReplicates = true;

	collection = CreateDefaultSubobject<UBoxComponent>(TEXT("WhereToSpawn"));
	collection->AttachTo(RootComponent);

	collection->OnComponentBeginOverlap.AddDynamic(this, &ADustPickup::OnOverlap);

}

// Called when the game starts or when spawned
void ADustPickup::BeginPlay()
{
	Super::BeginPlay();

}

//will not be called because this call can not TICK
void ADustPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//When this up has been collected destroy it/ Remove it from the level.
void ADustPickup::WasCollected_Implementation()
{

	Super::WasCollected_Implementation();
	//destroys the actor 
	Destroy();

}
//returns the active state of the Dust Pickup
bool ADustPickup::IsDustActive()
{
	return bIsDustActive;
}

void ADustPickup::OnOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult) {
	if (Role == ROLE_Authority) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OVERLAP DETECTED"));
		ARWBY_CodenameColorsCharacter* Test = Cast<ARWBY_CodenameColorsCharacter>(OtherActor);

		if (Test) {
			Test->Collect();

		}
	}
}

FString ADustPickup::GetDustType() {

	return "";

}