#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;

UCLASS()
class NUMBB_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;

	void PrintChatMessageString(ANBPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	
	FString GenerateAnswerNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InAnswerNumberString, const FString& InGuessNumberString);

	void IncreaseGuessCount(ANBPlayerController* InPlayerController);

	void ResetGame();

	void JudgeGame(ANBPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString AnswerNumberString;

	TArray<TObjectPtr<ANBPlayerController>> AllPlayerControllers;
};
