// comments for function and variables are in the .h files

//todo: have to improve wall run later, right now player moves towards local right


/*
	* flow of things explained here
	* Character movement is handled by the two MoveFunctions, the binding of these functions is done
	  in the SetupPlayerInput function.
	* When player is sprinting, then line tracing continously happens, to check for walls on either
	  side of the player. If any wall is found then the player can run on the wall. To create a wall
	  run effect, gravity scale is set to zero for the duration of wall run and the player is 
	  prohibited from moving the mouse or moving left or right during this process. I had created a 
	  wall run animation my self so there is no root motion involed.
	* When equiping or unequiping a weapon, a function is called to decide if the player should equip
	  or unequip the weapn. If already equiping, then unequip is called and vice-versa. The process of
	  equiping weapon involves 2 steps. 
	  1)The first is setting bits in the boolVals integer for equiping
		weapon and then setting the weapon specific bit. Now in the animation BP, the animation of equiping
		weapon is played. That animation has PlayerNotifiStates that attaches weapon to player's hand
		or back at the right time.
	  2)The second step is done from PlayerNotifiStates itself, but from the done notify part or the 
		ending notifi. It unsets the equiping weapon bit and equiping specific weapon(primary or 
		secondary) bit and then sets the equiped weapon and equiped weapon type bits.
	* Initially player is unarmed, so unequiped bit is set to true
	* For knowing which bit represents what check the .h files
	* 

*/

#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

#include "WeaponActor.h"
#include "PlayerScreen.h"
#include "Components/DecalComponent.h"

//debug function to display the value of bit in binary form
void DisplayBitValue(int bitVal)
{
	UE_LOG(LogTemp, Warning, TEXT("Value recieved: %d"),bitVal);
	FString bitInStr;
	FString Indices;
	int cnt = 0;
	while (bitVal != 0)
	{
		bitInStr += (bitVal % 2) ? "1," : "0,";
		bitVal /= 2;
		Indices += FString::FromInt(cnt) + ",";
		++cnt;
	}
	bitInStr = bitInStr.Reverse();
	Indices = Indices.Reverse();
	UE_LOG(LogTemp, Warning, TEXT("bitVal recieved: %s"), *(bitInStr));
	UE_LOG(LogTemp, Warning, TEXT("indices        : %s"), *(Indices));
	FString beautifying = "----------------------------------------------------------------";
	UE_LOG(LogTemp, Error, TEXT("%s"),*beautifying);
}

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AMainCharacter::DebugObject(bool val, FString name)
{
	if (val)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s found."), *name);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s not found."), *name);
	}
}


bool AMainCharacter::IsBitSet(int bitToCheck, int pos)
{
	return (bitToCheck & (1<<pos));
}

void AMainCharacter::ClearBitAtPos(int &bitToWorkOn, int pos)
{
	bitToWorkOn = (bitToWorkOn & (~(1<<pos)));
	//DisplayBitValue(boolVals);
}

void AMainCharacter::SetBitAtPos(int &bitToWorkOn, int pos)
{
	bitToWorkOn = (bitToWorkOn | (1<<pos));
	//DisplayBitValue(boolVals);
}

bool AMainCharacter::RayCast(FHitResult& hitRes, FColor colorToDraw, FVector StartPoint, FVector Dir, float dis, ECollisionChannel ChannelType, FCollisionQueryParams CollisionParam)
{
	//DrawDebugLine(GetWorld(), StartPoint, StartPoint + Dir * dis, colorToDraw, true, 3.0f);
	return (GetWorld()->LineTraceSingleByChannel(hitRes, StartPoint, StartPoint + Dir * dis, ChannelType, CollisionParam));
}

