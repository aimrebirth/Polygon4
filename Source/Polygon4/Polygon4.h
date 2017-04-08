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

#include <memory>

#include "Engine.h"

#define DUMMY_OBJECT TEXT("Class'/Game/Mods/Common/Objects/Dummy/Dummy.Dummy_C'")
#define DUMMY_BUILDING DUMMY_OBJECT

extern UFont *RobotoFont;

template <typename T>
TSharedPtr<T> MakeTSharedPtr(const T &value)
{
    return MakeShareable(new T(value));
}

template <class T, class U>
TArray<U> MakeTArray(const T &stl_container)
{
    TArray<U> array;
    for (auto &v : stl_container)
        array.Add(v);
    return array;
}

template <class T>
TArray<TSharedPtr<typename T::value_type>> MakeTArrayTSharedPtr(const T &stl_container)
{
    TArray<TSharedPtr<typename T::value_type>> array;
    for (auto &v : stl_container)
        array.Add(MakeTSharedPtr(v));
    return array;
}

UClass* LoadClass(const TCHAR* Name);

template <typename ObjClass>
FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
    if (Path == NAME_None)
        return NULL;

    return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

DECLARE_LOG_CATEGORY_EXTERN(Polygon4, Log, All);
