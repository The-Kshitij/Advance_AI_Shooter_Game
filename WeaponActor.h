#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS()
class RESUMESHOWCASE_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	int Current_Ammo;
	int Current_Mag;		

	bool bOwnedByPlayer;

	UPROPERTY(EditAnywhere)
		//the number of shots in one magazine
		int MaxAmmo = 7;
	UPROPERTY(EditAnywhere)
		//number of magazine the player has
		int MaxMag = 3;
	UPROPERTY(EditAnywhere)
		/*
		* 0 : single shot
		* 1 : automatic
		*/
		int WeaponType;
	

	//function to be called for firing the weapon
	void FireWeapon();
	//function to be called for reloading the weapon
	void ReloadWeapon();

	//returns true if the weapon can fire
	bool CanFire(bool& bShouldReload);


private:	
	FCollisionQueryParams FireCollisionParam;	

	//list of blood material instances for decals to spawn from  once 
	//a character is hit
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInstance*> BloodMaterials;
	//list of bullet hole material instances for decals
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInstance*> BulletHoleMaterials;
	UPROPERTY(EditAnywhere)
		//body of weapon
		USkeletalMeshComponent* WeaponBody;
	UPROPERTY(EditAnywhere)
		//firing animation to be played on body of weapon
		UAnimationAsset* FiringAnimation;
	UPROPERTY(EditAnywhere)
		//reloading animation to be played on body of weapon
		UAnimationAsset* ReloadingAnimation;
	UPROPERTY(EditAnywhere)
		FName FiringSocketName;

	UPROPERTY(EditAnywhere)
		float DamageOfWeapon = 20.0f;
	UPROPERTY(EditAnywhere)
		float RangeOfWeapon = 1000.0f;
	UPROPERTY(EditAnywhere)
		//damageclass of weapon, to be set in blueprint
		TSubclassOf<class UDamageType> DamageClass;	

};
