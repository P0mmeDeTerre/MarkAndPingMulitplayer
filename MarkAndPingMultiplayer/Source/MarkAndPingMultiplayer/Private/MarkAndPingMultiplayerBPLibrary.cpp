// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#include "MarkAndPingMultiplayerBPLibrary.h"
#include "MarkAndPingMultiplayer.h"

UMarkTeamComponent* UMarkAndPingMultiplayerBPLibrary::TeamComponent = nullptr;
AGameStateBase* UMarkAndPingMultiplayerBPLibrary::GameState = nullptr;
TArray<FMarkTeamInfo> UMarkAndPingMultiplayerBPLibrary::Teams;
int32 UMarkAndPingMultiplayerBPLibrary::MyTeamId = INDEX_NONE;

UMarkAndPingMultiplayerBPLibrary::UMarkAndPingMultiplayerBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}



void UMarkAndPingMultiplayerBPLibrary::UpdateTeamComponent(UMarkTeamComponent* Component)
{
	if(Component)
	{
		TeamComponent = Component;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Team Component not found"));
	}
}

const TArray<FMarkTeamInfo>& UMarkAndPingMultiplayerBPLibrary::GetTeams()
{
	return TeamComponent->GetTeams();
}

void UMarkAndPingMultiplayerBPLibrary::UpdateTeam(const TArray<FMarkTeamInfo>& NewTeams, int32 NewTeamId)
{
	Teams = NewTeams;
	MyTeamId = NewTeamId;
}


const TArray<TArray<APlayerState*>>& UMarkAndPingMultiplayerBPLibrary::GetMembers()
{
	static TArray<TArray<APlayerState*>> Members;

	if(TeamComponent)
	{
		Members = TeamComponent->GetMembers();
	}

	return Members;
}

const FMarkTeamInfo& UMarkAndPingMultiplayerBPLibrary::GetMyTeam()
{

	if(Teams.IsValidIndex(MyTeamId))
	{
		return Teams[MyTeamId];
	}

	UE_LOG(LogTemp, Warning, TEXT("My team %d not found"), MyTeamId);
	
	static FMarkTeamInfo EmptyTeam;
	return EmptyTeam;
}

const UTexture2D* UMarkAndPingMultiplayerBPLibrary::GetMemberIcon(int32 Id)
{
	static const UTexture2D* Icon = nullptr;
	
	if(Teams.IsValidIndex(MyTeamId) &&  Teams[MyTeamId].Icons.IsValidIndex(Id))
	{
		if(Teams[MyTeamId].Icons.IsValidIndex(Id))
		{
			Icon = Teams[MyTeamId].Icons[Id];
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Icon index"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Team index"));
	}
	
	return Icon;
}

const FMarkTeamInfo& UMarkAndPingMultiplayerBPLibrary::GetTeamMember(APlayerState* PlayerState, int& TeamId)
{
	static FMarkTeamInfo Team;

	if(TeamComponent)
	{
		Team = TeamComponent->GetTeamMember(PlayerState, TeamId);
	}
	
	return Team;
}

void UMarkAndPingMultiplayerBPLibrary::ClearWidgets(AActor* Target, EMarkType Type)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ClearWidgetsOfAllPlayers(Type);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerPingOnTeam(AActor* Target, const FVector& Location, const float Lifetime, const int PlayerId,
                                                        const FString& FromTeamName, const FString& TeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerPingOnTeam(Location, Lifetime, PlayerId, FromTeamName, TeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerRemovePingOnTeam(AActor* Target, const int PlayerId,
	const FString& FromTeamName, const FString& TeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerRemovePingOnTeam(PlayerId, FromTeamName, TeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerPingOnAllTeams(AActor* Target, const FVector& Location,
                                                            const float Lifetime, const int PlayerId, const FString& FromTeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerPingOnAllTeams(Location, Lifetime, PlayerId, FromTeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerRemovePingOnAllTeams(AActor* Target, const int PlayerId,
	const FString& FromTeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}

	if(TeamComponent)
	{
		TeamComponent->ServerRemovePingOnAllTeams(PlayerId, FromTeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerMarkOnTeam(AActor* Target, AActor* Actor, const FLinearColor Color, const FString& TeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerMarkOnTeam(Actor, Color, TeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerRemoveMarkOnTeam(AActor* Target, AActor* Actor, const FString& TeamName)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerRemoveMarkOnTeam(Actor, TeamName);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerMarkOnAllTeams(AActor* Target, AActor* Actor, const FLinearColor Color)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to ping on team"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerMarkOnAllTeams(Actor, Color);
	}
}

void UMarkAndPingMultiplayerBPLibrary::ServerRemoveMarkOnAllTeams(AActor* Target, AActor* Actor)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to access at the Team Component"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->ServerRemoveMarkOnAllTeams(Actor);
	}
}

void UMarkAndPingMultiplayerBPLibrary::UpdateMembersIcon(AActor* Target)
{
	if(!Target || !Target->HasAuthority())
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to access at the Team Component"));
		return;
	}
	
	if(TeamComponent)
	{
		TeamComponent->UpdateMembersIcon();
	}
}

