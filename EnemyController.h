#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class RESUMESHOWCASE_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void DoneSearchingForPlayer();
	UFUNCTION(BlueprintCallable)
		FVector GetLastPlayerLocation();
	UFUNCTION(BlueprintCallable)
		//enemy will be alerted when engaging the player or searching for the player
		bool GetIsAlerted();

private:	
	bool bSearching;
	bool bCanSeePlayer;
	FVector LastPlayerLocation;
	class AEnemy* EnemyOwner;
	

	class UAIPerceptionComponent* PerceptionComponent;
	class UBlackboardComponent* BlackboardComp;
	UPROPERTY(EditAnywhere)
		class UBehaviorTree* DefaultBehaviourTree;
	UPROPERTY(EditAnywhere)
		//the name of the property to handle seeing in the blackboard
		FName NameCanEngage = "bEngagingPlayer";
	UPROPERTY(EditAnywhere)
		//the name of the property that holds location in the blackboard
		FName NameMoveLocation;
	UPROPERTY(EditAnywhere)
		//the name of the property that holds the boolean for searching on the blackboard
		FName NameSearchPlayer = "bSearchPlayer";


	void OnPossess(class APawn* InPawn) override;
	void OnUnPossess() override;	
	
	/*
	  * sets value for local variable can see as well as variable on the
	  * blackboard used by the behaviour tree.
	*/
	void SetCanSeePlayer(bool val);
	UFUNCTION()
		void PerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
};
