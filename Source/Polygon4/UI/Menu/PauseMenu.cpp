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
#include "../../Polygon4GameMode.h"

#include <Polygon4/Modification.h>

#include "Common.h"

#define LOCTEXT_NAMESPACE "PauseMenu"

void SPauseMenu::Construct(const FArguments& InArgs)
{
    GameMode = InArgs._GameMode;
    PlayerController = InArgs._PlayerController;
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }

    // create message line
    MessageLine = SNew(STextBlock)
        .ShadowColorAndOpacity(FLinearColor::Black)
        .ColorAndOpacity(FLinearColor::Red)
        .ShadowOffset(FIntPoint(-1, 1))
        .Font(FSlateFontInfo("Veranda", 30));

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
				.Font(FSlateFontInfo("Veranda", 100))
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
                    + PauseMenuButton(LOCTEXT("ContinueGameButtonLabel" , "Continue Game" ), &SPauseMenu::OnContinue)
                    + PauseMenuButton(LOCTEXT("SaveGameButtonLabel" , "Save Game" ), &SPauseMenu::OnNotImplemented)
                    + PauseMenuButton(LOCTEXT("LoadGameButtonLabel", "Load Game"), &SPauseMenu::OnNotImplemented)
                    + PauseMenuButton(LOCTEXT("OptionsButtonLabel", "Options"), &SPauseMenu::OnNotImplemented)
                    + PauseMenuButton(LOCTEXT("ExitGameButtonLabel", "Exit Game"), &SPauseMenu::OnExit)
                ]
                // right part of the screen
                + SHorizontalBox::Slot()
			    .VAlign(VAlign_Fill)
			    .HAlign(HAlign_Fill)
                [
			        SNew(SVerticalBox)
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
    GameMode->ShowMenu();
    return FReply::Handled();
}

FReply SPauseMenu::OnLoadGame()
{
    return FReply::Handled();
}

FReply SPauseMenu::OnExit()
{
    if (PlayerController)
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