// Authour: Antonio Morales.

#pragma once

#include "GameFramework/Actor.h"
#include "RosePetal.generated.h"

UCLASS()
class RWBY_CODENAMECOLORS_API ARosePetal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARosePetal();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:

	//Sets the material of the mesh
	UFUNCTION(BlueprintCallable, Category = "RosePetal")
	void SetPetalMaterial(FString Material);

	//Finds and follows the character for a limited time
	void FollowCharacter();

	//Once the RosePetal has hit a RWBY charcter deal damage of element type
	//and add a slight amount of knockback
	void OnHit();

	FORCEINLINE class UStaticMeshComponent * GetMesh() const { return PetalMesh; }

	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetMesh(class UStaticMeshComponent* NewMesh);

	UPROPERTY(BlueprintReadOnly, Category = "RosePetal", meta = (AllowPrivateAccess = "true"))
	class ARWBY_CodenameColorsCharacter* ActorToIgnore;

	UFUNCTION(BlueprintCallable, Category = "RosePetal")
		void SetActorToIgore(ARWBY_CodenameColorsCharacter* IgnoreThis);

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"), Replicated)
	TArray<class ARWBY_CodenameColorsCharacter*> ActorsToFollow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * PetalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RosePetal", meta = (AllowPrivateAccess = "true"))
	TArray<UMaterial*> DustTypes;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION()
	void OnOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);
	
	UFUNCTION(BlueprintCallable, Category = "RosePetal", meta = (AllowPrivateAccess = "true"))
	void OnOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DestroySelf();

	UFUNCTION()
	void RoseOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RosePetal", meta = (AllowPrivateAccess = "true"), Replicated)
	FString MaterialChoice;

};
