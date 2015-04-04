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

#include "Engine.h"

#include <codecvt>
#include <locale>
#include <string>

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

inline std::wstring to_wstring(const char *s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s);
}

inline std::wstring to_wstring(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(s.c_str());
}

inline std::string to_string(const std::wstring &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(s.c_str());
}