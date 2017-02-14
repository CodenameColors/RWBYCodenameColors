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
	/*
	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* Melee override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* LightMontage override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* HeavyMontage override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* LightDashMontage override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose", Replicated)
		UAnimMontage* HeavyDashMontage override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ruby Rose", Replicated)
		float SemblanceMultiplier;

		*/
	


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

	virtual void BeginPlay();


	void UseSemblance() override;
	void PerformSemblance() override;

	void OnDodge() override;

	void StartDodging() override;

	void StopDodging() override;

	void OnFire() override;

	void PerformDodge(bool bDodge) override;

	//method used start shooting
	void StartShooting() override;

	//method used to stop shooting
	void StopShooting() override;

	void PerformTask(ETask::Type NewTask) override;

	UPROPERTY(EditAnywhere, Category = "Ruby Rose")
	TSubclassOf<class ARosePetal> RosePetal;
	 
	void Spawn(FVector SpawnLocation);

	FTimerHandle PetalSpawnDelay;

	void SetAttackingBool(bool *OldBool, bool NewBoolState);

	FTimerHandle Attack;

	void CreateTree();
	

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

		/*
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void LightAttack();
		void LightAttack_Implementation();
		bool LightAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerLightAttack();
		void ServerLightAttack_Implementation();
		bool ServerLightAttack_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void AnimationTesting();
		void AnimationTesting_Implementation();
		bool AnimationTesting_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void AnimationTesting1();
		void AnimationTesting1_Implementation();
		bool AnimationTesting1_Validate1();
		*/
	

	UFUNCTION(Server, Reliable, WithValidation)
		void StopAttack();
		void StopAttack_Implementation();
		bool StopAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformTask_(ETask::Type NewTask);
		void ServerPerformTask__Implementation(ETask::Type NewTask);
		bool ServerPerformTask__Validate(ETask::Type NewTask);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformSemblance();
		void ServerPerformSemblance_Implementation();
		bool ServerPerformSemblance_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetValue(float NewFloatValue);
		void ServerSetValue_Implementation(float NewFloatValue);
		bool ServerSetValue_Validate(float NewFloatValue);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		 void LightAttack();
		 void LightAttack_Implementation();
		 bool LightAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		 void ServerPerformLightAttack();
		 void ServerPerformLightAttack_Implementation();
		 bool ServerPerformLightAttack_Validate();

		UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMultiCastPerformLightAttack();
		void NetMultiCastPerformLightAttack_Implementation();
		bool NetMultiCastPerformLightAttack_Validate();




	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void HeavyAttack();
		void HeavyAttack_Implementation();
		bool HeavyAttack_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformHeavyAttack();
		void ServerPerformHeavyAttack_Implementation();
		bool ServerPerformHeavyAttack_Validate();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMultiCastPerformHeavyAttack();
		void NetMultiCastPerformHeavyAttack_Implementation();
		bool NetMultiCastPerformHeavyAttack_Validate();
};
