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

#include "SBar.h"
#include "Polygon4.h"


void SBar::Construct(const FArguments& InArgs)
{
    Max = InArgs._Max;
    Current = InArgs._Current;
    Text = InArgs._Text;
    Color = InArgs._Color;
}

int32 SBar::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    // Initialize our brush here
    const FSlateBrush* BrushResource = new FSlateBrush();

    // Draw background box
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        BrushResource,
        ESlateDrawEffect::None,
        FLinearColor::Gray * 0.35f
        );

    // Calculate Current / Max ratio and turn it into screen width
    FVector2D WidgetSize = AllottedGeometry.Size;
    float HealthWidth = (Current * WidgetSize.X) / Max;

    // Draw current health
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2D::ZeroVector, FVector2D(HealthWidth, WidgetSize.Y)),
        BrushResource,
        ESlateDrawEffect::None,
        Color
        );

    // Draw text on health bar
    FSlateFontInfo MyFont(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 15);

    const FText TextNew = FText::FromString(FString::Printf(TEXT("%s: %.0f / %.0f"), Text.ToString().GetCharArray().GetData(), Current, Max));
    const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    FVector2D DrawSize = FontMeasureService->Measure(TextNew, MyFont);
    FVector2D Pos = WidgetSize / 2.0f - DrawSize / 2.0f;

    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(Pos, DrawSize),
        TextNew,
        MyFont,
        ESlateDrawEffect::None,
        FLinearColor::White
        );

    return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
