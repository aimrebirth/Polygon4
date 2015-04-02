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

#include "Paths.h"
#include "FileManagerGeneric.h"

#include "Common.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SMainMenu::Construct(const FArguments& InArgs)
{
    APlayerController* PlayerController = InArgs._PlayerController;
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }

    polygon4::Mods mods = polygon4::enumerateMods(FString(FPaths::GameContentDir() + "Mods/").GetCharArray().GetData());
    AvailableMods = MakeTArrayTSharedPtr(mods);

    // create Mods List View
    auto ListView = SNew(SListView<ListItem>)
        .SelectionMode(ESelectionMode::SingleToggle)
        .AllowOverscroll(EAllowOverscroll::No)
        .ItemHeight(40)
        .ListItemsSource( &AvailableMods )
        .OnGenerateRow(this, &SMainMenu::OnGenerateWidgetForList)
        .HeaderRow(
            SNew(SHeaderRow)
            + SHeaderRow::Column("Mod")
            .DefaultLabel(LOCTEXT("ModsLabel", "Available Mods"))
            .VAlignHeader(EVerticalAlignment::VAlign_Center)
            .HAlignHeader(EHorizontalAlignment::HAlign_Center)
            .VAlignCell(EVerticalAlignment::VAlign_Center)
            .HAlignCell(EHorizontalAlignment::HAlign_Center)
            )
        ;

    auto Padding = 40;

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
				.Font(FSlateFontInfo("Veranda", 72))
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
                    // New Game
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Top)
			        .HAlign(HAlign_Center)
                    .Padding(Padding)
                    [
                        SNew(SButton)
                        [
				            SNew(STextBlock)
				            .ShadowColorAndOpacity(FLinearColor::Black)
				            .ColorAndOpacity(FLinearColor::White)
				            .ShadowOffset(FIntPoint(-1, 1))
				            .Font(FSlateFontInfo("Veranda", 72))
				            .Text(LOCTEXT("NewGameButtonLabel", "New Game"))
                        ]
                    ]
                    // Load Game
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Top)
			        .HAlign(HAlign_Center)
                    .Padding(Padding)
                    [
                        SNew(SButton)
                        [
				            SNew(STextBlock)
				            .ShadowColorAndOpacity(FLinearColor::Black)
				            .ColorAndOpacity(FLinearColor::White)
				            .ShadowOffset(FIntPoint(-1, 1))
				            .Font(FSlateFontInfo("Veranda", 72))
				            .Text(LOCTEXT("LoadGameButtonLabel", "Load Game"))
                        ]
                    ]
                    // Exit Game
                    + SVerticalBox::Slot()
			        .VAlign(VAlign_Top)
			        .HAlign(HAlign_Center)
                    .Padding(Padding)
                    [
                        SNew(SButton)
                        .OnClicked_Lambda([PlayerController]()->FReply{ if (PlayerController) PlayerController->ConsoleCommand("quit"); return FReply::Handled(); })
                        [
				            SNew(STextBlock)
				            .ShadowColorAndOpacity(FLinearColor::Black)
				            .ColorAndOpacity(FLinearColor::White)
				            .ShadowOffset(FIntPoint(-1, 1))
				            .Font(FSlateFontInfo("Veranda", 72))
				            .Text(LOCTEXT("ExitGameButtonLabel", "Exit Game"))
                        ]
                    ]
                ]
                // List View
                + SHorizontalBox::Slot()
			    .VAlign(VAlign_Fill)
			    .HAlign(HAlign_Fill)
                .Padding(Padding)
                [
                    ListView
                ]
            ]
		];
}

TSharedRef<ITableRow> SMainMenu::OnGenerateWidgetForList( ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable )
{
    class SButtonRowWidget : public STableRow<ListItem>
    {
        ListItem Item;

    public:
        SLATE_BEGIN_ARGS(SButtonRowWidget){}
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, ListItem InItem)
        {
            Item = InItem;
        }

	    virtual TSharedRef<SWidget> AsWidget() override
	    {
		    return 
                //SNew(SButton)
            //[
                SNew(STextBlock)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ColorAndOpacity(FLinearColor::White)
                .ShadowOffset(FIntPoint(-1, 1))
                .Font(FSlateFontInfo("Veranda", 24))
                .Text(FText::FromString(Item->getName().c_str()))
            //]
            ;
	    }
    };
    return SNew(SButtonRowWidget, InItem);
}