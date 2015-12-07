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

#include "AIController.h"
#include "BotController.generated.h"


UCLASS()
class ABotController : public AAIController
{
    GENERATED_BODY()

public:
    ABotController(const FObjectInitializer& ObjectInitializer);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //	  Per Tick Modification of Path Following
    //			this is how you really customize 
    //				how units follow the path! 

    /** follow current path segment */
    //virtual void FollowPathSegment(float DeltaTime) override;

    /** sets variables related to current move segment */
    //virtual void SetMoveSegment(int32 SegmentStartIndex) override;

    /** check state of path following, update move segment if needed */
    //virtual void UpdatePathSegment() override;
};
