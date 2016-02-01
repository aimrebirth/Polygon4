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
	SLATE_BEGIN_ARGS(SPauseMenu){}
	SLATE_END_ARGS()

    TSharedPtr<STextBlock> MessageLine;
    int Padding = 20;

public:
    void Construct(const FArguments& InArgs);

    virtual void OnShow() override {}
    virtual void OnHide() override {}

private:
    template <typename F>
    SVerticalBox::FSlot& PauseMenuButton(FText Text, F function) const;

    FReply OnContinue();
    FReply OnLoadGame();
    FReply OnOptions();
    FReply OnExit();
    FReply OnNotImplemented();

    FReply PrintError(const FText& Text);
    void ClearError();
};
