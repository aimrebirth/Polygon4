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

class SPauseMenu : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SPauseMenu){}
        SLATE_ARGUMENT(APlayerController*, PlayerController)
        SLATE_ARGUMENT(class AP4GameMode*, GameMode)
	SLATE_END_ARGS()

    class AP4GameMode *GameMode;
    APlayerController* PlayerController;
    TSharedPtr<STextBlock> MessageLine;
    int Padding = 20;
 
public:
	void Construct(const FArguments& InArgs);

private:
    /*virtual bool SupportsKeyboardFocus() const override { return true; }
    virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override
	{
		return FReply::Handled().SetUserFocus(SharedThis( this ), EFocusCause::SetDirectly, true);
	}*/

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