FVector AMainCharacter::GetUnitVector(FVector v)
{
	float mag = v.Size();
	return (v / mag);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	IgnoreSelfCollisionParam.AddIgnoredActor(this);
	MovementComponent = GetCharacterMovement();

	DebugObject(MovementComponent!=nullptr, "movement component");
	DoneSprint();

	//spawning primary weapon
	PistolWeapon = GetWorld()->SpawnActor<AWeaponActor>(PistolClass);
	DebugObject(PistolWeapon!=nullptr, "Pistol");
	//spawning secondary weapon
	SecondaryWeapon = GetWorld()->SpawnActor<AWeaponActor>(SecondGunClass);
	DebugObject(SecondaryWeapon != nullptr, "Rifle");

	//setting owner as the player so that the weapon doesn't harm the player itself
	if (PistolWeapon) PistolWeapon->SetOwner(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (SecondaryWeapon) SecondaryWeapon->SetOwner(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	//storing player's body because it is often used
	PlayerBody = GetMesh();
	if (PlayerBody)
	{
		UE_LOG(LogTemp, Warning, TEXT("name of body: %s"),*(PlayerBody->GetName()));		
	}

	//initially want all weapons to stay attached to the holster
	if (PistolWeapon && PlayerBody) PistolWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, PistolHolsterName);
	if (SecondaryWeapon && PlayerBody) SecondaryWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, SecondaryHolsterName);
		

	//initially unsetting all bits
	boolVals = 0;
	//player is not equiping any weapon
	SetBitAtPos(boolVals, 13);

	DisplayBitValue(boolVals);
	PlayerScreenPtr = ReturnWidgetPtr();

	DebugObject(PlayerScreenPtr != nullptr, "PlayerScreenPtr ");
	if (PlayerScreenPtr)
	{
		PlayerScreenPtr->HideWeaponRelated();
	}
	Health = MaxHealth;

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//setting movement mode to normal walking, if change is required then below lines would change this
	if (MovementComponent)
	{		
		MovementComponent->GravityScale = 1.0f;
	}

	if (IsBitSet(boolVals, 0))
	{		
		SprintingRayCast();		
	}	

	//in order to help wall runing since root motion is not being used
	if (IsBitSet(boolVals, 1))
	{
		if (MovementComponent) MovementComponent->GravityScale = 0.0f;
	}
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//axis bindings
	PlayerInputComponent->BindAxis(FwdAxisName, this, &AMainCharacter::MoveForwardBackward);
	PlayerInputComponent->BindAxis(RightAxisName, this, &AMainCharacter::MoveRightLeft);
	PlayerInputComponent->BindAxis(LookAroundName, this, &AMainCharacter::ControllYaw);
	
	//action bindings
	//sprinting related
	PlayerInputComponent->BindAction(SprintingAxisName, IE_Pressed, this, &AMainCharacter::StartSprint);
	PlayerInputComponent->BindAction(SprintingAxisName, IE_Released, this, &AMainCharacter::DoneSprint);
	
	//firing related
	PlayerInputComponent->BindAction(FireName, IE_Pressed, this, &AMainCharacter::StartFire);
	PlayerInputComponent->BindAction(FireName, IE_Released, this, &AMainCharacter::DoneFire);

	//equiping or unequiping weapon related
	PlayerInputComponent->BindAction(PrimaryEquip, IE_Pressed, this, &AMainCharacter::DecideEquipOrUnequipPrimary);
	PlayerInputComponent->BindAction(SecondaryEquip, IE_Pressed, this, &AMainCharacter::DecideEquipOrUnequipSecondary);

	//aiming related
	PlayerInputComponent->BindAction(AimingAction, IE_Pressed, this, &AMainCharacter::StartAiming);
	PlayerInputComponent->BindAction(AimingAction, IE_Released, this, &AMainCharacter::DoneAiming);
	
}

void AMainCharacter::MoveForwardBackward(float val)
{	
	//when wall running then, the location is handled by code so need for
	//player's input on the movement
	if (!IsWallRunning()) AddMovementInput(GetActorForwardVector() * val);
}

void AMainCharacter::MoveRightLeft(float val)
{
	//don't want player to move left or right if the player is performing a wall run
	if (!(IsBitSet(boolVals, 1) && (IsBitSet(boolVals, 2) || IsBitSet(boolVals, 3)))) AddMovementInput(GetActorRightVector() * val);
}

