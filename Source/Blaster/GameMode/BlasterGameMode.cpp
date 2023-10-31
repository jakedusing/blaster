// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
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
