#include "EnemyController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Enemy.h"



void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);	
	
	if (DefaultBehaviourTree != nullptr) RunBehaviorTree(DefaultBehaviourTree);		

	//UE_LOG(LogTemp, Warning, TEXT("Possessed an enemy."));	
	BlackboardComp = GetBlackboardComponent();
	SetCanSeePlayer(false);

	PerceptionComponent = Cast<UAIPerceptionComponent>(FindComponentByClass(UAIPerceptionComponent::StaticClass()));
	if (PerceptionComponent == nullptr) UE_LOG(LogTemp, Error, TEXT("Perception component not found."))
	else
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::PerceptionUpdate);
	}
	
	EnemyOwner = Cast<AEnemy>(InPawn);	
	
}


void AEnemyController::OnUnPossess()
{
	Super::OnUnPossess();
	//UE_LOG(LogTemp, Warning, TEXT("UnPossessed an enemy."));
}


void AEnemyController::PerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	//UE_LOG(LogTemp, Warning, TEXT("Perception updated."));
	if (Stimulus.WasSuccessfullySensed())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Sensed %s"), *(Actor->GetName()));
		if (Actor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) SetCanSeePlayer(true);
	}
	else if (Stimulus.SensingFailed)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Can't sense %s anymore"), *(Actor->GetName()));
		if (Actor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) SetCanSeePlayer(false);
		//since this would be the last location the enemy could see the player
		LastPlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	}	
}

void AEnemyController::SetCanSeePlayer(bool val)
{		
	if (val)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Can see player"));
		if (EnemyOwner) EnemyOwner->StartFire();
	}
	else
	{
		if (EnemyOwner) EnemyOwner->DoneFire();
		//UE_LOG(LogTemp, Warning, TEXT("Can't see player."));
	}

	if (BlackboardComp == nullptr)
	{
		//UE_LOG(LogTemp, Error, TEXT("Blackboard component is null."));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Setting value on blackboard component"));
	//setting it to false would not work at times, so rather clearing the value
	//now if the player was not visible earlier then the enemy should go into engaging 
	//state. But if the player is not visible now then the enemy should move to the last
	//known location of the player.
	if (val) BlackboardComp->SetValueAsBool(NameCanEngage, val);
	else
	{
		BlackboardComp->ClearValue(NameCanEngage);
		if (bCanSeePlayer)
		{
			//it means the player was visible earlier, so maybe the enemy went into
			//cover and lost track of the player or the player ran away so need 
			//to search near last location of the player.
			BlackboardComp->SetValueAsBool(NameSearchPlayer, true);
		}
	}
	bCanSeePlayer = val;
	//no need to clear focus because the enemy will be focussed on the last known player's
	//location. And then goes into searching for the last location. After that enters 
	//patrolling again, where focus is cleared.
	if (val)
	{
		SetFocus(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}	
}

void AEnemyController::DoneSearchingForPlayer()
{
	bSearching = false;
	if (BlackboardComp) BlackboardComp->ClearValue(NameSearchPlayer);
}

FVector AEnemyController::GetLastPlayerLocation()
{
	//this is called when the enemy begins to move to the last known player location,
	//so the focus should be set on the last known player location and not on player itself
	//After searching is done, patrolling is started and focus is cleared.
	SetFocalPoint(LastPlayerLocation);
	bSearching = true;
	return LastPlayerLocation;
}


bool AEnemyController::GetIsAlerted()
{
	return bCanSeePlayer || bSearching;
}
