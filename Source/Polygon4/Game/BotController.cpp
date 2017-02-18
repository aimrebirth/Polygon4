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
#include "BotController.h"

ABotController::ABotController(const FObjectInitializer& ObjectInitializer)
    //: Super(ObjectInitializer.SetDefaultSubobjectClass<UJoyPathFollowComp>(TEXT("PathFollowingComponent")))
{

}

//Nav Data Main
/*FORCEINLINE const ANavigationData* GetMainNavData(FNavigationSystem::ECreateIfEmpty CreateNewIfNoneFound)
{
    UNavigationSystem* NavSys = GetWorld()->GetNavigationSystem();
    if (!NavSys) return NULL;
    return NavSys->GetMainNavData(CreateNewIfNoneFound);
}

//Choose Which Nav Data To Use
FORCEINLINE const ANavigationData* JoyGetNavData() const
{
    const FNavAgentProperties& AgentProperties = MovementComp->GetNavAgentPropertiesRef();
    const ANavigationData* NavData = GetNavDataForProps(AgentProperties);
    if (NavData == NULL)
    {
        VSCREENMSG("ERROR USING MAIN NAV DATA");
        NavData = GetMainNavData();
    }

    return NavData;
}

//VERY IMPORTANT FOR CRASH PROTECTION !!!!!
FORCEINLINE bool TileIsValid(const ARecastNavMesh* NavMesh, int32 TileIndex) const
{
    if (!NavMesh) return false;
    //~~~~~~~~~~~~~~
    const FBox TileBounds = NavMesh->GetNavMeshTileBounds(TileIndex);

    return TileBounds.IsValid != 0;
}

//add this to your custom path follow component!
bool NavPoly_GetAllPolys(TArray<NavNodeRef>& Polys);
//Rama's UE4 Nav code to get all the nav polys!
bool UJoyPathFollowComp::NavPoly_GetAllPolys(TArray<NavNodeRef>& Polys)
{
    if (!MovementComp) return false;
    //~~~~~~~~~~~~~~~~~~

    //Get Nav Data
    const ANavigationData* NavData = JoyGetNavData();

    const ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
    if (!NavMesh)
    {
        return false;
    }

    TArray<FNavPoly> EachPolys;
    for (int32 v = 0; v < NavMesh->GetNavMeshTilesCount(); v++)
    {

        //CHECK IS VALID FIRST OR WILL CRASH!!!
        //     256 entries but only few are valid!
        // use continue in case the valid polys are not stored sequentially
        if (!TileIsValid(NavMesh, v))
        {
            continue;
        }

        NavMesh->GetPolysInTile(v, EachPolys);
    }


    //Add them all!
    for (int32 v = 0; v < EachPolys.Num(); v++)
    {
        Polys.Add(EachPolys[v].Ref);
    }
}*/
