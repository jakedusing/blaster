// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> AllPlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllPlayerStarts);

		TArray<AActor*> AllPlayers;
		UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), AllPlayers);


		/**
		* Get all players distances from all players starts and spawns player at point furthest from players
		*/
		TArray<float> DistancesFromPlayerStarts;

		for (int i = 0; i < AllPlayerStarts.Num(); i++)
		{
			float MinDistance = (AllPlayerStarts[i]->GetActorLocation() - AllPlayers[0]->GetActorLocation()).Size();
			for (int j = 1; j < AllPlayers.Num(); j++)
			{
				float Distance = (AllPlayerStarts[i]->GetActorLocation() - AllPlayers[j]->GetActorLocation()).Size();
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
				}
			}
			DistancesFromPlayerStarts.Add(MinDistance);
		}

		float MaxDistance = DistancesFromPlayerStarts[0];
		int32 Selection = 0;
		for (int i = 1; i < DistancesFromPlayerStarts.Num(); i++)
		{
			if (MaxDistance < DistancesFromPlayerStarts[i])
			{
				MaxDistance = DistancesFromPlayerStarts[i];
				Selection = i;
			}
		}
				//int32 Selection = FMath::RandRange(0, AllPlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, AllPlayerStarts[Selection]);
	}
}