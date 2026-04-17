#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (numberOfPlayers == 2)
	{
		UWorld* world = GetWorld();

		if (IsValid(world))
		{
			bUseSeamlessTravel = true;
			world->ServerTravel(FString(TEXT("/Game/Maps/BlasterMap?listen")));
		}
	}
}
