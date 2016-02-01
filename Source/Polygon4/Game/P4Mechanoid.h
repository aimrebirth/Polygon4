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

#include <Polygon4/Mechanoid.h>

class AP4Glider;

class P4Mechanoid : public polygon4::Mechanoid
{
    using Base = polygon4::Mechanoid;

public:
    P4Mechanoid(const polygon4::detail::Mechanoid &rhs);

    virtual bool spawn() override;
};

POLYGON4_API
AP4Glider* spawn(polygon4::detail::Mechanoid *M, UWorld *W);
