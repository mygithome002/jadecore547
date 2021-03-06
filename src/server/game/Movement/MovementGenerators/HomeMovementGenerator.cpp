/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "HomeMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "WorldPacket.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

// ========== HomeMovementGenerator ============ //

void HomeMovementGenerator<Creature>::DoInitialize(Creature* owner)
{
    if (!owner)
        return;

    if (!owner->IsAlive())
        return;

    _setTargetLocation(owner);
}

void HomeMovementGenerator<Creature>::DoReset(Creature* owner) { }

void HomeMovementGenerator<Creature>::_setTargetLocation(Creature* owner)
{
    if (owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return;

    float x, y, z, o;
    owner->GetHomePosition(x, y, z, o);

    Movement::MoveSplineInit init(owner);
    init.SetFacing(o);
    init.MoveTo(x, y, z);
    init.SetWalk(false);
    init.Launch();

    arrived = false;
    owner->ClearUnitState(UNIT_STATE_ALL_STATE & ~UNIT_STATE_EVADE);
}

bool HomeMovementGenerator<Creature>::DoUpdate(Creature* owner, uint32 diff)
{
    arrived = owner->movespline->Finalized();
    return !arrived;
}

void HomeMovementGenerator<Creature>::DoFinalize(Creature* owner)
{
    if (!owner)
        return;

    if (arrived)
    {
        owner->ClearUnitState(UNIT_STATE_EVADE);
        owner->SetWalk(true);
        owner->LoadCreaturesAddon(true);
        owner->AI()->JustReachedHome();
    }
}
