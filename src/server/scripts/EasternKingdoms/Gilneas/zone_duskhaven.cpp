/*
 *
* Copyright (C) 2011-2016 ArkCORE <http://www.arkania.net/>
 *
* Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 *
* Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: duskhaven
SD%Complete: 0
SDComment: Quest support: 0
SDCategory: Gilneas
EndScriptData */

/* ContentData */

#include "CreatureTextMgr.h"
#include "PassiveAI.h"
#include "Player.h"
#include "MoveSplineInit.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"
#include "zone_gilneas.h"

// Phase 1

// 36332
class npc_king_genn_greymane_36332 : public CreatureScript
{
public:
    npc_king_genn_greymane_36332() : CreatureScript("npc_king_genn_greymane_36332") { }

    enum e14375
    {
        QUEST_LAST_CHANCE_AT_HUMANITY = 14375,
        SPELL_PHASE_QUEST_ZONE_SPECIFIC_06 = 68481,
        SPELL_FADE_BACK = 94053,
        SPELL_LAST_STAND_COMPLETE = 72799,
        NPC_LORD_GODFREY_36330 = 36330,
        NPC_LORD_GODFREY_36170 = 36170,
        NPC_KRENNAN_ARANAS_36331 = 36331,
        NPC_KING_GENN_GREYMANE_36332 = 36332,
    };

    bool OnQuestReward(Player* player, Creature* creature, const Quest *_Quest, uint32) override
    {
        if (_Quest->GetQuestId() == QUEST_LAST_CHANCE_AT_HUMANITY)
        {
            player->CastSpell(player, SPELL_PHASE_QUEST_ZONE_SPECIFIC_06, true);
            player->SetUInt32Value(UNIT_FIELD_FLAGS_2, 2048);
            player->RemoveAura(42716);
            player->RemoveAura(50220);
            player->RemoveAura(58284);
            player->RemoveAura(68630);
            return true;
        }
        return false;
    }
};

// 36331
class npc_krennan_aranas_36331 : public CreatureScript
{
public:
    npc_krennan_aranas_36331() : CreatureScript("npc_krennan_aranas_36331") { }

    enum eNpc
    {
        QUEST_LAST_CHANCE_AT_HUMANITY = 14375,
        QUEST_LAST_STAND = 14222,
        NPC_KING_GREYMANE = 36332,
        NPC_LORD_GODFREY = 36330,
        SPELL_CATACLYSM_1 = 68953,
        SPELL_CATACLYSM_2 = 80134,
        SPELL_CATACLYSM_3 = 80133,
        SPELL_LAST_STAND_COMPLETE_2 = 72799,
        EVENT_CHECK_ARRIVEL_PLAYER = 101,
        EVENT_TALK_0,
        EVENT_TALK_1,
        EVENT_TALK_2,
        EVENT_TALK_3,
        EVENT_TALK_4,
        EVENT_RESTART,
    };

    struct npc_krennan_aranas_36331AI : public ScriptedAI
    {
        npc_krennan_aranas_36331AI(Creature* creature) : ScriptedAI(creature) { }

        EventMap  m_events;
        bool      m_videoStarted;
        uint64    m_playerGUID;
        uint64    m_kingGUID;
        uint64    m_godfreyGUID;

        void Reset() override
        {
            m_videoStarted = false;
            m_playerGUID = NULL;
            m_kingGUID = NULL;
            m_godfreyGUID = NULL;
            m_events.Reset();
            m_events.ScheduleEvent(EVENT_CHECK_ARRIVEL_PLAYER, 1000);
        }

