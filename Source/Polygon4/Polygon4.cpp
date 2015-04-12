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

#include <fstream>

#include <Polygon4/Hotpatch.h>

#include "Common.h"

class FPolygon4ModuleImpl : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
        FDefaultGameModuleImpl::StartupModule();

#ifdef WIN32
        {
            std::string fn = polygon4::read_orig_module_filename_store();
            std::ofstream ofile(fn);
            if (ofile)
                ofile << "Engine.x64.dll";
        }
        {
            std::string fn = polygon4::read_ver_module_filename_store();
            std::ofstream ofile(fn);
            if (ofile)
                ofile << -1;
        }
#endif
    }

	virtual void ShutdownModule() override
	{
        FDefaultGameModuleImpl::ShutdownModule();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FPolygon4ModuleImpl, Polygon4, "Polygon4" );

