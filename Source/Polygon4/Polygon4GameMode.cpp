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
#include "Polygon4GameMode.h"

APolygon4GameMode::~APolygon4GameMode()
{
    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::~APolygon4GameMode()"));
}

void APolygon4GameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::BeginPlay()"));

    //InputComponent->BindAction("Exit", IE_Pressed, this, &APolygon4GameMode::ShowMenu);
    //InputComponent->BindAction("Pause", IE_Pressed, this, &APolygon4GameMode::ShowMenu);
}

void APolygon4GameMode::ShowMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::ShowMenu()"));
}