        void UpdateAI(uint32 diff) override
        {
            m_events.Update(diff);

            if (CheckPlayerIsInvalid())
            {
                Reset();
                return;
            }

            while (uint32 eventId = m_events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_ARRIVEL_PLAYER:
                    {
                        if (!m_videoStarted)
                            if (Player* player = me->FindNearestPlayer(10.0f))
                                if (player->GetQuestStatus(QUEST_LAST_STAND) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_LAST_CHANCE_AT_HUMANITY) == QUEST_STATUS_NONE)
                                {
                                    player->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                                    m_playerGUID = player->GetGUID();
                                    m_videoStarted = true;
                                    if (!m_kingGUID)
                                        if (Creature* king = me->FindNearestCreature(NPC_KING_GREYMANE, 25.0f))
                                            m_kingGUID = king->GetGUID();
                                    if (!m_godfreyGUID)
                                        if (Creature* lord = me->FindNearestCreature(NPC_LORD_GODFREY, 25.0f))
                                            m_godfreyGUID = lord->GetGUID();

                                    m_events.ScheduleEvent(EVENT_TALK_0, 4000);
                                    return;
                                }
                        m_events.ScheduleEvent(EVENT_CHECK_ARRIVEL_PLAYER, 1000);
                        break;
                    }
                    case EVENT_TALK_0:
                    {
                        m_events.ScheduleEvent(EVENT_TALK_1, 14000);
                        Talk(0);
                        break;
                    }
                    case EVENT_TALK_1:
                    {
                        if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                            player->CastSpell(player, SPELL_CATACLYSM_1);
                        if (Creature* lord = sObjectAccessor->GetCreature(*me, m_godfreyGUID))
                            lord->AI()->Talk(0);
                        m_events.ScheduleEvent(EVENT_TALK_2, 8000);
                        break;
                    }
                    case EVENT_TALK_2:
                    {
                        if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                            player->CastSpell(player, SPELL_CATACLYSM_2);
                        if (Creature* king = sObjectAccessor->GetCreature(*me, m_kingGUID))
                            king->AI()->Talk(0);
                        m_events.ScheduleEvent(EVENT_TALK_3, 9000);
                        break;
                    }
                    case EVENT_TALK_3:
                    {
                        if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                            player->CastSpell(player, SPELL_CATACLYSM_3);
                        if (Creature* king = sObjectAccessor->GetCreature(*me, m_kingGUID))
                            king->AI()->Talk(1);
                        m_events.ScheduleEvent(EVENT_TALK_4, 8000);
                        break;
                    }
                    case EVENT_TALK_4:
                    {
                        if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                            player->CastSpell(player, SPELL_LAST_STAND_COMPLETE_2);

                        m_events.ScheduleEvent(EVENT_RESTART, 30000);
                        break;
                    }
                    case EVENT_RESTART:
                    {
                        Reset();
                        break;
                    }
                }
            }

            if (!UpdateVictim())
                return;
            else
                DoMeleeAttackIfReady();
        }

        bool CheckPlayerIsInvalid()
        {
            if (!m_playerGUID)
                return false;

            if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                if (player->IsAlive() && player->IsInWorld())
                    if (player->GetDistance2d(me) < 10.0f)
                        return false;
            return true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_krennan_aranas_36331AI(creature);
    }
};

// Phase 4096

// 34571
class npc_gwen_armstead_34571 : public CreatureScript
{
public:
    npc_gwen_armstead_34571() : CreatureScript("npc_gwen_armstead_34571") {}

    enum eNpc
    {
        QUEST_KILL_OR_BE_KILLED = 14336,
        SPELL_PHASE_QUEST_ZONE_SPECIFIC_06 = 68481,
        SPELL_PHASE_QUEST_ZONE_SPECIFIC_07 = 68482,
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_KILL_OR_BE_KILLED)
        {
            player->RemoveAura(SPELL_PHASE_QUEST_ZONE_SPECIFIC_06);
            player->CastSpell(player, SPELL_PHASE_QUEST_ZONE_SPECIFIC_07);
        }
        return true;
    }
};

// 196394  
class go_mandragore_196394 : public GameObjectScript
{
public:
    go_mandragore_196394() : GameObjectScript("go_mandragore_196394") {}

    bool OnQuestReward(Player* player, GameObject *, Quest const* _Quest, uint32) override
    {
        if (_Quest->GetQuestId() == 14320)
        {
            player->SendCinematicStart(168);
            WorldPacket data(SMSG_PLAY_SOUND, 4);
            data << uint32(23676);
            player->GetSession()->SendPacket(&data);
        }
        return true;
    }
};

// Phase 8192

// 36231  // Quest - You Can't Take 'Em Alone - 14348
class npc_horrid_abomination_36231 : public CreatureScript
{
public:
    npc_horrid_abomination_36231() : CreatureScript("npc_horrid_abomination_36231") { }

