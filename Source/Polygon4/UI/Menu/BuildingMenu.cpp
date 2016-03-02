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
#include "EditorStyle.h"

#include "BuildingMenu.h"

#include "Widgets/MenuButton.h"

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
                        + VerticalSlotMenuButton(LOCTEXT("SaveButton", "Save"), this, &SBuildingMenu::DoNothing)
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
        ]
    ;
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

FReply SBuildingMenu::OnExit()
{
    GP4Engine()->HideBuildingMenu();
    GP4Engine()->spawnCurrentPlayer();
    return FReply::Unhandled();
}

void SBuildingMenu::SetCurrentBuilding(polygon4::detail::ModificationMapBuilding *B)
{
    if (!B)
        return;
    CurrentBuilding = B;
    Name->SetText(CurrentBuilding->getName().toFText());
}

FText SBuildingMenu::getFText() const
{
    return getText().toFText();
}
