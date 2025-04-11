// Ryckbosch Arthur 2025, Inc. All Rights Reserved.


#include "MarkTeamComponent.h"

#include "MarkAndPingMultiplayerBPLibrary.h"
#include "MarkComponent.h"
#include "PingComponent.h"

DEFINE_LOG_CATEGORY(LogMarkTeamComponent);

UMarkTeamComponent::UMarkTeamComponent()
{
	SetIsReplicatedByDefault(true);
}

const int32 UMarkTeamComponent::GetMyTeam(APlayerState* PlayerState, int32& Id) const
{
	Id = INDEX_NONE;

	int i = 0;
	if(PlayerState)
	{
		int j = 0;

		while(i < Members.Num() && j < Members[i].Num() && Members[i][j] && Members[i][j]->GetPlayerId() != PlayerState->GetPlayerId())
		{
			j = 0;
		
			while(j < Members[i].Num() && Members[i][j] && Members[i][j]->GetPlayerId() != PlayerState->GetPlayerId())
			{
				j++;
			}

			if(j >= Members[i].Num())
			{
				i++;
			}
		}

		if(i < Members.Num())
		{
			Id = j;
		}
	}
	
	return i;
}

const UTexture2D* UMarkTeamComponent::GetMemberIcon(int32 PlayerId) const
{
	static UTexture2D* Icon = nullptr;
	
	int i = 0;
	int j = 0;


	while(i < Members.Num() && j < Members[i].Num() && Members[i][j] && Members[i][j]->GetPlayerId() != PlayerId)
	{
		j = 0;
	
		while(j < Members[i].Num() && Members[i][j] && Members[i][j]->GetPlayerId() != PlayerId)
		{
			j++;
		}

		if(j >= Members[i].Num())
		{
			i++;
		}
	}

	if(i < Members.Num())
	{
		if(Teams.IsValidIndex(i) && Teams[i].Icons.IsValidIndex(j))
		{
			Icon = Teams[i].Icons[j];
		}
	}
	
	return Icon;
}

const TArray<FMarkTeamInfo>& UMarkTeamComponent::GetTeams() const
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		return Teams;
	}
	
	UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to get teams"));
	static const TArray<FMarkTeamInfo> EmptyTeams;
	return EmptyTeams;
}

const TArray<TArray<APlayerState*>>& UMarkTeamComponent::GetMembers() const
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		return Members;
	}

	
	UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to get members"));
	static const TArray<TArray<APlayerState*>> EmptyMembers;
	return EmptyMembers;
}

const FMarkTeamInfo& UMarkTeamComponent::GetTeamMember(APlayerState* PlayerState, int& TeamId) const
{
	static FMarkTeamInfo Team;
	if(GetOwnerRole() == ROLE_Authority)
	{
		int32 Id;
		TeamId = GetMyTeam(PlayerState, Id);

		if(TeamId != INDEX_NONE)
		{
			Team = Teams[TeamId];
		}
		else
		{
			UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team not found"));
		}
	}
	else
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to get team member"));
	}

	return Team;
		
}

