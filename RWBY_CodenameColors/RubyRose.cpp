// Authour: Antonio Morales.

#include "RWBY_CodenameColors.h"
#include "RubyRose.h"
#include "UnrealNetwork.h"

//Sets the default values of this class
ARubyRose::ARubyRose() {

	Super::SetMaxAmmo(25);

	CurrentAmmo = MaxAmmo / 5;

	//CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ARWBY_TestingCharacter::OnOverlap);
}

void ARubyRose::SetupPlayerInputComponent(class UInputComponent* InputComponent){

	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("Attack", IE_Pressed, this, &ARubyRose::StartAttack);
	//InputComponent->BindAction("Shoot", IE_Released, this, &ARWBY_CodenameColorsCharacter::StopShooting);

}

void ARubyRose::StartAttack(){

}

void ARubyRose::StopAttack(){

}

void ARubyRose::PerformAttack(){

}

void ARubyRose::OnAttack(){

}


void ARubyRose::ServerPerformAttack_Implementation(){

}

bool ARubyRose::ServerPerformAttack_Validate(){

	return true;
}

void ARubyRose::OnRep_Attack() {

}

void ARubyRose::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
