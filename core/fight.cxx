/***********************************************************************************
 *                                                                                  *
 *          _______.____    __    ____       _______                  _______       *
 *         /       |\   \  /  \  /   /  _   |   ____|          __    |   ____|      *
 *        |   (----` \   \/    \/   /  (_)  |  |__    ____   _/  |_  |  |__         *
 *         \   \      \            /    _   |   __|  /  _ \  \   __\ |   __|        *
 *     .----)   |      \    /\    /    (_)  |  |    (  <_> )  |  |   |  |____       *
 *     |_______/        \__/  \__/          |__|     \____/   |__|   |_______|      *
 *                                                                                  *
 * SWFotE v2.0 (FotE v1.1 cleaned up and considerably modded)  by:                  *
 * Greg (Keberus) Mosley                                                            *
 * Roman (Trelar) Arnold                                                            *
 *                                                                                  *
 * SWFotE v1 & v1.1 copyright (c) 2002 was created by                               *
 * Chris 'Tawnos' Dary (cadary@uwm.edu),                                            *
 * Korey 'Eleven' King (no email),                                                  *
 * Matt 'Trillen' White (mwhite17@ureach.com),                                      *
 * Daniel 'Danimal' Berrill (danimal924@yahoo.com),                                 *
 * Richard 'Bambua' Berrill (email unknown),                                        *
 * Stuart 'Ackbar' Unknown (email unknown)                                          *
 *                                                                                  *
 * SWR 1.0 copyright (c) 1997, 1998 was created by Sean Cooper                      *
 * based on a concept and ideas from the original SWR immortals:                    *
 * Himself (Durga), Mark Matt (Merth), Jp Coldarone (Exar), Greg Baily (Thrawn),    *
 * Ackbar, Satin, Streen and Bib as well as much input from our other builders      *
 * and players.                                                                     *
 *                                                                                  *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,                *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,                *
 * Grishnakh, Fireblade, and Nivek.                                                 *
 *                                                                                  *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                              *
 *                                                                                  *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,              *
 * Michael Seifert, and Sebastian Hammer.                                           *
 *                                                                                  *
 ***********************************************************************************/

#include "mud.hxx"

extern char lastplayercmd[MAX_INPUT_LENGTH];
extern CHAR_DATA* gch_prev;

/* From Skills.c */
int ris_save(CHAR_DATA* ch, int chance, int ris);

/* From newarena.c */
void lost_arena(CHAR_DATA* ch);

/* From space.c */
void remship(SHIP_DATA* ship);

/* From comm.c */
void name_log(const char* str, ...);

/*
 * Local functions.
 */
void dam_message(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt);
void death_cry(CHAR_DATA* ch);
void group_gain(CHAR_DATA* ch, CHAR_DATA* victim);
int xp_compute(CHAR_DATA* gch, CHAR_DATA* victim);
int align_compute(CHAR_DATA* gch, CHAR_DATA* victim);
ch_ret one_hit(CHAR_DATA* ch, CHAR_DATA* victim, int dt);
int obj_hitroll(OBJ_DATA* obj);
bool get_cover(CHAR_DATA* ch);
bool dual_flip = false;

/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned(CHAR_DATA* ch)
{
    OBJ_DATA* obj;

    if ((obj = get_eq_char(ch, WEAR_WIELD)) && (IS_SET(obj->extra_flags, ITEM_POISONED)))
        return true;

    return false;
}

/*
 * hunting, hating and fearing code				-Thoric
 */
bool is_hunting(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (!ch->hunting || ch->hunting->who != victim)
        return false;

    return true;
}

bool is_hating(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (!ch->hating || ch->hating->who != victim)
        return false;

    return true;
}

bool is_fearing(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (!ch->fearing || ch->fearing->who != victim)
        return false;

    return true;
}

void stop_hunting(CHAR_DATA* ch)
{
    if (ch->hunting)
    {
        STRFREE(ch->hunting->name);
        DISPOSE(ch->hunting);
        ch->hunting = nullptr;
    }
    return;
}

void stop_hating(CHAR_DATA* ch)
{
    if (ch->hating)
    {
        STRFREE(ch->hating->name);
        DISPOSE(ch->hating);
        ch->hating = nullptr;
    }
    return;
}

void stop_fearing(CHAR_DATA* ch)
{
    if (ch->fearing)
    {
        STRFREE(ch->fearing->name);
        DISPOSE(ch->fearing);
        ch->fearing = nullptr;
    }
    return;
}

void start_hunting(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (ch->hunting)
        stop_hunting(ch);

    CREATE(ch->hunting, HHF_DATA, 1);
    ch->hunting->name = QUICKLINK(victim->name);
    ch->hunting->who = victim;
    return;
}

void start_hating(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (ch->hating)
        stop_hating(ch);

    CREATE(ch->hating, HHF_DATA, 1);
    ch->hating->name = QUICKLINK(victim->name);
    ch->hating->who = victim;
    return;
}

void start_fearing(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (ch->fearing)
        stop_fearing(ch);

    CREATE(ch->fearing, HHF_DATA, 1);
    ch->fearing->name = QUICKLINK(victim->name);
    ch->fearing->who = victim;
    return;
}

int max_fight(CHAR_DATA* ch)
{
    return 8;
}

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Many hours spent fixing bugs in here by Thoric, as noted by residual
 * debugging checks.  If you never get any of these error messages again
 * in your logs... then you can comment out some of the checks without
 * worry.
 */
void violence_update(void)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA* ch;
    CHAR_DATA* lst_ch;
    CHAR_DATA* victim;
    CHAR_DATA *rch, *rch_next;
    AFFECT_DATA *paf, *paf_next;
    TIMER *timer, *timer_next;
    ch_ret retcode;
    SKILL_TYPE* skill;

    lst_ch = nullptr;
    for (ch = last_char; ch; lst_ch = ch, ch = gch_prev)
    {
        set_cur_char(ch);

        if (ch == first_char && ch->prev)
        {
            bug("ERROR: first_char->prev != nullptr, fixing...", 0);
            ch->prev = nullptr;
        }

        gch_prev = ch->prev;

        if (gch_prev && gch_prev->next != ch)
        {
            sprintf_s(buf, "FATAL: violence_update: %s->prev->next doesn't point to ch.", ch->name);
            bug(buf, 0);
            bug("Short-cutting here", 0);
            ch->prev = nullptr;
            gch_prev = nullptr;
            do_shout(ch, MAKE_TEMP_STRING("Thoric says, 'Prepare for the worst!'"));
        }

        /*
         * See if we got a pointer to someone who recently died...
         * if so, either the pointer is bad... or it's a player who
         * "died", and is back at the healer...
         * Since he/she's in the char_list, it's likely to be the later...
         * and should not already be in another fight already
         */
        if (char_died(ch))
            continue;

        /*
         * See if we got a pointer to some bad looking data...
         */
        if (!ch->in_room || !ch->name)
        {
            log_string("violence_update: bad ch record!  (Shortcutting.)");
            sprintf_s(buf, "ch: %p  ch->in_room: %p  ch->prev: %p  ch->next: %p", ch, ch->in_room, ch->prev, ch->next);
            log_string(buf);
            log_string(lastplayercmd);
            if (lst_ch)
                sprintf_s(buf, "lst_ch: %p  lst_ch->prev: %p  lst_ch->next: %p", lst_ch, lst_ch->prev, lst_ch->next);
            else
                strcpy_s(buf, "lst_ch: nullptr");
            log_string(buf);
            gch_prev = nullptr;
            continue;
        }

        /*
         * Experience gained during battle decreases as battle drags on
         */
        if (ch->fighting)
            if ((++ch->fighting->duration % 24) == 0)
                ch->fighting->xp = ((ch->fighting->xp * 9) / 10);

        for (timer = ch->first_timer; timer; timer = timer_next)
        {
            timer_next = timer->next;
            if (--timer->count <= 0)
            {
                if (timer->type == TIMER_DO_FUN)
                {
                    int tempsub;

                    tempsub = ch->substate;
                    ch->substate = timer->value;
                    (timer->do_fun)(ch, MAKE_TEMP_STRING(""));
                    if (char_died(ch))
                        break;
                    ch->substate = tempsub;
                }
                extract_timer(ch, timer);
            }
        }

        if (char_died(ch))
            continue;

        /*
         * We need spells that have shorter durations than an hour.
         * So a melee round sounds good to me... -Thoric
         */
        for (paf = ch->first_affect; paf; paf = paf_next)
        {
            paf_next = paf->next;
            if (paf->duration > 0)
                paf->duration--;
            else if (paf->duration < 0)
                ;
            else
            {
                if (!paf_next || paf_next->type != paf->type || paf_next->duration > 0)
                {
                    skill = get_skilltype(paf->type);
                    if (paf->type > 0 && skill && skill->msg_off)
                    {
                        set_char_color(AT_WEAROFF, ch);
                        send_to_char(skill->msg_off, ch);
                        send_to_char("\n\r", ch);
                    }
                }
                if (paf->type == gsn_possess && paf->type != gsn_blindness)
                {
                    bug(ch->name, 0);
                    ch->desc->character = ch->desc->original;
                    ch->desc->original = nullptr;
                    ch->desc->character->desc = ch->desc;
                    ch->desc->character->switched = nullptr;
                    ch->switched = nullptr;
                    ch->desc = nullptr;
                }
                affect_remove(ch, paf);
            }
        }

        if ((victim = who_fighting(ch)) == nullptr || IS_AFFECTED(ch, AFF_PARALYSIS))
            continue;

        retcode = rNONE;

        if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
        {
            sprintf_s(buf, "violence_update: %s fighting %s in a SAFE room.", ch->name, victim->name);
            log_string(buf);
            stop_fighting(ch, true);
        }
        else if (IS_AWAKE(ch) && ch->in_room == victim->in_room)
        {
            if (!IS_NPC(ch))
                ch->pcdata->lost_attacks = 0;
            retcode = multi_hit(ch, victim, TYPE_UNDEFINED);
        }
        else
            stop_fighting(ch, false);

        if (char_died(ch))
            continue;

        if (retcode == rCHAR_DIED || (victim = who_fighting(ch)) == nullptr)
            continue;

        /*
         *  Mob triggers
         */
        rprog_rfight_trigger(ch);
        if (char_died(ch))
            continue;
        mprog_hitprcnt_trigger(ch, victim);
        if (char_died(ch))
            continue;
        mprog_fight_trigger(ch, victim);
        if (char_died(ch))
            continue;

        /*
         * Fun for the whole family!
         */
        for (rch = ch->in_room->first_person; rch; rch = rch_next)
        {
            rch_next = rch->next_in_room;

            if (IS_AWAKE(rch) && !rch->fighting)
            {
                /*
                 * PC's auto-assist others in their group.
                 */
                if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM))
                {
                    if ((!IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM)) && is_same_group(ch, rch))
                        multi_hit(rch, victim, TYPE_UNDEFINED);
                    continue;
                }

                /*
                 * NPC's assist NPC's of same type or 12.5% chance regardless.
                 */
                if (IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM) && !IS_SET(rch->act, ACT_NOASSIST))
                {
                    if (char_died(ch))
                        break;
                    if (rch->pIndexData == ch->pIndexData || number_bits(3) == 0)
                    {
                        CHAR_DATA* vch;
                        CHAR_DATA* target;
                        int number;

                        target = nullptr;
                        number = 0;
                        for (vch = ch->in_room->first_person; vch; vch = vch->next)
                        {
                            if (can_see(rch, vch) && is_same_group(vch, victim) && number_range(0, number) == 0)
                            {
                                target = vch;
                                number++;
                            }
                        }

                        if (target)
                            multi_hit(rch, target, TYPE_UNDEFINED);
                    }
                }
            }
        }
    }

    return;
}

