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

#include "P4GameInstance.h"

#include "Windows/PreWindowsApi.h"
#include <Polygon4/Hotpatch.h>
#include "Windows/PostWindowsApi.h"

#include "Game/P4Engine.h"
#include "UI/Menu/MainMenu.h"

#include "Windows/PreWindowsApi.h"
#include <Polygon4/Memory.h>
#include "Windows/PostWindowsApi.h"
//POLYGON4_UNREAL_MEMORY_STUB

static void HotpatchEngine()
{
#ifdef WIN32
    UE_LOG(LogTemp, Warning, TEXT("UP4GameInstance::HotpatchEngine()"));
    auto gd = FPaths::ProjectDir();
    auto fgd = FPaths::ConvertRelativePathToFull(gd);
    FPaths::CollapseRelativeDirectories(fgd);
    auto dll = polygon4::prepare_module_for_hotload(*fgd, "Engine");
    if (!dll.empty() && !Windows::LoadLibraryW(dll.wstring().c_str()))
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadLibrary(%s) is failed!"), dll.c_str());
    }
#endif
}

void UP4GameInstance::Init()
{
#if !IS_MONOLITHIC
    //HotpatchEngine();
#endif

    Engine = P4Engine::create<P4Engine>(FPaths::ProjectDir(), this);

    Super::Init();

    // do some global loads
    RobotoFont = LoadObjFromPath<UFont>(TEXT("/Engine/EngineFonts/Roboto"));
}

void UP4GameInstance::Shutdown()
{
    Super::Shutdown();
}

void UP4GameInstance::StartGameInstance()
{
    Super::StartGameInstance();
}
