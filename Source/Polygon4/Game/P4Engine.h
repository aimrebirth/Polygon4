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

#include "P4Engine.generated.h"

#define DECLARE_MENU(name)                                                                                                                                                         \
public:                                                                                                                                                                            \
    TSharedPtr<S##name##Menu> Get##name##Menu();                                                                                                                                   \
    virtual void Show##name##Menu() override;                                                                                                                                      \
    virtual void Hide##name##Menu() override;                                                                                                                                      \
    virtual void Set##name##MenuVisibility(bool Visibility) override

class UP4GameInstance;

class SMenu;
class SMainMenu;
class SPauseMenu;
class SBuildingMenu;

class UDummyObject;

enum class MenuType
{
    MainMenu,
    PauseMenu,
    BuildingMenu,

    Max
};

enum class EP4EngineState
{
    None,
    Started,
};

class P4Engine final : public polygon4::Engine
{
    using Base = polygon4::Engine;

public:
    P4Engine(const FString &modificationsDirectory, UP4GameInstance *P4GameInstance);
    ~P4Engine();

    virtual void initChildren() override;

    TArray<TSharedPtr<struct ModificationDesc>> GetModificationDescriptors() const;

    void StartEngine()
    {
        P4EngineState = EP4EngineState::Started;
    }
    bool IsStarted() const
    {
        return P4EngineState == EP4EngineState::Started;
    }
    void Exit();

    UP4GameInstance *GetP4GameInstance() const
    {
        return P4GameInstance;
    }
    UWorld *GetWorld() const;

    FVector GetWorldScale() const
    {
        return WorldScale;
    }
    void SetWorldScale(const FVector &Scale)
    {
        WorldScale = Scale;
    }

    virtual void OnLevelLoaded() override;

    void ShowPauseMenuFromBinding();
    void SetPauseMenuBindings();
    void UnsetPauseMenuBindings() const;

    virtual polygon4::BuildingMenu *getBuildingMenu() override;
    virtual void DestroyBuildingMenu() override;

private:
    //
    // for some reasons ue4-side 32 bit code sees base class size = actual_size - sizeof(void*)
    int __buffer[3]; // do not delete! this prevents 32-bit version from crash

    //
    UP4GameInstance *P4GameInstance;

    EP4EngineState P4EngineState = EP4EngineState::None;
    FVector WorldScale;

    // this should be at the end (after __buffer) because of __buffer var
    DECLARE_MENU(Main);
    DECLARE_MENU(Pause);
    DECLARE_MENU(Building);

public:
    TSharedPtr<SMenu> GetMenu(MenuType Type);
    void ShowMenu(MenuType Type);
    void HideMenu(MenuType Type);
    void DestroyMenu(MenuType Type);
    void SetMenuVisibility(MenuType Type, bool Visibility);

    void ReturnToMainMenu();

private:
    TArray<TSharedPtr<SMenu>> Menus;
    bool paused = false;
    // this will keep some key bindings (pause etc.)
    UDummyObject *DummyObject = nullptr;

    friend class UDummyObject;
};

UCLASS()
class UDummyObject : public UObject
{
    GENERATED_BODY()

public:
    void ShowPauseMenuFromBinding();
};

POLYGON4_API
#ifdef _MSC_VER
__declspec(noinline)
#endif
    P4Engine *GP4Engine(P4Engine *Engine = nullptr);