/*
 * Do one group of attacks.
 */
ch_ret multi_hit(CHAR_DATA* ch, CHAR_DATA* victim, int dt)
{
    int chance;
    int dual_bonus;
    int extrahits;
    ch_ret retcode;

    /* add timer if player is attacking another player */
    if (!IS_NPC(ch) && !IS_NPC(victim))
        add_timer(ch, TIMER_RECENTFIGHT, 20, nullptr, 0);

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_NICE) && !IS_NPC(victim))
        return rNONE;

    if ((retcode = one_hit(ch, victim, dt)) != rNONE)
        return retcode;

    if (who_fighting(ch) != victim || dt == gsn_backstab || dt == gsn_circle || dt == gsn_dualstab)
        return rNONE;

    /* Very high chance of hitting compared to chance of going berserk */
    /* 40% or higher is always hit.. don't learn anything here though. */
    /* -- Altrag */
    chance = IS_NPC(ch) ? 100 : (ch->pcdata->learned[gsn_berserk] * 5 / 2);
    if (IS_AFFECTED(ch, AFF_BERSERK) && number_percent() < chance)
        if ((retcode = one_hit(ch, victim, dt)) != rNONE || who_fighting(ch) != victim)
            return retcode;

    if (get_eq_char(ch, WEAR_DUAL_WIELD))
    {
        dual_bonus = IS_NPC(ch) ? (ch->skill_level[COMBAT_ABILITY] / 10) : (ch->pcdata->learned[gsn_dual_wield] / 10);
        chance = IS_NPC(ch) ? ch->top_level : ch->pcdata->learned[gsn_dual_wield];
        if (number_percent() < chance)
        {
            learn_from_success(ch, gsn_dual_wield);
            retcode = one_hit(ch, victim, dt);
            if (retcode != rNONE || who_fighting(ch) != victim)
                return retcode;
        }
        else
            learn_from_failure(ch, gsn_dual_wield);
    }
    else
        dual_bonus = 0;

    if (dt == gsn_ambush)
        extrahits = number_range(1, 3);
    else if (dt == gsn_dualstab)
        extrahits = 2;
    else
        extrahits = 0;

    if (dt == gsn_ambush)
    {
        if (IS_AFFECTED(ch, AFF_SNEAK))
        {
            affect_strip(ch, gsn_sneak);
            REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        }
    }

    for (chance = 0; chance <= extrahits; chance++)
    {
        retcode = one_hit(ch, victim, dt);
        if (retcode != rNONE || who_fighting(ch) != victim)
            return retcode;
    }
    if (ch->move < 10)
        dual_bonus = -20;

    /*
     * NPC predetermined number of attacks			-Thoric
     */
    if (IS_NPC(ch) && ch->numattacks > 0)
    {
        for (chance = 0; chance <= (ch->numattacks); chance++)
        {
            retcode = one_hit(ch, victim, dt);
            if (retcode != rNONE || who_fighting(ch) != victim)
                return retcode;
        }
        return retcode;
    }

    chance = IS_NPC(ch) ? ch->top_level : (int)((ch->pcdata->learned[gsn_second_attack] + dual_bonus) / 1.5);
    if (number_percent() < chance)
    {
        learn_from_success(ch, gsn_second_attack);
        retcode = one_hit(ch, victim, dt);
        if (retcode != rNONE || who_fighting(ch) != victim)
            return retcode;
    }
    else
        learn_from_failure(ch, gsn_second_attack);

    chance = IS_NPC(ch) ? ch->top_level : (int)((ch->pcdata->learned[gsn_third_attack] + (dual_bonus * 1.5)) / 2);
    if (number_percent() < chance)
    {
        learn_from_success(ch, gsn_third_attack);
        retcode = one_hit(ch, victim, dt);
        if (retcode != rNONE || who_fighting(ch) != victim)
            return retcode;
    }
    else
        learn_from_failure(ch, gsn_third_attack);

    retcode = rNONE;

    chance = IS_NPC(ch) ? (int)(ch->top_level / 4) : 0;
    if (number_percent() < chance)
        retcode = one_hit(ch, victim, dt);

    if (retcode == rNONE)
    {
        int move;

        if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING))
            move = encumbrance(ch, movement_loss[UMIN(SECT_MAX - 1, ch->in_room->sector_type)]);
        else
            move = encumbrance(ch, 1);
        if (ch->move)
            ch->move = UMAX(0, ch->move - move);
    }
    return retcode;
}

/*
 * Weapon types, haus
 */
int weapon_prof_bonus_check(CHAR_DATA* ch, OBJ_DATA* wield, int* gsn_ptr)
{
    int bonus;

    bonus = 0;
    *gsn_ptr = -1;
    if (!IS_NPC(ch) && wield)
    {
        switch (wield->value[3])
        {
        default:
            *gsn_ptr = -1;
            break;
        case 3:
            *gsn_ptr = gsn_lightsabers;
            break;
        case 2:
            *gsn_ptr = gsn_vibro_blades;
            break;
        case 4:
            *gsn_ptr = gsn_flexible_arms;
            break;
        case 5:
            *gsn_ptr = gsn_talonous_arms;
            break;
        case 6:
            *gsn_ptr = gsn_blasters;
            break;
        case 8:
            *gsn_ptr = gsn_bludgeons;
            break;
        case 9:
            *gsn_ptr = gsn_bowcasters;
            break;
        case 11:
            *gsn_ptr = gsn_force_pikes;
            break;
        }
        if (*gsn_ptr != -1)
            bonus = (int)(ch->pcdata->learned[*gsn_ptr]);
    }
    if (IS_NPC(ch) && wield)
        bonus = get_trust(ch);
    return bonus;
}

/*
 * Calculate the tohit bonus on the object and return RIS values.
 * -- Altrag
 */
int obj_hitroll(OBJ_DATA* obj)
{
    int tohit = 0;
    AFFECT_DATA* paf;

    for (paf = obj->pIndexData->first_affect; paf; paf = paf->next)
        if (paf->location == APPLY_HITROLL)
            tohit += paf->modifier;
    for (paf = obj->first_affect; paf; paf = paf->next)
        if (paf->location == APPLY_HITROLL)
            tohit += paf->modifier;
    return tohit;
}

/*
 * Offensive shield level modifier
 */
sh_int off_shld_lvl(CHAR_DATA* ch, CHAR_DATA* victim)
{
    sh_int lvl;

    if (!IS_NPC(ch)) /* players get much less effect */
    {
        lvl = UMAX(1, (ch->skill_level[FORCE_ABILITY]));
        if (number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 35)
            return lvl;
        else
            return 0;
    }
    else
    {
        lvl = ch->top_level;
        if (number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 70)
            return lvl;
        else
            return 0;
    }
}

/*
 * Hit one guy once.
 */