void AMainCharacter::ControllYaw(float val)
{
	//while wall running don't want the player to change the rotation of yaw
	if (!IsWallRunning())
		AddControllerYawInput(val);
}

void AMainCharacter::StartSprint()
{
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = SprintingSpeed;
	}
	SetBitAtPos(boolVals, 0);
}

void AMainCharacter::DoneSprint()
{
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = WalkingSpeed;
	}
	ClearBitAtPos(boolVals, 0);

	//no wall running if not sprinting
	ClearBitAtPos(boolVals, 1);
	ClearBitAtPos(boolVals, 2);
	ClearBitAtPos(boolVals, 3);
}

void AMainCharacter::SprintingRayCast()
{
	//starting the raycast some distance ahead
	FVector StartPoint = GetActorLocation() + 30.0f * GetUnitVector(GetActorForwardVector());
	//extent to do raycast.
	float dis = 130.0f;

	//raycasting to player's left
	FVector EndPoint = StartPoint + dis * GetUnitVector(-1 * GetActorRightVector());
	FVector Direction = (EndPoint - StartPoint).GetSafeNormal();

	//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *(Direction.ToString()));
	FHitResult hitRes;

	//need to change this, otherwise player will sprint at any object on it's side
	if (RayCast(hitRes, FColor::Red, StartPoint, Direction, dis, ECollisionChannel::ECC_Visibility, IgnoreSelfCollisionParam))
	{		
		UE_LOG(LogTemp, Warning, TEXT("hitting %s"), *(hitRes.GetActor()->GetName()));
		//shifting the point back to original value.		
		FVector pt = -10.0f * GetActorForwardVector() + hitRes.ImpactPoint;
		//since ray cast happened from player's left, it means that 
		//if pt would be shifted towards player's right to find the proper
		//value for x.
		pt += 90.0f * (GetActorRightVector());		
		SetActorLocation(pt);
		SetBitAtPos(boolVals, 1);
		SetBitAtPos(boolVals, 2);
	}
	else
	{		

		//raycasting to player's right
		EndPoint = StartPoint + dis * GetUnitVector(GetActorRightVector());
		Direction = (EndPoint - StartPoint).GetSafeNormal();

		//UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *(Direction.ToString()));		

		if (RayCast(hitRes, FColor::Red, StartPoint, Direction, dis, ECollisionChannel::ECC_Visibility, IgnoreSelfCollisionParam))
		{
			UE_LOG(LogTemp, Warning, TEXT("hitting %s"), *(hitRes.GetActor()->GetName()));
			//shifting the point back to original value.
			//shifting the point back to original value.		
			FVector pt = -10.0f * GetActorForwardVector() + hitRes.ImpactPoint;
			//since ray cast happened from player's right, it means that 
			//if pt would be shifted towards player's left to find the proper
			//value for x.
			pt += 90.0f * (-GetActorRightVector());
			SetActorLocation(pt);			
			SetBitAtPos(boolVals, 1);
			SetBitAtPos(boolVals, 3);
		}
		else
		{
			//stop wall running if no object found.
			ClearBitAtPos(boolVals, 1);
			ClearBitAtPos(boolVals, 2);
			ClearBitAtPos(boolVals, 3);
		}
	}
}

bool AMainCharacter::IsWallRunning()
{
	return IsBitSet(boolVals, 1);
}

bool AMainCharacter::IsWallRunningOnLeft()
{
	//player would be wall running, just checking that too
	return IsBitSet(boolVals, 2) && IsBitSet(boolVals, 1);
}

bool AMainCharacter::IsWallRunningOnRight()
{
	//player would be wall running, just checking that too
	return IsBitSet(boolVals, 3) && IsBitSet(boolVals, 1);
}

bool AMainCharacter::IsEquipingPistol()
{
	return IsBitSet(boolVals, 5) && IsBitSet(boolVals, 4);
}

bool AMainCharacter::IsEquipingSecondaryWeapon()
{
	return IsBitSet(boolVals, 6) && IsBitSet(boolVals, 4);
}


