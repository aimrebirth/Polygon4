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

#include "Windows/PreWindowsApi.h"
#include <Polygon4/Map.h>
#include "Windows/PostWindowsApi.h"

class P4Map : public polygon4::Map
{
    using Base = polygon4::Map;

public:
    P4Map(const polygon4::detail::Map &rhs);

protected:
    virtual bool loadLevel() override final;
    virtual bool spawnMechanoid(polygon4::detail::Mechanoid *) override final;
};