ch_ret one_hit(CHAR_DATA* ch, CHAR_DATA* victim, int dt)
{
    OBJ_DATA* wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int plusris;
    int dam, x;
    int diceroll;
    int attacktype, cnt;
    int prof_bonus;
    int prof_gsn;
    ch_ret retcode;
    int chance;
    bool fail;
    AFFECT_DATA af;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if (!IS_NPC(ch))
    {
        if (ch->pcdata->lost_attacks < 0)
            ch->pcdata->lost_attacks = 0;
        if (ch->pcdata->lost_attacks != 0)
        {
            ch->pcdata->lost_attacks--;
            return rNONE;
        }
    }
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
        return rVICT_DIED;

    /*
     * Figure out the weapon doing the damage			-Thoric
     */
    if ((wield = get_eq_char(ch, WEAR_DUAL_WIELD)) != nullptr)
    {
        if (dual_flip == false)
        {
            dual_flip = true;
            wield = get_eq_char(ch, WEAR_WIELD);
        }
        else
            dual_flip = false;
    }
    else
        wield = get_eq_char(ch, WEAR_WIELD);

    prof_bonus = weapon_prof_bonus_check(ch, wield, &prof_gsn);

    if (ch->fighting /* make sure fight is already started */
        && dt == TYPE_UNDEFINED && IS_NPC(ch) && ch->attacks != 0)
    {
        cnt = 0;
        for (;;)
        {
            x = number_range(0, 6);
            attacktype = 1 << x;
            if (IS_SET(ch->attacks, attacktype))
                break;
            if (cnt++ > 16)
            {
                attacktype = 0;
                break;
            }
        }
        if (attacktype == ATCK_BACKSTAB)
            attacktype = 0;
        if (wield && number_percent() > 25)
            attacktype = 0;
        switch (attacktype)
        {
        default:
            break;
        case ATCK_BITE:
            do_bite(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_CLAWS:
            do_claw(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_TAIL:
            do_tail(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_STING:
            do_sting(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_PUNCH:
            do_punch(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_KICK:
            do_kick(ch, MAKE_TEMP_STRING(""));
            retcode = global_retcode;
            break;
        case ATCK_TRIP:
            retcode = 0;
            break;
        }
        if (attacktype)
            return retcode;
    }

    if (dt == TYPE_UNDEFINED)
    {
        dt = TYPE_HIT;
        if (wield && wield->item_type == ITEM_WEAPON)
            dt += wield->value[3];

        if (wield && wield->item_type == ITEM_WEAPON && wield->value[3] == WEAPON_DUAL_LIGHTSABER)
            dt = (TYPE_HIT + WEAPON_LIGHTSABER);
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    thac0_00 = 20;
    thac0_32 = 10;
    thac0 = interpolate(ch->skill_level[COMBAT_ABILITY], thac0_00, thac0_32) - GET_HITROLL(ch);
    victim_ac = (int)(GET_AC(victim) / 10);

    /* if you can't see what's coming... */
    if (wield && !can_see_obj(victim, wield))
        victim_ac += 1;
    if (!can_see(ch, victim))
        victim_ac -= 4;

    if (ch->race == RACE_DEFEL)
        victim_ac += 2;

    if (!IS_AWAKE(victim))
        victim_ac += 5;

    /* Weapon proficiency bonus */
    victim_ac += prof_bonus / 20;

    /* No more beating up stunned players */
    if (IS_AFFECTED(victim, AFF_PARALYSIS))
        affect_strip(ch, gsn_stun);

    /*
     * The moment of excitement!
     */
    diceroll = number_range(1, 20);

    if (diceroll == 1 || (diceroll < 20 && diceroll < thac0 - victim_ac))
    {
        /* Miss. */
        if (prof_gsn != -1)
            learn_from_failure(ch, prof_gsn);
        damage(ch, victim, 0, dt);
        return rNONE;
    }

    /*
     * Hit.
     * Calc damage.
     */

    if (!wield) /* dice formula fixed by Thoric */
        dam = number_range(ch->barenumdie, ch->baresizedie * ch->barenumdie) + ch->damplus;
    else
        dam = number_range(wield->value[1], wield->value[2]);

    /*
     * Bonuses.
     */

    dam += GET_DAMROLL(ch);

    if (prof_bonus)
        dam *= (1 + prof_bonus / 100);

    if (!IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0)
    {
        dam += (int)(dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
        learn_from_success(ch, gsn_enhanced_damage);
    }

    if (!IS_AWAKE(victim))
        dam *= 2;
    if (dt == gsn_backstab || dt == gsn_dualstab)
        dam *= (2 + URANGE(2, ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY] / 4), 30) / 8);

    if (dt == gsn_circle)
        dam *= (2 + URANGE(2, ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY] / 4), 30) / 16);

    plusris = 0;

    if (wield)
    {
        if (IS_SET(wield->extra_flags, ITEM_MAGIC))
            dam = ris_damage(victim, dam, RIS_MAGIC);
        else
            dam = ris_damage(victim, dam, RIS_NONMAGIC);

        /*
         * Handle PLUS1 - PLUS6 ris bits vs. weapon hitroll	-Thoric
         */
        plusris = obj_hitroll(wield);
    }
    else
        dam = ris_damage(victim, dam, RIS_NONMAGIC);

    /* check for RIS_PLUSx 					-Thoric */
    if (dam)
    {
        int x, res, imm, sus, mod;

        if (plusris)
            plusris = RIS_PLUS1 << UMIN(plusris, 7);

        /* initialize values to handle a zero plusris */
        imm = res = -1;
        sus = 1;

        /* find high ris */
        for (x = RIS_PLUS1; x <= RIS_PLUS6; x <<= 1)
        {
            if (IS_SET(victim->immune, x))
                imm = x;
            if (IS_SET(victim->resistant, x))
                res = x;
            if (IS_SET(victim->susceptible, x))
                sus = x;
        }
        mod = 10;
        if (imm >= plusris)
            mod -= 10;
        if (res >= plusris)
            mod -= 2;
        if (sus <= plusris)
            mod += 2;

        /* check if immune */
        if (mod <= 0)
            dam = -1;
        if (mod != 10)
            dam = (dam * mod) / 10;
    }

    /* race modifier */

    /*    if ( victim->race == RACE_DUINUOGWUIN )
           dam /= 5;    */

    // Jedi have a difficult time wielding a dualsaber

    if (ch->force_type == FORCE_JEDI && wield && wield->value[3] == WEAPON_DUAL_LIGHTSABER)
    {
        if (number_range(0, 5) == 4)
        {
            send_to_char("You are untrained in the use of dual-bladed lightsabers, and fumble with it.\n\r", ch);
            act(AT_YELLOW, "$n fumbles with their dual-bladed lightsaber.", ch, nullptr, nullptr, TO_ROOM);
            return rNONE;
        }
    }

    /*
     * check to see if weapon is charged
     */

    if (dt == (TYPE_HIT + WEAPON_BLASTER) && wield && wield->item_type == ITEM_WEAPON)
    {
        if (wield->value[4] < 1)
        {
            act(AT_YELLOW, "$n points their blaster at you but nothing happens.", ch, nullptr, victim, TO_VICT);
            act(AT_YELLOW, "*CLICK* ... your blaster needs a new ammunition cell!", ch, nullptr, victim, TO_CHAR);
            if (IS_NPC(ch))
            {
                do_remove(ch, wield->name);
            }
            return rNONE;
        }
        else if (wield->blaster_setting == BLASTER_FULL && wield->value[4] >= 5)
        {
            dam *= 1.5;
            wield->value[4] -= 5;
        }
        else if (wield->blaster_setting == BLASTER_HIGH && wield->value[4] >= 4)
        {
            dam *= 1.25;
            wield->value[4] -= 4;
        }
        else if (wield->blaster_setting == BLASTER_NORMAL && wield->value[4] >= 3)
        {
            wield->value[4] -= 3;
        }
        else if (wield->blaster_setting == BLASTER_STUN && wield->value[4] >= 5)
        {
            dam /= 10;
            wield->value[4] -= 3;
            fail = false;
            chance = ris_save(victim, ch->skill_level[COMBAT_ABILITY], RIS_PARALYSIS);
            if (chance == 1000)
                fail = true;
            else
                fail = saves_para_petri(chance, victim);
            if (victim->was_stunned > 0)
            {
                fail = true;
                victim->was_stunned--;
            }
            chance = 100 - get_curr_con(victim) - victim->skill_level[COMBAT_ABILITY] / 2;
            /* harder for player to stun another player */
            if (!IS_NPC(ch) && !IS_NPC(victim))
                chance -= sysdata.stun_plr_vs_plr;
            else
                chance -= sysdata.stun_regular;
            chance = URANGE(5, chance, 95);
            if (!fail && number_percent() < chance)
            {
                WAIT_STATE(victim, PULSE_VIOLENCE);
                act(AT_BLUE, "Blue rings of energy from $N's blaster knock you down leaving you stunned!", victim, nullptr,
                    ch, TO_CHAR);
                act(AT_BLUE, "Blue rings of energy from your blaster strike $N, leaving $M stunned!", ch, nullptr, victim,
                    TO_CHAR);
                act(AT_BLUE, "Blue rings of energy from $n's blaster hit $N, leaving $M stunned!", ch, nullptr, victim,
                    TO_NOTVICT);
                stop_fighting(victim, true);
                if (!IS_AFFECTED(victim, AFF_PARALYSIS))
                {
                    af.type = gsn_stun;
                    af.location = APPLY_AC;
                    af.modifier = 20;
                    af.duration = 7;
                    af.bitvector = AFF_PARALYSIS;
                    affect_to_char(victim, &af);
                    update_pos(victim);
                    if (IS_NPC(victim))
                    {
                        start_hating(victim, ch);
                        start_hunting(victim, ch);
                        victim->was_stunned = 10;
                    }
                }
            }
            else
            {
                act(AT_BLUE, "Blue rings of energy from $N's blaster hit you but have little effect", victim, nullptr, ch,
                    TO_CHAR);
                act(AT_BLUE, "Blue rings of energy from your blaster hit $N, but nothing seems to happen!", ch, nullptr,
                    victim, TO_CHAR);
                act(AT_BLUE, "Blue rings of energy from $n's blaster hit $N, but nothing seems to happen!", ch, nullptr,
                    victim, TO_NOTVICT);
            }
        }
        else if (wield->blaster_setting == BLASTER_HALF && wield->value[4] >= 2)
        {
            dam *= 0.75;
            wield->value[4] -= 2;
        }
        else
        {
            dam *= 0.5;
            wield->value[4] -= 1;
        }
    }
    else if (dt == (TYPE_HIT + WEAPON_VIBRO_BLADE) && wield && wield->item_type == ITEM_WEAPON)
    {
        if (wield->value[4] < 1)
        {
            act(AT_YELLOW, "Your vibro-blade needs recharging ...", ch, nullptr, victim, TO_CHAR);
            dam /= 3;
        }
    }
    else if (dt == (TYPE_HIT + WEAPON_FORCE_PIKE) && wield && wield->item_type == ITEM_WEAPON)
    {
        if (wield->value[4] < 1)
        {
            act(AT_YELLOW, "Your force-pike needs recharging ...", ch, nullptr, victim, TO_CHAR);
            dam /= 2;
        }
        else
            wield->value[4]--;
    }
    else if (dt == (TYPE_HIT + WEAPON_LIGHTSABER) && wield && wield->item_type == ITEM_WEAPON)
    {
        if (wield->value[4] < 1)
        {
            act(AT_YELLOW, "$n waves a dead hand grip around in the air.", ch, nullptr, victim, TO_VICT);
            act(AT_YELLOW, "You need to recharge your lightsaber ... it seems to be lacking a blade.", ch, nullptr, victim,
                TO_CHAR);
            if (IS_NPC(ch))
            {
                do_remove(ch, wield->name);
            }
            return rNONE;
        }
    }
    else if (dt == (TYPE_HIT + WEAPON_BOWCASTER) && wield && wield->item_type == ITEM_WEAPON)
    {
        if (wield->value[4] < 1)
        {
            act(AT_YELLOW, "$n points their bowcaster at you but nothing happens.", ch, nullptr, victim, TO_VICT);
            act(AT_YELLOW, "*CLICK* ... your bowcaster needs a new bolt cartridge!", ch, nullptr, victim, TO_CHAR);
            if (IS_NPC(ch))
            {
                do_remove(ch, wield->name);
            }
            return rNONE;
        }
        else
            wield->value[4]--;
    }

    if (dam <= 0)
        dam = 1;

    if (prof_gsn != -1)
    {
        if (dam > 0)
            learn_from_success(ch, prof_gsn);
        else
            learn_from_failure(ch, prof_gsn);
    }

    /* immune to damage */
    if (dam == -1)
    {
        if (dt >= 0 && dt < top_sn)
        {
            SKILL_TYPE* skill = skill_table[dt];
            bool found = false;

            if (skill->imm_char && skill->imm_char[0] != '\0')
            {
                act(AT_HIT, skill->imm_char, ch, nullptr, victim, TO_CHAR);
                found = true;
            }
            if (skill->imm_vict && skill->imm_vict[0] != '\0')
            {
                act(AT_HITME, skill->imm_vict, ch, nullptr, victim, TO_VICT);
                found = true;
            }
            if (skill->imm_room && skill->imm_room[0] != '\0')
            {
                act(AT_ACTION, skill->imm_room, ch, nullptr, victim, TO_NOTVICT);
                found = true;
            }
            if (found)
                return rNONE;
        }
        dam = 0;
    }

    if (dt == gsn_ambush)
        dam *= 2;

    if ((retcode = damage(ch, victim, dam, dt)) != rNONE)
        return retcode;
    if (char_died(ch))
        return rCHAR_DIED;
    if (char_died(victim))
        return rVICT_DIED;

    retcode = rNONE;
    if (dam == 0)
        return retcode;

    /* weapon spells	-Thoric */
    if (wield && !IS_SET(victim->immune, RIS_MAGIC) && !IS_SET(victim->in_room->room_flags, ROOM_NO_MAGIC))
    {
        AFFECT_DATA* aff;

        for (aff = wield->pIndexData->first_affect; aff; aff = aff->next)
            if (aff->location == APPLY_WEAPONSPELL && IS_VALID_SN(aff->modifier) &&
                skill_table[aff->modifier]->spell_fun)
                retcode = (*skill_table[aff->modifier]->spell_fun)(aff->modifier, (wield->level + 3) / 3, ch, victim);
        if (retcode != rNONE || char_died(ch) || char_died(victim))
            return retcode;
        for (aff = wield->first_affect; aff; aff = aff->next)
            if (aff->location == APPLY_WEAPONSPELL && IS_VALID_SN(aff->modifier) &&
                skill_table[aff->modifier]->spell_fun)
                retcode = (*skill_table[aff->modifier]->spell_fun)(aff->modifier, (wield->level + 3) / 3, ch, victim);
        if (retcode != rNONE || char_died(ch) || char_died(victim))
            return retcode;
    }

    /*
     * magic shields that retaliate				-Thoric
     */
    if (IS_AFFECTED(victim, AFF_FIRESHIELD) && !IS_AFFECTED(ch, AFF_FIRESHIELD))
        retcode = spell_fireball(gsn_fireball, off_shld_lvl(victim, ch), victim, ch);
    if (retcode != rNONE || char_died(ch) || char_died(victim))
        return retcode;

    if (retcode != rNONE || char_died(ch) || char_died(victim))
        return retcode;

    if (IS_AFFECTED(victim, AFF_SHOCKSHIELD) && !IS_AFFECTED(ch, AFF_SHOCKSHIELD))
        retcode = spell_lightning_bolt(gsn_lightning_bolt, off_shld_lvl(victim, ch), victim, ch);
    if (retcode != rNONE || char_died(ch) || char_died(victim))
        return retcode;

    /*
     *   folks with blasters move and snipe instead of getting neatin up in one spot.
     */
    /*
         if ( IS_NPC(victim) )
         {
             OBJ_DATA *wield;

             wield = get_eq_char( victim, WEAR_WIELD );
             if ( wield != nullptr && wield->value[3] == WEAPON_BLASTER && get_cover( victim ) == true )
             {
                   start_hating( victim, ch );
               start_hunting( victim, ch );
             }
         }
      */
    return retcode;
}

/*
 * Calculate damage based on resistances, immunities and suceptibilities
 *					-Thoric
 */
sh_int ris_damage(CHAR_DATA* ch, sh_int dam, int ris)
{
    sh_int modifier;

    modifier = 10;
    if (IS_SET(ch->immune, ris))
        modifier -= 10;
    if (IS_SET(ch->resistant, ris))
        modifier -= 2;
    if (IS_SET(ch->susceptible, ris))
        modifier += 2;
    if (modifier <= 0)
        return -1;
    if (modifier == 10)
        return dam;
    return (dam * modifier) / 10;
}

/*
 * Inflict damage from a hit.
 */
ch_ret damage(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt)
{
    CHAR_DATA* gch;
    char buf1[MAX_STRING_LENGTH];
    sh_int dameq;
    int room;
    bool npcvict;
    bool loot;
    int xp_gain;
    OBJ_DATA *damobj, *wield, *victwield;
    ch_ret retcode;
    sh_int dampmod;

    int init_gold, new_gold, gold_diff;
    int nocorpse = 0;
    retcode = rNONE;

    if (!ch)
    {
        bug("Damage: null ch!", 0);
        return rERROR;
    }
    if (!victim)
    {
        bug("Damage: null victim!", 0);
        return rVICT_DIED;
    }

    if (victim->position == POS_DEAD)
        return rVICT_DIED;

    npcvict = IS_NPC(victim);

    /*
     * Check damage types for RIS				-Thoric
     */
    if (dam && dt != TYPE_UNDEFINED)
    {
        if (IS_FIRE(dt))
            dam = ris_damage(victim, dam, RIS_FIRE);
        else if (IS_COLD(dt))
            dam = ris_damage(victim, dam, RIS_COLD);
        else if (IS_ACID(dt))
            dam = ris_damage(victim, dam, RIS_ACID);
        else if (IS_ELECTRICITY(dt))
            dam = ris_damage(victim, dam, RIS_ELECTRICITY);
        else if (IS_ENERGY(dt))
            dam = ris_damage(victim, dam, RIS_ENERGY);
        else if (IS_DRAIN(dt))
            dam = ris_damage(victim, dam, RIS_DRAIN);
        else if (dt == gsn_poison || IS_POISON(dt))
            dam = ris_damage(victim, dam, RIS_POISON);
        else if (dt == (TYPE_HIT + 7) || dt == (TYPE_HIT + 8))
            dam = ris_damage(victim, dam, RIS_BLUNT);
        else if (dt == (TYPE_HIT + 2) || dt == (TYPE_HIT + 11) || dt == (TYPE_HIT + 10))
            dam = ris_damage(victim, dam, RIS_PIERCE);
        else if (dt == (TYPE_HIT + 1) || dt == (TYPE_HIT + 3) || dt == (TYPE_HIT + 4) || dt == (TYPE_HIT + 5))
            dam = ris_damage(victim, dam, RIS_SLASH);

        if (dam == -1)
        {
            if (dt >= 0 && dt < top_sn)
            {
                bool found = false;
                SKILL_TYPE* skill = skill_table[dt];

                if (skill->imm_char && skill->imm_char[0] != '\0')
                {
                    act(AT_HIT, skill->imm_char, ch, nullptr, victim, TO_CHAR);
                    found = true;
                }
                if (skill->imm_vict && skill->imm_vict[0] != '\0')
                {
                    act(AT_HITME, skill->imm_vict, ch, nullptr, victim, TO_VICT);
                    found = true;
                }
                if (skill->imm_room && skill->imm_room[0] != '\0')
                {
                    act(AT_ACTION, skill->imm_room, ch, nullptr, victim, TO_NOTVICT);
                    found = true;
                }
                if (found)
                    return rNONE;
            }
            dam = 0;
        }
    }

    if (dam && npcvict && ch != victim)
    {
        if (!IS_SET(victim->act, ACT_SENTINEL))
        {
            if (victim->hunting)
            {
                if (victim->hunting->who != ch)
                {
                    STRFREE(victim->hunting->name);
                    victim->hunting->name = QUICKLINK(ch->name);
                    victim->hunting->who = ch;
                }
            }
            else
                start_hunting(victim, ch);
        }

        if (victim->hating)
        {
            if (victim->hating->who != ch)
            {
                STRFREE(victim->hating->name);
                victim->hating->name = QUICKLINK(ch->name);
                victim->hating->who = ch;
            }
        }
        else
            start_hating(victim, ch);
    }

    if (victim != ch)
    {
        /*
         * Certain attacks are forbidden.
         * Most other attacks are returned.
         */
        if (is_safe(ch, victim))
            return rNONE;

        if (victim->position > POS_STUNNED && dt != TYPE_MISSILE)
        {
            if (!victim->fighting)
                set_fighting(victim, ch);
            if (victim->fighting)
                victim->position = POS_FIGHTING;
        }

        if (victim->position > POS_STUNNED && dt != TYPE_MISSILE)
        {
            if (!ch->fighting)
                set_fighting(ch, victim);

            /*
             * If victim is charmed, ch might attack victim's master.
             */
            if (IS_NPC(ch) && npcvict && IS_AFFECTED(victim, AFF_CHARM) && victim->master &&
                victim->master->in_room == ch->in_room && number_bits(3) == 0)
            {
                stop_fighting(ch, false);
                retcode = multi_hit(ch, victim->master, TYPE_UNDEFINED);
                return retcode;
            }
        }

        /*
         * More charm stuff.
         */
        if (victim->master == ch)
            stop_follower(victim);

        /*
         * Inviso attacks ... not.
         */
        if (IS_AFFECTED(ch, AFF_INVISIBLE) && ch->race != RACE_DEFEL)
        {
            affect_strip(ch, gsn_invis);
            affect_strip(ch, gsn_mass_invis);
            REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
            act(AT_MAGIC, "$n fades into existence.", ch, nullptr, nullptr, TO_ROOM);
        }

        /* Take away Hide */
        if (IS_AFFECTED(ch, AFF_HIDE))
            REMOVE_BIT(ch->affected_by, AFF_HIDE);
        /*
         * Damage modifiers.
         */
        if (IS_AFFECTED(victim, AFF_SANCTUARY))
            dam /= 2;

        if (IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch))
            dam -= (int)(dam / 4);

        if (dam < 0)
            dam = 0;

        /*
         * Check for disarm, trip, parry, and dodge.
         */
        if (dt >= TYPE_HIT)
        {
            if (IS_NPC(ch) && IS_SET(ch->attacks, DFND_DISARM) &&
                number_percent() < ch->skill_level[COMBAT_ABILITY] / 2)
                disarm(ch, victim);

            if (IS_NPC(ch) && IS_SET(ch->attacks, ATCK_TRIP) && number_percent() < ch->skill_level[COMBAT_ABILITY])
                trip(ch, victim);

            if (check_parry(ch, victim))
            {
                if (!IS_NPC(ch) && ch->pcdata->learned[gsn_reflect] &&
                    (ch->pcdata->learned[gsn_reflect] / 2) > number_percent())
                {
                    if ((wield = get_eq_char(ch, WEAR_WIELD)) != nullptr)
                    {
                        if ((victwield = get_eq_char(victim, WEAR_WIELD)) != nullptr)
                        {
                            if ((victwield->value[3] == WEAPON_LIGHTSABER ||
                                 victwield->value[3] == WEAPON_DUAL_LIGHTSABER) &&
                                wield->value[3] == WEAPON_BLASTER)
                            {
                                act(AT_WHITE, "You swing your lightsaber and reflect the blaster bolt back at $n!", ch,
                                    nullptr, victim, TO_VICT);
                                act(AT_WHITE, "$N swings $s lightsaber and reflects the blast back at you!", ch, nullptr,
                                    victim, TO_CHAR);
                                act(AT_WHITE, "$N swings $s lightsaber and reflects the blast back at $N!", ch, nullptr,
                                    victim, TO_NOTVICT);
                                ch->hit -= wield->value[1];
                                dam_message(victim, ch, wield->value[1], (TYPE_HIT + WEAPON_BLASTER));
                                learn_from_success(victim, gsn_reflect);
                            }
                        }
                    }
                }
                else
                    return rNONE;
            }
            if (check_dodge(ch, victim))
                return rNONE;
            if (check_reflect(ch, victim, dam))
                return rNONE;
        }

        if (dam > 0 && dt > TYPE_HIT && !IS_AFFECTED(victim, AFF_POISON) && is_wielding_poisoned(ch) &&
            !IS_SET(victim->immune, RIS_POISON) && !saves_poison_death(ch->skill_level[COMBAT_ABILITY], victim))
        {
            AFFECT_DATA af;

            af.type = gsn_poison;
            af.duration = 20;
            af.location = APPLY_STR;
            af.modifier = -2;
            af.bitvector = AFF_POISON;
            affect_join(victim, &af);
            victim->mental_state = URANGE(20, victim->mental_state + 2, 100);
            dam = dam + (dam / 2);
        }

        /*
         * Check control panel settings and modify damage
         */
        if (IS_NPC(ch))
        {
            if (npcvict)
                dampmod = sysdata.dam_mob_vs_mob;
            else
                dampmod = sysdata.dam_mob_vs_plr;
        }
        else
        {
            if (npcvict)
                dampmod = sysdata.dam_plr_vs_mob;
            else
                dampmod = sysdata.dam_plr_vs_plr;
        }
        if (dampmod > 0)
            dam = (dam * dampmod) / 100;

        dam_message(ch, victim, dam, dt);
    }

    /*
     * Code to handle equipment getting damaged, and also support  -Thoric
     * bonuses/penalties for having or not having equipment where hit
     */
    if (dam > 10 && dt != TYPE_UNDEFINED && !IS_SET(ch->in_room->room_flags2, ROOM_ARENA))
    {
        /* get a random body eq part */
        dameq = number_range(WEAR_LIGHT, WEAR_EYES);
        damobj = get_eq_char(victim, dameq);
        if (damobj)
        {
            if (dam > get_obj_resistance(damobj))
            {
                set_cur_obj(damobj);
                damage_obj(damobj);
            }
            dam -= 5; /* add a bonus for having something to block the blow */
        }
        else
            dam += 5; /* add penalty for bare skin! */
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */

    victim->hit -= dam;

    /*
     * Get experience based on % of damage done			-Thoric
     */
    if (dam && ch != victim && !IS_NPC(ch) && ch->fighting && ch->fighting->xp)
    {
        xp_gain = (int)(xp_compute(ch, victim) * 0.1 * dam) / victim->max_hit;
        gain_exp(ch, xp_gain, COMBAT_ABILITY);
    }

    if (!IS_NPC(victim) && victim->top_level >= LEVEL_IMMORTAL && victim->hit < 1)
        victim->hit = 1;

    /* Make sure newbies dont die */

    if (!IS_NPC(victim) && NOT_AUTHED(victim) && victim->hit < 1)
        victim->hit = 1;

    if (victim->hit < 1 && !IS_NPC(victim) && IS_SET(victim->in_room->room_flags2, ROOM_ARENA))
    {
        char_from_room(victim);
        char_to_room(victim, victim->pcdata->roomarena);
        victim->hit = victim->max_hit;
        victim->mana = victim->max_mana;
        if (num_in_arena() == 1)
            find_game_winner();
        do_look(victim, MAKE_TEMP_STRING("auto"));
        stop_fighting(victim, true);
        lost_arena(victim);
        return rNONE;
    }

    if (!npcvict && get_trust(victim) >= LEVEL_IMMORTAL && get_trust(ch) >= LEVEL_IMMORTAL && victim->hit < 1)
        victim->hit = 1;
    update_pos(victim);

    switch (victim->position)
    {
    case POS_MORTAL:
        act(AT_DYING, "$n is mortally wounded, and will die soon, if not aided.", victim, nullptr, nullptr, TO_ROOM);
        send_to_char("&RYou are mortally wounded, and will die soon, if not aided.", victim);
        break;

    case POS_INCAP:
        act(AT_DYING, "$n is incapacitated and will slowly die, if not aided.", victim, nullptr, nullptr, TO_ROOM);
        send_to_char("&RYou are incapacitated and will slowly die, if not aided.", victim);
        break;

    case POS_STUNNED:
        if (!IS_AFFECTED(victim, AFF_PARALYSIS))
        {
            act(AT_ACTION, "$n is stunned, but will probably recover.", victim, nullptr, nullptr, TO_ROOM);
            send_to_char("&RYou are stunned, but will probably recover.", victim);
        }
        break;

    case POS_DEAD:
        if (dt >= 0 && dt < top_sn)
        {
            SKILL_TYPE* skill = skill_table[dt];

            if (skill->die_char && skill->die_char[0] != '\0')
                act(AT_DEAD, skill->die_char, ch, nullptr, victim, TO_CHAR);
            if (skill->die_vict && skill->die_vict[0] != '\0')
                act(AT_DEAD, skill->die_vict, ch, nullptr, victim, TO_VICT);
            if (skill->die_room && skill->die_room[0] != '\0')
                act(AT_DEAD, skill->die_room, ch, nullptr, victim, TO_NOTVICT);
        }
        if (IS_NPC(victim) && IS_SET(victim->act, ACT_NOKILL))
            act(AT_YELLOW, "$n flees for $s life ... barely escaping certain death!", victim, 0, 0, TO_ROOM);
        else if (IS_NPC(victim) && IS_SET(victim->act, ACT_DROID))
            act(AT_DEAD, "$n EXPLODES into many small pieces!", victim, 0, 0, TO_ROOM);
        else
            act(AT_DEAD, "$n is DEAD!", victim, 0, 0, TO_ROOM);
        send_to_char("&WYou have been KILLED!\n\r", victim);
        break;

    default:
        if (dam > victim->max_hit / 4)
        {
            act(AT_HURT, "That really did HURT!", victim, 0, 0, TO_CHAR);
            if (number_bits(3) == 0)
                worsen_mental_state(ch, 1);
        }
        if (victim->hit < victim->max_hit / 4)

        {
            act(AT_DANGER, "You wish that your wounds would stop BLEEDING so much!", victim, 0, 0, TO_CHAR);
            if (number_bits(2) == 0)
                worsen_mental_state(ch, 1);
        }
        break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if (!IS_AWAKE(victim) /* lets make NPC's not slaughter PC's */
        && !IS_AFFECTED(victim, AFF_PARALYSIS))
    {
        if (victim->fighting && victim->fighting->who->hunting && victim->fighting->who->hunting->who == victim)
            stop_hunting(victim->fighting->who);

        if (victim->fighting && victim->fighting->who->hating && victim->fighting->who->hating->who == victim)
            stop_hating(victim->fighting->who);

        stop_fighting(victim, true);
    }

    if (victim->hit <= 0 && !IS_NPC(victim))
    {
        OBJ_DATA* obj;
        OBJ_DATA* obj_next;
        int cnt = 0;

        REMOVE_BIT(victim->act, PLR_ATTACKER);

        stop_fighting(victim, true);

        if ((obj = get_eq_char(victim, WEAR_DUAL_WIELD)) != nullptr)
            unequip_char(victim, obj);
        if ((obj = get_eq_char(victim, WEAR_WIELD)) != nullptr)
            unequip_char(victim, obj);
        if ((obj = get_eq_char(victim, WEAR_HOLD)) != nullptr)
            unequip_char(victim, obj);
        if ((obj = get_eq_char(victim, WEAR_MISSILE_WIELD)) != nullptr)
            unequip_char(victim, obj);
        if ((obj = get_eq_char(victim, WEAR_LIGHT)) != nullptr)
            unequip_char(victim, obj);

        for (obj = victim->first_carrying; obj; obj = obj_next)
        {
            obj_next = obj->next_content;

            if (obj->wear_loc == WEAR_NONE)
            {
                if (obj->pIndexData->progtypes & DROP_PROG && obj->count > 1)
                {
                    ++cnt;
                    separate_obj(obj);
                    obj_from_char(obj);
                    if (!obj_next)
                        obj_next = victim->first_carrying;
                }
                else
                {
                    cnt += obj->count;
                    obj_from_char(obj);
                }
                act(AT_ACTION, "$n drops $p.", victim, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You drop $p.", victim, obj, nullptr, TO_CHAR);
                obj = obj_to_room(obj, victim->in_room);
            }
        }

        if (IS_NPC(ch) && !IS_NPC(victim))
        {
            long lose_exp;
            lose_exp = UMAX((victim->experience[COMBAT_ABILITY] - exp_level(victim->skill_level[COMBAT_ABILITY])), 0);
            ch_printf(victim, "You lose %ld experience.\n\r", lose_exp);
            victim->experience[COMBAT_ABILITY] -= lose_exp;
        }

        add_timer(victim, TIMER_RECENTFIGHT, 100, nullptr, 0);
    }

    /*
     * Payoff for killing things.
     */
    if (victim->position == POS_DEAD)
    {
        group_gain(ch, victim);

        if (!npcvict)
        {
            sprintf_s(log_buf, "%s killed by %s at %d", victim->name, (IS_NPC(ch) ? ch->short_descr : ch->name),
                      victim->in_room->vnum);
            log_string(log_buf);
            to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
        }
        else if (!IS_NPC(ch) && IS_NPC(victim)) /* keep track of mob vnum killed */
        {
            add_kill(ch, victim);

            /*
             * Add to kill tracker for grouped chars, as well. -Halcyon
             */
            for (gch = ch->in_room->first_person; gch; gch = gch->next_in_room)
                if (is_same_group(gch, ch) && !IS_NPC(gch) && gch != ch)
                    add_kill(gch, victim);
        }

        check_killer(ch, victim);

        if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
            loot = legal_loot(ch, victim);
        else
            loot = false;
        if (IS_SET(victim->act, ACT_NOCORPSE))
            nocorpse = 1;
        // Make sure snipe doesnt loot.
        room = victim->in_room->vnum;
        set_cur_char(victim);
        raw_kill(ch, victim);
        victim = nullptr;

        if (!IS_NPC(ch) && loot)
        {
            /* Autogold by Scryn 8/12 */
            if (IS_SET(ch->act, PLR_AUTOGOLD))
            {
                init_gold = ch->gold;
                do_get(ch, MAKE_TEMP_STRING("credits corpse"));
                new_gold = ch->gold;
                gold_diff = (new_gold - init_gold);
                if (gold_diff > 0)
                {
                    sprintf_s(buf1, "%d", gold_diff);
                    do_split(ch, buf1);
                }
            }
            if (room && ch && ch->in_room && ch->in_room->vnum == room)
            {
                if (IS_SET(ch->act, PLR_AUTOLOOT) && nocorpse == 0)
                    do_get(ch, MAKE_TEMP_STRING("all corpse"));
                else if (nocorpse == 0)
                    do_look(ch, MAKE_TEMP_STRING("in corpse"));

                if (IS_SET(ch->act, PLR_AUTOSAC))
                    do_sacrifice(ch, MAKE_TEMP_STRING("corpse"));
            }
        }

        if (IS_SET(sysdata.save_flags, SV_KILL))
            save_char_obj(ch);
        return rVICT_DIED;
    }

    if (victim == ch)
        return rNONE;

    /*
     * Take care of link dead people.
     */
    if (!npcvict && !victim->desc && !victim->switched)
    {
        if (number_range(0, victim->wait) == 0)
        {
            do_flee(victim, MAKE_TEMP_STRING(""));
            do_flee(victim, MAKE_TEMP_STRING(""));
            do_flee(victim, MAKE_TEMP_STRING(""));
            do_flee(victim, MAKE_TEMP_STRING(""));
            do_flee(victim, MAKE_TEMP_STRING(""));
            do_hail(victim, MAKE_TEMP_STRING(""));
            do_quit(victim, MAKE_TEMP_STRING(""));
            return rNONE;
        }
    }

    /*
     * Wimp out?
     */
    if (npcvict && dam > 0)
    {
        if ((IS_SET(victim->act, ACT_WIMPY) && number_bits(1) == 0 && victim->hit < victim->max_hit / 2) ||
            (IS_AFFECTED(victim, AFF_CHARM) && victim->master && victim->master->in_room != victim->in_room))
        {
            start_fearing(victim, ch);
            stop_hunting(victim);
            // do_flee( victim, "" );
        }
    }

    /* player wimpy deprecated in FotE.

        if ( !npcvict
        &&   victim->hit > 0
        &&   victim->hit <= victim->wimpy
        &&   victim->wait == 0 )
        do_flee( victim, "" );
    */

    return rNONE;
}

bool is_safe(CHAR_DATA* ch, CHAR_DATA* victim)
{
    if (!victim)
        return false;

    /* Thx Josh! */
    if (who_fighting(ch) == ch)
        return false;

    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
    {
        set_char_color(AT_MAGIC, ch);
        send_to_char("You'll have to do that elswhere.\n\r", ch);
        return true;
    }

    if (get_trust(ch) > LEVEL_HERO)
        return false;

    if (IS_NPC(ch) || IS_NPC(victim))
        return false;

    return false;
}

/* checks is_safe but without the output
   cuts out imms and safe rooms as well
   for info only */

bool is_safe_nm(CHAR_DATA* ch, CHAR_DATA* victim)
{
    return false;
}

/*
 * just verify that a corpse looting is legal
 */
bool legal_loot(CHAR_DATA* ch, CHAR_DATA* victim)
{
    /* pc's can now loot .. why not .. death is pretty final */
    if (!IS_NPC(ch))
        return true;
    /* non-charmed mobs can loot anything */
    if (IS_NPC(ch) && !ch->master)
        return true;

    return false;
}

/*
see if an attack justifies a KILLER flag --- edited so that none do but can't
murder a no pk person. --- edited again for planetary wanted flags -- well will be soon :p
 */

void check_killer(CHAR_DATA* ch, CHAR_DATA* victim)
{

    //    int x;

    /*
     * Charm-o-rama.
     */
    if (IS_SET(ch->affected_by, AFF_CHARM))
    {
        if (!ch->master)
        {
            char buf[MAX_STRING_LENGTH];

            sprintf_s(buf, "Check_killer: %s bad AFF_CHARM", IS_NPC(ch) ? ch->short_descr : ch->name);
            bug(buf, 0);
            affect_strip(ch, gsn_charm_person);
            REMOVE_BIT(ch->affected_by, AFF_CHARM);
            return;
        }

        /* stop_follower( ch ); */
        if (ch->master)
            check_killer(ch->master, victim);
    }

    if (IS_NPC(victim))
    {
        if (!IS_NPC(ch))
        {

            /* Commented out for the outlaw command.

                  for ( x = 0; x < 32; x++ )
                  {
                      if ( IS_SET(victim->vip_flags , 1 << x ) )
                      {
                         SET_BIT(ch->pcdata->wanted_flags, 1 << x );
                         ch_printf( ch, "&YYou are now wanted on %s.&w\n\r", planet_flags[x] , victim->short_descr );
                      }
                  }

            */
            if (ch->pcdata->clan)
                ch->pcdata->clan->mkills++;
            ch->pcdata->mkills++;
            if (ch->in_room->area)
                ch->in_room->area->mkills++;
        }
        return;
    }

    if (!IS_NPC(ch) && !IS_NPC(victim))
    {
        if (ch->pcdata->clan)
            ch->pcdata->clan->pkills++;
        ch->pcdata->pkills++;
        update_pos(victim);
        if (victim->pcdata->clan)
            victim->pcdata->clan->pdeaths++;
    }

    if (IS_NPC(ch))
        if (!IS_NPC(victim) && victim->in_room && victim->in_room->area)
            victim->in_room->area->mdeaths++;

    return;
}

/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA* victim)
{
    if (!victim)
    {
        bug("update_pos: null victim", 0);
        return;
    }

    if (victim->hit > 0)
    {
        if (victim->position <= POS_STUNNED)
            victim->position = POS_STANDING;
        if (IS_AFFECTED(victim, AFF_PARALYSIS))
            victim->position = POS_STUNNED;
        return;
    }

    if (IS_NPC(victim) || victim->hit <= -500)
    {
        if (victim->mount)
        {
            act(AT_ACTION, "$n falls from $N.", victim, nullptr, victim->mount, TO_ROOM);
            REMOVE_BIT(victim->mount->act, ACT_MOUNTED);
            victim->mount = nullptr;
        }
        victim->position = POS_DEAD;
        return;
    }

    if (victim->hit <= -400)
        victim->position = POS_MORTAL;
    else if (victim->hit <= -200)
        victim->position = POS_INCAP;
    else
        victim->position = POS_STUNNED;

    if (victim->position > POS_STUNNED && IS_AFFECTED(victim, AFF_PARALYSIS))
        victim->position = POS_STUNNED;

    if (victim->mount)
    {
        act(AT_ACTION, "$n falls unconscious from $N.", victim, nullptr, victim->mount, TO_ROOM);
        REMOVE_BIT(victim->mount->act, ACT_MOUNTED);
        victim->mount = nullptr;
    }
    return;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA* ch, CHAR_DATA* victim)
{
    FIGHT_DATA* fight;

    if (ch->fighting)
    {
        char buf[MAX_STRING_LENGTH];

        sprintf_s(buf, "Set_fighting: %s -> %s (already fighting %s)", ch->name, victim->name, ch->fighting->who->name);
        bug(buf, 0);
        return;
    }

    if (IS_AFFECTED(ch, AFF_SLEEP))
        affect_strip(ch, gsn_sleep);

    /* Limit attackers -Thoric */
    if (victim->num_fighting > max_fight(victim))
    {
        send_to_char("There are too many people fighting for you to join in.\n\r", ch);
        return;
    }

    CREATE(fight, FIGHT_DATA, 1);
    fight->who = victim;
    fight->xp = (int)xp_compute(ch, victim);
    fight->align = align_compute(ch, victim);
    if (!IS_NPC(ch) && IS_NPC(victim))
        fight->timeskilled = times_killed(ch, victim);
    ch->num_fighting = 1;
    ch->fighting = fight;
    ch->position = POS_FIGHTING;
    victim->num_fighting++;
    if (victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS))
    {
        send_to_char("You are disturbed!\n\r", victim->switched);
        do_return(victim->switched, MAKE_TEMP_STRING(""));
    }
    return;
}

CHAR_DATA* who_fighting(CHAR_DATA* ch)
{
    if (!ch)
    {
        bug("who_fighting: null ch", 0);
        return nullptr;
    }
    if (!ch->fighting)
        return nullptr;
    return ch->fighting->who;
}

void free_fight(CHAR_DATA* ch)
{
    if (!ch)
    {
        bug("Free_fight: null ch!", 0);
        return;
    }
    if (ch->fighting)
    {
        if (!char_died(ch->fighting->who))
            --ch->fighting->who->num_fighting;
        DISPOSE(ch->fighting);
    }
    ch->fighting = nullptr;
    if (ch->mount)
        ch->position = POS_MOUNTED;
    else
        ch->position = POS_STANDING;
    /* Berserk wears off after combat. -- Altrag */
    if (IS_AFFECTED(ch, AFF_BERSERK))
    {
        affect_strip(ch, gsn_berserk);
        set_char_color(AT_WEAROFF, ch);
        send_to_char(skill_table[gsn_berserk]->msg_off, ch);
        send_to_char("\n\r", ch);
    }
    return;
}

/*
 * Stop fights.
 */
void stop_fighting(CHAR_DATA* ch, bool fBoth)
{
    CHAR_DATA* fch;

    free_fight(ch);
    update_pos(ch);

    if (!fBoth) /* major short cut here by Thoric */
        return;

    for (fch = first_char; fch; fch = fch->next)
    {
        if (who_fighting(fch) == ch)
        {
            free_fight(fch);
            update_pos(fch);
        }
    }
    return;
}

void death_cry(CHAR_DATA* ch)
{

    return;
}

void raw_kill(CHAR_DATA* ch, CHAR_DATA* victim)
{

    CHAR_DATA* victmp;

    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    long exp;
    OBJ_DATA *obj, *obj_next;

    if (!victim)
    {
        bug("raw_kill: null victim!", 0);
        return;
    }

    strcpy_s(arg, victim->name);

    stop_fighting(victim, true);

    if (ch && !IS_NPC(ch) && !IS_NPC(victim))
    {
        CONTRACT_DATA* contract;
        CONTRACT_DATA* scontract = nullptr;
        claim_disintegration(ch, victim);

        for (contract = ch->first_contract; contract; contract = contract->next_in_contract)
        {
            if (!str_cmp(contract->target, victim->name))
            {
                scontract = contract;
                break;
            }
        }

        if (scontract != nullptr)
        {
            ch_printf(ch, "&w&RYou have claimed your contract on %s, and collect your reward of %d credits.\n\r",
                      scontract->target, scontract->amount);
            ch->gold += scontract->amount;
            exp = (exp_level(ch->skill_level[ASSASSIN_ABILITY] + 1) - exp_level(ch->skill_level[ASSASSIN_ABILITY]));
            gain_exp(ch, exp, ASSASSIN_ABILITY);

            STRFREE(scontract->target);
            UNLINK(scontract, ch->first_contract, ch->last_contract, next_in_contract, prev_in_contract);
            DISPOSE(scontract);
        }
    }

    /* Take care of polymorphed chars */
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_POLYMORPHED))
    {
        char_from_room(victim->desc->original);
        char_to_room(victim->desc->original, victim->in_room);
        victmp = victim->desc->original;
        do_revert(victim, MAKE_TEMP_STRING(""));
        raw_kill(ch, victmp);
        return;
    }

    if (victim->in_room && IS_NPC(victim) && victim->vip_flags != 0 && victim->in_room->area &&
        victim->in_room->area->planet)
    {
        victim->in_room->area->planet->population--;
        victim->in_room->area->planet->population = UMAX(victim->in_room->area->planet->population, 0);
        victim->in_room->area->planet->pop_support -= (float)(1 + 1 / (victim->in_room->area->planet->population + 1));
        if (victim->in_room->area->planet->pop_support < -100)
            victim->in_room->area->planet->pop_support = -100;
    }

    if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
        mprog_death_trigger(ch, victim);
    if (char_died(victim))
        return;

    if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
        rprog_death_trigger(ch, victim);
    if (char_died(victim))
        return;

    if (!IS_NPC(victim) || (!IS_SET(victim->act, ACT_NOKILL) && !IS_SET(victim->act, ACT_NOCORPSE)))
        make_corpse(victim, IS_NPC(ch) ? ch->short_descr : ch->name);
    else
    {
        for (obj = victim->last_carrying; obj; obj = obj_next)
        {
            obj_next = obj->prev_content;
            obj_from_char(obj);
            extract_obj(obj);
        }
    }

    /*    make_blood( victim ); */

    if (IS_NPC(victim))
    {
        victim->pIndexData->killed++;
        extract_char(victim, true);
        victim = nullptr;
        return;
    }

    set_char_color(AT_DIEMSG, victim);
    do_help(victim, MAKE_TEMP_STRING("_DIEMSG_"));

    /* swreality chnages begin here */

    if (victim->plr_home)
    {
        ROOM_INDEX_DATA* room = victim->plr_home;

        STRFREE(room->name);
        room->name = STRALLOC("An Empty Apartment");

        REMOVE_BIT(room->room_flags, ROOM_PLR_HOME);
        SET_BIT(room->room_flags, ROOM_EMPTY_HOME);

        fold_area(room->area, room->area->filename, false);
    }

    if (victim->pcdata && victim->pcdata->clan)
    {
        if (victim->pcdata->clan->shortname && victim->pcdata->clan->shortname[0] != '\0')
            remove_member(victim->name, victim->pcdata->clan->shortname);

        if (!str_cmp(victim->name, victim->pcdata->clan->leader))
        {
            STRFREE(victim->pcdata->clan->leader);
            if (victim->pcdata->clan->number1)
            {
                victim->pcdata->clan->leader = STRALLOC(victim->pcdata->clan->number1);
                STRFREE(victim->pcdata->clan->number1);
                victim->pcdata->clan->number1 = STRALLOC("");
            }
            else if (victim->pcdata->clan->number2)
            {
                victim->pcdata->clan->leader = STRALLOC(victim->pcdata->clan->number2);
                STRFREE(victim->pcdata->clan->number2);
                victim->pcdata->clan->number2 = STRALLOC("");
            }
            else
                victim->pcdata->clan->leader = STRALLOC("");
        }

        if (!str_cmp(victim->name, victim->pcdata->clan->number1))
        {
            STRFREE(victim->pcdata->clan->number1);
            if (victim->pcdata->clan->number2)
            {
                victim->pcdata->clan->number1 = STRALLOC(victim->pcdata->clan->number2);
                STRFREE(victim->pcdata->clan->number2);
                victim->pcdata->clan->number2 = STRALLOC("");
            }
            else
                victim->pcdata->clan->number1 = STRALLOC("");
        }

        if (!str_cmp(victim->name, victim->pcdata->clan->number2))
        {
            STRFREE(victim->pcdata->clan->number2);
            victim->pcdata->clan->number1 = STRALLOC("");
        }

        victim->pcdata->clan->members--;
    }

    if (!victim)
    {
        /* Make sure they aren't halfway logged in. */
        for (auto d : g_descriptors)
            if ((victim = d->character) && !IS_NPC(victim))
            {
                close_socket(d.get(), true);
                break;
            }
    }
    else
    {
        int x, y;

        quitting_char = victim;
        save_char_obj(victim);
        saving_char = nullptr;
        extract_char(victim, true);
        for (x = 0; x < MAX_WEAR; x++)
            for (y = 0; y < MAX_LAYERS; y++)
                save_equipment[x][y] = nullptr;
    }

    sprintf_s(buf, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), capitalize(arg).c_str());
    sprintf_s(buf2, "%s%c/%s", BACKUP_DIR, tolower(arg[0]), capitalize(arg).c_str());

    rename(buf, buf2);

    sprintf_s(buf, "%s%c/%s.clone", PLAYER_DIR, tolower(arg[0]), capitalize(arg).c_str());
    sprintf_s(buf2, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), capitalize(arg).c_str());

    rename(buf, buf2);

    return;

    /* original player kill started here

        extract_char( victim, false );
        if ( !victim )
        {
          bug( "oops! raw_kill: extract_char destroyed pc char", 0 );
          return;
        }
        while ( victim->first_affect )
        affect_remove( victim, victim->first_affect );
        victim->affected_by	= race_table[victim->race].affected;
        victim->resistant   = 0;
        victim->susceptible = 0;
        victim->immune      = 0;
        victim->carry_weight= 0;
        victim->armor	= 100;
        victim->mod_str	= 0;
        victim->mod_dex	= 0;
        victim->mod_wis	= 0;
        victim->mod_int	= 0;
        victim->mod_con	= 0;
        victim->mod_cha	= 0;
        victim->mod_lck   	= 0;
        victim->damroll	= 0;
        victim->hitroll	= 0;
        victim->mental_state = -10;
        victim->alignment	= URANGE( -1000, victim->alignment, 1000 );
        victim->saving_spell_staff = 0;
        victim->position	= POS_RESTING;
        victim->hit		= UMAX( 1, victim->hit  );
        victim->mana	= UMAX( 1, victim->mana );
        victim->move	= UMAX( 1, victim->move );

        victim->pcdata->condition[COND_FULL]   = 12;
        victim->pcdata->condition[COND_THIRST] = 12;

        if ( IS_SET( sysdata.save_flags, SV_DEATH ) )
        save_char_obj( victim );
        return;

    */
}

