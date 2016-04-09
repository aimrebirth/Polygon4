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

#include <Game/P4Modification.h>

struct SavedGameDesc
{
    FText Name;
};

class SSavedGamesListView : public SListView<TSharedPtr<SavedGameDesc>>
{
public:
    using ListItem = TSharedPtr<SavedGameDesc>;
    using Base = SListView<ListItem>;
    using FOnSelectionChanged = Base::FOnSelectionChanged;

public:
    SLATE_BEGIN_ARGS(SSavedGamesListView)
        : _OnSelectionChanged()
    {}
    SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
    SLATE_END_ARGS()

    TArray<ListItem> AvailableGames;
    int Padding = 20;

public:
	void Construct(const FArguments& InArgs);
    void ReloadSaves(bool save = false);

    TSharedRef<ITableRow> OnGenerateWidgetForList(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable);
};