    enum eHorrid
    {
        QUEST_YOU_CANT_TAKE_EM_ALONE = 14348,
        QUEST_14348_KILL_CREDIT = 36233,
        SPELL_BARREL_KEG = 69094,
        SPELL_BARREL_KEG_PLACED = 68555,
        SPELL_ABOMINATION_KILL_ME = 68558,
        SPELL_HORRID_ABOMINATION_EXPLOSION = 68560,
        SPELL_RANDOM_POINT_POISON = 42266,
        SPELL_RANDOM_POINT_BONE = 42267,
        SPELL_RANDOM_POINT_BONE_2 = 42274,
        SPELL_CORPSE_EXPLOSION = 43999,
        SAY_BARREL = 0,
        EVENT_KEG_PLACED = 101,
        EVENT_KEG_CREDIT,
        EVENT_EXPLOSION,
        EVENT_FINISH,
    };

    struct npc_horrid_abomination_36231AI : public ScriptedAI
    {
        npc_horrid_abomination_36231AI(Creature* creature) : ScriptedAI(creature) {}

        bool m_creditGiven;

        void Reset() override
        {
            me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
            m_creditGiven = false;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (Player* player = caster->ToPlayer())
            {
                if (spell->Id == SPELL_BARREL_KEG_PLACED)
                {
                    Talk(SAY_BARREL);
                    me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                }
                if (!m_creditGiven && player->GetQuestStatus(QUEST_YOU_CANT_TAKE_EM_ALONE) == QUEST_STATUS_INCOMPLETE)
                {
                        player->KilledMonsterCredit(QUEST_14348_KILL_CREDIT);
                        m_creditGiven = true;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horrid_abomination_36231AI(creature);
    }
};

// 69094
class spell_cast_keg_69094 : public SpellScriptLoader
{
public:
    spell_cast_keg_69094() : SpellScriptLoader("spell_cast_keg_69094") { }

    enum eNpc
    {
        NPC_HORRID_ABOMINATION = 36231,
    };

    class spell_cast_keg_69094_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_cast_keg_69094_SpellScript);

        void CheckTarget(WorldObject*& target)
        {
            if (target->ToPlayer())
                target = NULL;
            else if (Creature* npc = target->ToCreature())
                if (npc->GetEntry() != NPC_HORRID_ABOMINATION)
                    target = NULL;
        }

        void Register() override
        {
            OnObjectTargetSelect += SpellObjectTargetSelectFn(spell_cast_keg_69094_SpellScript::CheckTarget, EFFECT_0, TARGET_UNIT_TARGET_ANY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_cast_keg_69094_SpellScript();
    }
};

// 36287  // Quest save the children 14368
class npc_cynthia_36267 : public CreatureScript
{
public:
    npc_cynthia_36267() : CreatureScript("npc_cynthia_36267") {}

    enum eNpc
    {
        QUEST_SAVE_THE_CHILDREN = 14368,
        NPC_CYNTIA_CREDIT = 36287,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_SAVE_THE_CHILDREN) == QUEST_STATUS_INCOMPLETE)
        {
            sCreatureTextMgr->SendChat(creature, 0, NULL, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);
            creature->AI()->Talk(1);
            player->KilledMonsterCredit(NPC_CYNTIA_CREDIT);
            return true;
        }
        return false;
    }
};

// 36288
class npc_james_36268 : public CreatureScript
{
public:
    npc_james_36268() : CreatureScript("npc_james_36268") {}

    enum eNpc
    {
        QUEST_SAVE_THE_CHILDREN = 14368,
        NPC_JAMES_CREDIT = 36288,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_SAVE_THE_CHILDREN) == QUEST_STATUS_INCOMPLETE)
        {
            sCreatureTextMgr->SendChat(creature, 0, NULL, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);
            creature->AI()->Talk(1);
            player->KilledMonsterCredit(NPC_JAMES_CREDIT);
            return true;
        }
        return false;
    }
};

// 36289
class npc_ashley_36269 : public CreatureScript
{
public:
    npc_ashley_36269() : CreatureScript("npc_ashley_36269") {}

