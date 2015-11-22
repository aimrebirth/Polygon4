// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "DBToolStyle.h"

class FDBToolCommands : public TCommands<FDBToolCommands>
{
public:
	FDBToolCommands()
		: TCommands<FDBToolCommands>(TEXT("DBTool"), NSLOCTEXT("Contexts", "DBTool", "DBTool Plugin"), NAME_None, FDBToolStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
    TSharedPtr< FUICommandInfo > OpenPluginWindow;

    // db mgr
    TSharedPtr< FUICommandInfo > SaveDB;
    TSharedPtr< FUICommandInfo > ReloadDB;

    TSharedPtr< FUICommandInfo > AddRecord;
    TSharedPtr< FUICommandInfo > DeleteRecord;

    TSharedPtr< FUICommandInfo > LoadMapMechanoidsFromDB;
    TSharedPtr< FUICommandInfo > SaveMapMechanoidsToDB;

    TSharedPtr< FUICommandInfo > LoadMapHeightmap;    
};
