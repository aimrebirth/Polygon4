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

#include "Windows/PreWindowsApi.h"
#include <Polygon4/Hotpatch.h>
#include <primitives/executor.h>

#include <fstream>
#include "Windows/PostWindowsApi.h"

#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "GameFramework/Actor.h"

UFont *RobotoFont;

class FPolygon4ModuleImpl : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
        FDefaultGameModuleImpl::StartupModule();

#if !IS_MONOLITHIC
#ifdef WIN32
        {
            auto fn = polygon4::read_orig_module_filename_store();
            std::ofstream ofile(fn);
            if (ofile)
                ofile << "Engine.dll";
        }
        {
            auto gd = FPaths::ProjectDir();
            auto fgd = FPaths::ConvertRelativePathToFull(gd);
            FPaths::CollapseRelativeDirectories(fgd);
            //polygon4::write_module_last_write_time(*fgd, "Engine");
        }
#endif
#endif
    }

	virtual void ShutdownModule() override
	{
        FDefaultGameModuleImpl::ShutdownModule();
	}
};

UClass* LoadClass(const TCHAR* Name)
{
    if (Name == 0 || FString(Name).IsEmpty())
        Name = DUMMY_OBJECT;
    auto c = StaticLoadClass(AActor::StaticClass(), 0, Name);
    if (c)
        return c;
    return StaticLoadClass(AActor::StaticClass(), 0, DUMMY_OBJECT);
}

IMPLEMENT_PRIMARY_GAME_MODULE( FPolygon4ModuleImpl, Polygon4, "Polygon4" );

DEFINE_LOG_CATEGORY(Polygon4);
