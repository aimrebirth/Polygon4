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

#include <Polygon4/BuildingMenu.h>

#include "Menu.h"

namespace polygon4 {
    namespace detail {
        class ModificationMapBuilding;
    }
}

class InfoTreeView;

class TextDecorator : public ITextDecorator
{
public:
    TextDecorator(const FTextBlockStyle &DefaultStyle);
    virtual ~TextDecorator() {}

    virtual bool Supports(const FTextRunParseResults& RunInfo, const FString& Text) const override;
    virtual TSharedRef<ISlateRun> Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style) override;

protected:
    virtual TSharedRef<ISlateRun> CreateRun(const TSharedRef<FTextLayout>& TextLayout, const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange);
    FTextBlockStyle CreateTextBlockStyle(const FRunInfo& InRunInfo) const;
    void ExplodeRunInfo(const FRunInfo& InRunInfo, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const;

private:
    FTextBlockStyle DefaultStyle;
};

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

    SVerticalBox::FSlot& BottomText(FText Name, TSharedPtr<STextBlock> &Var) const;

    void OnHyperlinkClick(const FSlateHyperlinkRun::FMetadata &map);
    FSlateWidgetRun::FWidgetRunInfo WidgetDecorator(const FTextRunInfo& RunInfo, const ISlateStyle* Style) const;
};
