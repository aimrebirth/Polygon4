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

#include "P4Mechanoid.h"
#include "Polygon4.h"

#include "P4GameInstance.h"
#include "P4Engine.h"

#include "P4Glider.h"

#include <string>

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
        if (auto pc = GP4Engine()->GetWorld()->GetFirstPlayerController())
            pc->Possess(g);
    }

    return true;
}

AP4Glider* spawn(polygon4::detail::Mechanoid *M, UWorld *W)
{
    FTransform tr{
        { M->pitch, M->yaw, M->roll },
        { M->x, M->y, M->z }
    };

    auto conf = M->getConfiguration();
    auto glider = conf->glider;

    FString resource = glider->resource;
    if (resource.IsEmpty())
    {
        /*auto c = LoadClass(0);
        auto a = W->SpawnActor<AActor>(c, pos, FRotator(rot));
#if WITH_EDITOR
        a->SetFolderPath("Mechanoids");
        a->SetActorLabel(M->getName());
#endif*/
        return nullptr;
    }

    AP4Glider *g = nullptr;
    auto o = LoadObject<UStaticMesh>(0, resource.GetCharArray().GetData());
    if (o)
    {
        g = W->SpawnActorDeferred<AP4Glider>(AP4Glider::StaticClass(), tr);
        if (!g)
        {
            // place is busy
            auto t = tr.GetTranslation();
            auto z0 = t.Z;
            auto z1 = z0 + 10000;
            while (!g && t.Z < z1)
            {
                t.Z += 250;
                tr.SetTranslation(t);
                g = W->SpawnActorDeferred<AP4Glider>(AP4Glider::StaticClass(), tr);
            }
            if (!g)
                return g;
        }

        g->SetStaticMesh(o);
        FVector new_scale(glider->scale * 3, glider->scale * 3, glider->scale * 3);
        g->SetActorScale3D(new_scale);
    }

    int light_id = 0;
    int heavy_id = 0;
    for (auto &nw : conf->getWeapons())
    {
        auto w = (polygon4::detail::Weapon*)nw.second;
        if (!w->resource.empty())
        {
            auto r = LoadObject<UStaticMesh>(0, w->resource);
            if (r)
            {
                FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, true);
                /*rules.LocationRule = EAttachmentRule::SnapToTarget;
                rules.RotationRule = EAttachmentRule::KeepRelative;
                rules.ScaleRule = EAttachmentRule::SnapToTarget;*/

                {
                    auto w1 = W->SpawnActorDeferred<AActor>(AActor::StaticClass(), {});
                    auto r1 = NewObject<UStaticMeshComponent>(w1, TEXT("weapon"));
                    r1->SetStaticMesh(r);
                    r1->SetSimulatePhysics(false);
                    r1->SetVisibility(true);
                    w1->SetRootComponent(r1);
                    if (w->type == polygon4::detail::WeaponType::Light)
                        w1->AttachToActor(g, rules, ("WeaponLight_" + std::to_string(light_id++)).c_str());
                    else if (w->type == polygon4::detail::WeaponType::Heavy)
                        w1->AttachToActor(g, rules, ("WeaponHeavy_" + std::to_string(heavy_id++)).c_str());
                    w1->FinishSpawning({});
                    w1->SetActorHiddenInGame(false);
#if WITH_EDITOR
                    w1->SetActorLabel(w->getName());
#endif
                }

                if (w->type == polygon4::detail::WeaponType::Light)
                {
                    auto w1 = W->SpawnActorDeferred<AActor>(AActor::StaticClass(), {});
                    auto r1 = NewObject<UStaticMeshComponent>(w1, TEXT("weapon"));
                    r1->SetStaticMesh(r);
                    r1->SetSimulatePhysics(false);
                    r1->SetVisibility(true);
                    r1->SetRelativeScale3D({ 1,-1,-1 });
                    w1->SetRootComponent(r1);
                    w1->AttachToActor(g, rules, ("WeaponLight_" + std::to_string(light_id++)).c_str());
                    w1->FinishSpawning({});
#if WITH_EDITOR
                    w1->SetActorLabel(w->getName());
#endif
                }
            }
        }
    }
    g->FinishSpawning(tr);

    g->SetMechanoid(M);
    g->Data.TextID = M->text_id.toFString();
#if WITH_EDITOR
    g->SetFolderPath("Mechanoids");
    g->SetActorLabel(M->getName());
#endif
    return g;
}
