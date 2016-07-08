// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "RWBY_CodenameColorsCharacter.generated.h"

UENUM() 
	namespace ECameraType{
		enum Type {
			None,
			Side,
			Third,
		};
}

UENUM()
	namespace ETask {
		enum Type {
			None, 
			Shooting,
			Reload,
	};
}

UCLASS(config=Game)
class ARWBY_CodenameColorsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* CharMesh;


	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ThirdPersonCameraBoom;


protected:

	//method for replication of variables
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//method used to switch the camera, and switch movement modes
	void SwitchCamera();

	//method used to fire from the center of the camera (screen)
	void OnFire();

	void OnDodge();

	//method used start shooting
	void StartShooting();

	//method used to stop shooting
	void StopShooting();

	//method used to start dodging
	void StartDodging();

	//method used to stop doding
	void StopDodging();

	void PerformDodge(bool bDodge);

	//method used to preform tasks (Client)
	void PerformTask(ETask::Type NewTask);

	//Enum used to determine the camera/ movement state of the characters
	ECameraType::Type Perspective;

	//Boolean used to determine the dodging
	UPROPERTY(ReplicatedUsing = OnRep_Dodge, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "RWBYCharacter" )
	bool isDodging;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);


	/** Called for side to side input */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(ReplicatedUsing=OnRep_Health)
		float Health;

	UPROPERTY(ReplicatedUsing = OnRep_Task)
		TEnumAsByte<ETask::Type> Task;

	FTimerHandle TimerHandler_Task;

	FTimerHandle DodgeDelay;

public:
	ARWBY_CodenameColorsCharacter();

	//method from the base class AActor, used to take damage. includes damage amount, infomation about the event, and the player of who did this
	float TakeDamage(float DamageAmount, const FDamageEvent & DamageEvent, AController* EventInstigator, AActor * DamageCauser) override;

	//Deal damage
	void DealDamage(float Damage, FHitResult LineTrace);
	/*Replication methods (on_Reps)
	*
	*
	*/

	UFUNCTION()
		void OnRep_Task();
	
	UFUNCTION()
		void OnRep_Health();
	
	UFUNCTION()
		void OnRep_Dodge();
	
	/*Other methods
	*
	*
	*/

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;


	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return CharMesh; }

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetThirdPersonCameraBoom() const { return ThirdPersonCameraBoom; }

	/*
	* THIS SECTION IS FOR REPLICATION METHODS
	*
	*/
protected:

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformTask(ETask::Type NewTask);
		void ServerPerformTask_Implementation(ETask::Type NewTask);
		bool ServerPerformTask_Validate(ETask::Type NewTask);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPerformDodge(bool bDodging);
		void ServerPerformDodge_Implementation(bool bDodging);
		bool ServerPerformDodge_Validate(bool bDodging);

};
