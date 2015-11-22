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

typedef TSharedPtr<ModificationDesc> ListItem;

class SModListView : public SListView<ListItem>
{
    SLATE_BEGIN_ARGS(SModListView) {}
        SLATE_ARGUMENT(APlayerController*, PlayerController)
    SLATE_END_ARGS()

    typedef SListView<ListItem> ParentType;

    APlayerController* PlayerController;
    TArray<ListItem> AvailableMods;
    int Padding = 20;
 
public:
	void Construct(const FArguments& InArgs);
    void ReloadMods(bool reload = true);

    TSharedRef<ITableRow> OnGenerateWidgetForList(ListItem InItem, const TSharedRef<STableViewBase>& OwnerTable);
};
