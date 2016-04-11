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

#include "BuildingMenu.h"

#include "Widgets/MenuButton.h"
#include "Widgets/SavedGamesListView.h"

#include <Game/P4Engine.h>

#define LOCTEXT_NAMESPACE "BuildingMenu"

void SBuildingMenu::Construct(const FArguments& InArgs)
{
    auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }

    auto BackgroundTexture = LoadObject<UTexture2D>(0, TEXT("Texture2D'/Game/Mods/Common/Images/bg_base.bg_base'"));
    FSlateBrush *BackgroundBrush;
    if (BackgroundTexture)
        BackgroundBrush = new FSlateDynamicImageBrush(BackgroundTexture, FVector2D{ 100,100 }, FName("bg_base"));
    else
        BackgroundBrush = new FSlateColorBrush(FColor::Black);

    ChildSlot
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBorder)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            .Padding(10)
            .BorderImage(BackgroundBrush)
            [
                SNew(SVerticalBox)
                // building menu
                + SVerticalBox::Slot()
                [
                    SAssignNew(BuildingMenuVB, SVerticalBox)
                    // top layout
                    + SVerticalBox::Slot()
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    [
                        SNew(SHorizontalBox)
                        // left menu
                        + SHorizontalBox::Slot()
                        .FillWidth(0.25)
                        [
                            SNew(SBorder)
                            .Padding(10)
                            [
                                SNew(SVerticalBox)
                                // exit button
                                + VerticalSlotMenuButton(LOCTEXT("SaveButton", "Save"), this, &SBuildingMenu::OnSave)
                                .VAlign(VAlign_Top)
                                + VerticalSlotMenuButton(LOCTEXT("JournalButton", "Journal"), this, &SBuildingMenu::DoNothing)
                                .VAlign(VAlign_Top)
                                + VerticalSlotMenuButton(LOCTEXT("GliderButton", "Glider"), this, &SBuildingMenu::DoNothing)
                                .VAlign(VAlign_Top)
                                + VerticalSlotMenuButton(LOCTEXT("TradeButton", "Trade"), this, &SBuildingMenu::DoNothing)
                                .VAlign(VAlign_Top)
                                + VerticalSlotMenuButton(LOCTEXT("ClansButton", "Clans"), this, &SBuildingMenu::DoNothing)
                                .VAlign(VAlign_Top)
                                + VerticalSlotMenuButton(LOCTEXT("ExitButton", "Exit"), this, &SBuildingMenu::OnExit)
                                .VAlign(VAlign_Bottom)
                            ]
                        ]
                        // middle
                        + SHorizontalBox::Slot()
                        .FillWidth(0.5)
                        [
                            SNew(SBorder)
                            .Padding(10)
                            [
                                SNew(SVerticalBox)
                                // caption
                                + SVerticalBox::Slot()
                                .VAlign(VAlign_Top)
                                .HAlign(HAlign_Fill)
                                .AutoHeight()
                                [
                                    SNew(SBorder)
                                    .Padding(10)
                                    [
                                        SNew(SVerticalBox)
                                        + SVerticalBox::Slot()
                                        .HAlign(HAlign_Center)
                                        [
                                            SAssignNew(Name, STextBlock)
                                            .Font(FSlateFontInfo("Tahoma", 14))
                                            .Text(FText::FromString(L"Building Name"))
                                        ]
                                    ]
                                ]
                                // text
                                + SVerticalBox::Slot()
                                .VAlign(VAlign_Fill)
                                .HAlign(HAlign_Fill)
                                [
                                    SNew(SBorder)
                                    .Padding(10)
                                    //.BorderBackgroundColor(FColor::Black)
                                    //FColor(16, 23, 50, 128)
                                    //.ColorAndOpacity(FColor::Black)
                                    //.ForegroundColor(FColor::Black)
                                    [
                                        SNew(SVerticalBox)
                                        + SVerticalBox::Slot()
                                        [
                                            SAssignNew(Text, TextWidget)
                                            .Font(FSlateFontInfo("Tahoma", 14))
                                            .Text(this, &SBuildingMenu::getFText)
                                            //.TextStyle()
                                            .AutoWrapText(true)
                                            //.DecoratorStyleSet(&FEditorStyle::Get())
                                        ]
                                    ]
                                ]
                            ]
                        ]
                        // right menu
                        + SHorizontalBox::Slot()
                        .FillWidth(0.25)
                        [
                            SNew(SBorder)
                            .Padding(10)
                        ]
                    ]
                    // bottom layout (money, rating etc.)
                    + SVerticalBox::Slot()
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Bottom)
                    .AutoHeight()
                    [
                        SNew(SBorder)
                        .Padding(10)
                        [
                            SNew(SVerticalBox)
                            + BottomText(LOCTEXT("MoneyLabel", "Money: "), MoneyTB)
                            + BottomText(LOCTEXT("RatingLabel", "Rating: "), RatingTB)
                            + BottomText(LOCTEXT("MassLabel", "Mass: "), MassTB)
                        ]
                    ]
                ]
                // load menu
                + SVerticalBox::Slot()
                [
                    SAssignNew(SaveVB, SVerticalBox)
                    .Visibility(EVisibility::Collapsed)
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
                            + VerticalSlotMenuButton(LOCTEXT("SaveButtonLabel", "Save"), this, &SBuildingMenu::OnSaveSave)
                            + VerticalSlotMenuButton(LOCTEXT("BackButtonLabel", "Back"), this, &SBuildingMenu::OnSaveBack)
                            + VerticalSlotMenuButton(LOCTEXT("DeleteButtonLabel", "Delete"), this, &SBuildingMenu::OnSaveDelete)
                        ]
                        // right part of the screen
                        + SHorizontalBox::Slot()
			            .VAlign(VAlign_Fill)
			            .HAlign(HAlign_Fill)
                        [
                            SNew(SVerticalBox)
                            + SVerticalBox::Slot()
                            [
                                SNew(SVerticalBox)
                                // label
                                + SVerticalBox::Slot()
			                    .VAlign(VAlign_Top)
			                    .HAlign(HAlign_Center)
                                .Padding(20)
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
                                .Padding(20)
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
                                .Padding(20)
                                [
                                    SNew(SHorizontalBox)
                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Left)
                                    .AutoWidth()
                                    [
                                        SNew(STextBlock)
                                        .Text(LOCTEXT("SaveNameLabel", "Save Name: "))
                                        .Font(FSlateFontInfo("Verdana", 30))
                                    ]
                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Fill)
                                    [
                                        SAssignNew(SaveNameTB, SEditableTextBox)
                                        .RevertTextOnEscape(true)
                                        .Font(FSlateFontInfo("Verdana", 30))
                                    ]
                                ]
                            ]
                        ]
                    ]
                ]
            ]
        ]
    ;
}