bool AMainCharacter::IsUnEquipingPistol()
{
	return IsBitSet(boolVals, 10) && IsBitSet(boolVals, 11);
}

bool AMainCharacter::IsUnEquipingSecondaryWeapon()
{
	return IsBitSet(boolVals, 10) && IsBitSet(boolVals, 12);
}

bool AMainCharacter::HasEquipedSecondary()
{
	//need to ensure that secondary has been equiped **and not in the process of being equiped
	return IsBitSet(boolVals, 7) && IsBitSet(boolVals, 9) && !IsBitSet(boolVals, 4);
}

bool AMainCharacter::HasEquipedPrimary()
{
	//need to ensure that primary has been equiped **and not in the process of being equiped
	return IsBitSet(boolVals, 7) && IsBitSet(boolVals, 8) && !IsBitSet(boolVals, 4);
}

bool AMainCharacter::IsAiming()
{
	return IsBitSet(boolVals, 15);
}

void AMainCharacter::StartFire()
{
	//player should not be reloading and be aiming
	if (!IsReloadingWeapon() && IsAiming())
	{
		bool bReload;
		bool bCanFire;
		int weaponIndex = 0;
		if (HasEquipedPrimary()) bCanFire = CanFireWeapon(0, bReload);
		else if (HasEquipedSecondary())
		{
			bCanFire = CanFireWeapon(1, bReload);
			weaponIndex = 1;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("(MainCharacter)Couldn't find weapon index in StartFire"));
		}

		//bCanFire and !bReload are handled by CanFireWeapon, so the below is just a
		//check for condition.
		if (bCanFire && !bReload)
		{
			switch (weaponIndex)
			{
			case 0:
				if (PistolWeapon)
				{
					PistolWeapon->FireWeapon();
				}
				break;
			case 1:
				if (SecondaryWeapon)
				{
					SecondaryWeapon->FireWeapon();					
				}
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("no case for %d"), weaponIndex);
			}
			SetAmmoOnUI(weaponIndex);
			SetMagOnUI(weaponIndex);

		}
		else if (!bCanFire && bReload)
		{
			ReloadWeapon();
		}
	}
}

void AMainCharacter::DoneFire()
{

}

bool AMainCharacter::CanFireWeapon(int weaponIndex, bool &bReload)
{
	bool bRes = false;
	bReload = false;
	//CanFire will return false if weapon has to be fired.
	switch (weaponIndex)
	{
	case 0:
		if (PistolWeapon != nullptr) bRes = PistolWeapon->CanFire(bReload);
		break;
	case 1:
		if (SecondaryWeapon != nullptr) bRes = SecondaryWeapon->CanFire(bReload);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("no case for weapon index %d"),weaponIndex);
	}
	return bRes;
}

bool AMainCharacter::IsReloadingWeapon()
{
	return IsBitSet(boolVals, 14);
}


void AMainCharacter::StartAiming()
{
	//only to enter aiming state if player has equiped a weapon
	if (HasEquipedPrimary() || HasEquipedSecondary()) SetAiming(true);
}

void AMainCharacter::DoneAiming()
{
	SetAiming(false);
}

void AMainCharacter::SetReloading(bool val)
{
	UE_LOG(LogTemp, Warning, TEXT("Called set reloading"));
	if (val)
		SetBitAtPos(boolVals, 14);
	else
	{
		ClearBitAtPos(boolVals, 14);
		if (HasEquipedPrimary())
		{
			PistolWeapon->ReloadWeapon();
			SetAmmoOnUI(0);
			SetMagOnUI(0);
		}
		else if (HasEquipedSecondary())
		{
			SecondaryWeapon->ReloadWeapon();
			SetAmmoOnUI(1);
			SetMagOnUI(1);
		}
	}
}

void AMainCharacter::SetAiming(bool val)
{
	if (val) SetBitAtPos(boolVals, 15);
	else ClearBitAtPos(boolVals, 15);
}

void AMainCharacter::AttachWeaponToBack()
{
	if (PlayerBody)
	{
		if (IsBitSet(boolVals, 11) && PistolWeapon)
		{
			PistolWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, PistolHolsterName);
		}
		else if (IsBitSet(boolVals, 12) && SecondaryWeapon)
		{
			SecondaryWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, SecondaryHolsterName);
		}
	}
}

