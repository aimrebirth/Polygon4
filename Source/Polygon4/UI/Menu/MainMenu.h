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

#include "Menu.h"

#include <Polygon4/DataManager/Localization.h>

struct LanguageComboBoxItem
{
	FText Text;
	polygon4::LocalizationType id = polygon4::LocalizationType::max;
};

using cb_item_type = TSharedPtr<LanguageComboBoxItem>;
using cb_item_array = TArray<cb_item_type>;
using cb_item_array_ptr = TSharedPtr<cb_item_array>;
using cb_type = SComboBox<cb_item_type>;

class SMainMenu : public SMenu
{
public:
    using SModsListView = TSharedPtr<class SModListView>;
    using SSGamesListView = TSharedPtr<class SSavedGamesListView>;

	SLATE_BEGIN_ARGS(SMainMenu){}
	SLATE_END_ARGS()

private:
    SModsListView ModsListView;
    TSharedPtr<STextBlock> MessageLine;
    int Padding = 20;

    SSGamesListView SavedGamesListView;
    TSharedPtr<SVerticalBox> LoadVB;
    TSharedPtr<SVerticalBox> MenuVB;
    TSharedPtr<SVerticalBox> ModsVB;
    TSharedPtr<SVerticalBox> SavedGamesVB;

public:
    void Construct(const FArguments& InArgs);

    virtual void OnShow() override;
    virtual void OnHide() override {}

    void ReloadMods();

private:
	TSharedPtr<cb_type> LangWidget;
	cb_item_array_ptr LanguageItems;
    TSharedPtr<STextBlock> CurrentLangItem;

    template <typename F>
    SVerticalBox::FSlot& MainMenuButton(FText Text, F function) const;

    FReply OnNewGame();
    FReply OnLoadGame();
    FReply OnReloadMods();
    FReply OnOptions();
    FReply OnAuthors();
    FReply OnExit();

    FReply OnLoadBack();
    FReply OnLoadDelete();
    FReply OnLoadLoad();

    FReply OnNotImplemented();

    FReply PrintError(const FText& Text);
    void ClearError();
};
