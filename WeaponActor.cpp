#include "WeaponActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/DecalComponent.h"

AWeaponActor::AWeaponActor()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponBody = CreateDefaultSubobject<USkeletalMeshComponent>("Body");
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
		
	//so that the weapon doesn't harm the owner of this weapon
	FireCollisionParam.AddIgnoredActor(GetOwner());
		
	
	//since there is a single level and no saving in the game, so starting all weapons 
	//with max ammo and max mag
	Current_Ammo = MaxAmmo;
	Current_Mag = MaxMag;
	
}

void AWeaponActor::FireWeapon()
{
	
	//if weapon is owned by enemy
	if (GetOwner() == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) bOwnedByPlayer = true;
	else bOwnedByPlayer = false;
	
	if (WeaponBody && FiringAnimation)
	{
		WeaponBody->PlayAnimation(FiringAnimation, false);

		//tracing a line and drawing it for debugging
		FVector StartLocation = WeaponBody->GetSocketLocation(FiringSocketName);
		FVector EndLocation = StartLocation + WeaponBody->GetSocketRotation(FiringSocketName).Vector() * RangeOfWeapon;
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 2.0f);
		TArray<FHitResult> hitResults;
		//in my project there is a trace channel created using project 
		//settings, which has a default response of overlap, using that channel	
		GetWorld()->LineTraceMultiByChannel(hitResults, StartLocation, EndLocation,
			ECC_Pawn, FireCollisionParam);
		bool bHitChar = false;

		//going through the overlapped objects to find if there is any character hit
		for (FHitResult h : hitResults)
		{			
			FString name = "";
			UClass* cls;
			cls = h.GetActor()->GetClass();			
			if (cls)
			{
				name = cls->GetName();
				cls = cls->GetSuperClass();
				if (cls)
				{
					cls = cls->GetSuperClass();
					name = cls->GetName();
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("Class name: %s"), *(cls->GetName()));

			//since all moveable objects will be characters
			if (name.Compare("Character") == 0)
			{
				bHitChar = true;
				UGameplayStatics::ApplyDamage
				(h.GetActor(), DamageOfWeapon, GetInstigatorController(), this, DamageClass);
			}			
		}

				
		//loop through all the objects overlapped with during line trace, spawn a bullet hole on 
		// the first object overlapped if no character was overlapped with during line trace or
		//if a character was hit then, spawn blood on the character hit and blood on the next object
		//overlapped after the character.
		if (hitResults.Num() != 0)
		{
			if (bHitChar)
			{
				bool bFlag = true;
				FHitResult h = hitResults[0];
				if (h.GetActor() != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
				{
					if (bFlag)
					{
						//for finding the rotation
						int ind = FMath::RandRange(0, BloodMaterials.Num() - 1);
						FVector hitPoint = h.ImpactPoint;
						UE_LOG(LogTemp, Warning, TEXT("Impact point: %s"), *(hitPoint.ToString()));
						FVector dir = GetOwner()->GetActorLocation() - hitPoint;
						//FRotator::FRotator(0, 0, 90)
						UDecalComponent* dec = UGameplayStatics::SpawnDecalAtLocation(
							GetWorld(), BloodMaterials[ind], 15 * FVector::FVector(1.0f, 1.0f, 1.0f), h.ImpactPoint,
							dir.Rotation()
						);
						if (dec)
						{
							UE_LOG(LogTemp, Warning, TEXT("Blood Decal spawned."));
							//if this not a char rather an object behind character then this casting will
							//fail.
							ACharacter* someChar = Cast<ACharacter>(h.GetActor());
							if (someChar)
							{
								dec->AttachTo(someChar->GetMesh(), NAME_None, EAttachLocation::KeepWorldPosition);
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("Casting to acharacter failed."));
							}
						}
					}
				}
			}
			else
			{
				//for finding the rotation
				FHitResult h = hitResults[0];
				int ind = FMath::RandRange(0, BulletHoleMaterials.Num() - 1);
				FVector hitPoint = h.ImpactPoint;
				FVector dir = GetOwner()->GetActorLocation() - hitPoint;				
				//FRotator::FRotator(0, 0, 90)
				UDecalComponent* dec = UGameplayStatics::SpawnDecalAtLocation(
					GetWorld(), BulletHoleMaterials[ind], 10 * FVector::FVector(1.0f, 1.0f, 1.0f), h.ImpactPoint,
					dir.Rotation()
				);
				if (dec)
				{
					UE_LOG(LogTemp, Warning, TEXT("Bullet hole spawned."));
				}
			}
		}
		if (bOwnedByPlayer)
		{			
			--Current_Ammo;
		}

		//CollisionTest();
		//FHitResult hitResult;			
		/*
		if (GetWorld()->LineTraceSingleByChannel(hitRes, StartLocation, EndLocation, ECollisionChannel::ECC_Pawn, FireCollisionParam))
		{
			UGameplayStatics::ApplyDamage(hitRes.GetActor(), DamageOfWeapon, GetInstigatorController(), this, DamageClass);			
		}
		*/			
	}
}

void AWeaponActor::ReloadWeapon()
{
	//this function is called when reload animation is done playing.
	//so once finished reloading then, ammo is set back to max and mag is 
	//reduced by one.
	Current_Ammo = MaxAmmo;
	--Current_Mag;
}

bool AWeaponActor::CanFire(bool &bShouldReload)
{
	bShouldReload = false;
	if (Current_Ammo == 0 && Current_Mag == 0)
	{
		return false;
	}
	else if (Current_Ammo == 0 && Current_Mag != 0)
	{
		bShouldReload = true;
		return false;
	}
	else if (Current_Ammo != 0 && Current_Mag != 0)
	{
		return true;
	}
	//called when ammo is non zero and mag is zero, when using the last magazine
	else return true;
}