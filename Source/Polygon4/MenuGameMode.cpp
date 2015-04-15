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
#include "MenuGameMode.h"

#include <Polygon4/Hotpatch.h>

#include "Common.h"
#include "Menu/MainMenu.h"

static void HotpatchEngine()
{
    UE_LOG(LogTemp, Warning, TEXT("UPolygon4BlueprintFunctionLibrary::HotpatchEngine()"));
#ifdef WIN32
    auto gd = FPaths::GameDir();
    auto fgd = FPaths::ConvertRelativePathToFull(gd);
    FPaths::CollapseRelativeDirectories(fgd);
    auto dll = polygon4::prepare_module_for_hotload(*fgd, "Engine").wstring();
    if (!dll.empty() && !LoadLibrary(dll.c_str()))
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadLibrary(%s) is failed!"), dll.c_str());
    }
#endif
}

void AMenuGameMode::ShowMainMenu()
{
    if (!MainMenu.Get())
    {
        MainMenu = SNew(SMainMenu).PlayerController(GetWorld()->GetFirstPlayerController());
    }
    if (GEngine->IsValidLowLevel())
    {
        GEngine->GameViewport->AddViewportWidgetContent(MainMenu.ToSharedRef());
    }
    MainMenu->SetVisibility(EVisibility::Visible);
}

void AMenuGameMode::OpenLevel(std::string level)
{
    MainMenu->SetVisibility(EVisibility::Hidden);
    GetWorld()->ServerTravel(level.c_str());
}

void AMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AMenuGameMode::BeginPlay()"));

#ifdef WITH_EDITOR
    HotpatchEngine();
#endif
    
    REGISTER_API(OpenLevel, std::bind(&AMenuGameMode::OpenLevel, this, std::placeholders::_1));

    ShowMainMenu();
}

AMenuGameMode::~AMenuGameMode()
{
    UNREGISTER_API(OpenLevel);

    UE_LOG(LogTemp, Warning, TEXT("AMenuGameMode::~AMenuGameMode()"));
}




