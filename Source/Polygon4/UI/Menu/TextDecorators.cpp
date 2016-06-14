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

#include "Polygon4.h"
#include "TextDecorators.h"

#include <UI/Colors.h>

TextDecorator::TextDecorator(const FTextBlockStyle &DefaultStyle)
    : DefaultStyle(DefaultStyle)
{
}

bool TextDecorator::Supports(const FTextRunParseResults& RunInfo, const FString& Text) const
{
    return
        RunInfo.Name == TEXT("span") ||
        0;
}

TSharedRef<ISlateRun> TextDecorator::Create(const TSharedRef<FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef<FString>& InOutModelText, const ISlateStyle* Style)
{
    FRunInfo RunInfo(RunParseResult.Name);
    for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
    {
        RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    return CreateRun(TextLayout, RunInfo, InOutModelText, CreateTextBlockStyle(RunInfo), ModelRange);
}

TSharedRef<ISlateRun> TextDecorator::CreateRun(const TSharedRef<FTextLayout>& TextLayout, const FRunInfo& InRunInfo, const TSharedRef< const FString >& InText, const FTextBlockStyle& Style, const FTextRange& InRange)
{
    return FSlateTextRun::Create(InRunInfo, InText, Style, InRange);
}

FTextBlockStyle TextDecorator::CreateTextBlockStyle(const FRunInfo& InRunInfo) const
{
    FSlateFontInfo Font;
    FLinearColor FontColor;
    ExplodeRunInfo(InRunInfo, Font, FontColor);

    FTextBlockStyle TextBlockStyle;
    TextBlockStyle.SetFont(Font);
    TextBlockStyle.SetColorAndOpacity(FontColor);

    return TextBlockStyle;
}

void TextDecorator::ExplodeRunInfo(const FRunInfo& InRunInfo, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const
{
    OutFont = DefaultStyle.Font;

    const FString* const FontFamilyString = InRunInfo.MetaData.Find(TEXT("font"));
    const FString* const FontSizeString = InRunInfo.MetaData.Find(TEXT("size"));
    const FString* const FontStyleString = InRunInfo.MetaData.Find(TEXT("style"));
    const FString* const FontColorString = InRunInfo.MetaData.Find(TEXT("color"));
    const FString* const ObjString = InRunInfo.MetaData.Find(TEXT("item"));
    const FString* const BldString = InRunInfo.MetaData.Find(TEXT("building"));

    if (FontFamilyString)
    {
        FStringAssetReference Font(**FontFamilyString);
        if (UObject* FontAsset = Font.TryLoad())
        {
            OutFont.FontObject = FontAsset;
        }
    }

    if (FontSizeString)
    {
        OutFont.Size = static_cast<uint8>(FPlatformString::Atoi(**FontSizeString));
    }

    if (FontStyleString)
    {
        OutFont.TypefaceFontName = FName(**FontStyleString);
    }

    OutFontColor = DefaultStyle.ColorAndOpacity.GetSpecifiedColor();
    if (FontColorString)
    {
        auto FontColorStringRef = FontColorString->ToLower();

        // Is Hex color?
        if (!FontColorStringRef.IsEmpty() && FontColorStringRef[0] == TCHAR('#'))
        {
            OutFontColor = FLinearColor(FColor::FromHex(FontColorStringRef));
        }
#define CHANGE_COLOR(c, cc) else if (FontColorStringRef == L ## #c) { OutFontColor = cc; }
        CHANGE_COLOR(green, P4_COLOR_GREEN)
            CHANGE_COLOR(blue, P4_COLOR_BLUE)
            CHANGE_COLOR(red, P4_COLOR_RED)
            CHANGE_COLOR(yellow, P4_COLOR_YELLOW)
            // add more text colors here
    }
    else
    {
        if (ObjString || BldString)
            OutFontColor = P4_COLOR_BLUE;
    }
}


TSharedRef< HyperlinkDecorator > HyperlinkDecorator::Create(FString Id, const FTextBlockStyle &DefaultStyle, const FSlateHyperlinkRun::FOnClick& NavigateDelegate, const FSlateHyperlinkRun::FOnGetTooltipText& InToolTipTextDelegate, const FSlateHyperlinkRun::FOnGenerateTooltip& InToolTipDelegate)
{
    return MakeShareable(new HyperlinkDecorator(Id, DefaultStyle, NavigateDelegate, InToolTipTextDelegate, InToolTipDelegate));
}

HyperlinkDecorator::HyperlinkDecorator(FString InId, const FTextBlockStyle &DefaultStyle, const FSlateHyperlinkRun::FOnClick& InNavigateDelegate, const FSlateHyperlinkRun::FOnGetTooltipText& InToolTipTextDelegate, const FSlateHyperlinkRun::FOnGenerateTooltip& InToolTipDelegate)
    : NavigateDelegate(InNavigateDelegate)
    , Id(InId)
    , ToolTipTextDelegate(InToolTipTextDelegate)
    , ToolTipDelegate(InToolTipDelegate)
    , DefaultStyle(DefaultStyle)
{

}

bool HyperlinkDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
    const FTextRange* const MetaDataIdRange = RunParseResult.MetaData.Find(TEXT("id"));
    FString MetaDataId;
    if (MetaDataIdRange)
    {
        MetaDataId = Text.Mid(MetaDataIdRange->BeginIndex, MetaDataIdRange->EndIndex - MetaDataIdRange->BeginIndex);
    }

    return (RunParseResult.Name == TEXT("a") && MetaDataId == Id);
}

TSharedRef< ISlateRun > HyperlinkDecorator::Create(const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style)
{
    FString StyleName = TEXT("Hyperlink");
    FString TextStyleName = TEXT("");

    const FTextRange* const MetaDataStyleNameRange = RunParseResult.MetaData.Find(TEXT("style"));
    if (MetaDataStyleNameRange != NULL)
    {
        const FString MetaDataStyleName = OriginalText.Mid(MetaDataStyleNameRange->BeginIndex, MetaDataStyleNameRange->EndIndex - MetaDataStyleNameRange->BeginIndex);
        StyleName = *MetaDataStyleName;
    }

    const FTextRange* const MetaDataTextStyleNameRange = RunParseResult.MetaData.Find(TEXT("textstyle"));
    if (MetaDataTextStyleNameRange != NULL)
    {
        const FString MetaDataTextStyleName = OriginalText.Mid(MetaDataTextStyleNameRange->BeginIndex, MetaDataTextStyleNameRange->EndIndex - MetaDataTextStyleNameRange->BeginIndex);
        TextStyleName = *MetaDataTextStyleName;
    }

    FTextRange ModelRange;
    ModelRange.BeginIndex = InOutModelText->Len();
    *InOutModelText += OriginalText.Mid(RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex);
    ModelRange.EndIndex = InOutModelText->Len();

    if (!Style->HasWidgetStyle<FHyperlinkStyle>(FName(*StyleName)))
    {
        Style = &FCoreStyle::Get();
    }

    FRunInfo RunInfo(RunParseResult.Name);
    for (const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData)
    {
        RunInfo.MetaData.Add(Pair.Key, OriginalText.Mid(Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex));
    }

    FHyperlinkStyle HyperlinkStyle = Style->GetWidgetStyle<FHyperlinkStyle>(FName(*StyleName));

    if (!TextStyleName.IsEmpty() && Style->HasWidgetStyle<FTextBlockStyle>(FName(*TextStyleName)))
    {
        HyperlinkStyle.SetTextStyle(Style->GetWidgetStyle<FTextBlockStyle>(FName(*TextStyleName)));
    }

    return FSlateHyperlinkRun::Create(RunInfo, InOutModelText, HyperlinkStyle, NavigateDelegate, ToolTipDelegate, ToolTipTextDelegate, ModelRange);
}
