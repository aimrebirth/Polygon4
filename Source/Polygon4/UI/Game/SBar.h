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

class SBar : public SCompoundWidget
{
    using value_type = float;

public:
    typedef SCompoundWidget ParentType;

private:
    SLATE_BEGIN_ARGS(SBar)
    {
    }
    SLATE_ARGUMENT(value_type, Max)
    SLATE_ARGUMENT(value_type, Current)
    SLATE_ARGUMENT(FText, Text)
    SLATE_ARGUMENT(FLinearColor, Color)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                          const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    void SetCurrentValue(value_type value)
    {
        Current = value;
    }
    void SetMaxValue(value_type value)
    {
        Max = value;
    }

private:
    value_type Max;
    value_type Current;
    FText Text;
    FLinearColor Color;
};
