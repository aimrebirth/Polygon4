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

#include "P4Engine.h"

#include "P4Modification.h"
#include <P4GameInstance.h>

#include "P4Map.h"
#include "P4Mechanoid.h"
#include "P4MapObject.h"
#include "P4MapBuilding.h"

#include <UI/Menu/MainMenu.h>
#include <UI/Menu/BuildingMenu.h>
#include <UI/Menu/PauseMenu.h>

#include <Polygon4/Configuration.h>
#include <Polygon4/ConfigurationWeapon.h>
#include <Polygon4/Glider.h>

#include <Game/GliderHUD.h>

#include "Landscape.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"

#define DEFINE_MENU(name)                                                           \
    TSharedPtr<S##name##Menu> P4Engine::Get##name##Menu()                           \
    {                                                                               \
        return StaticCastSharedPtr<S##name##Menu>(GetMenu(MenuType::##name##Menu)); \
    }                                                                               \
                                                                                    \
    void P4Engine::Show##name##Menu()                                               \
    {                                                                               \
        ShowMenu(MenuType::##name##Menu);                                           \
    }                                                                               \
                                                                                    \
    void P4Engine::Hide##name##Menu()                                               \
    {                                                                               \
        HideMenu(MenuType::##name##Menu);                                           \
    }                                                                               \
                                                                                    \
    void P4Engine::Set##name##MenuVisibility(bool Visibility)                       \
    {                                                                               \
        SetMenuVisibility(MenuType::##name##Menu, Visibility);                      \
    }

P4Engine *GP4Engine(P4Engine *Engine)
{
    static P4Engine *P4Engine = nullptr;
    if (Engine)
        P4Engine = Engine;
    return P4Engine;
}

P4Engine::P4Engine(const FString &modificationsDirectory, UP4GameInstance *P4GameInstance)
    : Base(modificationsDirectory.GetCharArray().GetData()), P4GameInstance(P4GameInstance)
{
    // set engine pointers
    polygon4::getEngine(this);
    GP4Engine(this);

    Menus.AddDefaulted(static_cast<int>(MenuType::Max));
    DummyObject = (UDummyObject*)calloc(1, sizeof(UDummyObject));
}

P4Engine::~P4Engine()
{
    free(DummyObject);
}

void P4Engine::initChildren()
{
    if (!storage)
        return;

#define REPLACE(a, t) \
    for (auto &v : storage->a) \
        v.second->replace<t>(v.second)

    REPLACE(modifications, P4Modification);
    REPLACE(maps, P4Map);
    REPLACE(mechanoids, P4Mechanoid);
    REPLACE(mapObjects, P4MapObject);
    REPLACE(mapBuildings, P4MapBuilding);
    REPLACE(gliders, polygon4::Glider);
    REPLACE(configurations, polygon4::Configuration);
    REPLACE(configurationWeapons, polygon4::ConfigurationWeapon);
}

TArray<TSharedPtr<struct ModificationDesc>> P4Engine::GetModificationDescriptors() const
{
    TArray<TSharedPtr<struct ModificationDesc>> mods;
    if (!storage)
        return mods;
    for (auto &modification : storage->modifications)
    {
        auto &m = modification.second;
        auto d = TSharedPtr<ModificationDesc>(new ModificationDesc);
        d->Name = m->getName().toFString();
        d->Dir = m->directory.toFString();
        d->Author = m->author.toFString();
        d->Version = m->version.toFString();
        d->DateCreated = m->date_created.toFString();
        d->DateModified = m->date_modified.toFString();
        d->modification = m;
        mods.Add(d);
    }
    return mods;
}

TSharedPtr<SMenu> P4Engine::GetMenu(MenuType Type)
{
    auto &Menu = Menus[static_cast<int>(Type)];
    if (Menu.IsValid())
        return Menu;
    switch (Type)
    {
    case MenuType::MainMenu:
        Menu = SNew(SMainMenu);
        break;
    case MenuType::BuildingMenu:
        Menu = SNew(SBuildingMenu);
        break;
    case MenuType::PauseMenu:
        Menu = SNew(SPauseMenu);
        break;
    }
    return Menu;
}

void P4Engine::DestroyMenu(MenuType Type)
{
    auto &Menu = Menus[static_cast<int>(Type)];
    Menu.Reset();
}

void P4Engine::ShowMenu(MenuType Type)
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD()))
            HUD->SetVisible(false);
    }

    for (auto &m : Menus)
    {
        if (m.IsValid())
        {
            m->SetVisibility(EVisibility::Hidden);
            m->OnHide();
        }
    }

    auto m = GetMenu(Type);
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->AddViewportWidgetContent(m.ToSharedRef());
    }
    m->SetVisibility(EVisibility::Visible);
    m->OnShow();
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
        PlayerController->bShowMouseCursor = true;
    FSlateApplication::Get().ReleaseMouseCapture();
}

