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

#include "ModListView.h"

#include "Paths.h"
#include "FileManagerGeneric.h"

#include <Game/P4Modification.h>
#include <Game/P4Engine.h>

#define LOCTEXT_NAMESPACE "MainMenu"

void SModListView::Construct(const FArguments& InArgs)
{
    ReloadMods(false);

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

    ParentType::FArguments args;
    args
        .SelectionMode(ESelectionMode::Single)
        .AllowOverscroll(EAllowOverscroll::No)
        .ItemHeight(40)
        .ListItemsSource( &AvailableMods )
        .OnGenerateRow(this, &SModListView::OnGenerateWidgetForList)
        .HeaderRow(
            SNew(SHeaderRow)
            + ListViewColumn("Name", LOCTEXT("ModsNameLabel", "Name"))
            + ListViewColumn("Dir", LOCTEXT("ModsDirLabel", "Directory"))
            + ListViewColumn("Author", LOCTEXT("ModsAuthorLabel", "Author"))
            + ListViewColumn("Version", LOCTEXT("ModsVersionLabel", "Version"))
            + ListViewColumn("DateCreated", LOCTEXT("ModsDateCreatedLabel", "Date Created"))
            + ListViewColumn("DateModified", LOCTEXT("ModsDateModifiedLabel", "Date Modified"))
            )
        ;
    ParentType::Construct(args);
}

void SModListView::ReloadMods(bool reload)
{
    bool Empty = AvailableMods.Num() == 0;

    if (!reload || GP4Engine()->reloadMods())
    {
        AvailableMods = GP4Engine()->GetModificationDescriptors();
    }

    if (!Empty)
    {
        ClearSelection();
        ReGenerateItems(PanelGeometryLastTick);
        RequestListRefresh();
    }
}

TSharedRef<ITableRow> SModListView::OnGenerateWidgetForList( ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable )
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
            FString ItemText;
            if (ColumnName == "Name")
            {
                ItemText = Item->Name;
            }
            else if (ColumnName == "Dir")
            {
				ItemText = Item->Dir;
            }
            else if (ColumnName == "Author")
            {
				ItemText = Item->Author;
            }
            else if (ColumnName == "Version")
            {
                ItemText = Item->Version;
            }
            else if (ColumnName == "DateCreated")
            {
				ItemText = Item->DateCreated;
            }
            else if (ColumnName == "DateModified")
            {
				ItemText = Item->DateModified;
            }
            else
            {
                ItemText = "Unknown field!";
            }
            return
                SNew(STextBlock)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ColorAndOpacity(FLinearColor::White)
                .ShadowOffset(FIntPoint(-1, 1))
                .Font(FSlateFontInfo(RobotoFont, 24))
                .Text(FText::FromString(ItemText))
                .ColorAndOpacity(FSlateColor(FLinearColor(FColor(0, 0, 0, 255))))
                ;
        }
    };
    return SNew(SButtonRowWidget, OwnerTable, InItem);
}