void UMarkTeamComponent::UpdateMembersIcon()
{
	for(int i = 0; i < Members.Num(); i++)
	{
		for(int j = 0; j < Members[i].Num(); j++)
		{
			APlayerState* Member = Members[i][j];
			if(Member)
			{
				APawn* Pawn = Member->GetPawn();
				if(Pawn)
				{
					UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
					if(MarkComponent)
					{
						MarkComponent->UpdateTeam(Teams, i, j);
					}
					else
					{
						UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
				}
			}
		}
	}
}

void UMarkTeamComponent::ServerRemovePingOnTeam_Implementation(const int PlayerId, const FString& FromTeamName,
	const FString& TeamName)
{
	int FromTeamId = FindTeamIndex(FromTeamName);

	if(FromTeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From team %s not found"), *FromTeamName);
		return;
	}

	int TeamId = FindTeamIndex(TeamName);

	if(TeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team %s not found"), *TeamName);
		return;
	}

	if(!Members.IsValidIndex(FromTeamId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From Team Index %d is invalid"), FromTeamId);
		return;
	}

	if(!Members[FromTeamId].IsValidIndex(PlayerId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Player Index %d is invalid"), PlayerId);
		return;
	}

	int32 PlayerStateId = Members[FromTeamId][PlayerId]->GetPlayerId();
	
	
	UMarkComponent* FromMarkComponent = Members[FromTeamId][PlayerId]->GetPawn()->FindComponentByClass<UMarkComponent>(); // MAY CRASH HERE @TODO
	
	// Remove ping on all members of the team
	for(APlayerState* Member : Members[TeamId])
	{
		if(Member)
		{
			APawn* Pawn = Member->GetPawn();
			if(Pawn)
			{
				UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
				if(MarkComponent)
				{
					if(PlayerStateId == Member->GetPlayerId())
					{
						MarkComponent->PlayerControllerRemovePing();
					}
					else
					{
						MarkComponent->ClientRemovePing(PlayerStateId, PlayerId, Members[FromTeamId][PlayerId]->GetPawn(), FromMarkComponent);
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
				}
			}
			else
			{
				UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
			}
		}
	}
}

void UMarkTeamComponent::ServerPingOnAllTeams_Implementation(const FVector& Location, const float Lifetime,
                                                             const int PlayerId, const FString& FromTeamName)
{
	int FromTeamId = FindTeamIndex(FromTeamName);

	if(FromTeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From team %s not found"), *FromTeamName);
		return;
	}
	
	if(!Members.IsValidIndex(FromTeamId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From Team Index %d is invalid"), FromTeamId);
		return;
	}

	if(!Members[FromTeamId].IsValidIndex(PlayerId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Player Index %d is invalid"), PlayerId);
		return;
	}

	int32 PlayerStateId = Members[FromTeamId][PlayerId]->GetPlayerId();
	
	UMarkComponent* FromMarkComponent = Members[FromTeamId][PlayerId]->GetPawn()->FindComponentByClass<UMarkComponent>(); // MAY CRASH HERE @TODO
	
	// Ping on all members of the team
	for(int i = 0; i < Members.Num(); i++)
	{
		for(APlayerState* Member : Members[i])
		{
			if(Member)
			{
				APawn* Pawn = Member->GetPawn();
				if(Pawn)
				{
					UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
					if(MarkComponent)
					{
						if(PlayerStateId == Member->GetPlayerId())
						{
							MarkComponent->PlayerControllerShowPing(PlayerId, Location, Teams[FromTeamId].Color, Lifetime, FDateTime::Now().ToUnixTimestamp());
						}
						else
						{
							MarkComponent->ClientShowPing(PlayerStateId, PlayerId, Members[FromTeamId][PlayerId]->GetPawn(), FromMarkComponent, Location, Teams[FromTeamId].Color, Lifetime, FDateTime::Now().ToUnixTimestamp());
						}
					}
					else
					{
						UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
				}
			}
		}
	}
}

void UMarkTeamComponent::ServerPingOnTeam_Implementation(const FVector& Location, const float Lifetime, const int PlayerId,
                                                         const FString& FromTeamName, const FString& TeamName)
{
	int FromTeamId = FindTeamIndex(FromTeamName);

	if(FromTeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From team %s not found"), *FromTeamName);
		return;
	}

	int TeamId = FindTeamIndex(TeamName);

	if(TeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team %s not found"), *TeamName);
		return;
	}

	if(!Members.IsValidIndex(FromTeamId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From Team Index %d is invalid"), FromTeamId);
		return;
	}

	if(!Members[FromTeamId].IsValidIndex(PlayerId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Player Index %d is invalid"), PlayerId);
		return;
	}

	int32 PlayerStateId = Members[FromTeamId][PlayerId]->GetPlayerId();
	
	
	UMarkComponent* FromMarkComponent = Members[FromTeamId][PlayerId]->GetPawn()->FindComponentByClass<UMarkComponent>(); // MAY CRASH HERE @TODO
	
	// Ping on all members of the team
	for(APlayerState* Member : Members[TeamId])
	{
		if(Member)
		{
			APawn* Pawn = Member->GetPawn();
			if(Pawn)
			{
				UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
				if(MarkComponent)
				{
					if(PlayerStateId == Member->GetPlayerId())
					{
						MarkComponent->PlayerControllerShowPing(PlayerId, Location, Teams[FromTeamId].Color, Lifetime, FDateTime::Now().ToUnixTimestamp());
					}
					else
					{
						MarkComponent->ClientShowPing(PlayerStateId, PlayerId, Members[FromTeamId][PlayerId]->GetPawn(), FromMarkComponent, Location, Teams[FromTeamId].Color, Lifetime, FDateTime::Now().ToUnixTimestamp());
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
				}
			}
			else
			{
				UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
			}
		}
	}
}

void UMarkTeamComponent::ServerMarkOnTeam_Implementation(AActor* Owner, const FLinearColor Color, const FString& TeamName)
{
	if(!Owner)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Owner not found"));
		return;
	}

	UMarkedComponent* MarkedComponent = Owner->FindComponentByClass<UMarkedComponent>();

	if(!MarkedComponent)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkedComponent not found"));
		return;
	}

	int TeamId = FindTeamIndex(TeamName);

	if(TeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team %s not found"), *TeamName);
		return;
	}
	
	for(APlayerState* Member : Members[TeamId])
	{
		if(Member)
		{
			AActor* MemberActor = Member->GetPawn();
			if(MemberActor)
			{
				UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
				if(MarkComponent)
				{
					MarkComponent->ClientShowMark(-1, nullptr, Owner->GetActorLocation(), Owner, MarkedComponent, FDateTime::Now().ToUnixTimestamp(), Color, false, true);
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
				}
			}
			else
			{
				UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
			}
		}
	}
}

void UMarkTeamComponent::AddTeam(const FString& Name, const FLinearColor& Color, const TArray<UTexture2D*>& Icons, const float PingLifetime)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		FMarkTeamInfo Team;
		Team.Name = Name;
		Team.Color = Color;
		Team.Icons = Icons;
		Team.Lifetime = PingLifetime;
		Teams.Add(Team);
		Members.Add(TArray<APlayerState*>());

		UE_LOG(LogMarkTeamComponent, Display, TEXT("Team %s added"), *Name);
	}
	else
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to add team"));
	}
}

void UMarkTeamComponent::AddMember(const FString& TeamName, APlayerController* Member)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(Member)
		{
			APlayerState* PlayerState = Member->GetPlayerState<APlayerState>();

			if(PlayerState)
			{
				const int TeamIndex = FindTeamIndex(TeamName);
				if(TeamIndex != INDEX_NONE)
				{
					Members[TeamIndex].Add(PlayerState);

					UE_LOG(LogMarkTeamComponent, Display, TEXT("Member %s added to team %s"), *PlayerState->GetPlayerName(), *TeamName);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to add member"));
	}
}

void UMarkTeamComponent::RemoveMember(AController* Member)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		if(Member)
		{
			APlayerState* PlayerState = Member->GetPlayerState<APlayerState>();
            
            if(PlayerState)
            {
            	int i = 0;
            	int j = 0;
            	
            	while(i < Members.Num() && j < Members[i].Num() )
				{
					j = 0;
					
					while(j < Members[i].Num() && Members[i][j] != PlayerState)
					{
						j++;
					}
	
					if(j < Members[i].Num())
					{
						Members[i].RemoveAt(j);

						UE_LOG(LogMarkTeamComponent, Display, TEXT("Member %s removed from team %s"), *PlayerState->GetPlayerName(), *Teams[i].Name);
					}
            		else
            		{
            			i++;
            		}
				}
            }
		}
	}
	else
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to add member"));
	}
}

void UMarkTeamComponent::RemoveTeam(const FString& TeamName)
{
	if(GetOwnerRole() == ROLE_Authority)
	{
		const int TeamIndex = FindTeamIndex(TeamName);
		if(TeamIndex != INDEX_NONE)
		{
			Teams.RemoveAt(TeamIndex);

			UE_LOG(LogMarkTeamComponent, Display, TEXT("Team %s removed"), *TeamName);
		}
	}
	else
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Client trying to remove team"));
	}
}

void UMarkTeamComponent::ClearWidgetsOfAllPlayers_Implementation(EMarkType Type)
{
	for(const TArray<APlayerState*>& TeamMembers : Members)
	{
		for(APlayerState* Member : TeamMembers)
		{
			if(Member)
			{
				ClearWidgets(Member, Type);
			}
		}
	}
}

void UMarkTeamComponent::ServerMarkOnAllTeams_Implementation(AActor* Owner, FLinearColor Color)
{
	if(!Owner)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Owner not found"));
		return;
	}
	
	UMarkedComponent* MarkedComponent = Owner->FindComponentByClass<UMarkedComponent>();
	

	if(!MarkedComponent)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkedComponent not found"));
		return;
	}
	
	for(int i = 0; i < Members.Num(); i++)
	{
		for(APlayerState* Member : Members[i])
		{
			if(Member)
			{
				AActor* MemberActor = Member->GetPawn();
				if(MemberActor)
				{
					UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
					if(MarkComponent)
					{
						MarkComponent->ClientShowMark(INDEX_NONE, nullptr, Owner->GetActorLocation(), Owner, MarkedComponent, FDateTime::Now().ToUnixTimestamp(), Color, false, true);
					}
					else
					{
						UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
				}
			}
		}
	}
}

void UMarkTeamComponent::ServerRemovePingOnAllTeams_Implementation(const int PlayerId, const FString& FromTeamName)
{
	int FromTeamId = FindTeamIndex(FromTeamName);

	if(FromTeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From team %s not found"), *FromTeamName);
		return;
	}
	
	if(!Members.IsValidIndex(FromTeamId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("From Team Index %d is invalid"), FromTeamId);
		return;
	}

	if(!Members[FromTeamId].IsValidIndex(PlayerId))
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Player Index %d is invalid"), PlayerId);
		return;
	}

	int32 PlayerStateId = Members[FromTeamId][PlayerId]->GetPlayerId();
	
	UMarkComponent* FromMarkComponent = Members[FromTeamId][PlayerId]->GetPawn()->FindComponentByClass<UMarkComponent>(); // MAY CRASH HERE @TODO
	
	// Ping on all members of the team
	for(int i = 0; i < Members.Num(); i++)
	{
		for(APlayerState* Member : Members[i])
		{
			if(Member)
			{
				APawn* Pawn = Member->GetPawn();
				if(Pawn)
				{
					UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
					if(MarkComponent)
					{
						if(PlayerStateId == Member->GetPlayerId())
						{
							MarkComponent->PlayerControllerRemovePing();
						}
						else
						{
							MarkComponent->ClientRemovePing(PlayerStateId, PlayerId, Members[FromTeamId][PlayerId]->GetPawn(), FromMarkComponent);
						}
					}
					else
					{
						UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
				}
			}
		}
	}
}

void UMarkTeamComponent::ServerRemoveMarkOnTeam_Implementation(AActor* Owner, const FString& TeamName)
{
	if(!Owner)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Owner not found"));
		return;
	}

	UMarkedComponent* MarkedComponent = Owner->FindComponentByClass<UMarkedComponent>();

	if(!MarkedComponent)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkedComponent not found"));
		return;
	}

	int TeamId = FindTeamIndex(TeamName);

	if(TeamId == INDEX_NONE)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team %s not found"), *TeamName);
		return;
	}
	
	for(APlayerState* Member : Members[TeamId])
	{
		if(Member)
		{
			AActor* MemberActor = Member->GetPawn();
			if(MemberActor)
			{
				UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
				if(MarkComponent)
				{
					MarkComponent->ClientRemoveMark(MarkedComponent);
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
				}
			}
			else
			{
				UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
			}
		}
		else
		{
			UE_LOG(LogMarkTeamComponent, Warning, TEXT("Member not found"));
		}
	}
}

