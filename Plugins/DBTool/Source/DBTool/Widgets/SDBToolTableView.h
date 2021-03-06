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

#include <Polygon4/DataManager/TreeItem.h>
#include <Polygon4/DataManager/Types.h>
#include <Polygon4/DataManager/Schema.h>
#include <Polygon4/DataManager/Storage.h>

struct TreeItem;
class SDBToolTreeView;

struct RowData
{
    TreeItem *Item;
    polygon4::TreeItem *Data;
    Variable Var;
};

struct ComboBoxItem
{
    FText Text;
    polygon4::detail::IObjectBase *Object;
};

class SDBToolTableView : public SListView<TSharedPtr<RowData>>
{
    SLATE_BEGIN_ARGS(SDBToolTableView) {}
        SLATE_ARGUMENT(polygon4::detail::Storage*, Storage)
    SLATE_END_ARGS()

public:
    using ListItem = TSharedPtr<RowData>;
    using ParentType = SListView<ListItem>;

    polygon4::detail::Storage* Storage;

private:
    TArray<ListItem> Data;

public:
    void Construct(const FArguments& InArgs);
    void ResetTable();
    void RefreshTable(TreeItem *Item);

private:
    TSharedRef<ITableRow> OnGenerateRow(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable);
};

class SButtonRowWidget : public SMultiColumnTableRow<SDBToolTableView::ListItem>
{
    SLATE_BEGIN_ARGS(SButtonRowWidget) {}
        SLATE_ARGUMENT(SDBToolTableView*, TableView)
    SLATE_END_ARGS()

    using ListItem = SDBToolTableView::ListItem;

private:
    SDBToolTableView* TableView;
    ListItem Item;

public:
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, ListItem InItem);
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
};
