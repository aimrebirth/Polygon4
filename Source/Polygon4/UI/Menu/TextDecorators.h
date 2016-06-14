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

class TextDecorator : public ITextDecorator
{
public:
    TextDecorator(const FTextBlockStyle &DefaultStyle);
    virtual ~TextDecorator() {}

    virtual bool Supports(const FTextRunParseResults &RunInfo, const FString &Text) const override;
    virtual TSharedRef<ISlateRun> Create(const TSharedRef<FTextLayout> &TextLayout, const FTextRunParseResults &RunParseResult, const FString &OriginalText, const TSharedRef<FString> &InOutModelText, const ISlateStyle *Style) override;

protected:
    virtual TSharedRef<ISlateRun> CreateRun(const TSharedRef<FTextLayout> &TextLayout, const FRunInfo &InRunInfo, const TSharedRef<const FString> &InText, const FTextBlockStyle &Style, const FTextRange &InRange);
    FTextBlockStyle CreateTextBlockStyle(const FRunInfo &InRunInfo) const;
    void ExplodeRunInfo(const FRunInfo &InRunInfo, FSlateFontInfo &OutFont, FLinearColor &OutFontColor) const;

private:
    FTextBlockStyle DefaultStyle;
};

class HyperlinkDecorator : public ITextDecorator
{
public:
    static TSharedRef<HyperlinkDecorator> Create(FString Id, const FTextBlockStyle &DefaultStyle, const FSlateHyperlinkRun::FOnClick &NavigateDelegate, const FSlateHyperlinkRun::FOnGetTooltipText &InToolTipTextDelegate = FSlateHyperlinkRun::FOnGetTooltipText(), const FSlateHyperlinkRun::FOnGenerateTooltip &InToolTipDelegate = FSlateHyperlinkRun::FOnGenerateTooltip());

public:
    virtual ~HyperlinkDecorator() {}

public:
    virtual bool Supports(const FTextRunParseResults &RunParseResult, const FString &Text) const override;
    virtual TSharedRef<ISlateRun> Create(const TSharedRef<class FTextLayout> &TextLayout, const FTextRunParseResults &RunParseResult, const FString &OriginalText, const TSharedRef<FString> &InOutModelText, const ISlateStyle *Style) override;

protected:
    HyperlinkDecorator(FString InId, const FTextBlockStyle &DefaultStyle, const FSlateHyperlinkRun::FOnClick &InNavigateDelegate, const FSlateHyperlinkRun::FOnGetTooltipText &InToolTipTextDelegate, const FSlateHyperlinkRun::FOnGenerateTooltip &InToolTipDelegate);

protected:
    FSlateHyperlinkRun::FOnClick NavigateDelegate;

protected:
    FString Id;
    FSlateHyperlinkRun::FOnGetTooltipText ToolTipTextDelegate;
    FSlateHyperlinkRun::FOnGenerateTooltip ToolTipDelegate;

private:
    FTextBlockStyle DefaultStyle;
};
