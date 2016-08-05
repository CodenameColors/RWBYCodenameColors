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

	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* Melee;

protected:

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void PerformAttack(bool ShouldAttack);

	void OnAttack();

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	//Boolean used to determine the dodging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RWBYCharacter", Replicated)
		bool bCanDealAttackDamage;

	//Boolean used to determine the dodging
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "RWBYCharacter", Replicated)
		bool bCanDamageSameActor;

private:

	void OnDodge() override;

	void StartDodging() override;

	void StopDodging() override;

	void PerformDodge(bool bDodge) override;

	//method used start shooting
	void StartShooting();

	//method used to stop shooting
	void StopShooting();

	//method used to preform tasks (Client)
	void PerformTask(ETask::Type NewTask) override;

	void OnFire() override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose")
	TSubclassOf<class ARosePetal> RosePetal;
	 
	void Spawn(FVector SpawnLocation);

	FTimerHandle PetalSpawnDelay;

	void SetAttackingBool(bool NewBoolState);

	FTimerHandle Attack;

	//void setStupid(class ARWBY_CodenameColorsCharacter* EPoweredUpState::Type);

//***** ON REP FUNCTIONS*****\\

public:

	UFUNCTION()
	void OnRep_MeleeAttack() override;

	UFUNCTION()
	void OnRep_Task() override;


public:

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformAttack(bool ShouldAttack);
		void ServerPerformAttack_Implementation(bool ShouldAttack);
		bool ServerPerformAttack_Validate(bool ShouldAttack);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformDodge(bool bDodging) ;
		void ServerPerformDodge_Implementation(bool bDodging);
		bool ServerPerformDodge_Validate(bool bDodging);

	UFUNCTION(Server, Reliable, WithValidation)
		void StartAttack();
		void StartAttack_Implementation();
		bool StartAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void StopAttack();
		void StopAttack_Implementation();
		bool StopAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformTask_(ETask::Type NewTask);
		void ServerPerformTask__Implementation(ETask::Type NewTask);
		bool ServerPerformTask__Validate(ETask::Type NewTask);


};
