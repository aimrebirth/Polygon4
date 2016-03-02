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
#include "P4Mechanoid.h"

#include "P4GameInstance.h"
#include "P4Engine.h"

#include "P4Glider.h"

P4Mechanoid::P4Mechanoid(const polygon4::detail::Mechanoid &rhs)
    : Base(rhs)
{
}

bool P4Mechanoid::spawn()
{
    if (!enabled)
        return false;

    if (player && building)
    {
        enterBuilding(building->building);
        return false;
    }

    auto g = ::spawn(this, GP4Engine()->GetWorld());
    if (!g)
        return false;

    if (player)
    {
        auto pc = GP4Engine()->GetWorld()->GetFirstPlayerController();
        if (pc)
        {
            pc->Possess(g);
        }
    }

    return true;
}

AP4Glider* spawn(polygon4::detail::Mechanoid *M, UWorld *W)
{
    FString resource = M->getConfiguration()->glider->resource;
    auto c = StaticLoadClass(AP4Glider::StaticClass(), nullptr, resource.GetCharArray().GetData());
    if (!c)
    {
        auto name = FPaths::GetCleanFilename(resource);
        resource = "Class'" + resource + "." + name + "_C'";
        c = StaticLoadClass(AP4Glider::StaticClass(), nullptr, resource.GetCharArray().GetData());
    }
    if (!c)
        return nullptr;

    FVector loc{ M->x, M->y, M->z };
    FRotator rot{ M->pitch, M->yaw, M->roll };
    FVector scale(M->getConfiguration()->glider->scale, M->getConfiguration()->glider->scale, M->getConfiguration()->glider->scale);
    auto g = W->SpawnActor<AP4Glider>(c, loc, rot);
    if (!g)
    {
        // place is busy
        auto z0 = loc.Z;
        auto z1 = z0 + 10000;
        while (!g && loc.Z < z1)
        {
            loc.Z += 250;
            g = W->SpawnActor<AP4Glider>(c, loc, rot);
        }
        if (!g)
            return nullptr;
    }
    g->SetActorScale3D(scale);
    g->SetMechanoid(M);
    g->Data.TextID = M->text_id.toFString();
#if WITH_EDITOR
    g->SetActorLabel(g->Data.TextID);
#endif
    return g;
}
