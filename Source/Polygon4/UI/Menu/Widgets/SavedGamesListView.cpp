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

#include "SavedGamesListView.h"

#include "Polygon4.h"

#include <Game/P4Engine.h>

#define LOCTEXT_NAMESPACE "MainMenu"

void SSavedGamesListView::Construct(const FArguments& InArgs)
{
    auto ListViewColumn = [](FName Name, FText Text)
    {
        return
            SHeaderRow::Column(Name)
            .DefaultLabel(Text)
            .VAlignHeader(EVerticalAlignment::VAlign_Center)
            .HAlignHeader(EHorizontalAlignment::HAlign_Center)
            .VAlignCell(EVerticalAlignment::VAlign_Center)
            .HAlignCell(EHorizontalAlignment::HAlign_Center);
    };

    this->OnSelectionChanged = InArgs._OnSelectionChanged;

    Base::FArguments args;
    args
        .SelectionMode(ESelectionMode::Single)
        .AllowOverscroll(EAllowOverscroll::No)
        .ItemHeight(40)
        .ListItemsSource( &AvailableGames )
        .OnSelectionChanged(OnSelectionChanged)
        .OnGenerateRow(this, &SSavedGamesListView::OnGenerateWidgetForList)
        .HeaderRow(
            SNew(SHeaderRow)
            + ListViewColumn("Name", LOCTEXT("ModsNameLabel", "Name"))
            )
        ;
    Base::Construct(args);

    ReloadSaves();
}

void SSavedGamesListView::ReloadSaves(bool save)
{
    AvailableGames.Empty();

    auto games = GP4Engine()->getSavedGames(save);
    for (auto &g : games)
    {
        ListItem i = MakeShareable(new SavedGameDesc);
        i->Name = g;
        AvailableGames.Add(i);
    }

    ClearSelection();
    ReGenerateItems(PanelGeometryLastTick);
    RequestListRefresh();
}

TSharedRef<ITableRow> SSavedGamesListView::OnGenerateWidgetForList( ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable )
{
    class SButtonRowWidget : public SMultiColumnTableRow<ListItem>
    {
        ListItem Item;

        SLATE_BEGIN_ARGS(SButtonRowWidget){}
        SLATE_END_ARGS()

        void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, ListItem InItem)
        {
            Item = InItem;

            SMultiColumnTableRow<ListItem>::Construct(FSuperRowType::FArguments(), InOwnerTable);
        }

        virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
        {
            FText ItemText;
            if (ColumnName == "Name")
            {
                ItemText = Item->Name;
            }
            else
            {
                ItemText = FText::FromString("Unknown field!");
            }
            return
                SNew(STextBlock)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ColorAndOpacity(FLinearColor::White)
                .ShadowOffset(FIntPoint(-1, 1))
                .Font(FSlateFontInfo(RobotoFont, 24))
                .Text(ItemText)
                ;
        }
    };
    return SNew(SButtonRowWidget, OwnerTable, InItem);
}