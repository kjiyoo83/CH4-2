

#include "ChatUI.h"
#include "CX/CXPlayerController.h"
#include "CXPawn.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"

TSharedRef<SWidget> UChatUI::RebuildWidget()
{
 if (!WidgetTree->RootWidget)
 {
  UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>();
  WidgetTree->RootWidget = Root;
  StatusText = WidgetTree->ConstructWidget<UTextBlock>();
  auto* StatusSlot = Root->AddChildToCanvas(StatusText);
  StatusSlot->SetPosition(FVector2D(24,24)); StatusSlot->SetAutoSize(true);
  UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>();
  auto* ChatSlot = Root->AddChildToCanvas(Box);
  ChatSlot->SetAnchors(FAnchors(0,1)); ChatSlot->SetAlignment(FVector2D(0,1));
  ChatSlot->SetPosition(FVector2D(24,-24)); ChatSlot->SetSize(FVector2D(540,240));
  ChatScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("ChatScrollBox"));
  Box->AddChildToVerticalBox(ChatScrollBox)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
  MultilineEditableTextBox_ChatLog = WidgetTree->ConstructWidget<UMultiLineEditableTextBox>(UMultiLineEditableTextBox::StaticClass(), TEXT("MultilineEditableTextBox_ChatLog"));
  MultilineEditableTextBox_ChatLog->SetIsReadOnly(true);
  ChatScrollBox->AddChild(MultilineEditableTextBox_ChatLog);
  EditableTextBox_ChatInput = WidgetTree->ConstructWidget<UEditableTextBox>(UEditableTextBox::StaticClass(), TEXT("EditableTextBox_ChatInput"));
  EditableTextBox_ChatInput->SetHintText(FText::FromString(TEXT("Enter: chat / send | Left click: attack")));
  Box->AddChildToVerticalBox(EditableTextBox_ChatInput);
 }
 return Super::RebuildWidget();
}
void UChatUI::FocusChat()
{
 if (!EditableTextBox_ChatInput) return;
 if (APlayerController* PC = GetOwningPlayer())
 {
  FInputModeUIOnly Mode; Mode.SetWidgetToFocus(EditableTextBox_ChatInput->TakeWidget()); PC->SetInputMode(Mode);
  if (auto* Car = Cast<ACXPawn>(PC->GetPawn())) Car->StopDrivingForChat();
 }
 EditableTextBox_ChatInput->SetKeyboardFocus();
}
void UChatUI::UpdateStatus(const FString& Text)
{
 if (StatusText) StatusText->SetText(FText::FromString(Text));
}
FReply UChatUI::NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent)
{
 if (KeyEvent.GetKey() == EKeys::Escape)
 {
  if (EditableTextBox_ChatInput) EditableTextBox_ChatInput->SetText(FText::GetEmpty());
  if (APlayerController* PC = GetOwningPlayer()) PC->SetInputMode(FInputModeGameOnly());
  return FReply::Handled();
 }
 return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}

void UChatUI::NativeConstruct()
{
	Super::NativeConstruct();

	if(EditableTextBox_ChatInput && !EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}
}

void UChatUI::NativeDestruct()
{
	if (EditableTextBox_ChatInput && EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &UChatUI::OnChatInputTextCommitted))
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &UChatUI::OnChatInputTextCommitted);
	}

	Super::NativeDestruct();
}

void UChatUI::OnNewChatMessage(const FString& Sender, const FString& Message)
{
	Messages.Add(Sender + TEXT(": ") + Message);
	if (Messages.Num() > 100) Messages.RemoveAt(0);
	if (MultilineEditableTextBox_ChatLog) MultilineEditableTextBox_ChatLog->SetText(FText::FromString(FString::Join(Messages, TEXT("\n"))));
	if (ChatScrollBox) ChatScrollBox->ScrollToEnd();
}


void UChatUI::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (APlayerController* PC = GetOwningPlayer()) PC->SetInputMode(FInputModeGameOnly());
		if (Text.IsEmpty() == false)
		{
			FString ChatMessage = Text.ToString();
			EditableTextBox_ChatInput->SetText(FText::GetEmpty());
			if (ACXPlayerController* PC = GetOwningPlayer<ACXPlayerController>())
			{
				PC->CTSChatMessage(ChatMessage);
			}
		}
	}
}
