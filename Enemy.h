// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class RESUMESHOWCASE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:	
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void StartFire();
	void DoneFire();

	UFUNCTION(BlueprintCallable)
		FVector GetNextPoint();	
	UFUNCTION(BlueprintCallable)
		bool bIsDead();


	//the duration to wait before firing the weapon.
	UPROPERTY(EditAnywhere)
		float DelayOfFire = 0.5f;

	float Timer;
	
private:
	int PatrolInd;
	bool bFiring;
	bool bDead;
	class AAIController* Aicontroller;

	UPROPERTY(EditAnywhere)
		float Health = 100.0f;
	UPROPERTY(EditAnywhere)
		bool bPatrolRandom = false;
	//so that the desinger can place the points on the level freely
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
		TArray<FVector> PatrolPoints;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeaponActor> WeaponClass;
	UPROPERTY(EditAnywhere)
		FName WeaponHolsterName;

	AWeaponActor* Weapon;

	//on receiving any damage
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
};