void group_gain(CHAR_DATA* ch, CHAR_DATA* victim)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch, *gch_next;
    CHAR_DATA* lch;
    int xp;
    int members;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if (IS_NPC(ch) || victim == ch)
        return;

    members = 0;

    for (gch = ch->in_room->first_person; gch; gch = gch->next_in_room)
    {
        if (is_same_group(gch, ch))
            members++;
    }

    if (members == 0)
    {
        bug("Group_gain: members.", members);
        members = 1;
    }

    lch = ch->leader ? ch->leader : ch;

    for (gch = ch->in_room->first_person; gch; gch = gch_next)
    {
        OBJ_DATA* obj;
        OBJ_DATA* obj_next;

        gch_next = gch->next_in_room;

        if (!is_same_group(gch, ch))
            continue;

        xp = (int)(xp_compute(gch, victim) / members);

        gch->alignment = align_compute(gch, victim);

        if (!IS_NPC(gch) && IS_NPC(victim) && gch->pcdata && gch->pcdata->clan &&
            !str_cmp(gch->pcdata->clan->name, victim->mob_clan))
        {
            xp = 0;
            sprintf_s(buf, "You receive no experience for killing your organizations resources.\n\r");
            send_to_char(buf, gch);
        }
        else
        {
            sprintf_s(buf, "You receive %d combat experience.\n\r", xp);
            send_to_char(buf, gch);
        }

        gain_exp(gch, xp, COMBAT_ABILITY);

        if (lch == gch && members > 1)
        {
            xp = URANGE(members, xp * members,
                        (exp_level(gch->skill_level[POLITICIAN_ABILITY] + 1) -
                         exp_level(gch->skill_level[POLITICIAN_ABILITY]) / 10));
            sprintf_s(buf, "You get %d leadership experience for leading your group to victory.\n\r", xp);
            send_to_char(buf, gch);
            gain_exp(gch, xp, POLITICIAN_ABILITY);
        }

        for (obj = gch->first_carrying; obj; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc == WEAR_NONE)
                continue;

            if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(gch)) ||
                (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(gch)) ||
                (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(gch)))
            {
                act(AT_MAGIC, "You are zapped by $p.", gch, obj, nullptr, TO_CHAR);
                act(AT_MAGIC, "$n is zapped by $p.", gch, obj, nullptr, TO_ROOM);

                obj_from_char(obj);
                obj = obj_to_room(obj, gch->in_room);
                //		oprog_zap_trigger(ch, obj);  /* mudprogs */
                if (char_died(gch))
                    return;
            }
        }
    }

    return;
}

