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
#include <Polygon4/DataManager/Schema.h>
#include <Polygon4/DataManager/Storage.h>

struct TreeItem;
class SDBToolTreeView;

struct RowData
{
    TreeItem *Item;
    polygon4::TreeItem *Data;
    //const polygon4::DatabaseSchema *Schema;
    //const polygon4::Table *Table;
    Variable Var;
};

class SDBToolTableView : public SListView<TSharedPtr<RowData>>
{
    SLATE_BEGIN_ARGS(SDBToolTableView) {}
        SLATE_ARGUMENT(std::shared_ptr<polygon4::detail::Storage>, Storage)
    SLATE_END_ARGS()

public:
    using ListItem = TSharedPtr<RowData>;
    using ParentType = SListView<ListItem>;

private:
    TArray<ListItem> Data;
    std::shared_ptr<polygon4::detail::Storage> Storage;

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
        SLATE_ARGUMENT(std::shared_ptr<polygon4::detail::Storage>, Storage)
    SLATE_END_ARGS()

    using ListItem = SDBToolTableView::ListItem;
        
private:
    std::shared_ptr<polygon4::detail::Storage> Storage;
    ListItem Item;

public:
    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable, ListItem InItem);
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
};

class SValidatedEditableTextBox : public SEditableTextBox
{
    SLATE_BEGIN_ARGS(SValidatedEditableTextBox) {}
        SLATE_ARGUMENT(SDBToolTableView::ListItem, Item)
    SLATE_END_ARGS()

    using ParentType = SEditableTextBox;

private:
    SDBToolTableView::ListItem Item;

public:
    void Construct(const FArguments& InArgs);
    void OnTextCommited(const FText &NewText, ETextCommit::Type Type);
    FReply OnKeyDownHandler(const FGeometry &Geometry, const FKeyEvent &KeyEvent);
};