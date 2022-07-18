#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UCLASS()
class RESUMESHOWCASE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//function that attaches weapon to holster
	void AttachWeaponToBack();

	//function that attches weapon to hand
	void AttachWeaponToHand();

	//function to be called for doing necessary changes once the weapon is equiped
	void EquipedWeapon();
	//function to be called for doing necessary changes once the weapon is unequiped
	void UnEquipedWeapon();

	//function to set reloading
	void SetReloading(bool val);

	UFUNCTION(BlueprintImplementableEvent)
		class UPlayerScreen* ReturnWidgetPtr();

private:
	/*
	 * using an integer to store several booleans as this saves storage
	 * stores various boolean values
	 * 0 : is sprinting
	 * 1 : wall running
	 * 2 : wall running on a wall to the left
	 * 3 : wall running on a wall to the right
	 * 4 : equiping a weapon(in process of equiping and not equiped yet)
	 * 5 : equiping a pistol(in process of equiping and not equiped yet)
	 * 6 : equiping secondary weapon(in process of equiping and not equiped yet)
	 * 7 : equiped a weapon, i.e weapon is in hand
	 * 8 : equiped Pistol(pistol is in hand)
	 * 9 : equiped Secondary weapon(secondary weapon in hand)
	 * 10: unequiping a weapon(in process of unequiping and not unequiped yet)
	 * 11: unequiping pistol(in process of unequiping and not unequiped yet)
	 * 12: unequiping secondary weapon(in process of unequiping and not unequiped yet)
	 * 13: unequiped weapon
	 * 14: is realoding weapon ?
	 * 15: is aiming
	 */
	int boolVals;
	float Health;

	//parameter which that can be used as collision query params where the player actor is only to be ignored
	FCollisionQueryParams IgnoreSelfCollisionParam;

	//pointer to player's mesh
	USkeletalMeshComponent* PlayerBody;

	//pointer to pistol
	class AWeaponActor* PistolWeapon;
	//pointer to secondary weapon
	AWeaponActor* SecondaryWeapon;
	//ptr to player screen
	UPlayerScreen* PlayerScreenPtr;

	//stores the players movement component
	UCharacterMovementComponent* MovementComponent;
	UPROPERTY(EditAnywhere)
		//sprinting speed value
		float SprintingSpeed = 600.0f;
	UPROPERTY(EditAnywhere)
		//walking speed value
		float WalkingSpeed = 300.0f;
	UPROPERTY(EditAnywhere)
		//axis name used for forward or backward as set in project settings
		FName FwdAxisName;
	UPROPERTY(EditAnywhere)
		//axis name used for right or left as set in project settings
		FName RightAxisName;
	UPROPERTY(EditAnywhere)
		//axis name used for sprinting as set in project settings
		FName SprintingAxisName;
	UPROPERTY(EditAnywhere)
		//axis name used for mouse x as set in project settings
		FName LookAroundName = "LookAround";
	UPROPERTY(EditAnywhere)
		FName FireName = "Fire";	
	UPROPERTY(EditAnywhere)
		//class of the pistol, used for spawning a pistol
		TSubclassOf<AWeaponActor> PistolClass;
	UPROPERTY(EditAnywhere)
		//class of the pistol, used for spawning the secondary weapon
		TSubclassOf<AWeaponActor> SecondGunClass;	
				
	UPROPERTY(EditAnywhere)
		float MaxHealth = 100.0f;
	UPROPERTY(EditAnywhere)
		//name of socket at which primary weapon is attached when player has equiped the weapon
		FName PrimarySocket = "PrimaryWeaponSocket";
	UPROPERTY(EditAnywhere)
		//name of socket at which secondary weapon is attached when player has equiped the weapon
		FName SecondarySocket = "SecondaryWeaponSocket";
	UPROPERTY(EditAnywhere)
		//name of socket where primary weapon is holstered
		FName PistolHolsterName = "PistolHolster";
	UPROPERTY(EditAnywhere)
		//name of socket where secondary weapon is holstered
		FName SecondaryHolsterName = "SecondaryHolster";

	UPROPERTY(EditAnywhere)
		//name of action of pressing keyboard for equiping primary weapon
		FName PrimaryEquip = "PrimaryEquip";
	UPROPERTY(EditAnywhere)
		//name of action of pressing keyboard for equiping secondary weapon
		FName SecondaryEquip = "SecondaryEquip";
	UPROPERTY(EditAnywhere)
		FName AimingAction = "Aim";
	UPROPERTY(EditAnywhere)
		FVector ScaleForBloodDecal = FVector(100.0f, 100.0f, 100.0f);


	UFUNCTION(BlueprintCallable)
		//returns true if player is performing a wall on run on the wall to the left
		bool IsWallRunningOnLeft();
	UFUNCTION(BlueprintCallable)
		//returns true if player is performing a wall on run on the wall to the right
		bool IsWallRunningOnRight();

	UFUNCTION(BlueprintCallable)
		//returns true if player is performing a wall run
		bool IsWallRunning();


	UFUNCTION(BlueprintCallable)
		//returns true if player is equiping pistol
		bool IsEquipingPistol();
	UFUNCTION(BlueprintCallable)
		//returns true if player is equiping secondary weapon
		bool IsEquipingSecondaryWeapon();


	UFUNCTION(BlueprintCallable)
		//returns true if player is unequiping pistol
		bool IsUnEquipingPistol();	
	UFUNCTION(BlueprintCallable)
		//returns true if player is unequiping secondary weapon
		bool IsUnEquipingSecondaryWeapon();


	UFUNCTION(BlueprintCallable)
		//returns true if player is currently equiped secondary **and not in the process of equiping
		bool HasEquipedSecondary();
	UFUNCTION(BlueprintCallable)
		//returns true if player is currently equiped primary **and not in the process of equiping
		bool HasEquipedPrimary();
	UFUNCTION(BlueprintCallable)
		//returns true if aiming
		bool IsAiming();


	//returns true if reloading weapon
	UFUNCTION(BlueprintCallable)		
		bool IsReloadingWeapon();

	//on receiving any damage
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void MoveForwardBackward(float val);
	void MoveRightLeft(float val);

	//function to get the unit vector
	FVector GetUnitVector(FVector v);

	//sets max walk speed to sprinting speed
	void StartSprint();
	//sets max walk speed to walking speed
	void DoneSprint();

	//function for yaw movement of mouse
	void ControllYaw(float val);

	//function that handles starting fire
	void StartFire();

	//function that handles ending fire
	void DoneFire();	

	void StartAiming();
	void DoneAiming();
	void ReloadWeapon();

	//function that determines if player has to equip a weapon or unequip the weapon
	void DecideEquipOrUnequipPrimary();
	void DecideEquipOrUnequipSecondary();

	//function to be called for equiping weapon
	void EquipWeapon(int weaponIndex); 
	//function to be called for unequiping weapon
	void UnEquipWeapon(int weaponIndex); 

	
	//performs null check on the ui ptr and then calls ui ptr functions 
	void SetAmmoOnUI(int weaponIndex);
	//performs null check on the ui ptr and then calls ui ptr functions 
	void SetMagOnUI(int weaponIndex);
	//show everything on the ui
	void ShowEverythingOnUI();
	//function for calling a function on screen ptr to only show player image and health bar
	void ShowWeaponRelated();
	//function for calling a function on screen ptr to show only health bar and image of player
	void ShowOnlyHealthBarAndImage();

	//calls function of weapon pointer to check if weapon can be fired or not
	bool CanFireWeapon(int weaponIndex, bool& bReload);	

	//function to aim weapon
	void SetAiming(bool val);

	//for debugging boolean messages
	void DebugObject(bool val, FString name);

	//function to return if a bit is set or not
	bool IsBitSet(int bitToCheck, int pos);
	//function to clear a bit at a position and return the new bit
	void ClearBitAtPos(int &bitToWorkOn, int pos);
	//function to set bit at a position and return the new bit
	void SetBitAtPos(int &bitToWorkOn, int pos);

	//function for handling the raycast while sprinting, this just sets up parameters
	void SprintingRayCast();
	//function for raycasting and then drawing
	bool RayCast(FHitResult &hitRes, FColor colorToDraw, FVector StartPoint, FVector Dir, float dis, ECollisionChannel ChannelType, FCollisionQueryParams CollisionParam);
};
