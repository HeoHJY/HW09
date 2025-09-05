#include "UI/NBChatInputWidget.h"
#include "Components/EditableTextBox.h"
#include "Player/NBPlayerController.h"

void UNBChatInputWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInputWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UNBChatInputWidget::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPC = GetOwningPlayer();
		if (IsValid(OwningPC) == true)
		{
			ANBPlayerController* OwningNBPC = Cast<ANBPlayerController>(OwningPC);
			if (IsValid(OwningNBPC) == true)
			{
				OwningNBPC->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			}
		}
	}
}