void AMainCharacter::AttachWeaponToHand()
{
	if (PlayerBody)
	{
		if (IsBitSet(boolVals, 5) && PistolWeapon)
		{
			PistolWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, PrimarySocket);
		}
		else if (IsBitSet(boolVals, 6) && SecondaryWeapon)
		{
			SecondaryWeapon->AttachToComponent(PlayerBody, FAttachmentTransformRules::KeepRelativeTransform, SecondarySocket);
		}
	}
}

void AMainCharacter::DecideEquipOrUnequipPrimary()
{
	//don't want the player to equip or unequip weapon if already one animation is playing
	if (!IsBitSet(boolVals, 4) && !IsBitSet(boolVals, 10))
	{
		if (IsBitSet(boolVals, 7) && !IsBitSet(boolVals, 13)) UnEquipWeapon(0);
		else if (!IsBitSet(boolVals, 7) && IsBitSet(boolVals, 13)) EquipWeapon(0);
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't decide what to do in DecideEquipOrUnequipPrimary"));
		}
	}
}

void AMainCharacter::DecideEquipOrUnequipSecondary()
{
	//don't want the player to equip or unequip weapon if already one animation is playing
	if (!IsBitSet(boolVals, 4) && !IsBitSet(boolVals, 10))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Deciding equip or unequip for secondary weapon"));
		if (IsBitSet(boolVals, 7) && !IsBitSet(boolVals, 13)) UnEquipWeapon(1);
		else if (!IsBitSet(boolVals, 7) && IsBitSet(boolVals, 13)) EquipWeapon(1);
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Couldn't decide what to do in DecideEquipOrUnequipSecondary"));
		}
	}
}

void AMainCharacter::EquipWeapon(int weaponIndex)
{	

	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon called for weaponIndex %d"),weaponIndex);
	switch (weaponIndex)
	{
	case 0:	
		SetBitAtPos(boolVals, 4);
		SetBitAtPos(boolVals, 5);
		break;
	case 1:		
		SetBitAtPos(boolVals, 4);
		SetBitAtPos(boolVals, 6);
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("(UnEquipWeapon)No case for weapon index %d"),weaponIndex);
	}
}

void AMainCharacter::ReloadWeapon()
{
	//don't want to reload weapon if already reloading it.
	if (!IsReloadingWeapon())
	{
		SetReloading(true);	
	}
}


void AMainCharacter::UnEquipWeapon(int weaponIndex)
{		

	UE_LOG(LogTemp, Warning, TEXT("UnEquipWeapon called for weaponIndex %d"), weaponIndex);
	switch (weaponIndex)
	{
	case 0:
		ClearBitAtPos(boolVals, 7);
		ClearBitAtPos(boolVals, 8);
		SetBitAtPos(boolVals, 10);
		SetBitAtPos(boolVals, 11);
		break;
	case 1:		
		ClearBitAtPos(boolVals, 7);
		ClearBitAtPos(boolVals, 9);
		SetBitAtPos(boolVals, 10);
		SetBitAtPos(boolVals, 12);
		break;

	default:		
		UE_LOG(LogTemp, Error, TEXT("(UnEquipWeapon)No case for weapon index %d"), weaponIndex);
	}
}

