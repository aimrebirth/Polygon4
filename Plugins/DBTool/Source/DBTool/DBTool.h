// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <memory>

class FToolBarBuilder;
class FMenuBuilder;

namespace polygon4
{
    namespace detail
    {
        class Storage;
    }
    struct Table;
    class Database;
    struct DatabaseSchema;
}

class FDBToolModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    void PluginButtonClicked();

    void AddToolbarExtension(FToolBarBuilder& Builder);
    void AddMenuExtension(FMenuBuilder& Builder);

    void SetDataChanged();
    void SetDataCommitted();

    TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
    TSharedPtr<class FUICommandList> PluginCommands;
    TSharedPtr<FToolBarBuilder> ToolBarBuilder;
    TSharedPtr<class SDBToolTableView> TableView;
    TSharedPtr<class SDBToolTreeView> TreeView;
    TSharedPtr<SDockTab> Tab;
    TSharedPtr<FTabSpawnerEntry> TabSpawnerEntry;
    TSharedPtr<STextBlock> StatusBar;

private:
    std::unique_ptr<polygon4::Database> database;
    std::unique_ptr<polygon4::detail::Storage> storage;
    bool dataChanged = false;

    bool LoadDB();
    bool SaveDB();
    void ReloadDB();

    void LoadMapMechanoidsFromDB();
    void SaveMapMechanoidsToDB();

    void LoadMapObjectsFromDB();
    void SaveMapObjectsToDB();

    void LoadMapHeightmap();
};

extern FDBToolModule *GDBToolModule;
