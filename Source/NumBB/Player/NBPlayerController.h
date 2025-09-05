#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UNBChatInputWidget;
class UUserWidget;

UCLASS()
class NUMBB_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANBPlayerController();
	
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNBChatInputWidget> ChatInputWidgetClass;
	UPROPERTY()
	UNBChatInputWidget* ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;
	UPROPERTY()
	UUserWidget* NotificationTextWidgetInstance;

	FString ChatMessageString;

public:
	UPROPERTY(Replicated, BlueprintReadWrite)
	FText NotificationText;
};
