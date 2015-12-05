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
#include "P4Engine.h"

#include "Landscape.h"

#include "P4Modification.h"
#include <P4GameInstance.h>

#include <UI/Menu/MainMenu.h>
#include <Game/GliderHUD.h>

P4Engine *GP4Engine;

P4Engine::P4Engine(const FString &modificationsDirectory, UP4GameInstance *P4GameInstance)
    : Base(modificationsDirectory.GetCharArray().GetData()), P4GameInstance(P4GameInstance)
{
    GP4Engine = this;
}

void P4Engine::initChildren()
{
    for (auto &modification : storage->modifications)
    {
        auto m = modification.second.get();
        m->replace<P4Modification>(m);
    }
}

TArray<TSharedPtr<struct ModificationDesc>> P4Engine::GetModificationDescriptors() const
{
    TArray<TSharedPtr<struct ModificationDesc>> mods;
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
        d->modification = m.get();
        mods.Add(d);
    }
    return mods;
}

TSharedPtr<SMainMenu> P4Engine::GetMainMenu()
{
    if (MainMenu.IsValid())
        return MainMenu;
    return MainMenu = SNew(SMainMenu)
        .PlayerController(GetWorld()->GetFirstPlayerController())
        .Engine(this)
        ;
}

void P4Engine::ShowMainMenu()
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
        if (auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD()))
            HUD->SetVisible(false);

    auto mm = GetMainMenu();
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->AddViewportWidgetContent(mm.ToSharedRef());
    }
    mm->SetVisibility(EVisibility::Visible);
}

void P4Engine::HideMainMenu()
{
    if (auto PlayerController = GetWorld()->GetFirstPlayerController())
        if (auto HUD = Cast<AGliderHUD>(PlayerController->GetHUD()))
            HUD->SetVisible(true);

    auto mm = GetMainMenu();
    mm->SetVisibility(EVisibility::Hidden);
}

void P4Engine::OnLevelLoaded()
{
    if (!IsStarted())
        return;
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
    auto world = GetWorld();
    auto c = world->GetFirstPlayerController();
    if (c)
        c->ConsoleCommand("quit");
}
