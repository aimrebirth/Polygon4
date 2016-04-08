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
        .Font(FSlateFontInfo("Verdana", 30));

    SavedGamesListView = SNew(SSavedGamesListView);

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
				.Font(FSlateFontInfo("Verdana", 100))
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
                        + PauseMenuButton(LOCTEXT("SaveGameButtonLabel" , "Save Game"), &SPauseMenu::OnNotImplemented)
                        + PauseMenuButton(LOCTEXT("LoadGameButtonLabel", "Load Game"), &SPauseMenu::OnLoadGame)
                        + PauseMenuButton(LOCTEXT("OptionsButtonLabel", "Options"), &SPauseMenu::OnNotImplemented)
                        + PauseMenuButton(LOCTEXT("MainMenuButtonLabel" , "Exit to Main Menu"), &SPauseMenu::OnExitToMenu)
                        + PauseMenuButton(LOCTEXT("ExitGameButtonLabel", "Exit Game"), &SPauseMenu::OnExit)
                    ]
                    + SVerticalBox::Slot()
                    [
                        SAssignNew(LoadVB, SVerticalBox)
                        .Visibility(EVisibility::Collapsed)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "LoadButtonLabel", "Load"), &SPauseMenu::OnLoadLoad)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "BackButtonLabel", "Back"), &SPauseMenu::OnLoadBack)
                        + PauseMenuButton(NSLOCTEXT("MainMenu", "DeleteButtonLabel", "Delete"), &SPauseMenu::OnLoadDelete)
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
				            .Font(FSlateFontInfo("Verdana", 30))
				            .Text(LOCTEXT("SavedGamesLabel", "Saved Games"))
                        ]
                        // List View
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Fill)
			            .HAlign(HAlign_Fill)
                        .Padding(Padding)
                        [
                            SavedGamesListView.ToSharedRef()
                        ]
                        // save name edit
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
    GP4Engine()->ShowPauseMenuFromBinding();
    return FReply::Handled();
}

FReply SPauseMenu::OnExitToMenu()
{
    auto world = GP4Engine()->GetWorld();
    world->ServerTravel("/Game/Mods/Common/Maps/start");
    return FReply::Handled();
}

FReply SPauseMenu::OnLoadGame()
{
    SavedGamesListView->ReloadSaves();
    MenuVB->SetVisibility(EVisibility::Collapsed);
    LoadVB->SetVisibility(EVisibility::Visible);
    SavedGamesVB->SetVisibility(EVisibility::Visible);

    return FReply::Handled();
}

FReply SPauseMenu::OnExit()
{
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
    return FReply::Handled();
}

FReply SPauseMenu::OnNotImplemented()
{
    return PrintError(LOCTEXT("NotImplemented", "Not Implemented"));
}

FReply SPauseMenu::OnLoadBack()
{
    LoadVB->SetVisibility(EVisibility::Collapsed);
    SavedGamesVB->SetVisibility(EVisibility::Collapsed);
    MenuVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SPauseMenu::OnLoadDelete()
{
    return OnNotImplemented();
}

FReply SPauseMenu::OnLoadLoad()
{
    return OnNotImplemented();
}