int align_compute(CHAR_DATA* gch, CHAR_DATA* victim)
{

    /* never cared much for this system

        int align, newalign;

        align = gch->alignment - victim->alignment;

        if ( align >  500 )
        newalign  = UMIN( gch->alignment + (align-500)/4,  1000 );
        else
        if ( align < -500 )
        newalign  = UMAX( gch->alignment + (align+500)/4, -1000 );
        else
        newalign  = gch->alignment - (int) (gch->alignment / 4);

        return newalign;

    make it simple instead */

    if (IS_SET(gch->in_room->room_flags2, ROOM_ARENA))
        return 0;
    return URANGE(-1000, (int)(gch->alignment - victim->alignment / 5), 1000);
}

/*
 * Calculate how much XP gch should gain for killing victim
 * Lots of redesigning for new exp system by Thoric
 */
int xp_compute(CHAR_DATA* gch, CHAR_DATA* victim)
{
    int align;
    int xp;

    xp = (get_exp_worth(victim) *
          URANGE(1, (victim->skill_level[COMBAT_ABILITY] - gch->skill_level[COMBAT_ABILITY]) + 10, 20)) /
         10;
    align = gch->alignment - victim->alignment;

    /* bonus for attacking opposite alignment */
    if (align > 990 || align < -990)
        xp = (xp * 5) >> 2;
    else
        /* penalty for good attacking same alignment */
        if (gch->alignment > 300 && align < 250)
            xp = (xp * 3) >> 2;

    xp = number_range((xp * 3) >> 2, (xp * 5) >> 2);

    /* reduce exp for killing the same mob repeatedly		-Thoric */
    if (!IS_NPC(gch) && IS_NPC(victim))
    {
        int times = times_killed(gch, victim);

        if (times >= 5)
            xp = 0;
        else if (times)
            xp = (xp * (5 - times)) / 5;
    }

    /* new xp cap for swreality */

    return URANGE(1, xp,
                  (exp_level(gch->skill_level[COMBAT_ABILITY] + 1) - exp_level(gch->skill_level[COMBAT_ABILITY])));
}

