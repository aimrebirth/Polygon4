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

struct RowData;
class SDBToolTableView;

struct ComboBoxItem
{
    FText Text;
    polygon4::detail::IObjectBase *Object;
};

class SDBToolTextComboBox : public SComboBox<TSharedPtr<ComboBoxItem>>
{
    SLATE_BEGIN_ARGS(SDBToolTextComboBox) {}
        SLATE_ARGUMENT(RowData*, ParentData)
        SLATE_ARGUMENT(SDBToolTableView*, TableView)
    SLATE_END_ARGS()

private:
    using ListItem = TSharedPtr<ComboBoxItem>;
    using ParentType = SComboBox<ListItem>;

    RowData* ParentData;
    SDBToolTableView* TableView;

public:
    TArray<ListItem> Items;

    void Construct(const FArguments& InArgs);
    void SetInitialized(bool i = true) { Initialized = i; }

private:
    bool Initialized = false;

    TSharedRef<SWidget> OnGenerateWidget(ListItem InItem);
    void OnSelectionChanged(ListItem Item, ESelectInfo::Type SelectInfo);
};
