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

#include "GliderHUD.h"

#include <UI/Game/SBar.h>

#include "P4Glider.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"

AGliderHUD::AGliderHUD(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("Texture2D'/Game/Mods/Common/HUD/crosshair.crosshair'"));
    CrosshairTex = CrosshairTexObj.Object;

    //"WidgetBlueprint'/Game/Mods/Common/HUD/WG_INT_HUD_M1.WG_INT_HUD_M1'";

    EnergyBar = SNew(SBar)
        .Max(100)
        .Current(100)
        .Text(FText::FromString("Energy"))
        .Color(FLinearColor::Yellow * 0.65f)
        ;
    EnergyShieldBar = SNew(SBar)
        .Max(100)
        .Current(10)
        .Text(FText::FromString("Shield"))
        .Color(FLinearColor::Blue * 0.65f)
        ;
    ArmorBar = SNew(SBar)
        .Max(100)
        .Current(50)
        .Text(FText::FromString("Armor"))
        .Color(FLinearColor::Green * 0.65f)
        ;
}

void AGliderHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Visible)
    {
        if (WidgetsDrawn)
        {
            WidgetsDrawn = false;

            auto Viewport = GetWorld()->GetGameViewport();
            if (auto PlayerController = GetWorld()->GetFirstPlayerController())
                Viewport->RemoveViewportWidgetForPlayer(PlayerController->GetLocalPlayer(), BarsWidget.ToSharedRef());
        }
        return;
    }

    decltype(MousePosition) Position(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
    if (MousePosition.X != -1)
        Position = MousePosition;

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Position.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Position.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->GetResource(), FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

    if (Glider)
    {
        auto m = Glider->GetMechanoid();
        auto c = m->getConfiguration();

        EnergyBar->SetMaxValue(c->getMaxEnergy());
        EnergyShieldBar->SetMaxValue(c->getMaxEnergyShield());
        ArmorBar->SetMaxValue(c->getMaxArmor());

        EnergyBar->SetCurrentValue(c->getCurrentEnergy());
        EnergyShieldBar->SetCurrentValue(c->getCurrentEnergyShield());
        ArmorBar->SetCurrentValue(c->getCurrentArmor());
    }

    // draw only once
    if (WidgetsDrawn)
        return;

    WidgetsDrawn = true;

    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
    {
        GetWorld()->GetGameViewport()->AddViewportWidgetForPlayer(PlayerController->GetLocalPlayer(),
            SAssignNew(BarsWidget, SVerticalBox)
            + SVerticalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            .AutoHeight()
            .Padding(30, 30, 10, 10)
            [
                SNew(SBox)
                .WidthOverride(500)
                .HeightOverride(40)
                [
                    EnergyBar.ToSharedRef()
                ]
            ]
            + SVerticalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            .AutoHeight()
            .Padding(30, 10, 10, 10)
            [
                SNew(SBox)
                .WidthOverride(500)
                .HeightOverride(40)
                [
                    EnergyShieldBar.ToSharedRef()
                ]
            ]
            + SVerticalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Top)
            .AutoHeight()
            .Padding(30, 10, 10, 10)
            [
                SNew(SBox)
                .WidthOverride(500)
                .HeightOverride(40)
                [
                    ArmorBar.ToSharedRef()
                ]
            ], 0
            );
    }
}
