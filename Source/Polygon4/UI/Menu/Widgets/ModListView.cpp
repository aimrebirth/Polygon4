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

#include "ModListView.h"

#include "Paths.h"
#include "FileManagerGeneric.h"

#include <Polygon4/Mods.h>

#include "Common.h"

#define LOCTEXT_NAMESPACE "MainMenu"

void SModListView::Construct(const FArguments& InArgs)
{
    ReloadMods();

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

void SModListView::ReloadMods()
{
    bool Empty = AvailableMods.Num() == 0;

    std::wstring s1 = FString(FPaths::GameDir() + "Mods/").GetCharArray().GetData();
    std::wstring s2 = FString(FPaths::GameContentDir() + "Mods/").GetCharArray().GetData();
    auto mods = polygon4::enumerateMods(s1, s2);
    AvailableMods = MakeTArrayTSharedPtr(mods);

    if (!Empty)
    {
        ClearSelection();
        ReGenerateItems(PanelGeometryLastTick);
        ItemsSource = &AvailableMods;
    }
}

TSharedRef<ITableRow> SModListView::OnGenerateWidgetForList( ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable )
{
    class SButtonRowWidget : public SMultiColumnTableRow<ListItem>
    {
        ListItem Item;

    public:
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
                ItemText = Item->getName().wstring().c_str();
            }
            else if (ColumnName == "Dir")
            {
                ItemText = Item->getDir().wstring().c_str();
            }
            else if (ColumnName == "Author")
            {
                ItemText = Item->getAuthor().wstring().c_str();
            }
            else if (ColumnName == "Version")
            {
                ItemText = Item->getVersion().wstring().c_str();
            }
            else if (ColumnName == "DateCreated")
            {
                ItemText = Item->getCreated().wstring().c_str();
            }
            else if (ColumnName == "DateModified")
            {
                ItemText = Item->getModified().wstring().c_str();
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
                .Font(FSlateFontInfo("Veranda", 24))
                .Text(FText::FromString(ItemText))
                .ColorAndOpacity(FSlateColor(FLinearColor(FColor(0, 0, 0, 255))))
                ;
        }
    };
    return SNew(SButtonRowWidget, OwnerTable, InItem);
}