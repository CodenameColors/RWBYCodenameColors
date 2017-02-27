// Authour: Antonio Morales.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class RWBY_CODENAMECOLORS_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASpawnPoint();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//returns the pointToSpawn sub object
	FORCEINLINE class UBoxComponent* GetPointToSpawn() const { return PointToSpawn; }

	// Decides whether of not to spawn pickups
	UFUNCTION(BlueprintCallable, Category = "Spawning")
		void SetSpawningActive(bool bShouldSpawn);

	//If Set True if a pickup has been spawned, and not picked up
	void SetAlreadySpawned(bool NewState);

	//Our Server funtion to update power up state.
	//UFUNCTION(Reliable, Server, WithValidation)
	//void UpdatePowerup(bool newPowerupState);
	//void UpdatePowerup_Implementation(bool newPowerupState);
	//bool UpdatePowerup_Validate(bool newPowerupState);
	//

protected:

	//the pickup to spawn (can be chosen in the editor)
	UPROPERTY(EditAnywhere, Category = "Spawning")
		TSubclassOf<class APickup> SpawningMesh;

	//the timer used to spawn
	FTimerHandle SpawnTimer;

	//Maximum spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float SpawnDelayHigh;

	//Minimum spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float SpawnDelayLow;

	//returns the point of where to spawn the pickups
	UFUNCTION(BlueprintCallable, Category = " Spawning")
		FVector GetSpawnPoint();

	// returns true if the Spawn Point can spawn
	bool GetShouldSpawn();

	// Makes a pickup spawn
	void SetShouldSpawn(bool NewSpawnState);

private:

	// Box component to specfiy where the pickups should be spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "True"))
	class UBoxComponent* PointToSpawn;

	//spawns the pickups
	void Spawn();

	//return the current spawn delay of the pickups
	float SpawnDelay;

	//where the pickups will be spawned
	FVector SpawnOrigin;

	// determines whether or not this spawn point should spawn
	bool bShouldSpawn;

	//Set true if the a pickup has been spawned
	bool AlreadySpawned;

};
