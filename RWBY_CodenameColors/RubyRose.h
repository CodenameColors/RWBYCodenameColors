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


//***** ON REP FUNCTIONS*****\\

public:

	UFUNCTION()
	void OnRep_Attack();

protected:

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformAttack();
		void ServerPerformAttack_Implementation();
		bool ServerPerformAttack_Validate();


};
