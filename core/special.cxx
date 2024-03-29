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

/* jails for wanted flags */

#define ROOM_JAIL_CORUSCANT 0

bool remove_obj(CHAR_DATA* ch, int iWear, bool fReplace);

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(spec_jedi);
DECLARE_SPEC_FUN(spec_dark_jedi);
DECLARE_SPEC_FUN(spec_fido);
DECLARE_SPEC_FUN(spec_guardian);
DECLARE_SPEC_FUN(spec_janitor);
DECLARE_SPEC_FUN(spec_poison);
DECLARE_SPEC_FUN(spec_thief);
DECLARE_SPEC_FUN(spec_auth);
DECLARE_SPEC_FUN(spec_giveslug);
DECLARE_SPEC_FUN(spec_stormtrooper);
DECLARE_SPEC_FUN(spec_new_republic_trooper);
DECLARE_SPEC_FUN(spec_customs_smut);
DECLARE_SPEC_FUN(spec_customs_alcohol);
DECLARE_SPEC_FUN(spec_customs_weapons);
DECLARE_SPEC_FUN(spec_customs_spice);
DECLARE_SPEC_FUN(spec_police_attack);
DECLARE_SPEC_FUN(spec_police_jail);
DECLARE_SPEC_FUN(spec_police_fine);
DECLARE_SPEC_FUN(spec_police);
DECLARE_SPEC_FUN(spec_clan_guard);
DECLARE_SPEC_FUN(spec_newbie_pilot);
DECLARE_SPEC_FUN(spec_ground_troop);
DECLARE_SPEC_FUN(spec_make_apprentice_jedi);
DECLARE_SPEC_FUN(spec_make_master_jedi);
DECLARE_SPEC_FUN(spec_make_apprentice_sith);

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN* spec_lookup(const char* name)
{
    if (!str_cmp(name, "spec_jedi"))
        return spec_jedi;
    if (!str_cmp(name, "spec_dark_jedi"))
        return spec_dark_jedi;
    if (!str_cmp(name, "spec_fido"))
        return spec_fido;
    if (!str_cmp(name, "spec_guardian"))
        return spec_guardian;
    if (!str_cmp(name, "spec_janitor"))
        return spec_janitor;
    if (!str_cmp(name, "spec_poison"))
        return spec_poison;
    if (!str_cmp(name, "spec_thief"))
        return spec_thief;
    if (!str_cmp(name, "spec_auth"))
        return spec_auth;
    if (!str_cmp(name, "spec_giveslug"))
        return spec_giveslug;
    if (!str_cmp(name, "spec_stormtrooper"))
        return spec_stormtrooper;
    if (!str_cmp(name, "spec_new_republic_trooper"))
        return spec_new_republic_trooper;
    if (!str_cmp(name, "spec_customs_smut"))
        return spec_customs_smut;
    if (!str_cmp(name, "spec_customs_alcohol"))
        return spec_customs_alcohol;
    if (!str_cmp(name, "spec_customs_weapons"))
        return spec_customs_weapons;
    if (!str_cmp(name, "spec_customs_spice"))
        return spec_customs_spice;
    if (!str_cmp(name, "spec_police_attack"))
        return spec_police_attack;
    if (!str_cmp(name, "spec_police_jail"))
        return spec_police_jail;
    if (!str_cmp(name, "spec_police_fine"))
        return spec_police_fine;
    if (!str_cmp(name, "spec_police"))
        return spec_police;
    if (!str_cmp(name, "spec_clan_guard"))
        return spec_clan_guard;
    if (!str_cmp(name, "spec_newbie_pilot"))
        return spec_newbie_pilot;
    if (!str_cmp(name, "spec_ground_troop"))
        return spec_ground_troop;
    if (!str_cmp(name, "spec_make_apprentice_jedi"))
        return spec_make_apprentice_jedi;
    if (!str_cmp(name, "spec_make_master_jedi"))
        return spec_make_master_jedi;
    if (!str_cmp(name, "spec_make_apprentice_sith"))
        return spec_make_apprentice_sith;
    return 0;
}

/*
 * Given a pointer, return the appropriate spec fun text.
 */