/*
 * Revamped by Thoric to be more realistic
 */
void dam_message(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt)
{
    char buf1[256], buf2[256], buf3[256];
    const char* vs;
    const char* vp;
    const char* attack;
    char punct;
    sh_int dampc;
    SKILL_TYPE* skill = nullptr;
    bool gcflag = false;
    bool gvflag = false;

    if (!dam)
        dampc = 0;
    else
        dampc = ((dam * 1000) / victim->max_hit) + (50 - ((victim->hit * 50) / victim->max_hit));

    /*		     10 * percent					*/
    if (dam == 0)
    {
        vs = "miss";
        vp = "misses";
    }
    else if (dampc <= 5)
    {
        vs = "barely scratch";
        vp = "barely scratches";
    }
    else if (dampc <= 10)
    {
        vs = "scratch";
        vp = "scratches";
    }
    else if (dampc <= 20)
    {
        vs = "nick";
        vp = "nicks";
    }
    else if (dampc <= 30)
    {
        vs = "graze";
        vp = "grazes";
    }
    else if (dampc <= 40)
    {
        vs = "bruise";
        vp = "bruises";
    }
    else if (dampc <= 50)
    {
        vs = "hit";
        vp = "hits";
    }
    else if (dampc <= 60)
    {
        vs = "injure";
        vp = "injures";
    }
    else if (dampc <= 75)
    {
        vs = "thrash";
        vp = "thrashes";
    }
    else if (dampc <= 80)
    {
        vs = "wound";
        vp = "wounds";
    }
    else if (dampc <= 90)
    {
        vs = "maul";
        vp = "mauls";
    }
    else if (dampc <= 125)
    {
        vs = "decimate";
        vp = "decimates";
    }
    else if (dampc <= 150)
    {
        vs = "devastate";
        vp = "devastates";
    }
    else if (dampc <= 200)
    {
        vs = "maim";
        vp = "maims";
    }
    else if (dampc <= 300)
    {
        vs = "MUTILATE";
        vp = "MUTILATES";
    }
    else if (dampc <= 400)
    {
        vs = "DISEMBOWEL";
        vp = "DISEMBOWELS";
    }
    else if (dampc <= 500)
    {
        vs = "MASSACRE";
        vp = "MASSACRES";
    }
    else if (dampc <= 600)
    {
        vs = "PULVERIZE";
        vp = "PULVERIZES";
    }
    else if (dampc <= 750)
    {
        vs = "EVISCERATE";
        vp = "EVISCERATES";
    }
    else if (dampc <= 990)
    {
        vs = "* OBLITERATE *";
        vp = "* OBLITERATES *";
    }
    else
    {
        vs = "*** ANNIHILATE ***";
        vp = "*** ANNIHILATES ***";
    }

    punct = (dampc <= 30) ? '.' : '!';

    if (dam == 0 && (!IS_NPC(ch) && (IS_SET(ch->pcdata->flags, PCFLAG_GAG))))
        gcflag = true;

    if (dam == 0 && (!IS_NPC(victim) && (IS_SET(victim->pcdata->flags, PCFLAG_GAG))))
        gvflag = true;

    if (dt >= 0 && dt < top_sn)
        skill = skill_table[dt];

    if (dt == (TYPE_HIT + WEAPON_BLASTER))
    {
        char sound[MAX_STRING_LENGTH];
        int vol = number_range(20, 80);

        sprintf_s(sound, "!!SOUND(blaster V=%d)", vol);
        sound_to_room(ch->in_room, sound);
    }

    if (dt == TYPE_HIT || dam == 0)
    {
        sprintf_s(buf1, "$n %s $N%c", vp, punct);
        sprintf_s(buf2, "You %s $N%c", vs, punct);
        sprintf_s(buf3, "$n %s you%c", vp, punct);
    }
    else if (dt > TYPE_HIT && is_wielding_poisoned(ch))
    {
        if (dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0]))
            attack = attack_table[dt - TYPE_HIT];
        else
        {
            bug("Dam_message: bad dt %d.", dt);
            dt = TYPE_HIT;
            attack = attack_table[0];
        }

        sprintf_s(buf1, "$n's poisoned %s %s $N%c", attack, vp, punct);
        sprintf_s(buf2, "Your poisoned %s %s $N%c", attack, vp, punct);
        sprintf_s(buf3, "$n's poisoned %s %s you%c", attack, vp, punct);
    }
    else
    {
        if (skill)
        {
            attack = skill->noun_damage;
            if (dam == 0)
            {
                bool found = false;

                if (skill->miss_char && skill->miss_char[0] != '\0')
                {
                    act(AT_HIT, skill->miss_char, ch, nullptr, victim, TO_CHAR);
                    found = true;
                }
                if (skill->miss_vict && skill->miss_vict[0] != '\0')
                {
                    act(AT_HITME, skill->miss_vict, ch, nullptr, victim, TO_VICT);
                    found = true;
                }
                if (skill->miss_room && skill->miss_room[0] != '\0')
                {
                    act(AT_ACTION, skill->miss_room, ch, nullptr, victim, TO_NOTVICT);
                    found = true;
                }
                if (found) /* miss message already sent */
                    return;
            }
            else
            {
                if (skill->hit_char && skill->hit_char[0] != '\0')
                    act(AT_HIT, skill->hit_char, ch, nullptr, victim, TO_CHAR);
                if (skill->hit_vict && skill->hit_vict[0] != '\0')
                    act(AT_HITME, skill->hit_vict, ch, nullptr, victim, TO_VICT);
                if (skill->hit_room && skill->hit_room[0] != '\0')
                    act(AT_ACTION, skill->hit_room, ch, nullptr, victim, TO_NOTVICT);
            }
        }
        else if (dt >= TYPE_HIT && dt < TYPE_HIT + sizeof(attack_table) / sizeof(attack_table[0]))
            attack = attack_table[dt - TYPE_HIT];
        else
        {
            bug("Dam_message: bad dt %d.", dt);
            dt = TYPE_HIT;
            attack = attack_table[0];
        }

        sprintf_s(buf1, "$n's %s %s $N%c", attack, vp, punct);
        sprintf_s(buf2, "Your %s %s $N%c", attack, vp, punct);
        sprintf_s(buf3, "$n's %s %s you%c", attack, vp, punct);
    }

    if (ch->skill_level[COMBAT_ABILITY] >= 15)
        sprintf_s(buf2, "%s You do %d points of damage.", buf2, dam);
    if (dt != TYPE_MISSILE)
    {
        act(AT_ACTION, buf1, ch, nullptr, victim, TO_NOTVICT);
        if (!gcflag)
            act(AT_HIT, buf2, ch, nullptr, victim, TO_CHAR);
        if (!gvflag)
            act(AT_HITME, buf3, ch, nullptr, victim, TO_VICT);
    }
    return;
}