    enum eNpc
    {
        QUEST_SAVE_THE_CHILDREN = 14368,
        NPC_ASHLEY_CREDIT = 36289,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_SAVE_THE_CHILDREN) == QUEST_STATUS_INCOMPLETE)
        {
            sCreatureTextMgr->SendChat(creature, 0, NULL, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);
            creature->AI()->Talk(1);
            player->KilledMonsterCredit(NPC_ASHLEY_CREDIT);
            return true;
        }
        return false;
    }
};

// 36283 quest 14382 Two by Sea (enter the ship)
class npc_forsaken_catapult_36283 : public CreatureScript
{
public:
    npc_forsaken_catapult_36283() : CreatureScript("npc_forsaken_catapult_36283") { }

    enum eHorrid
    {
        NPC_FORSAKEN_MACHINIST = 36292,
        NPC_GENERIC_TRIGGER_LAB = 36286,
        PASSENGER_GUID = 99998,
        PLAYER_GUID = 99999,
        SPELL_FIERY_BOULDER = 68591, 
        SPELL_LAUNCH1 = 68659,
        SPELL_LAUNCH2 = 66251,
        SPELL_LAUNCH3 = 66227,
        SPELL_LAUNCH4 = 96185,
        EVENT_CHECK_PLAYER = 101,
        EVENT_CAST_BOULDER_PREPARE,
        EVENT_MASTER_RESET,
    };

    struct npc_forsaken_catapult_36283AI : public ScriptedAI
    {
        npc_forsaken_catapult_36283AI(Creature* creature) : ScriptedAI(creature) {}

        EventMap m_events;
        uint64   m_playerGUID;
        uint64   m_forsakenGUID;
        int32    m_phase;

        void Reset() override
        {
            m_phase = 0;
            m_events.Reset();
            m_events.ScheduleEvent(EVENT_CHECK_PLAYER, 1000);
            m_playerGUID = NULL;
            m_forsakenGUID = NULL;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            me->SetReactState(REACT_PASSIVE);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
        { 
            m_playerGUID = NULL;
            m_forsakenGUID = NULL;
            if (apply)
                if (Player* player = passenger->ToPlayer())
                    m_playerGUID = player->GetGUID();
                else if (Creature* npc = passenger->ToCreature())
                    if (npc->GetEntry() == NPC_FORSAKEN_MACHINIST)
                    {
                        m_forsakenGUID = npc->GetGUID();
                        m_events.ScheduleEvent(EVENT_CAST_BOULDER_PREPARE, urand(1000, 15000));
                    }
        }

        void UpdateAI(uint32 diff) override
        {
            m_events.Update(diff);

            while (uint32 eventId = m_events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_PLAYER:
                    {
                        Vehicle* catapult = me->GetVehicleKit();
                        Unit* unit = catapult->GetPassenger(0);
                        if (catapult)
                            if (unit)
                            {
                                if (Creature* forsaken = unit->ToCreature())
                                {
                                    if (forsaken->IsInCombat())
                                    {
                                        forsaken->ExitVehicle();
                                        forsaken->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                        m_events.ScheduleEvent(EVENT_MASTER_RESET, 10000);
                                    }
                                }
                            }
                            else
                            {
                                if (me->getFaction() != 35)
                                {
                                    m_events.CancelEvent(EVENT_CAST_BOULDER_PREPARE);
                                    me->setFaction(35);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                                }
                                break;
                            }

                        m_events.ScheduleEvent(EVENT_CHECK_PLAYER, 1000);
                        break;
                    }
                    case EVENT_CAST_BOULDER_PREPARE:
                    {
                        me->CastSpell(me, SPELL_FIERY_BOULDER, true);
                        m_events.ScheduleEvent(EVENT_CAST_BOULDER_PREPARE, urand(8000,15000));
                        break;
                    }
                    case EVENT_MASTER_RESET:
                    {
                        me->DespawnOrUnsummon(1);
                        break;
                    }
                }
            }

            if (!UpdateVictim())
                return;
            else
                DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_catapult_36283AI(creature);
    }
};

// 68659
class spell_launch_68659 : public SpellScriptLoader
{
public:
    spell_launch_68659() : SpellScriptLoader("spell_launch_68659") { }

