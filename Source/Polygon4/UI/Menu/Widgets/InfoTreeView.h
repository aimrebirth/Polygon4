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

#pragma once

#include "SlateBasics.h"

#include <Polygon4/DataManager/Types.h>
#include <Polygon4/BuildingMenu.h>

using P4InfoTreeItem = polygon4::InfoTreeItem;

struct InfoTreeItem
{
    P4InfoTreeItem *P4Item;

    InfoTreeItem *Parent;
    TArray<TSharedPtr<InfoTreeItem>> Children;

    TSharedPtr<STextBlock> Widget;

    TSharedPtr<InfoTreeItem> AddChild(P4InfoTreeItem *P4Item);
    void Build(P4InfoTreeItem *Root);
};

class InfoTreeView : public STreeView<TSharedPtr<InfoTreeItem>>
{
    SLATE_BEGIN_ARGS(InfoTreeView) {}
        SLATE_ARGUMENT(P4InfoTreeItem*, RootItem)
    SLATE_END_ARGS()

public:
    using ListItem = TSharedPtr<InfoTreeItem>;
    using ParentType = STreeView<ListItem>;

public:
    void Construct(const FArguments& InArgs);
    void Reset(P4InfoTreeItem *P4RootItem);

    bool SelectFirstNotParent();

private:
    ListItem RootItem;
    P4InfoTreeItem *P4RootItem;

    void OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo);
    TSharedRef<ITableRow> OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable);
    void OnGetChildren(ListItem Item, TArray<ListItem>& OutChildren);

    void SetExpandedItems(const TArray<TSharedPtr<InfoTreeItem>> &Items);
};