void UMarkTeamComponent::ServerRemoveMarkOnAllTeams_Implementation(AActor* Owner)
{
	if(!Owner)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("Owner not found"));
		return;
	}

	UMarkedComponent* MarkedComponent = Owner->FindComponentByClass<UMarkedComponent>();

	if(!MarkedComponent)
	{
		UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkedComponent not found"));
		return;
	}
	
	for(int i = 0; i < Members.Num(); i++)
	{
		for(APlayerState* Member : Members[i])
		{
			if(Member)
			{
				AActor* MemberActor = Member->GetPawn();
				if(MemberActor)
				{
					UMarkComponent* MarkComponent = MemberActor->FindComponentByClass<UMarkComponent>();
					if(MarkComponent)
					{
						MarkComponent->ClientRemoveMark(MarkedComponent);
					}
					else
					{
						UE_LOG(LogMarkTeamComponent, Warning, TEXT("MarkComponent not found"));
					}
				}
				else
				{
					UE_LOG(LogMarkTeamComponent, Warning, TEXT("Pawn not found"));
				}
			}
		}
	}
}

// Called when the game starts
void UMarkTeamComponent::BeginPlay()
{
	Super::BeginPlay();

	UMarkAndPingMultiplayerBPLibrary::UpdateTeamComponent(this);
}

void UMarkTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMarkTeamComponent, Teams);
}

void UMarkTeamComponent::ClearWidgets_Implementation(APlayerState* PlayerState, EMarkType Type)
{
	if (PlayerState)
	{
		APawn* Pawn = PlayerState->GetPawn();
		if (Pawn)
		{
			UMarkComponent* MarkComponent = Pawn->FindComponentByClass<UMarkComponent>();
			if (MarkComponent)
			{
				MarkComponent->ClearWidgets(Type);
			}
		}
	}
}

int UMarkTeamComponent::FindTeamIndex(const FString& TeamName) const
{
	int i = 0;

	while(i < Teams.Num() && Teams[i].Name != TeamName)
	{
		i++;
	}

	if(i < Teams.Num())
	{
		return i;
	}

	UE_LOG(LogMarkTeamComponent, Warning, TEXT("Team %s not found"), *TeamName);

	return INDEX_NONE;
}

