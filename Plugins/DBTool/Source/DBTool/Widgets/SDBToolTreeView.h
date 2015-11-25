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

using P4TreeItem = polygon4::TreeItem;
using SPtrP4TreeItem = std::shared_ptr<P4TreeItem>;

struct TreeItem
{
    TSharedPtr<STextBlock> Widget;
    FText Name;
    SPtrP4TreeItem P4Item;
    TreeItem *Parent;
    TArray<TSharedPtr<TreeItem>> Children;

    TSharedPtr<TreeItem> AddChild(const SPtrP4TreeItem &P4Item);
    void Build(P4TreeItem *Root);
    void UpdateName();
};

class SDBToolTableView;

class SDBToolTreeView : public STreeView<TSharedPtr<TreeItem>>
{
    SLATE_BEGIN_ARGS(SDBToolTreeView) {}
        SLATE_ARGUMENT(TSharedPtr<SDBToolTableView>, TableView)
        SLATE_ARGUMENT(SPtrP4TreeItem, RootItem)
        SLATE_ARGUMENT(std::shared_ptr<polygon4::detail::Storage>, Storage)
    SLATE_END_ARGS()

private:
    using ListItem = TSharedPtr<TreeItem>;
    using ParentType = STreeView<ListItem>;
    
public:
    void Construct(const FArguments& InArgs);
    void Reset(SPtrP4TreeItem P4RootItem);

    void AddRecord();
    void DeleteRecord();

private:
    TSharedPtr<SDBToolTableView> TableView;
    ListItem RootItem;
    SPtrP4TreeItem P4RootItem;
    std::shared_ptr<polygon4::detail::Storage> Storage;

    void OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo);
    TSharedRef<ITableRow> OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable);
    void OnGetChildren(ListItem Item, TArray<ListItem>& OutChildren);
};
