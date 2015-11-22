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
#include "P4Map.h"

#include "P4GameInstance.h"
#include "P4Engine.h"

#include "P4MapBuilding.h"
#include "P4MapObject.h"

P4Map::P4Map(const polygon4::detail::Map &rhs)
    : Base(rhs)
{
}

void P4Map::initChildren()
{
    initObjects<P4MapObject>();
    initBuildings<P4MapBuilding>();
    //initGoods<>();
}

bool P4Map::loadLevel()
{
    auto world = GP4Engine->GetWorld();
    world->ServerTravel(to_string(resource).c_str());
    return true;
}
