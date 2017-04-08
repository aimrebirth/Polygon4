/*
 * Polygon-4
 * Copyright (C) 2015 lzwdgc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Polygon4.h"

#include "PauseMenu.h"
#include "Widgets/MenuButton.h"
#include "Widgets/SavedGamesListView.h"

#include <P4GameMode.h>
#include <Game/P4Engine.h>
#include <Game/P4Modification.h>

#define LOCTEXT_NAMESPACE "PauseMenu"

void SPauseMenu::Construct(const FArguments& InArgs)
{
    if (auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController())
    {
        PlayerController->bShowMouseCursor = true;
    }

    // create message line
    MessageLine = SNew(STextBlock)
        .ShadowColorAndOpacity(FLinearColor::Black)
        .ColorAndOpacity(FLinearColor::Red)
        .ShadowOffset(FIntPoint(-1, 1))
        .Font(FSlateFontInfo(RobotoFont, 30));

    // Create GUI
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
            // Label
            + SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
            .AutoHeight()
			[
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo(RobotoFont, 100))
				.Text(FText::FromString("Polygon-4"))
			]
            // Other
            + SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
            [
                SNew(SHorizontalBox)
                // Left part of the screen (buttons)
                + SHorizontalBox::Slot()
			    .VAlign(VAlign_Center)
			    .HAlign(HAlign_Left)
                .AutoWidth()
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    [
                        SAssignNew(MenuVB, SVerticalBox)
                        + PauseMenuButton(LOCTEXT("ContinueGameButtonLabel" , "Continue Game"), &SPauseMenu::OnContinue)
                        + PauseMenuButton(LOCTEXT("SaveGameButtonLabel" , "Save Game"), &SPauseMenu::OnSaveGame)
                        + PauseMenuButton(LOCTEXT("LoadGameButtonLabel", "Load Game"), &SPauseMenu::OnLoadGame)
                        //+ PauseMenuButton(LOCTEXT("OptionsButtonLabel", "Options"), &SPauseMenu::OnNotImplemented)
                        + PauseMenuButton(LOCTEXT("MainMenuButtonLabel" , "Exit to Menu"), &SPauseMenu::OnExitToMenu)
                        + PauseMenuButton(LOCTEXT("ExitGameButtonLabel", "Exit Game"), &SPauseMenu::OnExit)
                    ]
                    + SVerticalBox::Slot()
                    [
                        SAssignNew(LoadVB, SVerticalBox)
                        .Visibility(EVisibility::Collapsed)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "LoadButtonLabel", "Load"), &SPauseMenu::OnLoadLoad)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "BackButtonLabel", "Back"), &SPauseMenu::OnLoadBack)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "DeleteButtonLabel", "Delete"), &SPauseMenu::OnLoadDelete)
                        .Padding(Padding, 200, Padding, Padding)
                    ]
                    + SVerticalBox::Slot()
                    [
                        SAssignNew(SaveVB, SVerticalBox)
                        .Visibility(EVisibility::Collapsed)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "SaveButtonLabel", "Save"), &SPauseMenu::OnSaveSave)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "BackButtonLabel", "Back"), &SPauseMenu::OnLoadBack)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "DeleteButtonLabel", "Delete"), &SPauseMenu::OnSaveDelete)
                        .Padding(Padding, 200, Padding, Padding)
                    ]
                ]
                // right part of the screen
                + SHorizontalBox::Slot()
			    .VAlign(VAlign_Fill)
			    .HAlign(HAlign_Fill)
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .VAlign(VAlign_Fill)
                    .HAlign(HAlign_Fill)
                    [
                        SAssignNew(SavedGamesVB, SVerticalBox)
                        .Visibility(EVisibility::Collapsed)
                        // label
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Top)
			            .HAlign(HAlign_Center)
                        .Padding(Padding)
                        .AutoHeight()
                        [
				            SNew(STextBlock)
				            .ShadowColorAndOpacity(FLinearColor::Black)
				            .ColorAndOpacity(FLinearColor::White)
				            .ShadowOffset(FIntPoint(-1, 1))
				            .Font(FSlateFontInfo(RobotoFont, 30))
				            .Text(LOCTEXT("SavedGamesLabel", "Saved Games"))
                        ]
                        // List View
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Fill)
			            .HAlign(HAlign_Fill)
                        .Padding(Padding)
                        [
                            SAssignNew(SavedGamesListView, SSavedGamesListView)
                            .OnSelectionChanged_Lambda([this](SSavedGamesListView::ListItem Item, ESelectInfo::Type Type)
                        {
                            if (!Item.IsValid())
                                return;
                            SaveNameTB->SetText(Item->Name);
                        })
                        ]
                        // save name edit
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Top)
			            .HAlign(HAlign_Fill)
                        .Padding(Padding)
                        [
                            SAssignNew(SaveGameNameHB, SHorizontalBox)
                            .Visibility(EVisibility::Collapsed)
                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Left)
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .Text(LOCTEXT("SaveNameLabel", "Save Name: "))
                                .Font(FSlateFontInfo(RobotoFont, 30))
                            ]
                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Fill)
                            [
                                SAssignNew(SaveNameTB, SEditableTextBox)
                                .RevertTextOnEscape(true)
                                .Font(FSlateFontInfo(RobotoFont, 30))
                            ]
                        ]
                    ]
                    // Text line
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Bottom)
			        .HAlign(HAlign_Center)
                    .Padding(Padding)
                    .AutoHeight()
                    [
                        MessageLine.ToSharedRef()
                    ]
                ]
            ]
		];
}

template <typename F>
SVerticalBox::FSlot& SPauseMenu::PauseMenuButton(FText Text, F function) const
{
    return
        SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
            .Padding(Padding)
            [
                SNew(SMenuButton)
                .Text(Text)
                .ParentArguments(SMenuButton::FParentArguments().OnClicked(this, function))
            ]
        ;
}

FReply SPauseMenu::OnContinue()
{
    ClearError();
    GP4Engine()->ShowPauseMenuFromBinding();
    return FReply::Handled();
}

FReply SPauseMenu::OnExitToMenu()
{
    ClearError();
    GP4Engine()->ReturnToMainMenu();
    return FReply::Handled();
}

FReply SPauseMenu::OnLoadGame()
{
    ClearError();
    SavedGamesListView->ReloadSaves();
    MenuVB->SetVisibility(EVisibility::Collapsed);
    LoadVB->SetVisibility(EVisibility::Visible);
    SavedGamesVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SPauseMenu::OnSaveGame()
{
    ClearError();
    SavedGamesListView->ReloadSaves(true);
    MenuVB->SetVisibility(EVisibility::Collapsed);
    SaveVB->SetVisibility(EVisibility::Visible);
    SavedGamesVB->SetVisibility(EVisibility::Visible);
    SaveGameNameHB->SetVisibility(EVisibility::Visible);

    return FReply::Handled();
}

FReply SPauseMenu::OnExit()
{
    ClearError();
    if (auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController())
        PlayerController->ConsoleCommand("quit");
    return FReply::Handled();
}

FReply SPauseMenu::PrintError(const FText& Text)
{
    MessageLine->SetText(Text);
    return FReply::Unhandled();
}

void SPauseMenu::ClearError()
{
    MessageLine->SetText(FText::FromString(FString()));
}

FReply SPauseMenu::OnOptions()
{
    ClearError();
    return FReply::Handled();
}

FReply SPauseMenu::OnNotImplemented()
{
    return PrintError(LOCTEXT("NotImplemented", "Not Implemented"));
}

FReply SPauseMenu::OnLoadBack()
{
    ClearError();
    LoadVB->SetVisibility(EVisibility::Collapsed);
    SaveVB->SetVisibility(EVisibility::Collapsed);
    SavedGamesVB->SetVisibility(EVisibility::Collapsed);
    SaveGameNameHB->SetVisibility(EVisibility::Collapsed);
    MenuVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SPauseMenu::OnLoadDelete()
{
    ClearError();
    auto selected = SavedGamesListView->GetSelectedItems();
    if (!selected.Num())
        return PrintError(LOCTEXT("ModNotSelected", "Please, select a saved game from the list"));
    auto n = selected[0]->Name.ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    GP4Engine()->deleteSaveGame(n);
    SavedGamesListView->ReloadSaves();
    return FReply::Handled();
}

FReply SPauseMenu::OnSaveDelete()
{
    auto r = OnLoadDelete();
    SavedGamesListView->ReloadSaves(true);
    return r;
}

FReply SPauseMenu::OnLoadLoad()
{
    ClearError();
    auto selected = SavedGamesListView->GetSelectedItems();
    if (!selected.Num())
        return PrintError(LOCTEXT("ModNotSelected", "Please, select a saved game from the list"));
    auto n = selected[0]->Name.ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    if (!GP4Engine()->load(n))
        return PrintError(LOCTEXT("LoadFailed", "Load Game failed. See logs for more information"));
    OnLoadBack();
    return FReply::Handled();
}

FReply SPauseMenu::OnSaveSave()
{
    ClearError();
    auto n = SaveNameTB->GetText().ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    if (!GP4Engine()->save(n))
        return PrintError(LOCTEXT("SaveFailed", "Save Game failed. See logs for more information"));
    SavedGamesListView->ReloadSaves(true);
    return FReply::Handled();
}