SVerticalBox::FSlot& SBuildingMenu::BottomText(FText Name, TSharedPtr<STextBlock> &Var) const
{
    return
        SVerticalBox::Slot()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SNew(STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Verdana", 18))
				.Text(Name)
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
				SAssignNew(Var, STextBlock)
				.ShadowColorAndOpacity(FLinearColor::Black)
				.ColorAndOpacity(FLinearColor::White)
				.ShadowOffset(FIntPoint(-1, 1))
				.Font(FSlateFontInfo("Verdana", 18))
            ]
        ];
}

void SBuildingMenu::refresh()
{
    if (currentBuilding)
    {
        Name->SetText(currentBuilding->getName().toFText());
    }
    if (mechanoid)
    {
        MoneyTB->SetText(FString(std::to_string(mechanoid->money).c_str()));
        RatingTB->SetText(FString(std::to_string(mechanoid->rating).c_str()));
        //MassTB->SetText(FString(std::to_string(mechanoid->money).c_str()));
        MassTB->SetText(FString("0"));
    }
}

void SBuildingMenu::OnShow()
{
    GP4Engine()->UnsetPauseMenuBindings();
}

void SBuildingMenu::OnHide()
{
    auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = false;
    }
    GP4Engine()->SetPauseMenuBindings();
}

FReply SBuildingMenu::OnSave()
{
    BuildingMenuVB->SetVisibility(EVisibility::Collapsed);
    SaveVB->SetVisibility(EVisibility::Visible);
    return FReply::Handled();
}

FReply SBuildingMenu::OnExit()
{
    GP4Engine()->HideBuildingMenu();
    GP4Engine()->spawnCurrentPlayer();
    return FReply::Handled();
}

FText SBuildingMenu::getFText() const
{
    return getText().toFText();
}

FReply SBuildingMenu::OnSaveSave()
{
    auto n = SaveNameTB->GetText().ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    if (!GP4Engine()->save(n))
        return FReply::Unhandled();
    SavedGamesListView->ReloadSaves(true);
    return FReply::Handled();
}

FReply SBuildingMenu::OnSaveBack()
{
    BuildingMenuVB->SetVisibility(EVisibility::Visible);
    SaveVB->SetVisibility(EVisibility::Collapsed);
    return FReply::Handled();
}

FReply SBuildingMenu::OnSaveDelete()
{
    auto selected = SavedGamesListView->GetSelectedItems();
    if (!selected.Num())
        return FReply::Unhandled();
    auto n = selected[0]->Name.ToString();
    if (n.IsEmpty())
        return FReply::Unhandled();
    GP4Engine()->deleteSaveGame(n);
    SavedGamesListView->ReloadSaves(true);
    return FReply::Handled();
}