const char* lookup_spec(SPEC_FUN* special)
{
    if (special == spec_jedi)
        return "spec_jedi";
    if (special == spec_dark_jedi)
        return "spec_dark_jedi";
    if (special == spec_fido)
        return "spec_fido";
    if (special == spec_guardian)
        return "spec_guardian";
    if (special == spec_janitor)
        return "spec_janitor";
    if (special == spec_poison)
        return "spec_poison";
    if (special == spec_thief)
        return "spec_thief";
    if (special == spec_auth)
        return "spec_auth";
    if (special == spec_giveslug)
        return "spec_giveslug";
    if (special == spec_stormtrooper)
        return "spec_stormtrooper";
    if (special == spec_new_republic_trooper)
        return "spec_new_republic_trooper";
    if (special == spec_customs_smut)
        return "spec_customs_smut";
    if (special == spec_customs_weapons)
        return "spec_customs_weapons";
    if (special == spec_customs_alcohol)
        return "spec_customs_alcohol";
    if (special == spec_customs_spice)
        return "spec_customs_spice";
    if (special == spec_police_attack)
        return "spec_police_attack";
    if (special == spec_police_jail)
        return "spec_police_jail";
    if (special == spec_police_fine)
        return "spec_police_fine";
    if (special == spec_police)
        return "spec_police";
    if (special == spec_clan_guard)
        return "spec_clan_guard";
    if (special == spec_newbie_pilot)
        return "spec_newbie_pilot";
    if (special == spec_ground_troop)
        return "spec_ground_troop";
    if (special == spec_make_apprentice_jedi)
        return "spec_make_apprentice_jedi";
    if (special == spec_make_master_jedi)
        return "spec_make_master_jedi";
    if (special == spec_make_apprentice_sith)
        return "spec_make_apprentice_sith";
    return "";
}

bool spec_make_apprentice_jedi(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (!IS_NPC(victim) && victim->pcdata->forcerank == 0 && get_curr_frc(victim) > 0)
        {
            victim->pcdata->forcerank = 1;
            do_say(ch, MAKE_TEMP_STRING("You are now an apprentice of the jedi order."));
            SET_BIT(victim->pcdata->act2, ACT_JEDI);
        }
    }
    return false;
}

bool spec_make_master_jedi(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (!IS_NPC(victim) && victim->pcdata->forcerank > 0 && victim->pcdata->forcerank != 3 &&
            get_curr_frc(victim) > 0)
        {
            victim->pcdata->forcerank = 3;
            do_say(ch, MAKE_TEMP_STRING("Master of the Jedi order you are now."));
            SET_BIT(victim->pcdata->act2, ACT_JEDI);
        }
    }
    return false;
}

bool spec_make_apprentice_sith(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (!IS_NPC(victim) && victim->pcdata->forcerank == 0 && get_curr_frc(victim) > 0)
        {
            victim->pcdata->forcerank = 1;
            do_say(ch, MAKE_TEMP_STRING("You are now an apprentice of the sith order."));
            SET_BIT(victim->pcdata->act2, ACT_SITH);
        }
    }
    return false;
}

bool spec_newbie_pilot(CHAR_DATA* ch)
{
    int home = 32149;
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    OBJ_DATA* obj;
    char buf[MAX_STRING_LENGTH];
    bool diploma = false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || victim->position == POS_FIGHTING)
            continue;

        for (obj = victim->last_carrying; obj; obj = obj->prev_content)
            if (obj->pIndexData->vnum == OBJ_VNUM_SCHOOL_DIPLOMA)
                diploma = true;

        if (!diploma)
            continue;

        switch (victim->race)
        {
        case RACE_HUMAN:
            home = 201;
            strcpy_s(buf, "After a brief journey you arrive at Coruscants Menari Spaceport.\n\r\n\r");
            echo_to_room(AT_ACTION, ch->in_room, buf);
            break;

        default:
            sprintf_s(buf, "Hmm, a %s.", race_table[victim->race].race_name);
            do_look(ch, victim->name);
            do_say(ch, buf);
            do_say(ch, MAKE_TEMP_STRING("You're home planet is a little hard to get to right now."));
            do_say(ch, MAKE_TEMP_STRING("I'll take you to the Pluogus instead."));
            echo_to_room(AT_ACTION, ch->in_room,
                         "After a brief journey the shuttle docks with the Serin Pluogus.\n\r\n\r");
            break;
        }

        char_from_room(victim);
        char_to_room(victim, get_room_index(home));

        do_look(victim, MAKE_TEMP_STRING(""));

        sprintf_s(buf, "%s steps out and the shuttle quickly returns to the academy.\n\r", victim->name);
        echo_to_room(AT_ACTION, ch->in_room, buf);
    }

    return false;
}