    enum eNpc
    {
        // TRAJ  short from trajectories = Flugbahnen, Flugkurven, Bewegungsbahnen 
        NPC_FORSAKEN_CATAPULT = 36283,
        NPC_FORSAKEN_MACHINIST = 36292,
        NPC_GENERIC_TRIGGER_LAB = 36286,
        PASSENGER_GUID = 99998,
        PLAYER_GUID = 99999,
    };

    class spell_launch_68659_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_launch_68659_SpellScript);

        uint64 m_playerGuid = NULL;
        std::list<uint32> _guids;
        Position pos = Position();

        void Initialize()
        {
            if (_guids.empty())
            {
                _guids.push_back(116836);
                _guids.push_back(116837);
                _guids.push_back(116838);
                _guids.push_back(116839);
                _guids.push_back(116840);
                _guids.push_back(116841);
                _guids.push_back(116842);
                _guids.push_back(116843);
                _guids.push_back(116844);
            }
        }

        void HandleBeforeCast()
        {
            if (Creature* catapult = GetCaster()->ToCreature())
                if (Vehicle* vehicle = catapult->GetVehicleKit())
                    if (Unit* unit = vehicle->GetPassenger(0))
                        if (Player* player = unit->ToPlayer())
                        {
                            m_playerGuid = player->GetGUID();
                            player->ExitVehicle(0);
                        }
                        else
                            m_playerGuid = NULL;
        }

        void CheckTargets(std::list<WorldObject*>& targets)
        {
            Initialize();
            targets.remove_if(IsNotInDBGuidList(_guids));
            if (targets.size())
            {
                std::list<WorldObject*>::iterator itr = targets.begin();
                pos = (*itr)->GetPosition();
            }
            targets.remove_if(IsNotBetweenAngle(GetCaster(), 1.57f, 4.71f));
            targets.remove_if(ReduceToOneRandomTarget(targets));
            if (targets.size())
            {
                std::list<WorldObject*>::iterator itr = targets.begin();
                pos = (*itr)->GetPosition();
            }
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (Player* player = sObjectAccessor->GetPlayer(*GetCaster(), m_playerGuid))
               if (pos.GetPositionX())
                    player->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), player->GetOrientation());
        }

        void Register() override
        {
            BeforeCast += SpellCastFn(spell_launch_68659_SpellScript::HandleBeforeCast);
            OnEffectHit += SpellEffectFn(spell_launch_68659_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_launch_68659_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_launch_68659_SpellScript();
    }
};

// 68591
class spell_fire_boulder_68591 : public SpellScriptLoader
{
public:
    spell_fire_boulder_68591() : SpellScriptLoader("spell_fire_boulder_68591") { }

    enum eNpc
    {
        // TRAJ  short from trajectories = Flugbahnen, Flugkurven, Bewegungsbahnen 
        NPC_FORSAKEN_CATAPULT = 36283,
        NPC_FORSAKEN_MACHINIST = 36292,
        NPC_GENERIC_TRIGGER_LAB = 36286,
        PASSENGER_GUID = 99998,
        PLAYER_GUID = 99999,
    };

    class spell_fire_boulder_68591_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fire_boulder_68591_SpellScript);

        uint64 m_playerGuid = NULL;
        std::list<Position> _posHouse;
        std::list<Position> _posGarden;
        std::list<Position> _posWay;
        std::list<uint32> _guids;

        void Initialize()
        {
            if (_guids.empty())
            {
                _guids.push_back(116845);
                _guids.push_back(116846);
                _guids.push_back(116847);
                _guids.push_back(116848);
                _guids.push_back(116849);
                _guids.push_back(116850);
                _guids.push_back(116851);
                _guids.push_back(116852);
                _guids.push_back(116853);
                _guids.push_back(116854);
                _guids.push_back(116855);
                _guids.push_back(116856);
                _guids.push_back(116857);
                _guids.push_back(116858);
                _guids.push_back(116859);
                _guids.push_back(116860);
            }
        }

        void CheckTargets(std::list<WorldObject*>& targets)
        {
            Initialize();
            targets.remove_if(IsNotInDBGuidList(_guids));
            targets.remove_if(IsOutsideDistance(GetCaster(), 30, 120));
            targets.remove_if(ReduceToOneRandomTarget(targets));
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fire_boulder_68591_SpellScript::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_fire_boulder_68591_SpellScript();
    }
};

