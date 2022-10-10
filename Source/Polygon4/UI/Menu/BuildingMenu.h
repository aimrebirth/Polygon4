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

#include "Windows/PreWindowsApi.h"
#include <Polygon4/BuildingMenu.h>
#include "Windows/PostWindowsApi.h"

#include "Menu.h"

namespace polygon4 {
    namespace detail {
        class ModificationMapBuilding;
    }
}

class InfoTreeView;
class SBar;

class SBuildingMenu : public SMenu, public polygon4::BuildingMenu
{
    using SSGamesListView = TSharedPtr<class SSavedGamesListView>;
    using TextWidget = SRichTextBlock;

	SLATE_BEGIN_ARGS(SBuildingMenu){}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

    virtual void OnShow() override;
    virtual void OnHide() override;

    FText getFText() const;

    virtual void refresh() override final;

private:
    TSharedPtr<STextBlock> Name;
    TSharedPtr<TextWidget> Text;

    SSGamesListView SavedGamesListView;
    TSharedPtr<SVerticalBox> SaveVB;
    TSharedPtr<SVerticalBox> BuildingMenuVB;
    TSharedPtr<SEditableTextBox> SaveNameTB;
    TSharedPtr<STextBlock> MoneyTB;
    TSharedPtr<STextBlock> RatingTB;
    TSharedPtr<STextBlock> MassTB;
    TSharedPtr<SBar> RatingBar;
    TSharedPtr<SBar> MassBar;

    TSharedPtr<SVerticalBox> ButtonsVB;

    TSharedPtr<SVerticalBox> BackLeftVB;
    TSharedPtr<SVerticalBox> BackRightVB;

    TSharedPtr<SVerticalBox> ThemesVB;
    TSharedPtr<SVerticalBox> JournalVB;
    TSharedPtr<SVerticalBox> GliderVB;
    TSharedPtr<SVerticalBox> GliderStoreVB;
    TSharedPtr<SVerticalBox> HoldStoreVB;
    TSharedPtr<SVerticalBox> GoodsStoreVB;

    TSharedPtr<InfoTreeView> ThemesTV;
    TSharedPtr<InfoTreeView> JournalTV;
    TSharedPtr<InfoTreeView> GliderTV;
    TSharedPtr<InfoTreeView> GliderStoreTV;
    TSharedPtr<InfoTreeView> HoldStoreTV;
    TSharedPtr<InfoTreeView> GoodsStoreTV;

    std::vector<TSharedPtr<class SMenuButton>> LeftMenuButtons;

    FReply OnSave();
    FReply OnJournal();
    FReply OnGlider();
    FReply OnTrade();
    FReply OnBack();
    FReply OnExit();
    FReply DoNothing() const { return FReply::Handled(); }

    FReply OnSaveSave();
    FReply OnSaveBack();
    FReply OnSaveDelete();

    auto BottomText(FText Name, TSharedPtr<STextBlock> &Var) const;
    auto BottomText(FText Name, TSharedPtr<STextBlock> &Var, TSharedPtr<SBar> &Bar) const;

    void OnHyperlinkClick(const FSlateHyperlinkRun::FMetadata &map);
    FSlateWidgetRun::FWidgetRunInfo EditWidgetDecorator(const FTextRunInfo& RunInfo, const ISlateStyle* Style) const;
};
