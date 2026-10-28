
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "ChatUI.generated.h"


UCLASS()
class CX_API UChatUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnNewChatMessage(const FString& Sender, const FString& Message);
	void FocusChat();
	void UpdateStatus(const FString& Text);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent) override;
	UPROPERTY(Transient) TObjectPtr<class UTextBlock> StatusText;
	TArray<FString> Messages;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> ChatScrollBox;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UMultiLineEditableTextBox> MultilineEditableTextBox_ChatLog;
};