void do_kill(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA* victim;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Kill whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg)) == nullptr)
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim))
    {
        send_to_char("You must MURDER a player.\n\r", ch);
        return;
    }

    /*
     *
     else
     {
     if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != nullptr )
     {
         send_to_char( "You must MURDER a charmed creature.\n\r", ch );
         return;
     }
     }
     *
     */

    if (victim == ch)
    {
        send_to_char("You hit yourself.  Ouch!\n\r", ch);
        multi_hit(ch, ch, TYPE_UNDEFINED);
        return;
    }

    if (is_safe(ch, victim))
        return;

    if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
    {
        act(AT_PLAIN, "$N is your beloved master.", ch, nullptr, victim, TO_CHAR);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char("You do the best you can!\n\r", ch);
        return;
    }

    if (!IS_NPC(ch))
        ch->pcdata->lost_attacks = 0;
    if (victim->vip_flags != 0)
        ch->alignment -= 10;

    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}

void do_murde(CHAR_DATA* ch, char* argument)
{
    send_to_char("If you want to MURDER, spell it out.\n\r", ch);
    return;
}

void do_murder(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    CHAR_DATA* victim;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Murder whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg)) == nullptr)
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("Suicide is a mortal sin.\n\r", ch);
        return;
    }

    if (is_safe(ch, victim))
        return;

    if (IS_AFFECTED(ch, AFF_CHARM))
    {
        if (ch->master == victim)
        {
            act(AT_PLAIN, "$N is your beloved master.", ch, nullptr, victim, TO_CHAR);
            return;
        }
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char("You do the best you can!\n\r", ch);
        return;
    }
    if (!IS_NPC(ch))
        ch->pcdata->lost_attacks = 0;
    if (!IS_NPC(victim) && IS_SET(ch->act, PLR_NICE))
    {
        send_to_char("You feel too nice to do that!\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Thats a bit hard to do right now...\n\r", ch);
        return;
    }

    ch->alignment -= 10;

    if ((!ch->in_room->area || !in_arena(ch)) || !IS_NPC(ch) || !IS_NPC(victim))
    {
        sprintf_s(logbuf, "%s: murder %s", ch->name, argument);
        log_string(logbuf);
    }
    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
    multi_hit(ch, victim, TYPE_UNDEFINED);
    return;
}

