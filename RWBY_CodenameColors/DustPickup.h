// Authour: Antonio Morales.

#pragma once

#include "Pickup.h"
#include "DustPickup.generated.h"

UCLASS()
class RWBY_CODENAMECOLORS_API ADustPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADustPickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//this function is called when this pickup has been collected 
	void WasCollected_Implementation();

	//Returns the state of the Dust power up
	UFUNCTION(BlueprintCallable, Category = "Dust_Pickup")
		bool IsDustActive();

protected:
	//holds the state of the dust power up's active state
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = " Dust_Pickup", meta = (BlueprintProtect = "true"))
		bool bIsDustActive;

	UFUNCTION()
		void OnOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* collection;

	//
};
	

