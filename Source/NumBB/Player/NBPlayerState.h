#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"


UCLASS()
class NUMBB_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANBPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();

	UPROPERTY(Replicated)
	FString PlayerNameString;
	UPROPERTY(Replicated)
	int32 MaxGuessCount;
	UPROPERTY(Replicated)
	int32 CurrentGuessCount;
};