// 36409
class npc_mastiff_36409 : public CreatureScript
{
public:
    npc_mastiff_36409() : CreatureScript("npc_mastiff_36409") { }

    enum eNPC
    {
        QUEST_LEADER_OF_THE_PACK = 14386,
        NPC_DARK_RANGER_THYALA = 36312,
        NPC_MASTIFF = 36405,
        NPC_TRIGGER = 36198,
        PLAYER_GUID = 99999,
        EVENT_CHECK_ATTACK = 101,
        EVENT_SEND_MORE_MASTIFF,
    };

    struct npc_mastiff_36409AI : public ScriptedAI
    {
        npc_mastiff_36409AI(Creature* creature) : ScriptedAI(creature) { Initialize(); }

        EventMap m_events;
        uint64   m_thyalaGUID;
        uint64   m_player_GUID;
        uint32   m_mastiff_counter;

        void Initialize()
        {
            m_events.Reset();
        }

        void Reset() override
        {
            m_thyalaGUID = NULL;
            m_player_GUID = NULL;
            m_mastiff_counter = 0;
            m_events.ScheduleEvent(EVENT_CHECK_ATTACK, 500);
            m_events.ScheduleEvent(EVENT_SEND_MORE_MASTIFF, 250);
        }

        void IsSummonedBy(Unit* summoner) override 
        { 
            if (Player* player = summoner->ToPlayer())
                m_player_GUID = player->GetGUID();
            if (Creature* thyala = me->FindNearestCreature(NPC_DARK_RANGER_THYALA, 100.0f))
            {
                m_thyalaGUID = thyala->GetGUID();
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveChase(thyala, 3.0f, 0.0f);
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage) override 
        { 
            damage = 0;
        }

        void JustSummoned(Creature* summon) override 
        { 
            m_mastiff_counter += 1;
            summon->GetAI()->SetGUID(m_player_GUID, PLAYER_GUID);
            summon->GetAI()->SetGUID(m_thyalaGUID, NPC_DARK_RANGER_THYALA);
        }

        void SummonedCreatureDies(Creature* summon, Unit* killer) override 
        { 
            m_mastiff_counter -= 1;
        }

        void SummonedCreatureDespawn(Creature* summon) 
        { 
            m_mastiff_counter -= 1;
        }

        void EnterEvadeMode() override { }

        void UpdateAI(uint32 diff) override
        {
            m_events.Update(diff);

            while (uint32 eventId = m_events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_ATTACK:
                    {
                        
                        Creature* thyala = sObjectAccessor->GetCreature(*me, m_thyalaGUID);
                        if (!thyala->IsAlive() || !thyala->IsInWorld())
                        {
                            if (Player* player = sObjectAccessor->GetPlayer(*me, m_player_GUID))
                                player->KilledMonsterCredit(NPC_DARK_RANGER_THYALA);

                            me->DespawnOrUnsummon(1000);
                        }

                        if (me->GetDistance2d(thyala) < 20.0f)
                        {
                            me->SetWalk(false);
                            me->GetMotionMaster()->MoveIdle();
                        }

                        m_events.ScheduleEvent(EVENT_CHECK_ATTACK, 1000);
                        break;
                    }
                    case EVENT_SEND_MORE_MASTIFF:
                    {
                        if (m_mastiff_counter < 50)
                        {
                            std::list<Creature*>trigger = me->FindNearestCreatures(NPC_TRIGGER, 100.0f);
                            for (std::list<Creature*>::const_iterator itr = trigger.begin(); itr != trigger.end(); ++itr)
                                me->SummonCreature(NPC_MASTIFF, (*itr)->GetNearPosition(5.0f, frand(0.0f, 6.28f)), TEMPSUMMON_TIMED_DESPAWN, urand(30000, 60000));
                        }

                        m_events.ScheduleEvent(EVENT_SEND_MORE_MASTIFF, 250);
                        break;
                    }
                }
            }

            if (!UpdateVictim())
                return;
            else
                DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mastiff_36409AI(creature);
    }
};

// 36405
class npc_mastiff_36405 : public CreatureScript
{
public:
    npc_mastiff_36405() : CreatureScript("npc_mastiff_36405") { }

