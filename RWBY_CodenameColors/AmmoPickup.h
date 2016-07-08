// Authour: Antonio Morales.

#pragma once

#include "Pickup.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	
	AAmmoPickup();

	//This will be called when this pickup is collected
	void WasCollected_Implementation();

	// returns the float AmmoPickup
	float GetAmmoPickup();

	//manually sets the ammo amount for this pickup
	void SetAmmoPickup(float NewAmmoAmount);

protected:

	//base ammo
	float AmmoPickup;

	UFUNCTION()
		void OnOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", Meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* Collection;
	
};