void AMainCharacter::EquipedWeapon()
{
	if (IsBitSet(boolVals, 4))
	{
		UE_LOG(LogTemp, Warning, TEXT("In EquipedWeapon."));
		//irrespective of weapon type, weapon equiping bit has to be cleared
		ClearBitAtPos(boolVals, 4);
		if (IsBitSet(boolVals, 5))
		{
			//since player has done the process of equiping the weapon and has equiped the primary 
			//weapon			
			ClearBitAtPos(boolVals, 5);

			//setting bits that ensure that weapon equiped status is set to true
			SetBitAtPos(boolVals, 8);

			SetAmmoOnUI(0);
			SetMagOnUI(0);			
		}
		else if (IsBitSet(boolVals, 6))
		{
			//since player has done the process of equiping the weapon and has equiped the primary 
			//weapon
			
			ClearBitAtPos(boolVals, 6);

			//setting bits that ensure that weapon equiped status is set to true			
			SetBitAtPos(boolVals, 9);
			SetAmmoOnUI(1);
			SetMagOnUI(1);
		}

		//irresepctive of the weapon type, equiped weapon bit has to be set to true 
		SetBitAtPos(boolVals, 7);
		ClearBitAtPos(boolVals, 13);

		//want to show everything ui when the weapon is equiped.
		ShowEverythingOnUI();
		
	}
}

void AMainCharacter::UnEquipedWeapon()
{
	if (IsBitSet(boolVals, 10))
	{
		//irrespective of the type of weapon, unequiping weapon has to bet set to false
		ClearBitAtPos(boolVals, 10);

		if (IsBitSet(boolVals, 11))
		{
			//since player has done the process of unequiping the weapon and has unequiped the primary 
			//weapon			
			ClearBitAtPos(boolVals, 11);
			
		}
		else if (IsBitSet(boolVals, 12))
		{
			//since player has done the process of unequiping the weapon and has unequiped the primary 
			//weapon			
			ClearBitAtPos(boolVals, 12);
		}

		//setting bits that ensure that weapon unequiped bit is set to true
		SetBitAtPos(boolVals, 13);
		ClearBitAtPos(boolVals, 7);
		ShowOnlyHealthBarAndImage();
	}
}

void AMainCharacter::SetAmmoOnUI(int weaponIndex)
{
	if (PlayerScreenPtr == nullptr) return;
	switch (weaponIndex)
	{
	case 0:
		//case for primary weapon
		if (PistolWeapon != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PistolAmmo: %d"),PistolWeapon->Current_Ammo);
			PlayerScreenPtr->SetAmmoText(PistolWeapon->Current_Ammo);
		}
		break;
	case 1:
		//case for secondary weapon
		if (SecondaryWeapon != nullptr)
		{			
			PlayerScreenPtr->SetAmmoText(SecondaryWeapon->Current_Ammo);
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("(SetAmmoOnUI)No case for weapon index %d"),weaponIndex);
	}
}

void AMainCharacter::SetMagOnUI(int weaponIndex)
{
	if (PlayerScreenPtr == nullptr) return;
	switch (weaponIndex)
	{
	case 0:
		//case for primary weapon
		if (PistolWeapon != nullptr)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Pistol Mag: %d"), PistolWeapon->Current_Mag);
			PlayerScreenPtr->SetMagText(PistolWeapon->Current_Mag * PistolWeapon->MaxAmmo);
		}
		break;
	case 1:
		//case for secondary weapon
		if (SecondaryWeapon != nullptr)
		{			
			PlayerScreenPtr->SetMagText(SecondaryWeapon->Current_Mag * SecondaryWeapon->MaxAmmo);
		}
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("(SetMagOnUI)No case for weapon index %d"),weaponIndex);
	}
}

void AMainCharacter::ShowEverythingOnUI()
{
	if (PlayerScreenPtr == nullptr) return;

	PlayerScreenPtr->ShowEverything();
}

void AMainCharacter::ShowWeaponRelated()
{
	if (PlayerScreenPtr == nullptr) return;

	//showing everything on screen
	PlayerScreenPtr->ShowEverything();
	//then hiding unimportant widgets
	PlayerScreenPtr->ShowWeaponRelated();
}

void AMainCharacter::ShowOnlyHealthBarAndImage()
{
	if (PlayerScreenPtr == nullptr) return;

	PlayerScreenPtr->HideEverything();
	PlayerScreenPtr->ShowHealthRelated();
}

float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);		
	Health -= DamageAmount;
	if (PlayerScreenPtr)
	{
		PlayerScreenPtr->SetHealth(Health, false);
	}
	
	//UE_LOG(LogTemp, Warning,TEXT("Player was hit."));
	return DamageAmount;
}