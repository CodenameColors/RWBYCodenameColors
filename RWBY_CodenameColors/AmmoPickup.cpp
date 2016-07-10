// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "AmmoPickup.h"
#include "RWBY_CodenameColorsCharacter.h"

AAmmoPickup::AAmmoPickup() {

	//THIS NEEDS TO BE TRUE IN ORDER TO HAVE FUNCTION ON ALL WINDOWS
	bReplicates = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//default ammo multiplayer (unused for now)
	AmmoPickup = 1.4;

	Collection = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnSocket"));
	Collection->AttachTo(RootComponent);

	Collection->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickup::OnOverlap);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> AmmoCrate(TEXT("StaticMesh'/Game/RWBY/Props/Ammo/Ammo_Pickup.Ammo_Pickup'"));

	if (AmmoCrate.Succeeded()) {
		GetMesh()->SetStaticMesh(AmmoCrate.Object);
	}

	GetMesh()->SetWorldScale3D(FVector(.3f, .3f, .3f));
	Collection->SetWorldScale3D(FVector(1.f, 1.f, 1.f));

}

void AAmmoPickup::WasCollected_Implementation() {

	Super::WasCollected_Implementation();

	Destroy();
}

//Returns the base ammo multiplier (Not used ATM)
float AAmmoPickup::GetAmmoPickup()
{
	return AmmoPickup;
}

// Sets a new ammo multipiler (not used ATM)
void AAmmoPickup::SetAmmoPickup(float NewAmmoAmount)
{
	AmmoPickup = NewAmmoAmount;

}

void AAmmoPickup::OnOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult) {

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OVERLAP DETECTED"));
	
	ARWBY_CodenameColorsCharacter* RWBYCharacter = Cast<ARWBY_CodenameColorsCharacter>(OtherActor);

	if (RWBYCharacter) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OVERLAPED WITH RWBYCHARACTER"));

		RWBYCharacter->Collect();
	}

}