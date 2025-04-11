// Ryckbosch Arthur 2025, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MarkTeamComponent.h"
#include "kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "MarkAndPingMultiplayerBPLibrary.generated.h"


UCLASS()
class UMarkAndPingMultiplayerBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Team Component", Keywords = "Update Team Component"), Category = "Teams")
	static void UpdateTeam(const TArray<FMarkTeamInfo>& NewTeams, int32 NewTeamId);

	static void UpdateTeamComponent(UMarkTeamComponent* Component);
	static const TArray<TArray<APlayerState*>>& GetMembers();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Teams", Keywords = "Get Teams"), Category = "Teams")
	static const TArray<FMarkTeamInfo>& GetTeams();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get My Team", Keywords = "Get Teams"), Category = "Teams")
	static const FMarkTeamInfo& GetMyTeam();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Member Icon", Keywords = "Get Teams"), Category = "Teams")
	static const UTexture2D* GetMemberIcon(int32 Id);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Team Member", Keywords = "Get Teams"), Category = "Teams")
	static const FMarkTeamInfo& GetTeamMember(APlayerState* PlayerState, int& TeamId);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear Widgets", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ClearWidgets(AActor* Target, EMarkType Type);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Ping On Team", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerPingOnTeam(AActor* Target, const FVector& Location, const float Lifetime, const int PlayerId, const FString& FromTeamName, const FString& TeamName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Remove Ping On Team", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerRemovePingOnTeam(AActor* Target, const int PlayerId, const FString& FromTeamName, const FString& TeamName);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Ping On All Teams", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerPingOnAllTeams(AActor* Target, const FVector& Location, const float Lifetime, const int PlayerId, const FString& FromTeamName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Remove Ping On All Teams", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerRemovePingOnAllTeams(AActor* Target, const int PlayerId, const FString& FromTeamName);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Mark On Team", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerMarkOnTeam(AActor* Target, AActor* Actor, const FLinearColor Color, const FString& TeamName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Remove Mark On Team", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerRemoveMarkOnTeam(AActor* Target, AActor* Actor, const FString& TeamName);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Mark On All Teams", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerMarkOnAllTeams(AActor* Target, AActor* Actor, const FLinearColor Color);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Server Remove Mark On All Teams", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void ServerRemoveMarkOnAllTeams(AActor* Target, AActor* Actor);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update Members Icon", DefaultToSelf = "Target", Keywords = "Get Teams"), Category = "Teams")
	static void UpdateMembersIcon(AActor* Target);
	
private:
	static UMarkTeamComponent* TeamComponent;
	static AGameStateBase* GameState;
	static TArray<FMarkTeamInfo> Teams;
	static int32 MyTeamId;
};
