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
#include "BuildingMenu.h"

#include "Widgets/MenuButton.h"

#include <Game/P4Engine.h>

#define LOCTEXT_NAMESPACE "BuildingMenu"

void SBuildingMenu::Construct(const FArguments& InArgs)
{
    auto PlayerController = GWorld->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->bShowMouseCursor = true;
    }
}

/*void SBuildingMenu::SetVisibility(TAttribute<EVisibility> InVisibility) override
{
    Super::SetVisibility(InVisibility);

    if (InVisibility == EVisibility::Visible)
    {
        auto PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            auto PlayerInputComponent = PlayerController->InputComponent;
            if (PlayerInputComponent)
            {
                //PlayerInputComponent->
                PlayerInputComponent->BindAction("Exit", IE_Pressed, this, &AP4GameMode::ShowMenu).bExecuteWhenPaused = true;
                PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AP4GameMode::ShowMenu).bExecuteWhenPaused = true;
            }
        }
    }
}
*/
