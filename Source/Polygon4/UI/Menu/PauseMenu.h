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

class SPauseMenu : public SMenu
{
    using SSGamesListView = TSharedPtr<class SSavedGamesListView>;

	SLATE_BEGIN_ARGS(SPauseMenu){}
	SLATE_END_ARGS()

    TSharedPtr<STextBlock> MessageLine;
    int Padding = 20;

    SSGamesListView SavedGamesListView;
    TSharedPtr<SVerticalBox> LoadVB;
    TSharedPtr<SVerticalBox> SaveVB;
    TSharedPtr<SVerticalBox> MenuVB;
    TSharedPtr<SVerticalBox> SavedGamesVB;
    TSharedPtr<SHorizontalBox> SaveGameNameHB;
    TSharedPtr<SEditableTextBox> SaveNameTB;

public:
    void Construct(const FArguments& InArgs);

    virtual void OnShow() override {}
    virtual void OnHide() override {}

private:
    template <typename F>
    auto PauseMenuButton(FText Text, F function) const;

    FReply OnContinue() const;
    FReply OnExitToMenu() const;
    FReply OnLoadGame() const;
    FReply OnSaveGame() const;
    FReply OnOptions() const;
    FReply OnExit() const;
    FReply OnNotImplemented() const;

    FReply OnLoadBack() const;
    FReply OnLoadDelete() const;
    FReply OnLoadLoad() const;
    FReply OnSaveSave() const;
    FReply OnSaveDelete() const;

    FReply PrintError(const FText& Text) const;
    void ClearError() const;
};
