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

#include "MainMenu.h"
#include "Widgets/ModListView.h"
#include "Widgets/SavedGamesListView.h"
#include "Widgets/MenuButton.h"

#include <Game/P4Engine.h>
#include <Game/P4Modification.h>

#include "Version.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenu::Construct(const FArguments& InArgs)
{
    auto PlayerController = GWorld->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }

    // create message line
    MessageLine = SNew(STextBlock)
        .ShadowColorAndOpacity(FLinearColor::Black)
        .ColorAndOpacity(FLinearColor::Red)
        .ShadowOffset(FIntPoint(-1, 1))
        .Font(FSlateFontInfo("Verdana", 30));

    FString Ver = LOCTEXT("VersionLabel", "Version").ToString();
    Ver += ": " + GetPolygon4Version();
    FText Version = FText::FromString(Ver);

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
                        + MainMenuButton(LOCTEXT("NewGameButtonLabel" , "New Game" ), &SMainMenu::OnNewGame)
                        + MainMenuButton(LOCTEXT("LoadGameButtonLabel", "Load Game"), &SMainMenu::OnLoadGame)
                        //+ MainMenuButton(LOCTEXT("AuthorsButtonLabel", "Authors"), &SMainMenu::OnNotImplemented)
                        //+ MainMenuButton(LOCTEXT("OptionsButtonLabel", "Options"), &SMainMenu::OnNotImplemented)
                        + MainMenuButton(LOCTEXT("ExitGameButtonLabel", "Exit Game"), &SMainMenu::OnExit)
                    ]
                    + SVerticalBox::Slot()
                    [
                        SAssignNew(LoadVB, SVerticalBox)
                        .Visibility(EVisibility::Collapsed)
                        + MainMenuButton(LOCTEXT("LoadButtonLabel", "Load"), &SMainMenu::OnLoadLoad)
                        + MainMenuButton(LOCTEXT("BackButtonLabel", "Back"), &SMainMenu::OnLoadBack)
                        + MainMenuButton(LOCTEXT("DeleteButtonLabel", "Delete"), &SMainMenu::OnLoadDelete)
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
                    [
                        SAssignNew(ModsVB, SVerticalBox)
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
				            .Text(LOCTEXT("ModsLabel", "Available Mods"))
                        ]
                        // List View
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Fill)
			            .HAlign(HAlign_Fill)
                        .Padding(Padding)
                        [
                            SAssignNew(ModsListView, SModListView)
                        ]
                        // Reload Button
                        + SVerticalBox::Slot()
			            .VAlign(VAlign_Bottom)
			            .HAlign(HAlign_Center)
                        .Padding(Padding)
                        .AutoHeight()
                        [
                            SNew(SButton)
                            .OnClicked(this, &SMainMenu::OnReloadMods)
                            [
                                SNew(STextBlock)
                                .ShadowColorAndOpacity(FLinearColor::Black)
                                .ColorAndOpacity(FLinearColor::White)
                                .ShadowOffset(FIntPoint(-1, 1))
                                .Font(FSlateFontInfo("Verdana", 40))
                                .Text(LOCTEXT("ReloadModsButtonLabel", "Reload mod list"))
                            ]
                        ]
                    ]
                    + SVerticalBox::Slot()
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
                            SAssignNew(SavedGamesListView, SSavedGamesListView)
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
            // version
            + SVerticalBox::Slot()
            .VAlign(VAlign_Bottom)
            .HAlign(HAlign_Right)
            .AutoHeight()
            [
                SNew(STextBlock)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ColorAndOpacity(FLinearColor::Green)
                .ShadowOffset(FIntPoint(-1, 1))
                .Font(FSlateFontInfo("Verdana", 20))
                .Text(Version)
            ]
		];
}

template <typename F>
SVerticalBox::FSlot& SMainMenu::MainMenuButton(FText Text, F function) const
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

void SMainMenu::OnShow()
{
    ReloadMods();
}

void SMainMenu::ReloadMods()
{
    if (ModsListView.IsValid())
        ModsListView->ReloadMods();
}

FReply SMainMenu::OnNewGame()
{
    auto selected = ModsListView->GetSelectedItems();
    if (!selected.Num())
        return PrintError(LOCTEXT("ModNotSelected", "Please, select a modification from the list"));
    ClearError();
    if (!selected[0]->modification->newGame())
        return PrintError(LOCTEXT("NewGameFailed", "Cannot start a New Game. See logs for more information"));
    return FReply::Handled();
}

FReply SMainMenu::OnLoadGame()
{
    SavedGamesListView->ReloadSaves();
    MenuVB->SetVisibility(EVisibility::Collapsed);
    ModsVB->SetVisibility(EVisibility::Collapsed);
    LoadVB->SetVisibility(EVisibility::Visible);
    SavedGamesVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SMainMenu::OnReloadMods()
{
    ClearError();
    ModsListView->ReloadMods();
    return FReply::Handled();
}

FReply SMainMenu::OnExit()
{
    GP4Engine()->Exit();
    return FReply::Handled();
}

FReply SMainMenu::PrintError(const FText& Text)
{
    MessageLine->SetText(Text);
    return FReply::Unhandled();
}

void SMainMenu::ClearError()
{
    MessageLine->SetText(FText::FromString(FString()));
}

FReply SMainMenu::OnAuthors()
{
    return FReply::Handled();
}

FReply SMainMenu::OnOptions()
{
    return FReply::Handled();
}

FReply SMainMenu::OnNotImplemented()
{
    return PrintError(LOCTEXT("NotImplemented", "Not Implemented"));
}

FReply SMainMenu::OnLoadBack()
{
    LoadVB->SetVisibility(EVisibility::Collapsed);
    SavedGamesVB->SetVisibility(EVisibility::Collapsed);
    MenuVB->SetVisibility(EVisibility::Visible);
    ModsVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SMainMenu::OnLoadDelete()
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

FReply SMainMenu::OnLoadLoad()
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
