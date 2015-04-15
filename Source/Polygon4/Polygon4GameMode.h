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

#include "GameFramework/GameMode.h"

#include <Polygon4/API.h>

#include "Polygon4GameMode.generated.h"

/**
 * 
 */
UCLASS()
class POLYGON4_API APolygon4GameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	APolygon4GameMode(const FObjectInitializer& ObjectInitializer);
    ~APolygon4GameMode();

public:
    void BeginPlay();
	
private:
    void ShowMenu();

private: /* API */
    void SpawnPlayer(polygon4::Vector v, polygon4::Rotation r);
};
