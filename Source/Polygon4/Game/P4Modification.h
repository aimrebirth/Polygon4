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

#include "Windows/WindowsHWrapper.h"
#include <Polygon4/Modification.h>
#include "Windows/WindowsHWrapper.h"

struct ModificationDesc
{
    FString Name;
    FString Dir;
    FString Author;
    FString Version;
    FString DateCreated;
    FString DateModified;

    polygon4::detail::Modification* modification;
};

class P4Modification : public polygon4::Modification
{
    using Base = polygon4::Modification;

public:
    P4Modification(const polygon4::detail::Modification &rhs);
};