bool spec_jedi(CHAR_DATA* ch)
{
    return false;
}

bool spec_clan_guard(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (!can_see(ch, victim))
            continue;
        if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
            continue;
        if (!IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_BOUND))
            continue;
        if (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan && IS_AWAKE(victim) && ch->mob_clan &&
            str_cmp(ch->mob_clan, victim->pcdata->clan->name))
        {
            do_yell(ch, MAKE_TEMP_STRING("Hey your not allowed in here!"));
            multi_hit(ch, victim, TYPE_UNDEFINED);
            return true;
        }
    }

    return false;
}

bool spec_ground_troop(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    char buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;
    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        /*if ( !can_see( ch, victim ) )
           continue;*/
        /*if ( get_timer(victim, TIMER_RECENTFIGHT) > 0 )
       continue;*/
        if (!IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_BOUND))
            continue;
        if (!IS_NPC(victim) && victim->pcdata->clan && victim->pcdata->clan != nullptr &&
            str_cmp(ch->mob_clan, victim->pcdata->clan->name))
        {
            sprintf_s(buf, "You are not loyal to %s", ch->mob_clan);
            do_yell(ch, buf);
            multi_hit(ch, victim, TYPE_UNDEFINED);
            return true;
        }
        if (IS_NPC(victim) && IS_AWAKE(victim) && str_cmp(ch->mob_clan, victim->mob_clan))
        {
            sprintf_s(buf, "You are not loyal to %s", ch->mob_clan);
            do_yell(ch, buf);
            multi_hit(ch, victim, TYPE_UNDEFINED);
            return true;
        }
    }

    return false;
}