bool in_arena(CHAR_DATA* ch)
{

    if (!str_cmp(ch->in_room->area->filename, "arena.are"))
        return true;

    if (ch->in_room->vnum < 29 || ch->in_room->vnum > 43)
        return false;

    return true;
}

void do_flee(CHAR_DATA* ch, char* argument)
{
    ROOM_INDEX_DATA* was_in;
    ROOM_INDEX_DATA* now_in;
    char buf[MAX_STRING_LENGTH];
    int attempt;
    sh_int door;
    EXIT_DATA* pexit;

    if (!who_fighting(ch))
    {
        if (ch->position == POS_FIGHTING)
        {
            if (ch->mount)
                ch->position = POS_MOUNTED;
            else
                ch->position = POS_STANDING;
        }
        send_to_char("You aren't fighting anyone.\n\r", ch);
        return;
    }

    if (ch->move <= 0)
    {
        send_to_char("You're too exhausted to flee from combat!\n\r", ch);
        return;
    }

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch, AFF_CHARM))
    {
        send_to_char("You're too brave to flee!\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Thats a bit hard to do right now...\n\r", ch);
        return;
    }

    /* No fleeing while stunned. - Narn */
    if (ch->position < POS_FIGHTING)
        return;

    was_in = ch->in_room;
    for (attempt = 0; attempt < 8; attempt++)
    {

        door = number_door();
        if ((pexit = get_exit(was_in, door)) == nullptr || !pexit->to_room ||
            (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(ch, AFF_PASS_DOOR)) ||
            (IS_NPC(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)))
            continue;

        affect_strip(ch, gsn_sneak);
        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        if (ch->mount && ch->mount->fighting)
            stop_fighting(ch->mount, true);
        move_char(ch, pexit, 0);
        if ((now_in = ch->in_room) == was_in)
            continue;

        ch->in_room = was_in;
        act(AT_FLEE, "$n runs for cover!", ch, nullptr, nullptr, TO_ROOM);
        ch->in_room = now_in;
        act(AT_FLEE, "$n glances around for signs of pursuit.", ch, nullptr, nullptr, TO_ROOM);
        sprintf_s(buf, "You run for cover!");
        send_to_char(buf, ch);

        stop_fighting(ch, true);
        return;
    }

    sprintf_s(buf, "You attempt to run for cover!");
    send_to_char(buf, ch);
    return;
}

bool get_cover(CHAR_DATA* ch)
{
    ROOM_INDEX_DATA* was_in;
    ROOM_INDEX_DATA* now_in;
    int attempt;
    sh_int door;
    EXIT_DATA* pexit;

    if (!who_fighting(ch))
        return false;

    if (ch->position < POS_FIGHTING)
        return false;

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch, AFF_CHARM))
        return false;

    was_in = ch->in_room;

    for (attempt = 0; attempt < 10; attempt++)
    {

        door = number_door();
        if ((pexit = get_exit(was_in, door)) == nullptr || !pexit->to_room ||
            (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(ch, AFF_PASS_DOOR)) ||
            (IS_NPC(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)))
            continue;

        affect_strip(ch, gsn_sneak);
        REMOVE_BIT(ch->affected_by, AFF_SNEAK);
        if (ch->mount && ch->mount->fighting)
            stop_fighting(ch->mount, true);
        move_char(ch, pexit, 0);
        if ((now_in = ch->in_room) == was_in)
            continue;

        ch->in_room = was_in;
        act(AT_FLEE, "$n sprints for cover!", ch, nullptr, nullptr, TO_ROOM);
        ch->in_room = now_in;
        act(AT_FLEE, "$n spins around and takes aim.", ch, nullptr, nullptr, TO_ROOM);

        stop_fighting(ch, true);

        return true;
    }

    return false;
}

void do_sla(CHAR_DATA* ch, char* argument)
{
    send_to_char("If you want to SLAY, spell it out.\n\r", ch);
    return;
}

// Assassin skill - retreat. Fairly shoddily thrown together.
void do_retreat(CHAR_DATA* ch, char* argument)
{
    ROOM_INDEX_DATA* was_in;
    ROOM_INDEX_DATA* now_in;
    char buf[MAX_STRING_LENGTH];
    int edir, chance;
    EXIT_DATA* pexit;
    EXIT_DATA* xit;

    if (!who_fighting(ch))
    {
        if (ch->position == POS_FIGHTING)
        {
            if (ch->mount)
                ch->position = POS_MOUNTED;
            else
                ch->position = POS_STANDING;
        }
        send_to_char("You aren't fighting anyone.\n\r", ch);
        return;
    }

    if (ch->move <= 0)
    {
        send_to_char("You're too exhausted to retreat!\n\r", ch);
        return;
    }

    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NOFLEE) && !IS_AFFECTED(ch, AFF_CHARM))
    {
        send_to_char("You're too brave to retreat!\n\r", ch);
        return;
    }

    if (ch->position < POS_FIGHTING)
        return;

    if (!IS_NPC(ch) && IS_SET(ch->pcdata->act2, ACT_BOUND))
    {
        send_to_char("Thats a bit hard to do right now...\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char("Retreat in what direction?\n\r", ch);
        return;
    }

    if ((edir = get_door(argument)) == -1)
    {
        send_to_char("Retreat where??\n\r", ch);
        return;
    }

    chance = IS_NPC(ch) ? ch->top_level : (int)ch->pcdata->learned[gsn_retreat];

    if (chance < number_percent())
    {
        send_to_char("&RYou fail to find the correct exit in the heat of battle!\n\r", ch);
        learn_from_failure(ch, gsn_retreat);
        return;
    }

    xit = get_exit(ch->in_room, edir);
    was_in = ch->in_room;

    if ((pexit = get_exit(ch->in_room, edir)) == nullptr || !pexit->to_room)
    {
        send_to_char("Theres no exit there!\n\r", ch);
        return;
    }

    if (IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(ch, AFF_PASS_DOOR))
    {
        send_to_char("You slam into the door!\n\r", ch);
        return;
    }

    affect_strip(ch, gsn_sneak);
    REMOVE_BIT(ch->affected_by, AFF_SNEAK);
    if (ch->mount && ch->mount->fighting)
        stop_fighting(ch->mount, true);
    move_char(ch, pexit, 0);
    if ((now_in = ch->in_room) == was_in)
        return;

    ch->in_room = was_in;
    act(AT_FLEE, "&Y$n makes a hasty retreat.", ch, nullptr, nullptr, TO_ROOM);
    ch->in_room = now_in;
    act(AT_FLEE, "$n glances around for signs of pursuit.", ch, nullptr, nullptr, TO_ROOM);
    sprintf_s(buf, "&YYou make a hasty retreat!");
    send_to_char(buf, ch);
    learn_from_success(ch, gsn_retreat);
    stop_fighting(ch, true);
    return;
}
