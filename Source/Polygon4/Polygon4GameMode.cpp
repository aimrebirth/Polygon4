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

#include "Glider.h"
#include "GliderHUD.h"

#include "UI/Menu/PauseMenu.h"

#include <Polygon4/API.h>

APolygon4GameMode::APolygon4GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::APolygon4GameMode()"));

    DefaultPawnClass = AGlider::StaticClass();
    HUDClass = AGliderHUD::StaticClass();
}

APolygon4GameMode::~APolygon4GameMode()
{
    UNREGISTER_API(SpawnMechanoid);

    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::~APolygon4GameMode()"));
}

void APolygon4GameMode::BeginPlay()
{
    Super::BeginPlay();

    FSlateApplication::Get().SetAllUserFocusToGameViewport();
    
    auto InputComponent = GetWorld()->GetFirstPlayerController()->InputComponent;
    if (InputComponent)
    {
        InputComponent->BindAction("Exit", IE_Pressed, this, &APolygon4GameMode::ShowMenu).bExecuteWhenPaused = true;
        InputComponent->BindAction("Pause", IE_Pressed, this, &APolygon4GameMode::ShowMenu).bExecuteWhenPaused = true;
    }

    REGISTER_API(SpawnStaticObjects, std::bind(&APolygon4GameMode::SpawnStaticObjects, this, std::placeholders::_1));
    REGISTER_API(SpawnMechanoid, std::bind(&APolygon4GameMode::SpawnMechanoid, this, std::placeholders::_1));

    API_CALL(OnOpenLevel, GetWorld()->GetCurrentLevel()->URL.ToString().GetCharArray().GetData());

    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::BeginPlay()"));
}

void APolygon4GameMode::ShowMenu()
{
    paused = !paused;
    GetWorld()->GetFirstPlayerController()->SetPause(paused);
    GetWorld()->GetFirstPlayerController()->bShowMouseCursor = paused;
    if (paused)
    {
        if (!PauseMenu.Get())
        {
            PauseMenu = SNew(SPauseMenu)
                .PlayerController(GetWorld()->GetFirstPlayerController())
                .GameMode(this)
                ;
        }
        if (GEngine->IsValidLowLevel())
        {
            GEngine->GameViewport->AddViewportWidgetContent(PauseMenu.ToSharedRef());
        }
        PauseMenu->SetVisibility(EVisibility::Visible);
        //FSlateApplication::Get().SetKeyboardFocus(PauseMenu);
        //FSlateApplication::Get().SetAllUserFocusToGameViewport();
    }
    else
    {
        PauseMenu->SetVisibility(EVisibility::Hidden);
        //FSlateApplication::Get().SetAllUserFocusToGameViewport();
    }

    UE_LOG(LogTemp, Warning, TEXT("APolygon4GameMode::ShowMenu()"));
}

void APolygon4GameMode::SpawnMechanoid(polygon4::Ptr<polygon4::detail::Mechanoid> mechanoid)
{
    FVector pos(mechanoid->coordinate->x, mechanoid->coordinate->y, mechanoid->coordinate->z);
    FRotator rot(mechanoid->coordinate->pitch, mechanoid->coordinate->yaw, mechanoid->coordinate->roll);
    GetWorld()->SpawnActor<AGlider>(pos, rot);
}

void APolygon4GameMode::SpawnStaticObjects(polygon4::Ptr<polygon4::detail::Map> map)
{
    for (auto &building : map->buildings)
    {
        auto c = StaticLoadClass(AActor::StaticClass(), 0, TEXT("Class'/Game/Mods/Common/Objects/Dummy/Dummy.Dummy_C'"));
        FVector pos(
            building->coordinate->x * map->x_k + map->x_b,
            building->coordinate->y * map->y_k + map->y_b,
            building->coordinate->z * map->z_k + map->z_b
            );
        FRotator rot(0, 0, 0);
        GetWorld()->SpawnActor<AActor>(c, pos, rot);
    }
    for (auto &object : map->objects)
    {
        auto c = StaticLoadClass(AActor::StaticClass(), 0, TEXT("Class'/Game/Mods/Common/Objects/Dummy/Dummy.Dummy_C'"));
        FVector pos(
            object->coordinate->x * map->x_k + map->x_b,
            object->coordinate->y * map->y_k + map->y_b,
            object->coordinate->z * map->z_k + map->z_b
            );
        FRotator rot(0, 0, 0);
        GetWorld()->SpawnActor<AActor>(c, pos, rot);
    }
}
