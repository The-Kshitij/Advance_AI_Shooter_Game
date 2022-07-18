#include "Enemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

#include "WeaponActor.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Aicontroller = Cast<AAIController>(GetController());
	if (Aicontroller == nullptr) UE_LOG(LogTemp, Error, TEXT("ai controller not found."));
	//because when setting returning the position for patrol the logic is to 
	//increment first and then compare, so the first point would be missed in the 
	//first call, therfore setting to 0 helps.
	PatrolInd = -1;


	Weapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponClass);
	if (Weapon == nullptr) UE_LOG(LogTemp, Error, TEXT("Weapon not found"))
	else
	{
		Weapon->SetOwner(this);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponHolsterName);
	}
	//the points set in the blueprint would be local to player, so have to convert
	//them to world space
	for (int i = 0; i < PatrolPoints.Num(); ++i)
	{		
		PatrolPoints[i] = this->GetTransform().TransformPosition(PatrolPoints[i]);
	}
	Timer = 0.0f;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bDead)
	{
		//UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);
		if (bFiring)
		{
			if ((UGameplayStatics::GetTimeSeconds(GetWorld()) - Timer > DelayOfFire) && Weapon)
			{
				//rotate the weapon to face the player
				FVector dir = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()
					- Weapon->GetActorLocation();
				dir.Normalize();
				FVector weaponforward = Weapon->GetActorForwardVector();
				weaponforward.Normalize();
				float ang = acos(FVector::DotProduct(weaponforward, dir));
				FVector rotvec = weaponforward.RotateAngleAxis(ang, weaponforward);
				FRotator rot = rotvec.Rotation();
				Weapon->SetActorRotation(rot);
				Weapon->FireWeapon();
				Timer = UGameplayStatics::GetTimeSeconds(GetWorld());
			}
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AEnemy::GetNextPoint()
{

	if (PatrolPoints.Num() == 0) return FVector::ZeroVector;

	if (bPatrolRandom)  PatrolInd = FMath::RandRange(0, PatrolPoints.Num() - 1);
	else PatrolInd = PatrolInd + 1 >= PatrolPoints.Num() ? 0 : PatrolInd + 1;

	//need to clear focus here, because this is called when the enemy is patrolling, so any
	//prev focus is not important here.
	
	if (Aicontroller) Aicontroller->SetFocalPoint(PatrolPoints[PatrolInd], (EAIFocusPriority::Type)2U);

	return PatrolPoints[PatrolInd];
}

void AEnemy::StartFire()
{
	if (!bDead)
	bFiring = true;
}

void AEnemy::DoneFire()
{
	if (!bDead)
	bFiring = false;
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= DamageAmount;
	if (Health <= 0)
	{
		if (GetMesh())
		{
			GetMesh()->SetSimulatePhysics(true);			
		}
		 DetachFromControllerPendingDestroy();
		bDead = true;
		bFiring = false;		
	}

	//UE_LOG(LogTemp, Warning,TEXT("Player was hit."));
	return DamageAmount;
}

bool AEnemy::bIsDead()
{
	return bDead;
}