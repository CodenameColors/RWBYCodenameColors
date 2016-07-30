// Authour: Antonio Morales.

#pragma once

#include "RWBY_CodenameColorsCharacter.h"
#include "RubyRose.generated.h"

/**
 * 
 */
UCLASS()
class RWBY_CODENAMECOLORS_API ARubyRose : public ARWBY_CodenameColorsCharacter
{
	GENERATED_BODY()
	
public:

	ARubyRose();

	void Tick(float DeltaSeconds) override;

protected:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void StartAttack();

	void StopAttack();

	void PerformAttack();

	void OnAttack();

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	//Boolean used to determine the dodging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RWBYCharacter", ReplicatedUsing=OnRep_Attack)
		bool bCanDealAttackDamage;

	//Boolean used to determine the dodging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "RWBYCharacter", Replicated)
		bool bCanDamageSameActor;

private:

	void OnDodge() override;

	void StartDodging() override;

	void StopDodging() override;

	void PerformDodge(bool bDodge) override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose")
	TSubclassOf<class ARosePetal> RosePetal;
	 
	void Spawn(FVector SpawnLocation);

	FTimerHandle PetalSpawnDelay;


//***** ON REP FUNCTIONS*****\\

public:

	UFUNCTION()
	void OnRep_Attack();

public:

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformAttack();
		void ServerPerformAttack_Implementation();
		bool ServerPerformAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformDodge(bool bDodging) ;
		void ServerPerformDodge_Implementation(bool bDodging) ;
		bool ServerPerformDodge_Validate(bool bDodging) ;

};
