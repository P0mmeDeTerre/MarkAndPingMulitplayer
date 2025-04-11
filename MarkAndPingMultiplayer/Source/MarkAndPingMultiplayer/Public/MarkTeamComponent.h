// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MarkMisc.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "MarkTeamComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMarkTeamComponent, Log, All);


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARKANDPINGMULTIPLAYER_API UMarkTeamComponent : public UActorComponent
{
	GENERATED_BODY()

	UMarkTeamComponent();

public:	
	
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	const int32 GetMyTeam(APlayerState* PlayerState, int32& Id) const;
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	const UTexture2D* GetMemberIcon(int32 PlayerId) const;
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	const TArray<FMarkTeamInfo>& GetTeams() const;
	
	const TArray<TArray<APlayerState*>>& GetMembers() const;
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	const FMarkTeamInfo& GetTeamMember(APlayerState* PlayerState, int& TeamId) const;

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void UpdateMembersIcon();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Teams")
	void ClearWidgetsOfAllPlayers(EMarkType Type);
	
	UFUNCTION(Server, Reliable)
	void ServerPingOnTeam(const FVector& Location, const float Lifetime, const int PlayerId, const FString& FromTeamName, const FString& TeamName);

	UFUNCTION(Server, Reliable)
	void ServerRemovePingOnTeam(const int PlayerId, const FString& FromTeamName, const FString& TeamName);
	
	UFUNCTION(Server, Reliable)
	void ServerPingOnAllTeams(const FVector& Location, const float Lifetime, const int PlayerId, const FString& FromTeamName);

	UFUNCTION(Server, Reliable)
	void ServerRemovePingOnAllTeams(const int PlayerId, const FString& FromTeamName);
	
	UFUNCTION(Server, Reliable)
	void ServerMarkOnTeam(AActor* Owner, const FLinearColor Color, const FString& TeamName);

	UFUNCTION(Server, Reliable)
	void ServerRemoveMarkOnTeam(AActor* Owner, const FString& TeamName);

	UFUNCTION(Server, Reliable)
	void ServerMarkOnAllTeams(AActor* Owner, FLinearColor Color);

	UFUNCTION(Server, Reliable)
	void ServerRemoveMarkOnAllTeams(AActor* Owner);

protected:
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:


	UFUNCTION(BlueprintCallable, Category = "Teams")
	void AddTeam(const FString& Name, const FLinearColor& Color, const TArray<UTexture2D*>& Icons, const float PingLifetime);
	
	UFUNCTION(BlueprintCallable, Category = "Teams")
	void AddMember(const FString& TeamName, APlayerController* Member);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void RemoveMember(AController* Member);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void RemoveTeam(const FString& TeamName);

	UFUNCTION(NetMulticast, Reliable)
	void ClearWidgets(APlayerState* PlayerState, EMarkType Type);
	
	int FindTeamIndex(const FString& TeamName) const;

	
	TArray<FMarkTeamInfo> Teams = TArray<FMarkTeamInfo>();
	
	TArray<TArray<APlayerState*>> Members = TArray<TArray<APlayerState*>>();
};