    enum eNPC
    {
        QUEST_LEADER_OF_THE_PACK = 14386,
        NPC_DARK_RANGER_THYALA = 36312,
        NPC_MASTIFF = 36405,
        NPC_TRIGGER = 36198,
        PLAYER_GUID = 99999,
        EVENT_CHECK_ATTACK = 101,
    };

    struct npc_mastiff_36405AI : public ScriptedAI
    {
        npc_mastiff_36405AI(Creature* creature) : ScriptedAI(creature) { Initialize(); }

        EventMap m_events;
        uint64   m_thyalaGUID;
        uint64   m_player_GUID;

        void Initialize()
        {
            m_events.Reset();
        }

        void Reset() override
        {
            me->SetWalk(true);
            me->SetSpeed(MOVE_RUN, true);
            me->SetReactState(REACT_AGGRESSIVE);
            m_thyalaGUID = NULL;
            m_player_GUID = NULL;
            m_events.RescheduleEvent(EVENT_CHECK_ATTACK, 1000);
        }

        void SetGUID(uint64 guid, int32 id) override
        {
            switch (id)
            {
                case PLAYER_GUID:
                {
                    m_player_GUID = guid;
                    break;
                }
                case NPC_DARK_RANGER_THYALA:
                {
                    m_thyalaGUID = guid;
                    break;
                }
            }
        }

        void EnterEvadeMode() override 
        { 
            StartAttackThyala();
            m_events.RescheduleEvent(EVENT_CHECK_ATTACK, 1000);
        }

        void UpdateAI(uint32 diff) override
        {
            m_events.Update(diff);

            while (uint32 eventId = m_events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_ATTACK:
                    {
                        StartAttackThyala();
                        m_events.ScheduleEvent(EVENT_CHECK_ATTACK, 1000);
                        break;
                    }
                }
            }

            if (!UpdateVictim())
                return;
            else
                DoMeleeAttackIfReady();
        }

        void StartAttackThyala()
        {
            Creature* thyala = sObjectAccessor->GetCreature(*me, m_thyalaGUID);
            if (!thyala)
                return;

            if (!thyala->IsAlive() || !thyala->IsInWorld())
            {
                me->DespawnOrUnsummon(1);
                return;
            }

            if (me->IsInCombat())
                if (Unit* npc = me->GetVictim())
                    if (npc->GetEntry() == NPC_DARK_RANGER_THYALA)
                        return;

            me->SetReactState(REACT_AGGRESSIVE);
            me->GetMotionMaster()->MoveChase(thyala, 3.0f, frand(0.0f, 6.28f));
            me->Attack(thyala, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mastiff_36405AI(creature);
    }
};

// 36290 
class npc_lord_godfrey_36290 : public CreatureScript
{
public:
    npc_lord_godfrey_36290() : CreatureScript("npc_lord_godfrey_36290") { }

    enum eNpc
    {
        QUEST_AS_THE_LAND_SHATTERS = 14396,
        SPELL_PHASE_QUEST_ZONE_SPECIFIC_07 = 68482,
        SPELL_PHASE_QUEST_ZONE_SPECIFIC_08 = 68483,
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == 14396)
        {
            player->RemoveAura(SPELL_PHASE_QUEST_ZONE_SPECIFIC_07);
            player->CastSpell(player, SPELL_PHASE_QUEST_ZONE_SPECIFIC_08);
        }

        return true;
    }
};

// Phase 16384




void AddSC_zone_gilneas_duskhaven()
{	
    new npc_krennan_aranas_36331();
    new npc_king_genn_greymane_36332();
    new npc_gwen_armstead_34571();
    new go_mandragore_196394();
    new npc_horrid_abomination_36231();
    new spell_cast_keg_69094();
    new npc_cynthia_36267();
    new npc_james_36268();
    new npc_ashley_36269();
    new npc_forsaken_catapult_36283();
    new spell_launch_68659();
    new spell_fire_boulder_68591();
    new npc_mastiff_36409();
    new npc_mastiff_36405();
    new npc_lord_godfrey_36290();
};
