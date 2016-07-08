// Authour: Antonio Morales.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class RWBY_CODENAMECOLORS_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FORCEINLINE class UStaticMeshComponent * GetMesh() const { return PickupMesh; }
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetMesh(class UStaticMeshComponent* NewMesh);

	//Returns the active state of the current pickup
	UFUNCTION(BlueprintPure, Category = "Pickups")
	bool IsActive();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool NewActiveState);

	UFUNCTION(BlueprintNativeEvent)
		void WasCollected();
		virtual void WasCollected_Implementation();

protected: 
	//holds the active state
	bool bIsActive;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * PickupMesh;

};
