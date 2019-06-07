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

#include "GameFramework/HUD.h"
#include "GliderHUD.generated.h"

class SBar;

class AP4Glider;

UCLASS()
class POLYGON4_API AGliderHUD : public AHUD
{
    GENERATED_BODY()

public:
    AGliderHUD(const FObjectInitializer& ObjectInitializer);

    virtual void DrawHUD() override;

    void SetMousePosition(FVector2D Position)
    {
        MousePosition = Position;
    }
    void SetVisible(bool VisibleIn)
    {
        Visible = VisibleIn;
    }
    void SetCurrentGlider(AP4Glider* GliderIn)
    {
        Glider = GliderIn;
    }

private:
    UTexture2D* CrosshairTex;
    FVector2D MousePosition = {-1, -1};
    bool Visible = true;

    bool WidgetsDrawn = false;
    TSharedPtr<SWidget> BarsWidget;
    TSharedPtr<SBar> EnergyShieldBar;
    TSharedPtr<SBar> ArmorBar;
    TSharedPtr<SBar> EnergyBar;

    AP4Glider* Glider = nullptr;
};