bool spec_customs_smut(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    OBJ_DATA* obj;
    char buf[MAX_STRING_LENGTH];
    long ch_exp;

    if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || victim->position == POS_FIGHTING)
            continue;

        for (obj = victim->last_carrying; obj; obj = obj->prev_content)
        {
            if (obj->pIndexData->item_type == ITEM_SMUT)
            {
                if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
                {
                    sprintf_s(buf, "%s is illegal contraband. I'm going to have to confiscate that.", obj->short_descr);
                    do_say(ch, buf);
                    if (obj->wear_loc != WEAR_NONE)
                        remove_obj(victim, obj->wear_loc, true);
                    separate_obj(obj);
                    obj_from_char(obj);
                    act(AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n takes $p from you.", ch, obj, victim, TO_VICT);
                    obj = obj_to_char(obj, ch);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You lose %ld experience.\n\r ", ch_exp);
                    gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
                    return true;
                }
                else if (can_see(ch, victim) && !IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    act(AT_ACTION, "$n looks at $N suspiciously.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n look at you suspiciously.", ch, nullptr, victim, TO_VICT);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);

                    return true;
                }
                else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    return true;
                }
            }
            else if (obj->item_type == ITEM_CONTAINER)
            {
                OBJ_DATA* content;
                for (content = obj->first_content; content; content = content->next_content)
                {
                    if (content->pIndexData->item_type == ITEM_SMUT && !IS_SET(content->extra_flags, ITEM_CONTRABAND))
                    {
                        ch_exp = UMIN(content->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                           exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp,
                                  content->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
                        SET_BIT(content->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool spec_customs_weapons(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    OBJ_DATA* obj;
    char buf[MAX_STRING_LENGTH];
    long ch_exp;

    if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || victim->position == POS_FIGHTING)
            continue;

        if (victim->pcdata && victim->pcdata->clan &&
            (!str_cmp(victim->pcdata->clan->name, ch->mob_clan) ||
             (ch->in_room->area && ch->in_room->area->planet && ch->in_room->area->planet->governed_by &&
              ch->in_room->area->planet->governed_by == victim->pcdata->clan)))
            continue;

        for (obj = victim->last_carrying; obj; obj = obj->prev_content)
        {
            if (obj->pIndexData->item_type == ITEM_WEAPON)
            {
                if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
                {
                    sprintf_s(buf, "Weapons are banned from non-military usage. I'm going to have to confiscate %s.",
                              obj->short_descr);
                    do_say(ch, buf);
                    if (obj->wear_loc != WEAR_NONE)
                        remove_obj(victim, obj->wear_loc, true);
                    separate_obj(obj);
                    obj_from_char(obj);
                    act(AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n takes $p from you.", ch, obj, victim, TO_VICT);
                    obj = obj_to_char(obj, ch);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You lose %ld experience.\n\r ", ch_exp);
                    gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
                    return true;
                }
                else if (can_see(ch, victim) && !IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %d.\n\r ", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    act(AT_ACTION, "$n looks at $N suspiciously.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n look at you suspiciously.", ch, nullptr, victim, TO_VICT);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    return true;
                }
                else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    return true;
                }
            }
            else if (obj->item_type == ITEM_CONTAINER)
            {
                OBJ_DATA* content;
                for (content = obj->first_content; content; content = content->next_content)
                {
                    if (content->pIndexData->item_type == ITEM_WEAPON && !IS_SET(content->extra_flags, ITEM_CONTRABAND))
                    {
                        ch_exp = UMIN(content->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                           exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp,
                                  content->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
                        SET_BIT(content->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool spec_customs_alcohol(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    OBJ_DATA* obj;
    char buf[MAX_STRING_LENGTH];
    int liquid;
    long ch_exp;

    if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || victim->position == POS_FIGHTING)
            continue;

        for (obj = victim->last_carrying; obj; obj = obj->prev_content)
        {
            if (obj->pIndexData->item_type == ITEM_DRINK_CON)
            {
                if ((liquid = obj->value[2]) >= LIQ_MAX)
                    liquid = obj->value[2] = 0;

                if (liq_table[liquid].liq_affect[COND_DRUNK] > 0)
                {
                    if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
                    {
                        sprintf_s(buf, "%s is illegal contraband. I'm going to have to confiscate that.",
                                  obj->short_descr);
                        do_say(ch, buf);
                        if (obj->wear_loc != WEAR_NONE)
                            remove_obj(victim, obj->wear_loc, true);
                        separate_obj(obj);
                        obj_from_char(obj);
                        act(AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT);
                        act(AT_ACTION, "$n takes $p from you.", ch, obj, victim, TO_VICT);
                        obj = obj_to_char(obj, ch);
                        SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                        ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                       exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You lose %ld experience. \n\r", ch_exp);
                        gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
                        return true;
                    }
                    else if (can_see(ch, victim) && !IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                    {
                        ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                       exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %d. \n\r", ch_exp,
                                  obj->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                        act(AT_ACTION, "$n looks at $N suspiciously.", ch, nullptr, victim, TO_NOTVICT);
                        act(AT_ACTION, "$n look at you suspiciously.", ch, nullptr, victim, TO_VICT);
                        SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                    else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                    {
                        ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                       exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %d. \n\r", ch_exp,
                                  obj->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                        SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                }
            }
            else if (obj->item_type == ITEM_CONTAINER)
            {
                OBJ_DATA* content;
                for (content = obj->first_content; content; content = content->next_content)
                {
                    if (content->pIndexData->item_type == ITEM_DRINK_CON &&
                        !IS_SET(content->extra_flags, ITEM_CONTRABAND))
                    {
                        if ((liquid = obj->value[2]) >= LIQ_MAX)
                            liquid = obj->value[2] = 0;
                        if (liq_table[liquid].liq_affect[COND_DRUNK] <= 0)
                            continue;
                        ch_exp = UMIN(content->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                           exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %d.\n\r ", ch_exp,
                                  content->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
                        SET_BIT(content->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool spec_customs_spice(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    OBJ_DATA* obj;
    char buf[MAX_STRING_LENGTH];
    long ch_exp;

    if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || victim->position == POS_FIGHTING)
            continue;

        for (obj = victim->last_carrying; obj; obj = obj->prev_content)
        {
            if (obj->pIndexData->item_type == ITEM_SPICE || obj->pIndexData->item_type == ITEM_RAWSPICE)
            {
                if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
                {
                    sprintf_s(buf, "%s is illegal contraband. I'm going to have to confiscate that.", obj->short_descr);
                    do_say(ch, buf);
                    if (obj->wear_loc != WEAR_NONE)
                        remove_obj(victim, obj->wear_loc, true);
                    separate_obj(obj);
                    obj_from_char(obj);
                    act(AT_ACTION, "$n confiscates $p from $N.", ch, obj, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n takes $p from you.", ch, obj, victim, TO_VICT);
                    obj = obj_to_char(obj, ch);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You lose %ld experience. \n\r", ch_exp);
                    gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
                    return true;
                }
                else if (can_see(ch, victim) && !IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %s. \n\r", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    act(AT_ACTION, "$n looks at $N suspiciously.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n look at you suspiciously.", ch, nullptr, victim, TO_VICT);
                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    return true;
                }
                else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
                {
                    ch_exp = UMIN(obj->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                   exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                    ch_printf(victim, "You receive %ld experience for smuggling %s. \n\r", ch_exp, obj->short_descr);
                    gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

                    SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
                    return true;
                }
            }
            else if (obj->item_type == ITEM_CONTAINER)
            {
                OBJ_DATA* content;
                for (content = obj->first_content; content; content = content->next_content)
                {
                    if (content->pIndexData->item_type == ITEM_SPICE && !IS_SET(content->extra_flags, ITEM_CONTRABAND))
                    {
                        ch_exp = UMIN(content->cost * 10, (exp_level(victim->skill_level[SMUGGLING_ABILITY] + 1) -
                                                           exp_level(victim->skill_level[SMUGGLING_ABILITY])));
                        ch_printf(victim, "You receive %ld experience for smuggling %s.\n\r ", ch_exp,
                                  content->short_descr);
                        gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
                        SET_BIT(content->extra_flags, ITEM_CONTRABAND);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool spec_police(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (IS_NPC(victim))
            continue;
        if (!can_see(ch, victim))
            continue;
        if (number_bits(1) == 0)
            continue;
        for (vip = 0; vip < 32; vip++)
            if (IS_SET(ch->vip_flags, 1 << vip) && IS_SET(victim->pcdata->wanted_flags, 1 << vip) && victim->hit >= 50)
            {
                sprintf_s(buf, "Hey you're wanted on %s!", planet_flags[vip]);
                do_say(ch, buf);
                // No longer used because of Outlaw System
                //            REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip );
                if (ch->top_level >= victim->top_level)
                    multi_hit(ch, victim, TYPE_UNDEFINED);
                else
                {
                    // changed amount because of outlaw... kinda bogus.
                    if (number_percent() >= 50)
                    {
                        act(AT_ACTION, "$n fines $N an enormous amount of money.", ch, nullptr, victim, TO_NOTVICT);
                        act(AT_ACTION, "$n fines you an enourmous amount of money.", ch, nullptr, victim, TO_VICT);
                        victim->gold = victim->gold * .75;
                    }
                    else
                    {
                        act(AT_ACTION, "$n fines $N a small amount of money.", ch, nullptr, victim, TO_NOTVICT);
                        act(AT_ACTION, "$n fines you a small amount of money.", ch, nullptr, victim, TO_VICT);
                        victim->gold = victim->gold * .9;
                    }
                }
                return true;
            }
    }

    return false;
}

bool spec_police_attack(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (IS_NPC(victim))
            continue;
        if (!can_see(ch, victim))
            continue;
        if (number_bits(1) == 0)
            continue;
        for (vip = 0; vip < 32; vip++)
            if (IS_SET(ch->vip_flags, 1 << vip) && IS_SET(victim->pcdata->wanted_flags, 1 << vip) && victim->hit >= 50)
            {
                sprintf_s(buf, "Hey you're wanted on %s!", planet_flags[vip]);
                do_say(ch, buf);
                //              REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip );
                multi_hit(ch, victim, TYPE_UNDEFINED);
                return true;
            }
    }

    return false;
}

bool spec_police_fine(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (IS_NPC(victim))
            continue;
        if (!can_see(ch, victim))
            continue;
        if (number_bits(1) == 0)
            continue;
        for (vip = 0; vip <= 31; vip++)
            if (IS_SET(ch->vip_flags, 1 << vip) && IS_SET(victim->pcdata->wanted_flags, 1 << vip))
            {
                sprintf_s(buf, "Hey you're wanted on %s!", planet_flags[vip]);
                do_say(ch, buf);
                if (number_percent() >= 50)
                {
                    act(AT_ACTION, "$n fines $N an enormous amount of money.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n fines you an enourmous amount of money.", ch, nullptr, victim, TO_VICT);
                    victim->gold = victim->gold * .75;
                }
                else
                {
                    act(AT_ACTION, "$n fines $N a small amount of money.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n fines you a small amount of money.", ch, nullptr, victim, TO_VICT);
                    victim->gold = victim->gold * .9;
                }
                REMOVE_BIT(victim->pcdata->wanted_flags, 1 << vip);
                return true;
            }
    }

    return false;
}

bool spec_police_jail(CHAR_DATA* ch)
{

    ROOM_INDEX_DATA* jail = nullptr;
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    int vip;
    char buf[MAX_STRING_LENGTH];

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (IS_NPC(victim))
            continue;
        if (!can_see(ch, victim))
            continue;
        if (number_bits(1) == 0)
            continue;
        for (vip = 0; vip <= 31; vip++)
            if (IS_SET(ch->vip_flags, 1 << vip) && IS_SET(victim->pcdata->wanted_flags, 1 << vip))
            {
                sprintf_s(buf, "Hey you're wanted on %s!", planet_flags[vip]);
                do_say(ch, buf);

                /* currently no jails */

                if (jail)
                {
                    //              REMOVE_BIT( victim->pcdata->wanted_flags , 1 << vip );
                    act(AT_ACTION, "$n ushers $N off to jail.", ch, nullptr, victim, TO_NOTVICT);
                    act(AT_ACTION, "$n escorts you to jail.", ch, nullptr, victim, TO_VICT);
                    char_from_room(victim);
                    char_to_room(victim, jail);
                }
                return true;
            }
    }

    return false;
}

bool spec_jedi_healer(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    if (!IS_AWAKE(ch))
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (victim != ch && can_see(ch, victim) && number_bits(1) == 0)
            break;
    }

    if (!victim)
        return false;

    switch (number_bits(12))
    {
    case 0:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_smaug(skill_lookup("armor"), ch->top_level, ch, victim);
        return true;

    case 1:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_smaug(skill_lookup("good fortune"), ch->top_level, ch, victim);
        return true;

    case 2:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_cure_blindness(skill_lookup("cure blindness"), ch->top_level, ch, victim);
        return true;

    case 3:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_smaug(skill_lookup("cure light"), ch->top_level, ch, victim);
        return true;

    case 4:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_cure_poison(skill_lookup("cure poison"), ch->top_level, ch, victim);
        return true;

    case 5:
        act(AT_MAGIC, "$n pauses and concentrates for a moment.", ch, nullptr, nullptr, TO_ROOM);
        spell_smaug(skill_lookup("refresh"), ch->top_level, ch, victim);
        return true;
    }

    return false;
}

bool spec_dark_jedi(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    const char* spell;
    int sn;

    if (ch->position != POS_FIGHTING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (who_fighting(victim) && number_bits(2) == 0)
            break;
    }

    if (!victim || victim == ch)
        return false;

    for (;;)
    {
        int min_level;

        switch (number_bits(4))
        {
        case 0:
            min_level = 5;
            spell = "blindness";
            break;
        case 1:
            min_level = 5;
            spell = "fingers of the force";
            break;
        case 2:
            min_level = 9;
            spell = "choke";
            break;
        case 3:
            min_level = 8;
            spell = "invade essence";
            break;
        case 4:
            min_level = 11;
            spell = "force projectile";
            break;
        case 6:
            min_level = 13;
            spell = "drain essence";
            break;
        case 7:
            min_level = 4;
            spell = "force whip";
            break;
        case 8:
            min_level = 13;
            spell = "harm";
            break;
        case 9:
            min_level = 9;
            spell = "force bolt";
            break;
        case 10:
            min_level = 1;
            spell = "force spray";
            break;
        default:
            return false;
        }

        if (ch->top_level >= min_level)
            break;
    }

    if ((sn = skill_lookup(spell)) < 0)
        return false;
    (*skill_table[sn]->spell_fun)(sn, ch->top_level, ch, victim);
    return true;
}

bool spec_fido(CHAR_DATA* ch)
{
    OBJ_DATA* corpse;
    OBJ_DATA* c_next;
    OBJ_DATA* obj;
    OBJ_DATA* obj_next;

    if (!IS_AWAKE(ch))
        return false;

    for (corpse = ch->in_room->first_content; corpse; corpse = c_next)
    {
        c_next = corpse->next_content;
        if (corpse->item_type != ITEM_CORPSE_NPC)
            continue;

        act(AT_ACTION, "$n savagely devours a corpse.", ch, nullptr, nullptr, TO_ROOM);
        for (obj = corpse->first_content; obj; obj = obj_next)
        {
            obj_next = obj->next_content;
            obj_from_obj(obj);
            obj_to_room(obj, ch->in_room);
        }
        extract_obj(corpse);
        return true;
    }

    return false;
}

bool spec_stormtrooper(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (!can_see(ch, victim))
            continue;
        if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
            continue;
        if (!IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_BOUND))
            continue;
        if ((IS_NPC(victim) && nifty_is_name("republic", victim->name) && victim->fighting &&
             who_fighting(victim) != ch) ||
            (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan && IS_AWAKE(victim) &&
             nifty_is_name("republic", victim->pcdata->clan->name)))
        {
            do_yell(ch, MAKE_TEMP_STRING("Die Rebel Scum!"));
            multi_hit(ch, victim, TYPE_UNDEFINED);
            return true;
        }
    }

    return false;
}

bool spec_new_republic_trooper(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (!can_see(ch, victim))
            continue;
        if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
            continue;
        if (!IS_NPC(victim) && IS_SET(victim->pcdata->act2, ACT_BOUND))
            continue;
        if ((IS_NPC(victim) && nifty_is_name("imperial", victim->name) && victim->fighting &&
             who_fighting(victim) != ch) ||
            (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan && IS_AWAKE(victim) &&
             nifty_is_name("empire", victim->pcdata->clan->name)))
        {
            do_yell(ch, MAKE_TEMP_STRING("Long live the New Republic!"));
            multi_hit(ch, victim, TYPE_UNDEFINED);
            return true;
        }
    }

    return false;
}

bool spec_guardian(CHAR_DATA* ch)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    CHAR_DATA* ech;
    const char* crime;
    int max_evil;

    if (!IS_AWAKE(ch) || ch->fighting)
        return false;

    max_evil = 300;
    ech = nullptr;
    crime = "";

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;
        if (victim->fighting && who_fighting(victim) != ch && victim->alignment < max_evil)
        {
            max_evil = victim->alignment;
            ech = victim;
        }
    }

    if (victim && IS_SET(ch->in_room->room_flags, ROOM_SAFE))
    {
        sprintf_s(buf, "%s is a %s!  As well as a COWARD!", victim->name, crime);
        do_yell(ch, buf);
        return true;
    }

    if (victim)
    {
        sprintf_s(buf, "%s is a %s!  PROTECT THE INNOCENT!!", victim->name, crime);
        do_shout(ch, buf);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        return true;
    }

    if (ech)
    {
        act(AT_YELL, "$n screams 'PROTECT THE INNOCENT!!", ch, nullptr, nullptr, TO_ROOM);
        multi_hit(ch, ech, TYPE_UNDEFINED);
        return true;
    }

    return false;
}

bool spec_janitor(CHAR_DATA* ch)
{
    OBJ_DATA* trash;
    OBJ_DATA* trash_next;

    if (!IS_AWAKE(ch))
        return false;

    for (trash = ch->in_room->first_content; trash; trash = trash_next)
    {
        trash_next = trash->next_content;
        if (!IS_SET(trash->wear_flags, ITEM_TAKE) || IS_OBJ_STAT(trash, ITEM_BURRIED))
            continue;
        if (trash->item_type == ITEM_DRINK_CON || trash->item_type == ITEM_TRASH || trash->cost < 10 ||
            (trash->pIndexData->vnum == OBJ_VNUM_SHOPPING_BAG && !trash->first_content))
        {
            act(AT_ACTION, "$n picks up some trash.", ch, nullptr, nullptr, TO_ROOM);
            obj_from_room(trash);
            obj_to_char(trash, ch);
            return true;
        }
    }

    return false;
}

bool spec_poison(CHAR_DATA* ch)
{
    CHAR_DATA* victim;

    if (ch->position != POS_FIGHTING || (victim = who_fighting(ch)) == nullptr || number_percent() > 2 * ch->top_level)
        return false;

    act(AT_HIT, "You bite $N!", ch, nullptr, victim, TO_CHAR);
    act(AT_ACTION, "$n bites $N!", ch, nullptr, victim, TO_NOTVICT);
    act(AT_POISON, "$n bites you!", ch, nullptr, victim, TO_VICT);
    spell_poison(gsn_poison, ch->top_level, ch, victim);
    return true;
}

bool spec_thief(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    int gold, maxgold;

    if (ch->position != POS_STANDING)
        return false;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (IS_NPC(victim) || get_trust(victim) >= LEVEL_IMMORTAL || number_bits(2) != 0 ||
            !can_see(ch, victim)) /* Thx Glop */
            continue;

        if (IS_AWAKE(victim) && number_range(0, ch->top_level) == 0)
        {
            act(AT_ACTION, "You discover $n's hands in your wallet!", ch, nullptr, victim, TO_VICT);
            act(AT_ACTION, "$N discovers $n's hands in $S wallet!", ch, nullptr, victim, TO_NOTVICT);
            return true;
        }
        else
        {
            maxgold = ch->top_level * ch->top_level * 1000;
            gold = victim->gold * number_range(1, URANGE(2, ch->top_level / 4, 10)) / 100;
            ch->gold += 9 * gold / 10;
            victim->gold -= gold;
            if (ch->gold > maxgold)
            {
                boost_economy(ch->in_room->area, ch->gold - maxgold / 2);
                ch->gold = maxgold / 2;
            }
            return true;
        }
    }

    return false;
}

bool spec_auth(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    char buf[MAX_STRING_LENGTH];
    OBJ_INDEX_DATA* pObjIndex;
    OBJ_DATA* obj;
    bool hasdiploma;

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (!IS_NPC(victim) && (pObjIndex = get_obj_index(OBJ_VNUM_SCHOOL_DIPLOMA)) != nullptr)
        {
            hasdiploma = false;

            for (obj = victim->last_carrying; obj; obj = obj->prev_content)
                if (obj->pIndexData == get_obj_index(OBJ_VNUM_SCHOOL_DIPLOMA))
                    hasdiploma = true;

            if (!hasdiploma)
            {
                obj = create_object(pObjIndex, 1);
                obj = obj_to_char(obj, victim);
                send_to_char("&cThe schoolmaster gives you a diploma, and shakes your hand.\n\r&w", victim);
            }
        }

        if (IS_NPC(victim) || !IS_SET(victim->pcdata->flags, PCFLAG_UNAUTHED))
            continue;

        victim->pcdata->auth_state = 3;
        REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
        if (victim->pcdata->authed_by)
            STRFREE(victim->pcdata->authed_by);
        victim->pcdata->authed_by = QUICKLINK(ch->name);
        sprintf_s(buf, "%s authorized %s", ch->name, victim->name);
        to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
    }
    return false;
}

bool spec_giveslug(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    CHAR_DATA* v_next;
    char buf[MAX_STRING_LENGTH];

    for (victim = ch->in_room->first_person; victim; victim = v_next)
    {
        v_next = victim->next_in_room;

        if (HAS_SLUG(victim) || IS_NPC(victim))
            continue;

        SET_BIT(victim->pcdata->flags, PCFLAG_HASSLUG);
        do_giveslug(ch, victim->name);
        sprintf_s(buf, "%s gave a slug to %s", ch->name, victim->name);
        to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
    }

    return false;
}
