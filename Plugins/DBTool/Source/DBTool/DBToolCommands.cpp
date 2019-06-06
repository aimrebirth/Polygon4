#include "DBToolCommands.h"

#define LOCTEXT_NAMESPACE "FDBToolModule"

void FDBToolCommands::RegisterCommands()
{
    UI_COMMAND(OpenPluginWindow, "DB Tool", "Bring up DBTool window", EUserInterfaceActionType::Button, FInputGesture());
    //
    UI_COMMAND(SaveDB, "Save DB", "Save DB", EUserInterfaceActionType::Button, FInputGesture());
    UI_COMMAND(ReloadDB, "Reload DB", "Reload DB", EUserInterfaceActionType::Button, FInputGesture());
    //
    UI_COMMAND(AddRecord, "Add Record", "Add Record", EUserInterfaceActionType::Button, FInputGesture());
    UI_COMMAND(DeleteRecord, "Delete Record", "Delete Record", EUserInterfaceActionType::Button, FInputGesture());
    //
    UI_COMMAND(LoadMapMechanoidsFromDB, "Load Map Mechanoids from DB", "Load Map Mechanoids from DB", EUserInterfaceActionType::Button, FInputGesture());
    UI_COMMAND(SaveMapMechanoidsToDB, "Save Map Mechanoids to DB", "Save Map Mechanoids to DB", EUserInterfaceActionType::Button, FInputGesture());
    //
    UI_COMMAND(LoadMapObjectsFromDB, "Load Map Objects from DB", "Load Map Objects from DB", EUserInterfaceActionType::Button, FInputGesture());
    UI_COMMAND(SaveMapObjectsToDB, "Save Map Objects to DB", "Save Map Objects to DB", EUserInterfaceActionType::Button, FInputGesture());
    //
    UI_COMMAND(LoadMapHeightmap, "Load Map Heightmap", "Load Map Heightmap", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
