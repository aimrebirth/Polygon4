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
#include "P4GameMode.h"

#include "Game/P4Engine.h"

#include "Game/P4Glider.h"
#include "Game/GliderHUD.h"

AP4GameMode::AP4GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    DefaultPawnClass = AP4Glider::StaticClass();
    HUDClass = AGliderHUD::StaticClass();
}

void AP4GameMode::BeginPlay()
{
    GP4Engine->OnLevelLoaded();

    Super::BeginPlay();

    auto PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        auto Viewport = GetWorld()->GetGameViewport();
        FIntPoint ViewSize = Viewport->Viewport->GetSizeXY();
        FIntPoint Center = ViewSize / 2;
        Viewport->Viewport->SetMouse(Center.X, Center.Y);
    }
}
