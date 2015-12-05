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

#include "EngineMinimal.h"

#include <Polygon4/Engine.h>

class UP4GameInstance;
class SMainMenu;

enum class EP4EngineState
{
    None,
    Started,
};

class P4Engine : public polygon4::Engine
{
    using Base = polygon4::Engine;

public:
    P4Engine(const FString &modificationsDirectory, UP4GameInstance *P4GameInstance);

    virtual void initChildren() override;

    TArray<TSharedPtr<struct ModificationDesc>> GetModificationDescriptors() const;

    void StartEngine() { P4EngineState = EP4EngineState::Started; }
    bool IsStarted() const { return P4EngineState == EP4EngineState::Started; }
    void Exit();

    UP4GameInstance* GetP4GameInstance() const { return P4GameInstance; }
    UWorld* GetWorld() const;

    TSharedPtr<SMainMenu> GetMainMenu();
    virtual void ShowMainMenu() override;
    virtual void HideMainMenu() override;

    FVector GetWorldScale() const { return WorldScale; }
    void SetWorldScale(const FVector &Scale) { WorldScale = Scale; }

    virtual void OnLevelLoaded() override;

private:
    //
    // for some reasons ue4-side 32 bit code sees base class size = actual_size - sizeof(void*)
    int __buffer[3]; // do not delete! this prevents 32-bit version from crash
    //
    UP4GameInstance *P4GameInstance;

    EP4EngineState P4EngineState = EP4EngineState::None;
    TSharedPtr<SMainMenu> MainMenu;
    FVector WorldScale;
};

extern P4Engine *GP4Engine;