void P4Engine::HideMenu(MenuType Type)
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD()))
            HUD->SetVisible(true);
    }

    for (auto &m : Menus)
    {
        if (m.IsValid())
        {
            m->SetVisibility(EVisibility::Hidden);
            m->OnHide();
        }
    }
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
        PlayerController->bShowMouseCursor = false;
    FSlateApplication::Get().SetAllUserFocusToGameViewport();
}

void P4Engine::SetMenuVisibility(MenuType Type, bool Visibility)
{
    auto m = GetMenu(Type);
    m->SetVisibility(Visibility ? EVisibility::Visible : EVisibility::Hidden);
}

DEFINE_MENU(Main)
DEFINE_MENU(Building)
DEFINE_MENU(Pause)

void P4Engine::OnLevelLoaded()
{
    if (!IsStarted())
        return;

    SetPauseMenuBindings();

    TActorIterator<ALandscape> landscapeIterator(GetWorld());
    ALandscape* landscape = *landscapeIterator;
    SetWorldScale(landscape->GetActorScale() / 100.0f);
    LoadLevelObjects();
}

UWorld* P4Engine::GetWorld() const
{
    return P4GameInstance->GetWorld();
}

void P4Engine::Exit()
{
    if (auto c = GetWorld()->GetFirstPlayerController())
        c->ConsoleCommand("quit");
}

void P4Engine::SetPauseMenuBindings()
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
    {
        auto PlayerInputComponent = PlayerController->InputComponent;
        if (PlayerInputComponent)
        {
            int n_binds = PlayerInputComponent->GetNumActionBindings();
            bool exit = false, pause = false;
            for (auto i = 0; i < n_binds; ++i)
            {
                auto &b = PlayerInputComponent->GetActionBinding(i);
                if (b.GetActionName() == "Exit")
                    exit = true;
                else if (b.GetActionName() == "Pause")
                    pause = true;
            }
            if (!exit)
                PlayerInputComponent->BindAction("Exit", IE_Pressed, DummyObject, &UDummyObject::ShowPauseMenuFromBinding).bExecuteWhenPaused = true;
            if (!pause)
                PlayerInputComponent->BindAction("Pause", IE_Pressed, DummyObject, &UDummyObject::ShowPauseMenuFromBinding).bExecuteWhenPaused = true;
        }
    }
}

void P4Engine::UnsetPauseMenuBindings() const
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
    {
        auto PlayerInputComponent = PlayerController->InputComponent;
        if (PlayerInputComponent)
        {
            int n_binds = PlayerInputComponent->GetNumActionBindings();
            for (auto i = n_binds - 1; i >= 0; --i)
            {
                auto &b = PlayerInputComponent->GetActionBinding(i);
                if (b.GetActionName() == "Exit" || b.GetActionName() == "Pause")
                {
                    PlayerInputComponent->RemoveActionBinding(i);
                }
            }
        }
    }
}

void P4Engine::ShowPauseMenuFromBinding()
{
    DummyObject->ShowPauseMenuFromBinding();
}

void P4Engine::ReturnToMainMenu()
{
    auto world = GetWorld();
    world->ServerTravel("/Game/Mods/Common/Maps/start");
    reloadMods();
    GetMainMenu()->ReloadMods();
}

polygon4::BuildingMenu *P4Engine::getBuildingMenu()
{
    return GetBuildingMenu().Get();
}

void P4Engine::DestroyBuildingMenu()
{
    DestroyMenu(MenuType::BuildingMenu);
}

void UDummyObject::ShowPauseMenuFromBinding()
{
    GP4Engine()->paused = !GP4Engine()->paused;

    if (auto PlayerController = GP4Engine()->GetWorld()->GetFirstPlayerController())
        PlayerController->SetPause(GP4Engine()->paused);

    if (GP4Engine()->paused)
        GP4Engine()->ShowPauseMenu();
    else
        GP4Engine()->HidePauseMenu();
}
