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
#include "connection.hxx"

/* From newarena.c -- Tawnos */
extern void start_arena();
extern void do_game();
extern int in_start_arena;
extern int ppl_in_arena;
extern int ppl_challenged;
extern int num_in_arena();

/* from swskills.c */
void add_reinforcements(CHAR_DATA* ch);

/*ships.c*/
void make_random_marketlist(void);

/*
 * Local functions.
 */
int hit_gain(CHAR_DATA* ch);
int mana_gain(CHAR_DATA* ch);
int move_gain(CHAR_DATA* ch);
void gain_addiction(CHAR_DATA* ch);
void mobile_update(void);
void weather_update(void);
void update_taxes(void);
void char_update(void);
void obj_update(void);
void aggr_update(void);
void room_act_update(void);
void obj_act_update(void);
void char_check(void);
void drunk_randoms(CHAR_DATA* ch);
void halucinations(CHAR_DATA* ch);

/*
 * Global Variables
 */

CHAR_DATA* gch_prev;
OBJ_DATA* gobj_prev;

CHAR_DATA* timechar;

const char* corpse_descs[] = {"The corpse of %s will soon be gone.", "The corpse of %s lies here.",
                              "The corpse of %s lies here.", "The corpse of %s lies here.",
                              "The corpse of %s lies here."};

const char* d_corpse_descs[] = {"The shattered remains %s will soon be gone.", "The shattered remains %s are here.",
                                "The shattered remains %s are here.", "The shattered remains %s are here.",
                                "The shattered remains %s are here."};

extern int top_exit;

/*
 * Advancement stuff.
 */
int max_level(CHAR_DATA* ch, int ability)
{
    int level = 0;

    if (IS_NPC(ch))
        return 100;

    if (IS_IMMORTAL(ch))
        return 200;

    if (ability == COMBAT_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == ENGINEERING_ABILITY)
            level = 3;
        if (ch->main_ability == TECHNICIAN_ABILITY)
            level = 3;
        if (ch->main_ability == HUNTING_ABILITY)
            level = 12;
        if (ch->main_ability == ASSASSIN_ABILITY)
            level = 12;
        if (ch->main_ability == SMUGGLING_ABILITY)
            level = 5;
        if (ch->secondary_ability == ability)
            level = 25;
        // No Human
        if (ch->race == RACE_WOOKIEE)
            level += 6;
        // No Twilek
        if (ch->race == RACE_RODIAN)
            level += 2;
        // No hutt
        // No moncal
        if (ch->race == RACE_NOGHRI)
            level += 2;
        if (ch->race == RACE_GAMORREAN)
            level += 3;
        if (ch->race == RACE_JAWA)
            level -= 3;
        if (ch->race == RACE_ADARIAN)
            level += 2;
        // No Ewok
        // No verpine
        // No Defel
        if (ch->race == RACE_TRANDOSHAN)
            level += 2;
        if (ch->race == RACE_HAPAN)
            level += 1;
        if (ch->race == RACE_QUARREN)
            level += 2;
        if (ch->race == RACE_SHISTAVANEN)
            level += 2;
        if (ch->race == RACE_FALLEEN)
            level += 1;
        if (ch->race == RACE_ITHORIAN)
            level -= 1;
        if (ch->race == RACE_DEVARONIAN)
            level += 1;
        // No Gotal
        // no droid
        // No Firrerreo
        if (ch->race == RACE_BARABEL)
            level -= 2;
        if (ch->race == RACE_BOTHAN)
            level -= 2;
        if (ch->race == RACE_TOGORIAN)
            level += 4;
        if (ch->race == RACE_DUG)
            level -= 3;
        // No Kubaz
        if (ch->race == RACE_SELONIAN)
            level -= 1;
        // No Gran
        if (ch->race == RACE_YEVETHA)
            level -= 2;
        if (ch->race == RACE_GAND)
            level += 2;
        if (ch->race == RACE_DUROS)
            level -= 1;
        if (ch->race == RACE_COYNITE)
            level -= 1;
        if (ch->race == RACE_SELONIAN)
            level += 2;
        if (ch->race == RACE_PROTOCAL_DROID)
            level -= 3;
        if (ch->race == RACE_ASSASSIN_DROID)
            level += 2;
        if (ch->race == RACE_GLADIATOR_DROID)
            level += 6;
        if (ch->race == RACE_ASTROMECH_DROID)
            level -= 2;

        level += (ch->perm_con / 5) + (ch->perm_dex / 5);
    }

    if (ability == PILOTING_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == ENGINEERING_ABILITY)
            level = 10;
        if (ch->main_ability == TECHNICIAN_ABILITY)
            level = 10;
        if (ch->main_ability == HUNTING_ABILITY)
            level = 5;
        if (ch->main_ability == ASSASSIN_ABILITY)
            level = 5;
        if (ch->main_ability == SMUGGLING_ABILITY)
            level = 10;
        if (ch->secondary_ability == ability)
            level = 25;
        if (ch->race == RACE_HUTT)
            level -= 2;
        if (ch->race == RACE_JAWA)
            level -= 1;
        if (ch->race == RACE_VERPINE)
            level += 2;
        if (ch->race == RACE_ASTROMECH_DROID)
            level += 6;
        if (ch->race == RACE_SELONIAN)
            level -= 1;
        if (ch->race == RACE_DUROS)
            level += 2;
        if (ch->race == RACE_HUMAN)
            level += 1;
        if (ch->race == RACE_COYNITE)
            level -= 1;
        if (ch->race == RACE_KUBAZ)
            level -= 1;
        if (ch->race == RACE_GAMORREAN)
            level -= 2;
        if (ch->race == RACE_QUARREN)
            level += 2;
        if (ch->race == RACE_HAPAN)
            level += 3;
        if (ch->race == RACE_NOGHRI)
            level = -7;
        level += (ch->perm_dex / 2) + (ch->perm_int / 3);
    }

    if (ability == ENGINEERING_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == TECHNICIAN_ABILITY)
            level = 10;
        if (ch->main_ability == PILOTING_ABILITY)
            level = 8;
        if (ch->main_ability == SLICER_ABILITY)
            level = 5;
        if (ch->secondary_ability == ability)
            level = 25;
        // No Human
        if (ch->race == RACE_WOOKIEE)
            level += 2;
        // No Twilek
        if (ch->race == RACE_RODIAN)
            level -= 2;
        if (ch->race == RACE_HUTT)
            level -= 3;
        if (ch->race == RACE_MON_CALAMARI)
            level += 3;
        if (ch->race == RACE_NOGHRI)
            level -= 2;
        if (ch->race == RACE_GAMORREAN)
            level -= 3;
        if (ch->race == RACE_JAWA)
            level += 3;
        if (ch->race == RACE_ADARIAN)
            level += 2;
        // No Ewok
        if (ch->race == RACE_VERPINE)
            level += 5;
        // No Defel
        if (ch->race == RACE_TRANDOSHAN)
            level -= 2;
        if (ch->race == RACE_HAPAN)
            level += 1;
        if (ch->race == RACE_QUARREN)
            level += 2;
        if (ch->race == RACE_SHISTAVANEN)
            level -= 2;
        if (ch->race == RACE_FALLEEN)
            level += 1;
        // No Ithorian
        // No Devaronian
        // No Gotal
        if (ch->race == RACE_DROID)
            level += 3;
        // No Firrerreo
        if (ch->race == RACE_BARABEL)
            level -= 2;
        if (ch->race == RACE_BOTHAN)
            level += 1;
        if (ch->race == RACE_TOGORIAN)
            level -= 2;
        if (ch->race == RACE_DUG)
            level -= 3;
        // No Kubaz
        if (ch->race == RACE_SELONIAN)
            level += 1;
        // No Gran
        if (ch->race == RACE_YEVETHA)
            level += 2;
        if (ch->race == RACE_GAND)
            level += 2;
        if (ch->race == RACE_DUROS)
            level += 3;
        if (ch->race == RACE_COYNITE)
            level -= 1;
        if (ch->race == RACE_SELONIAN)
            level += 1;
        if (ch->race == RACE_PROTOCAL_DROID)
            level += 3;
        // no ass droid
        if (ch->race == RACE_GLADIATOR_DROID)
            level -= 5;
        if (ch->race == RACE_ASTROMECH_DROID)
            level += 2;
        level += ch->perm_int / 2;
    }

    if (ability == SLICER_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == SMUGGLING_ABILITY)
            level = 12;
        if (ch->main_ability == ASSASSIN_ABILITY)
            level = 1;
        if (ch->main_ability == HUNTING_ABILITY)
            level = 1;
        if (ch->main_ability == ENGINEERING_ABILITY)
            level = 5;
        if (ch->secondary_ability == ability)
            level = 25;
        if (ch->race == RACE_JAWA)
            level -= 2;
        if (ch->race == RACE_VERPINE)
            level += 3;
        if (ch->race == RACE_BOTHAN)
            level += 5;
        if (ch->race == RACE_HUMAN)
            level += 3;
        if (ch->race == RACE_WOOKIEE)
            level += 3;
        if (ch->race == RACE_KUBAZ)
            level += 2;
        if (ch->race == RACE_ASTROMECH_DROID)
            level += 3;
        if (ch->race == RACE_NOGHRI)
            level -= 4;
        level += ((ch->perm_wis / 8) + (ch->perm_cha / 8) + (ch->perm_int / 8));
    }

    if (ability == HUNTING_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->secondary_ability == ability)
            level = 25;

        if (ch->main_ability != ASSASSIN_ABILITY)
        {
            if (ch->race == RACE_WOOKIEE)
                level += 2;
            if (ch->race == RACE_RODIAN)
                level += 5;
            if (ch->race == RACE_NOGHRI)
                level += 5;
            if (ch->race == RACE_TWI_LEK)
                level += 2;
            if (ch->race == RACE_MON_CALAMARI)
                level += 3;
            if (ch->race == RACE_TRANDOSHAN)
                level += 3;
            if (ch->race == RACE_KUBAZ)
                level += 1;
        }

        if (ch->race == RACE_HAPAN)
            level -= 2;
        if (ch->race == RACE_JAWA)
            level -= 2;
        if (ch->race == RACE_ADARIAN)
            level -= 3;
        if (ch->race == RACE_HUTT)
            level -= 3;
    }

    if (ability == ASSASSIN_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->secondary_ability == ability)
            level = 25;
        if (ch->main_ability != HUNTING_ABILITY)
        {
            if (ch->race == RACE_WOOKIEE)
                level += 2;
            if (ch->race == RACE_ASSASSIN_DROID)
                level += 8;
            if (ch->race == RACE_RODIAN)
                level += 3;
            if (ch->race == RACE_TWI_LEK)
                level += 2;
            if (ch->race == RACE_MON_CALAMARI)
                level += 3;
            if (ch->race == RACE_TRANDOSHAN)
                level += 3;
            if (ch->race == RACE_KUBAZ)
                level += 3;
        }

        if (ch->race == RACE_HUTT)
            level -= 3;
        if (ch->race == RACE_HAPAN)
            level -= 2;
        if (ch->race == RACE_JAWA)
            level -= 2;
        if (ch->race == RACE_ADARIAN)
            level -= 3;
    }

    if (ability == SMUGGLING_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == PILOTING_ABILITY)
            level = 10;
        if (ch->main_ability == SLICER_ABILITY)
            level = 7;
        if (ch->main_ability == HUNTING_ABILITY)
            level = 3;
        if (ch->secondary_ability == ability)
            level = 25;
        if (ch->race == RACE_HUMAN)
            level += 1;
        // No Wookiee
        if (ch->race == RACE_TWI_LEK)
            level += 3;
        if (ch->race == RACE_RODIAN)
            level += 2;
        if (ch->race == RACE_HUTT)
            level += 5;
        if (ch->race == RACE_MON_CALAMARI)
            level += 2;
        if (ch->race == RACE_NOGHRI)
            level -= 2;
        if (ch->race == RACE_GAMORREAN)
            level -= 3;
        if (ch->race == RACE_JAWA)
            level -= 2;
        // No Adarian
        if (ch->race == RACE_EWOK)
            level += 2;
        // No Verpine
        if (ch->race == RACE_DEFEL)
            level += 4;
        if (ch->race == RACE_TRANDOSHAN)
            level -= 2;
        if (ch->race == RACE_HAPAN)
            level += 1;
        if (ch->race == RACE_QUARREN)
            level += 2;
        if (ch->race == RACE_SHISTAVANEN)
            level -= 1;
        if (ch->race == RACE_FALLEEN)
            level += 3;
        // No Ithorian
        if (ch->race == RACE_DEVARONIAN)
            level += 1;
        // No Gotal
        // No Firrerreo
        // No Barabel
        if (ch->race == RACE_BOTHAN)
            level += 3;
        // No Togororian
        if (ch->race == RACE_DUG)
            level -= 1;
        if (ch->race == RACE_KUBAZ)
            level += 3;
        if (ch->race == RACE_SELONIAN)
            level -= 1;
        // No Gran
        if (ch->race == RACE_YEVETHA)
            level += 1;
        // No Gand
        if (ch->race == RACE_DUROS)
            level += 2;
        // No Coynite
        // No Selonian
        // No Droids
        level += ch->perm_lck / 2;
    }

    if (ability == POLITICIAN_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == PILOTING_ABILITY)
            level = 10;
        if (ch->secondary_ability == ability)
            level = 25;
        // Gonna stop all that 'no race' stuff. Too many nos on politician.
        if (ch->race == RACE_HUMAN)
            level += 2;
        if (ch->race == RACE_WOOKIEE)
            level -= 2;
        if (ch->race == RACE_TWI_LEK)
            level -= 1;
        if (ch->race == RACE_RODIAN)
            level -= 3;
        if (ch->race == RACE_BOTHAN)
            level += 4;
        if (ch->race == RACE_BARABEL)
            level -= 1;
        if (ch->race == RACE_FIRRERREO)
            level -= 1;
        if (ch->race == RACE_HUTT)
            level -= 2;
        if (ch->race == RACE_COYNITE)
            level -= 2;
        if (ch->race == RACE_PROTOCAL_DROID)
            level += 4;
        if (ch->race == RACE_YEVETHA)
            level -= 2;
        if (ch->race == RACE_ITHORIAN)
            level += 2;
        if (ch->race == RACE_SELONIAN)
            level -= 1;
        if (ch->race == RACE_MON_CALAMARI)
            level += 3;
        if (ch->race == RACE_NOGHRI)
            level -= 2;
        if (ch->race == RACE_GAMORREAN)
            level -= 4;
        if (ch->race == RACE_JAWA)
            level -= 3;
        if (ch->race == RACE_ADARIAN)
            level += 4;
        if (ch->race == RACE_VERPINE)
            level -= 2;
        if (ch->race == RACE_DEFEL)
            level -= 2;
        if (ch->race == RACE_TRANDOSHAN)
            level -= 3;
        if (ch->race == RACE_HAPAN)
            level += 3;
        if (ch->race == RACE_QUARREN)
            level -= 2;
        if (ch->race == RACE_KUBAZ)
            level -= 4;
        if (ch->race == RACE_DEVARONIAN)
            level -= 2;
        if (ch->race == RACE_ASSASSIN_DROID)
            level -= 4;
        if (ch->race == RACE_GLADIATOR_DROID)
            level -= 4;
        if (ch->race == RACE_ASTROMECH_DROID)
            level -= 3;
        level += ch->perm_cha / 3 + ch->perm_wis / 3;
    }

    if (ability == TECHNICIAN_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 30;
        if (ch->main_ability == ENGINEERING_ABILITY)
            level = 12;
        if (ch->main_ability == PILOTING_ABILITY)
            level = 10;
        if (ch->main_ability == SLICER_ABILITY)
            level = 8;
        if (ch->secondary_ability == ability)
            level = 25;
        // No Human
        if (ch->race == RACE_WOOKIEE)
            level += 3;
        // No Twilek
        if (ch->race == RACE_RODIAN)
            level -= 1;
        if (ch->race == RACE_HUTT)
            level -= 2;
        if (ch->race == RACE_MON_CALAMARI)
            level += 4;
        if (ch->race == RACE_NOGHRI)
            level -= 1;
        if (ch->race == RACE_GAMORREAN)
            level -= 3;
        if (ch->race == RACE_JAWA)
            level += 1;
        if (ch->race == RACE_ADARIAN)
            level += 2;
        if (ch->race == RACE_EWOK)
            level -= 1;
        if (ch->race == RACE_VERPINE)
            level += 3;
        // No Defel
        if (ch->race == RACE_TRANDOSHAN)
            level -= 3;
        if (ch->race == RACE_HAPAN)
            level += 2;
        if (ch->race == RACE_QUARREN)
            level += 1;
        if (ch->race == RACE_SHISTAVANEN)
            level -= 2;
        // No Falleen
        if (ch->race == RACE_ITHORIAN)
            level += 1;
        if (ch->race == RACE_DEVARONIAN)
            level -= 1;
        // No Gotal
        if (ch->race == RACE_DROID)
            level += 2;
        // No Firrerreo
        if (ch->race == RACE_BARABEL)
            level -= 2;
        // No Bothan
        if (ch->race == RACE_TOGORIAN)
            level -= 2;
        if (ch->race == RACE_DUG)
            level -= 3;
        // No Kubaz
        // No Selonian
        // No Gran
        // No Yevetha
        if (ch->race == RACE_GAND)
            level += 4;
        if (ch->race == RACE_DUROS)
            level += 3;
        if (ch->race == RACE_COYNITE)
            level -= 2;
        if (ch->race == RACE_SELONIAN)
            level += 1;
        // No Glad, Ass, Proto Droids
        if (ch->race == RACE_ASTROMECH_DROID)
            level += 6;
        level += (ch->perm_int / 2) + (ch->perm_wis / 6);
    }

    // Level Bonuses for Good RP
    level += ch->bonus[ability];

    level = URANGE(1, level, 30);

    if (ability == FORCE_ABILITY)
    {
        if (ch->main_ability == ability)
            level = 20;
        else
            level = 0;
        level += ch->perm_frc * 5;
    }

    return level;
}

void advance_level(CHAR_DATA* ch, int ability)
{

    if (ch->top_level < ch->skill_level[ability] && ch->top_level < 30)
    {
        ch->top_level = URANGE(1, ch->skill_level[ability], 30);
    }

    if (!IS_NPC(ch))
        REMOVE_BIT(ch->act, PLR_BOUGHT_PET);

    return;
}

void gain_exp2(CHAR_DATA* ch, int gain, int ability)
{

    if (IS_NPC(ch))
        return;

    ch->experience[ability] = UMAX(0, ch->experience[ability] + gain);

    while (ch->experience[ability] >= exp_level(ch->skill_level[ability] + 1))
    {
        if (ch->skill_level[ability] >= max_level(ch, ability))
        {
            ch->experience[ability] = (exp_level(ch->skill_level[ability] + 1) - 1);
            return;
        }
        ++ch->skill_level[ability];
        advance_level(ch, ability);
    }

    return;
}

void gain_exp(CHAR_DATA* ch, int gain, int ability)
{

    if (IS_NPC(ch))
        return;

    ch->experience[ability] = UMAX(0, ch->experience[ability] + gain);

    if (NOT_AUTHED(ch) && ch->experience[ability] >= exp_level(ch->skill_level[ability] + 1))
    {
        send_to_char("You can not ascend to a higher level until you are authorized.\n\r", ch);
        ch->experience[ability] = (exp_level(ch->skill_level[ability] + 1) - 1);
        return;
    }

    while (ch->experience[ability] >= exp_level(ch->skill_level[ability] + 1))
    {
        if (ch->skill_level[ability] >= max_level(ch, ability))
        {
            ch->experience[ability] = (exp_level(ch->skill_level[ability] + 1) - 1);
            return;
        }
        set_char_color(AT_WHITE + AT_BLINK, ch);
        ch_printf(ch, "You have now obtained %s level %d!\n\r", ability_name[ability], ++ch->skill_level[ability]);
        advance_level(ch, ability);
    }

    return;
}

/*
 * Regeneration stuff.
 */
int hit_gain(CHAR_DATA* ch)
{
    int gain;

    if (IS_NPC(ch))
    {
        gain = ch->top_level;
    }
    else
    {
        gain = UMIN(5, ch->top_level);

        switch (ch->position)
        {
        case POS_DEAD:
            return 0;
        case POS_MORTAL:
            return -25;
        case POS_INCAP:
            return -20;
        case POS_STUNNED:
            return get_curr_con(ch) * 2;
        case POS_SLEEPING:
            gain += get_curr_con(ch) * 1.5;
            break;
        case POS_RESTING:
            gain += get_curr_con(ch);
            break;
        }

        if (ch->pcdata->condition[COND_FULL] == 0)
            gain /= 2;

        if (ch->pcdata->condition[COND_THIRST] == 0)
            gain /= 2;
    }

    if (IS_AFFECTED(ch, AFF_POISON))
        gain /= 4;

    if (get_age(ch) > 800)
    {
        send_to_char("You are very old.\n\rYou are becoming weaker with every moment.\n\rSoon you will die.\n\r", ch);
        return -10;
    }
    else if (get_age(ch) > 500)
        gain /= 10;
    else if (get_age(ch) > 400)
        gain /= 5;
    else if (get_age(ch) > 300)
        gain /= 2;

    if (ch->race == RACE_TRANDOSHAN)
        gain *= 2;

    if (IS_AFFECTED(ch, AFF_FASTHEAL))
        gain *= 2;

    return UMIN(gain, ch->max_hit - ch->hit);
}

int mana_gain(CHAR_DATA* ch)
{
    int gain;

    if (IS_NPC(ch))
    {
        gain = ch->top_level;
    }
    else
    {
        gain = 5;

        if (ch->position < POS_SLEEPING)
            return 0;
        switch (ch->position)
        {
        case POS_SLEEPING:
            gain += get_curr_int(ch) * 3;
            break;
        case POS_RESTING:
            gain += get_curr_int(ch) * 1.5;
            break;
        }

        if (ch->pcdata->condition[COND_FULL] == 0)
            gain /= 2;

        if (ch->pcdata->condition[COND_THIRST] == 0)
            gain /= 2;
    }

    if (IS_AFFECTED(ch, AFF_POISON))
        gain /= 4;

    return UMIN(gain, ch->max_mana - ch->mana);
}

int move_gain(CHAR_DATA* ch)
{
    int gain;

    if (IS_NPC(ch))
    {
        gain = ch->top_level;
    }
    else
    {
        gain = UMAX(15, 2 * ch->top_level);

        switch (ch->position)
        {
        case POS_DEAD:
            return 0;
        case POS_MORTAL:
            return -1;
        case POS_INCAP:
            return -1;
        case POS_STUNNED:
            return 1;
        case POS_SLEEPING:
            gain += get_curr_dex(ch) * 2;
            break;
        case POS_RESTING:
            gain += get_curr_dex(ch);
            break;
        }

        if (ch->pcdata->condition[COND_FULL] == 0)
            gain /= 2;

        if (ch->pcdata->condition[COND_THIRST] == 0)
            gain /= 2;
    }

    if (IS_AFFECTED(ch, AFF_POISON))
        gain /= 4;

    if (get_age(ch) > 500)
        gain /= 10;
    else if (get_age(ch) > 300)
        gain /= 5;
    else if (get_age(ch) > 200)
        gain /= 2;

    return UMIN(gain, ch->max_move - ch->move);
}

void gain_addiction(CHAR_DATA* ch)
{
    short drug;
    ch_ret retcode;
    AFFECT_DATA af;

    for (drug = 0; drug <= 9; drug++)
    {

        if (ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug])
            ch->pcdata->addiction[drug]++;

        if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 150)
        {
            switch (ch->pcdata->addiction[drug])
            {
            default:
            case SPICE_GLITTERSTIM:
                if (!IS_AFFECTED(ch, AFF_BLIND))
                {
                    af.type = gsn_blindness;
                    af.location = APPLY_AC;
                    af.modifier = 10;
                    af.duration = ch->pcdata->addiction[drug];
                    af.bitvector = AFF_BLIND;
                    affect_to_char(ch, &af);
                }
            case SPICE_CARSANUM:
                if (!IS_AFFECTED(ch, AFF_WEAKEN))
                {
                    af.type = -1;
                    af.location = APPLY_DAMROLL;
                    af.modifier = -10;
                    af.duration = ch->pcdata->addiction[drug];
                    af.bitvector = AFF_WEAKEN;
                    affect_to_char(ch, &af);
                }
            case SPICE_RYLL:
                if (!IS_AFFECTED(ch, AFF_WEAKEN))
                {
                    af.type = -1;
                    af.location = APPLY_DEX;
                    af.modifier = -5;
                    af.duration = ch->pcdata->addiction[drug];
                    af.bitvector = AFF_WEAKEN;
                    affect_to_char(ch, &af);
                }
            case SPICE_ANDRIS:
                if (!IS_AFFECTED(ch, AFF_WEAKEN))
                {
                    af.type = -1;
                    af.location = APPLY_CON;
                    af.modifier = -5;
                    af.duration = ch->pcdata->addiction[drug];
                    af.bitvector = AFF_WEAKEN;
                    affect_to_char(ch, &af);
                }
            }
        }

        if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 200)
        {
            ch_printf(ch, "You feel like you are going to die. You NEED %s\n\r.", spice_table[drug]);
            worsen_mental_state(ch, 2);
            retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
        }
        else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 100)
        {
            ch_printf(ch, "You need some %s.\n\r", spice_table[drug]);
            worsen_mental_state(ch, 2);
        }
        else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 50)
        {
            ch_printf(ch, "You really crave some %s.\n\r", spice_table[drug]);
            worsen_mental_state(ch, 1);
        }
        else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 25)
        {
            ch_printf(ch, "Some more %s would feel quite nice.\n\r", spice_table[drug]);
        }
        else if (ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug] - 50)
        {
            act(AT_POISON, "$n bends over and vomits.\n\r", ch, nullptr, nullptr, TO_ROOM);
            act(AT_POISON, "You vomit.\n\r", ch, nullptr, nullptr, TO_CHAR);
            ch->pcdata->drug_level[drug] -= 10;
        }

        if (ch->pcdata->drug_level[drug] > 1)
            ch->pcdata->drug_level[drug] -= 2;
        else if (ch->pcdata->drug_level[drug] > 0)
            ch->pcdata->drug_level[drug] -= 1;
        else if (ch->pcdata->addiction[drug] > 0 && ch->pcdata->drug_level[drug] <= 0)
            ch->pcdata->addiction[drug]--;
    }
}

void gain_condition(CHAR_DATA* ch, int iCond, int value)
{
    int condition = 0;
    ch_ret retcode = 0;

    if (value == 0 || IS_NPC(ch) || get_trust(ch) >= LEVEL_IMMORTAL || NOT_AUTHED(ch) || IS_DROID(ch))
        return;

    condition = ch->pcdata->condition[iCond];
    ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);

    if (ch->pcdata->condition[iCond] == 0)
    {
        switch (iCond)
        {
        case COND_FULL:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_HUNGRY, ch);
                send_to_char("You are STARVING!\n\r", ch);
                act(AT_HUNGRY, "$n is starved half to death!", ch, nullptr, nullptr, TO_ROOM);
                worsen_mental_state(ch, 1);
                retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
            }
            break;

        case COND_THIRST:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_THIRSTY, ch);
                send_to_char("You are DYING of THIRST!\n\r", ch);
                act(AT_THIRSTY, "$n is dying of thirst!", ch, nullptr, nullptr, TO_ROOM);
                worsen_mental_state(ch, 2);
                retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
            }
            break;

        case COND_DRUNK:
            if (condition != 0)
            {
                set_char_color(AT_SOBER, ch);
                send_to_char("You are sober.\n\r", ch);
            }
            retcode = rNONE;
            break;
        default:
            bug("Gain_condition: invalid condition type %d", iCond);
            retcode = rNONE;
            break;
        }
    }

    if (retcode != rNONE)
        return;

    if (ch->pcdata->condition[iCond] == 1)
    {
        switch (iCond)
        {
        case COND_FULL:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_HUNGRY, ch);
                send_to_char("You are really hungry.\n\r", ch);
                act(AT_HUNGRY, "You can hear $n's stomach growling.", ch, nullptr, nullptr, TO_ROOM);
                if (number_bits(1) == 0)
                    worsen_mental_state(ch, 1);
            }
            break;

        case COND_THIRST:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_THIRSTY, ch);
                send_to_char("You are really thirsty.\n\r", ch);
                worsen_mental_state(ch, 1);
                act(AT_THIRSTY, "$n looks a little parched.", ch, nullptr, nullptr, TO_ROOM);
            }
            break;

        case COND_DRUNK:
            if (condition != 0)
            {
                set_char_color(AT_SOBER, ch);
                send_to_char("You are feeling a little less light headed.\n\r", ch);
            }
            break;
        }
    }

    if (ch->pcdata->condition[iCond] == 2)
    {
        switch (iCond)
        {
        case COND_FULL:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_HUNGRY, ch);
                send_to_char("You are hungry.\n\r", ch);
            }
            break;

        case COND_THIRST:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_THIRSTY, ch);
                send_to_char("You are thirsty.\n\r", ch);
            }
            break;
        }
    }

    if (ch->pcdata->condition[iCond] == 3)
    {
        switch (iCond)
        {
        case COND_FULL:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_HUNGRY, ch);
                send_to_char("You are a mite peckish.\n\r", ch);
            }
            break;

        case COND_THIRST:
            if (ch->top_level < LEVEL_NOHUNGER)
            {
                set_char_color(AT_THIRSTY, ch);
                send_to_char("You could use a sip of something refreshing.\n\r", ch);
            }
            break;
        }
    }
    return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update(void)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA* ch;
    EXIT_DATA* pexit;
    int door;
    ch_ret retcode;

    retcode = rNONE;

    /* Examine all mobs. */
    for (ch = last_char; ch; ch = gch_prev)
    {
        set_cur_char(ch);
        if (ch == first_char && ch->prev)
        {
            bug("mobile_update: first_char->prev != nullptr... fixed", 0);
            ch->prev = nullptr;
        }

        gch_prev = ch->prev;

        if (gch_prev && gch_prev->next != ch)
        {
            sprintf_s(buf, "FATAL: Mobile_update: %s->prev->next doesn't point to ch.", ch->name);
            bug(buf, 0);
            bug("Short-cutting here", 0);
            gch_prev = nullptr;
            ch->prev = nullptr;
            do_shout(ch, MAKE_TEMP_STRING("Thoric says, 'Prepare for the worst!'"));
        }

        if (!IS_NPC(ch))
        {
            if (ch->wait_state > 0)
                ch->wait_state--;
            if (ch->wait_state < 0)
                ch->wait_state = 0;

            drunk_randoms(ch);
            halucinations(ch);
            continue;
        }

        if (!ch->in_room || IS_AFFECTED(ch, AFF_CHARM) || IS_AFFECTED(ch, AFF_PARALYSIS))
            continue;

        /* Clean up 'animated corpses' that are not charmed' - Scryn */

        if (ch->pIndexData->vnum == 5 && !IS_AFFECTED(ch, AFF_CHARM))
        {
            if (ch->in_room->first_person)
                act(AT_MAGIC, "$n returns to the dust from whence $e came.", ch, nullptr, nullptr, TO_ROOM);

            if (IS_NPC(ch)) /* Guard against purging switched? */
                extract_char(ch, true);
            continue;
        }

        if (!IS_SET(ch->act, ACT_RUNNING) && !IS_SET(ch->act, ACT_SENTINEL) && !ch->fighting && ch->hunting)
        {
            if (ch->top_level < 20)
                WAIT_STATE(ch, 6 * PULSE_PER_SECOND);
            else if (ch->top_level < 40)
                WAIT_STATE(ch, 5 * PULSE_PER_SECOND);
            else if (ch->top_level < 60)
                WAIT_STATE(ch, 4 * PULSE_PER_SECOND);
            else if (ch->top_level < 80)
                WAIT_STATE(ch, 3 * PULSE_PER_SECOND);
            else if (ch->top_level < 100)
                WAIT_STATE(ch, 2 * PULSE_PER_SECOND);
            else
                WAIT_STATE(ch, 1 * PULSE_PER_SECOND);
            hunt_victim(ch);
            continue;
        }
        else if (!ch->fighting && !ch->hunting && !IS_SET(ch->act, ACT_RUNNING) && ch->was_sentinel &&
                 ch->position >= POS_STANDING)
        {
            act(AT_ACTION, "$n leaves.", ch, nullptr, nullptr, TO_ROOM);
            char_from_room(ch);
            char_to_room(ch, ch->was_sentinel);
            act(AT_ACTION, "$n arrives.", ch, nullptr, nullptr, TO_ROOM);
            SET_BIT(ch->act, ACT_SENTINEL);
            ch->was_sentinel = nullptr;
        }

        /* Examine call for special procedure */
        if (!IS_SET(ch->act, ACT_RUNNING) && ch->spec_fun)
        {
            if ((*ch->spec_fun)(ch))
                continue;
            if (char_died(ch))
                continue;
        }

        if (!IS_SET(ch->act, ACT_RUNNING) && ch->spec_2)
        {
            if ((*ch->spec_2)(ch))
                continue;
            if (char_died(ch))
                continue;
        }

        /* Check for mudprogram script on mob */
        if (IS_SET(ch->pIndexData->progtypes, SCRIPT_PROG))
        {
            mprog_script_trigger(ch);
            continue;
        }

        if (ch != cur_char)
        {
            bug("Mobile_update: ch != cur_char after spec_fun", 0);
            continue;
        }

        /* That's all for sleeping / busy monster */
        if (ch->position != POS_STANDING)
            continue;

        if (IS_SET(ch->act, ACT_MOUNTED))
        {
            if (IS_SET(ch->act, ACT_AGGRESSIVE))
                do_emote(ch, MAKE_TEMP_STRING("snarls and growls."));
            continue;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && IS_SET(ch->act, ACT_AGGRESSIVE))
            do_emote(ch, MAKE_TEMP_STRING("glares around and snarls."));

        /* MOBprogram random trigger */
        if (ch->in_room->area && ch->in_room->area->nplayer > 0)
        {
            mprog_random_trigger(ch);
            if (char_died(ch))
                continue;
            if (ch->position < POS_STANDING)
                continue;
        }

        /* MOBprogram hour trigger: do something for an hour */
        mprog_hour_trigger(ch);

        if (char_died(ch))
            continue;

        rprog_hour_trigger(ch);
        if (char_died(ch))
            continue;

        if (ch->position < POS_STANDING)
            continue;

        /* Scavenge */
        if (IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->first_content && number_bits(2) == 0)
        {
            OBJ_DATA* obj;
            OBJ_DATA* obj_best;
            int max;

            max = 1;
            obj_best = nullptr;
            for (obj = ch->in_room->first_content; obj; obj = obj->next_content)
            {
                if (CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max && !IS_OBJ_STAT(obj, ITEM_BURRIED))
                {
                    obj_best = obj;
                    max = obj->cost;
                }
            }

            if (obj_best)
            {
                obj_from_room(obj_best);
                obj_to_char(obj_best, ch);
                act(AT_ACTION, "$n gets $p.", ch, obj_best, nullptr, TO_ROOM);
            }
        }

        /* Wander */
        if (!IS_SET(ch->act, ACT_RUNNING) && !IS_SET(ch->act, ACT_SENTINEL) && !IS_SET(ch->act, ACT_PROTOTYPE) &&
            (door = number_bits(5)) <= 9 && (pexit = get_exit(ch->in_room, door)) != nullptr && pexit->to_room &&
            !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) &&
            ((!IS_SET(ch->act, ACT_STAY_AREA) && pexit->to_room->area) || pexit->to_room->area == ch->in_room->area))
        {
            retcode = move_char(ch, pexit, 0);
            /* If ch changes position due
            to it's or someother mob's
            movement via MOBProgs,
            continue - Kahn */
            if (char_died(ch))
                continue;
            if (retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL) || ch->position < POS_STANDING)
                continue;
        }

        /* Flee */
        if (ch->hit < ch->max_hit / 2 && (door = number_bits(4)) <= 9 &&
            (pexit = get_exit(ch->in_room, door)) != nullptr && pexit->to_room && !IS_SET(pexit->exit_info, EX_CLOSED) &&
            !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
        {
            CHAR_DATA* rch;
            bool found;

            found = false;
            for (rch = ch->in_room->first_person; rch; rch = rch->next_in_room)
            {
                if (is_fearing(ch, rch))
                {
                    switch (number_bits(2))
                    {
                    case 0:
                        sprintf_s(buf, "Get away from me, %s!", rch->name);
                        break;
                    case 1:
                        sprintf_s(buf, "Leave me be, %s!", rch->name);
                        break;
                    case 2:
                        sprintf_s(buf, "%s is trying to kill me!  Help!", rch->name);
                        break;
                    case 3:
                        sprintf_s(buf, "Someone save me from %s!", rch->name);
                        break;
                    }
                    do_yell(ch, buf);
                    found = true;
                    break;
                }
            }
            if (found)
                retcode = move_char(ch, pexit, 0);
        }
    }

    return;
}

void update_taxes(void)
{
    PLANET_DATA* planet = nullptr;
    CLAN_DATA* clan = nullptr;

    for (planet = first_planet; planet; planet = planet->next)
    {
        // TODO Do I need to set these values for cargo to work?
        /*
        for( i = 1; i < CARGO_MAX; i++)
        {
            planet->resource[i] += planet->produces[i];
            planet->resource[i] -= planet->consumes[i];
        }
        */
        save_planet(planet);
        clan = planet->governed_by;
        if (clan)
        {
            int sCount = 0;
            CLAN_DATA* subclan = nullptr;

            if (clan->first_subclan)
            {
                for (subclan = clan->first_subclan; subclan; subclan = subclan->next_subclan)
                    sCount++;

                for (subclan = clan->first_subclan; subclan; subclan = subclan->next_subclan)
                {
                    subclan->funds += get_taxes(planet) / sCount / 2;
                    save_clan(subclan);
                }

                clan->funds += get_taxes(planet) / 2;
                save_clan(clan);
            }
            else
            {
                clan->funds += get_taxes(planet) / 2;
                save_clan(clan);
            }
            save_planet(planet);
        }
    }
}

/*
 * Update the weather.
 */
void weather_update(void)
{
    char buf[MAX_STRING_LENGTH];
    int diff;
    sh_int AT_TEMP = AT_PLAIN;

    buf[0] = '\0';

    switch (++time_info.hour)
    {
    case 5:
        weather_info.sunlight = SUN_LIGHT;
        strcat_s(buf, "The day has begun.");
        AT_TEMP = AT_YELLOW;
        break;

    case 6:
        weather_info.sunlight = SUN_RISE;
        strcat_s(buf, "The sun rises in the east.");
        AT_TEMP = AT_ORANGE;
        break;

    case 12:
        weather_info.sunlight = SUN_LIGHT;
        strcat_s(buf, "It's noon.");
        AT_TEMP = AT_YELLOW;
        break;

    case 19:
        weather_info.sunlight = SUN_SET;
        strcat_s(buf, "The sun slowly disappears in the west.");
        AT_TEMP = AT_BLOOD;
        break;

    case 20:
        weather_info.sunlight = SUN_DARK;
        strcat_s(buf, "The night has begun.");
        AT_TEMP = AT_DGREY;
        break;

    case 24:
        time_info.hour = 0;
        time_info.day++;
        break;
    }

    if (time_info.day >= 30)
    {
        time_info.day = 0;
        time_info.month++;
    }

    if (time_info.month >= 17)
    {
        time_info.month = 0;
        time_info.year++;
    }

    if (buf[0] != '\0')
    {
        for (auto d : g_descriptors)
        {
            if (d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character) &&
                d->character->in_room && d->character->in_room->sector_type != SECT_UNDERWATER &&
                d->character->in_room->sector_type != SECT_OCEANFLOOR &&
                d->character->in_room->sector_type != SECT_UNDERGROUND)
                act(AT_TEMP, buf, d->character, 0, 0, TO_CHAR);
        }
        buf[0] = '\0';
    }
    /*
     * Weather change.
     */
    if (time_info.month >= 9 && time_info.month <= 16)
        diff = weather_info.mmhg > 985 ? -2 : 2;
    else
        diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change = UMAX(weather_info.change, -12);
    weather_info.change = UMIN(weather_info.change, 12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg = UMAX(weather_info.mmhg, 960);
    weather_info.mmhg = UMIN(weather_info.mmhg, 1040);

    AT_TEMP = AT_GREY;
    switch (weather_info.sky)
    {
    default:
        bug("Weather_update: bad sky %d.", weather_info.sky);
        weather_info.sky = SKY_CLOUDLESS;
        break;

    case SKY_CLOUDLESS:
        if (weather_info.mmhg < 990 || (weather_info.mmhg < 1010 && number_bits(2) == 0))
        {
            strcat_s(buf, "The sky is getting cloudy.");
            weather_info.sky = SKY_CLOUDY;
            AT_TEMP = AT_GREY;
        }
        break;

    case SKY_CLOUDY:
        if (weather_info.mmhg < 970 || (weather_info.mmhg < 990 && number_bits(2) == 0))
        {
            strcat_s(buf, "It starts to rain.");
            weather_info.sky = SKY_RAINING;
            AT_TEMP = AT_BLUE;
        }

        if (weather_info.mmhg > 1030 && number_bits(2) == 0)
        {
            strcat_s(buf, "The clouds disappear.");
            weather_info.sky = SKY_CLOUDLESS;
            AT_TEMP = AT_WHITE;
        }
        break;

    case SKY_RAINING:
        if (weather_info.mmhg < 970 && number_bits(2) == 0)
        {
            strcat_s(buf, "Lightning flashes in the sky.");
            weather_info.sky = SKY_LIGHTNING;
            AT_TEMP = AT_YELLOW;
        }

        if (weather_info.mmhg > 1030 || (weather_info.mmhg > 1010 && number_bits(2) == 0))
        {
            strcat_s(buf, "The rain stopped.");
            weather_info.sky = SKY_CLOUDY;
            AT_TEMP = AT_WHITE;
        }
        break;

    case SKY_LIGHTNING:
        if (weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits(2) == 0))
        {
            strcat_s(buf, "The lightning has stopped.");
            weather_info.sky = SKY_RAINING;
            AT_TEMP = AT_GREY;
            break;
        }
        break;
    }

    if (buf[0] != '\0')
    {
        for (auto d : g_descriptors)
        {
            if (d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character))
                act(AT_TEMP, buf, d->character, 0, 0, TO_CHAR);
        }
    }

    return;
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update(void)
{
    CHAR_DATA* ch;
    CHAR_DATA* ch_save;
    long amount;
    sh_int save_count = 0;

    ch_save = nullptr;
    for (ch = last_char; ch; ch = gch_prev)
    {
        if (ch == first_char && ch->prev)
        {
            bug("char_update: first_char->prev != nullptr... fixed", 0);
            ch->prev = nullptr;
        }
        gch_prev = ch->prev;
        set_cur_char(ch);
        if (gch_prev && gch_prev->next != ch)
        {
            bug("char_update: ch->prev->next != ch", 0);
            return;
        }

        /*
         *  Do a room_prog rand check right off the bat
         *   if ch disappears (rprog might wax npc's), continue
         */
        if (!IS_NPC(ch))
            rprog_random_trigger(ch);

        if (char_died(ch))
            continue;

        if (IS_NPC(ch))
            mprog_time_trigger(ch);

        if (char_died(ch))
            continue;

        rprog_time_trigger(ch);

        if (char_died(ch))
            continue;

        /*
         * See if player should be auto-saved.
         */
        if (!IS_NPC(ch) && !NOT_AUTHED(ch) && current_time - ch->save_time > (sysdata.save_frequency * 60))
            ch_save = ch;
        else
            ch_save = nullptr;

        if ((ch->perm_frc == 0 && !IS_IMMORTAL(ch)) || ch->skill_level[FORCE_ABILITY] == 0)
            ch->skill_level[FORCE_ABILITY] = 0;

        if (ch->position >= POS_STUNNED)
        {
            if (ch->hit < ch->max_hit)
                ch->hit += hit_gain(ch);

            if (ch->mana < ch->max_mana || ch->skill_level[FORCE_ABILITY] == 1)
                ch->mana += mana_gain(ch);

            if (ch->move < ch->max_move)
                ch->move += move_gain(ch);
        }

        if (ch->position == POS_STUNNED)
            update_pos(ch);

        if (ch->pcdata)
            gain_addiction(ch);

        if (!IS_NPC(ch) && ch->top_level < LEVEL_IMMORTAL)
        {
            OBJ_DATA* obj;

            if ((obj = get_eq_char(ch, WEAR_LIGHT)) != nullptr && obj->item_type == ITEM_LIGHT && obj->value[2] > 0)
            {
                if (--obj->value[2] == 0 && ch->in_room)
                {
                    ch->in_room->light -= obj->count;
                    act(AT_ACTION, "$p goes out.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "$p goes out.", ch, obj, nullptr, TO_CHAR);
                    if (obj->serial == cur_obj)
                        global_objcode = rOBJ_EXPIRED;
                    extract_obj(obj);
                }
            }

            if (ch->pcdata->condition[COND_DRUNK] > 8)
                worsen_mental_state(ch, ch->pcdata->condition[COND_DRUNK] / 8);
            if (ch->pcdata->condition[COND_FULL] > 1)
            {
                switch (ch->position)
                {
                case POS_SLEEPING:
                    better_mental_state(ch, 4);
                    break;
                case POS_RESTING:
                    better_mental_state(ch, 3);
                    break;
                case POS_SITTING:
                case POS_MOUNTED:
                    better_mental_state(ch, 2);
                    break;
                case POS_STANDING:
                    better_mental_state(ch, 1);
                    break;
                case POS_FIGHTING:
                    if (number_bits(2) == 0)
                        better_mental_state(ch, 1);
                    break;
                }
            }
            if (ch->pcdata->condition[COND_THIRST] > 1)
            {
                switch (ch->position)
                {
                case POS_SLEEPING:
                    better_mental_state(ch, 5);
                    break;
                case POS_RESTING:
                    better_mental_state(ch, 3);
                    break;
                case POS_SITTING:
                case POS_MOUNTED:
                    better_mental_state(ch, 2);
                    break;
                case POS_STANDING:
                    better_mental_state(ch, 1);
                    break;
                case POS_FIGHTING:
                    if (number_bits(2) == 0)
                        better_mental_state(ch, 1);
                    break;
                }
            }
            gain_condition(ch, COND_DRUNK, -1);

            if (number_bits(1) == 0)
                gain_condition(ch, COND_FULL, -1);

            if (ch->in_room)
                switch (ch->in_room->sector_type)
                {
                default:
                    if (number_bits(1) == 0)
                        gain_condition(ch, COND_THIRST, -1);
                    break;
                case SECT_DESERT:
                    if (number_bits(1) == 0)
                        gain_condition(ch, COND_THIRST, -2);
                    break;
                case SECT_UNDERWATER:
                case SECT_OCEANFLOOR:
                    if (number_bits(1) == 0)
                        gain_condition(ch, COND_THIRST, -1);
                    break;
                }
        }

        if (!char_died(ch))
        {
            /*
             * Careful with the damages here,
             *   MUST NOT refer to ch after damage taken,
             *   as it may be lethal damage (on NPC).
             */
            if (IS_AFFECTED(ch, AFF_POISON))
            {
                act(AT_POISON, "$n shivers and suffers.", ch, nullptr, nullptr, TO_ROOM);
                act(AT_POISON, "You shiver and suffer.", ch, nullptr, nullptr, TO_CHAR);
                ch->mental_state = URANGE(20, ch->mental_state + 4, 100);
                damage(ch, ch, 6, gsn_poison);
            }
            else if (ch->position == POS_INCAP)
                damage(ch, ch, 1, TYPE_UNDEFINED);
            else if (ch->position == POS_MORTAL)
                damage(ch, ch, 4, TYPE_UNDEFINED);
            if (char_died(ch))
                continue;
            if (ch->mental_state >= 30)
                switch ((ch->mental_state + 5) / 10)
                {
                case 3:
                    send_to_char("You feel feverish.\n\r", ch);
                    act(AT_ACTION, "$n looks kind of out of it.", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 4:
                    send_to_char("You do not feel well at all.\n\r", ch);
                    act(AT_ACTION, "$n doesn't look too good.", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 5:
                    send_to_char("You need help!\n\r", ch);
                    act(AT_ACTION, "$n looks like $e could use your help.", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 6:
                    send_to_char("Seekest thou a cleric.\n\r", ch);
                    act(AT_ACTION, "Someone should fetch a healer for $n.", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 7:
                    send_to_char("You feel reality slipping away...\n\r", ch);
                    act(AT_ACTION, "$n doesn't appear to be aware of what's going on.", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 8:
                    send_to_char("You begin to understand... everything.\n\r", ch);
                    act(AT_ACTION, "$n starts ranting like a madman!", ch, nullptr, nullptr, TO_ROOM);
                    break;
                case 9:
                    send_to_char("You are ONE with the universe.\n\r", ch);
                    act(AT_ACTION, "$n is ranting on about 'the answer', 'ONE' and other mumbo-jumbo...", ch, nullptr,
                        nullptr, TO_ROOM);
                    break;
                case 10:
                    send_to_char("You feel the end is near.\n\r", ch);
                    act(AT_ACTION, "$n is muttering and ranting in tongues...", ch, nullptr, nullptr, TO_ROOM);
                    break;
                }
            if (ch->mental_state <= -30)
                switch ((abs(ch->mental_state) + 5) / 10)
                {
                case 10:
                    if (ch->position > POS_SLEEPING)
                    {
                        if ((ch->position == POS_STANDING || ch->position < POS_FIGHTING) &&
                            number_percent() + 10 < abs(ch->mental_state))
                            do_sleep(ch, MAKE_TEMP_STRING(""));
                        else
                            send_to_char("You're barely conscious.\n\r", ch);
                    }
                    break;
                case 9:
                    if (ch->position > POS_SLEEPING)
                    {
                        if ((ch->position == POS_STANDING || ch->position < POS_FIGHTING) &&
                            (number_percent() + 20) < abs(ch->mental_state))
                            do_sleep(ch, MAKE_TEMP_STRING(""));
                        else
                            send_to_char("You can barely keep your eyes open.\n\r", ch);
                    }
                    break;
                case 8:
                    if (ch->position > POS_SLEEPING)
                    {
                        if (ch->position < POS_SITTING && (number_percent() + 30) < abs(ch->mental_state))
                            do_sleep(ch, MAKE_TEMP_STRING(""));
                        else
                            send_to_char("You're extremely drowsy.\n\r", ch);
                    }
                    break;
                case 7:
                    if (ch->position > POS_RESTING)
                        send_to_char("You feel very unmotivated.\n\r", ch);
                    break;
                case 6:
                    if (ch->position > POS_RESTING)
                        send_to_char("You feel sedated.\n\r", ch);
                    break;
                case 5:
                    if (ch->position > POS_RESTING)
                        send_to_char("You feel sleepy.\n\r", ch);
                    break;
                case 4:
                    if (ch->position > POS_RESTING)
                        send_to_char("You feel tired.\n\r", ch);
                    break;
                case 3:
                    if (ch->position > POS_RESTING)
                        send_to_char("You could use a rest.\n\r", ch);
                    break;
                }

            if (ch->backup_wait > 0)
            {
                --ch->backup_wait;
                if (ch->backup_wait == 0)
                    add_reinforcements(ch);
            }

            if (!IS_NPC(ch) && time_info.hour == 1 && ch->pcdata->salary && ch->pcdata->clan &&
                ch->pcdata->salary < ch->pcdata->clan->funds)
            {
                ch->pcdata->bank += ch->pcdata->salary;
                ch->pcdata->clan->funds -= ch->pcdata->salary;
                ch_printf(
                    ch, "&B[&YBank Info&W: %s&B] &WYour salary of %d credits has been deposited into your account.\n\r",
                    ch->name, ch->pcdata->salary);
            }

            if (!IS_NPC(ch) && ch->pcdata->bank &&
                (time_info.day == 5 || time_info.day == 15 || time_info.day == 25 || time_info.day == 10 ||
                 time_info.day == 20 || time_info.day == 30) &&
                time_info.hour == 1)
            {
                char buf[MAX_STRING_LENGTH];
                amount = ch->pcdata->bank * .02;
                ch->pcdata->bank += amount;
                sprintf_s(buf, "&B[&YBank Info&W: %s&B] &WYou made&W: &Y%ld&W interest this month.\n\r", ch->name,
                          amount);
                send_to_char(buf, ch);
            }

            if (!IS_NPC(ch))
            {
                if (++ch->timer > 15 && !ch->desc)
                {
                    if (ch->in_room && IS_SET(ch->in_room->room_flags, ROOM_HOTEL))
                    {
                        ch->hit = UMAX(1, ch->hit);
                        save_char_obj(ch);
                        ch->position = POS_RESTING;
                        do_quit(ch, MAKE_TEMP_STRING(""));
                    }
                    else
                    {
                        char_from_room(ch);
                        char_to_room(ch, get_room_index(1057));
                        ch->position = POS_RESTING;
                        ch->hit = UMAX(1, ch->hit);
                        save_char_obj(ch);
                        do_quit(ch, MAKE_TEMP_STRING(""));
                    }
                }
                else if (ch == ch_save && IS_SET(sysdata.save_flags, SV_AUTO) &&
                         ++save_count < 10) /* save max of 10 per tick */
                    save_char_obj(ch);
            }
        }
    }

    return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
    OBJ_DATA* obj;
    sh_int AT_TEMP;

    for (obj = last_object; obj; obj = gobj_prev)
    {
        CHAR_DATA* rch;
        const char* message;

        if (obj == first_object && obj->prev)
        {
            bug("obj_update: first_object->prev != nullptr... fixed", 0);
            obj->prev = nullptr;
        }
        gobj_prev = obj->prev;
        if (gobj_prev && gobj_prev->next != obj)
        {
            bug("obj_update: obj->prev->next != obj", 0);
            return;
        }
        set_cur_obj(obj);
        if (obj->carried_by)
            oprog_random_trigger(obj);
        else if (obj->in_room && obj->in_room->area && obj->in_room->area->nplayer > 0)
            oprog_random_trigger(obj);

        if (obj_extracted(obj))
            continue;

        if (obj->item_type == ITEM_WEAPON && obj->carried_by &&
            (obj->wear_loc == WEAR_WIELD || obj->wear_loc == WEAR_DUAL_WIELD) && obj->value[3] != WEAPON_BLASTER &&
            obj->value[4] > 0 && obj->value[3] != WEAPON_BOWCASTER && obj->value[3] != WEAPON_FORCE_PIKE)
        {
            obj->value[4]--;
            if (obj->value[4] <= 0)
            {
                if (obj->value[3] == WEAPON_LIGHTSABER)
                {
                    act(AT_PLAIN, "$p fizzles and dies.", obj->carried_by, obj, nullptr, TO_CHAR);
                    act(AT_PLAIN, "$n's lightsaber fizzles and dies.", obj->carried_by, nullptr, nullptr, TO_ROOM);
                }
                else if (obj->value[3] == WEAPON_VIBRO_BLADE)
                {
                    act(AT_PLAIN, "$p stops vibrating.", obj->carried_by, obj, nullptr, TO_CHAR);
                }
            }
        }

        if (obj->item_type == ITEM_PIPE)
        {
            if (IS_SET(obj->value[3], PIPE_LIT))
            {
                if (--obj->value[1] <= 0)
                {
                    obj->value[1] = 0;
                    REMOVE_BIT(obj->value[3], PIPE_LIT);
                }
                else if (IS_SET(obj->value[3], PIPE_HOT))
                    REMOVE_BIT(obj->value[3], PIPE_HOT);
                else
                {
                    if (IS_SET(obj->value[3], PIPE_GOINGOUT))
                    {
                        REMOVE_BIT(obj->value[3], PIPE_LIT);
                        REMOVE_BIT(obj->value[3], PIPE_GOINGOUT);
                    }
                    else
                        SET_BIT(obj->value[3], PIPE_GOINGOUT);
                }
                if (!IS_SET(obj->value[3], PIPE_LIT))
                    SET_BIT(obj->value[3], PIPE_FULLOFASH);
            }
            else
                REMOVE_BIT(obj->value[3], PIPE_HOT);
        }

        /* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

        if (obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC ||
            obj->item_type == ITEM_DROID_CORPSE)
        {
            sh_int timerfrac = UMAX(1, obj->timer - 1);
            if (obj->item_type == ITEM_CORPSE_PC)
                timerfrac = (int)(obj->timer / 8 + 1);

            if (obj->timer > 0 && obj->value[2] > timerfrac)
            {
                char buf[MAX_STRING_LENGTH];
                char name[MAX_STRING_LENGTH];
                char* bufptr;
                bufptr = one_argument(obj->short_descr, name);
                bufptr = one_argument(bufptr, name);
                bufptr = one_argument(bufptr, name);

                separate_obj(obj);
                obj->value[2] = timerfrac;
                if (obj->item_type == ITEM_DROID_CORPSE)
                    sprintf_s(buf, d_corpse_descs[UMIN(timerfrac - 1, 4)], bufptr);
                else
                    sprintf_s(buf, corpse_descs[UMIN(timerfrac - 1, 4)], capitalize(bufptr).c_str());

                STRFREE(obj->description);
                obj->description = STRALLOC(buf);
            }
        }

        /* don't let inventory decay */
        if (IS_OBJ_STAT(obj, ITEM_INVENTORY))
            continue;

        if (obj->timer > 0 && obj->timer < 5 && obj->item_type == ITEM_ARMOR)
        {
            if (obj->carried_by)
            {
                act(AT_TEMP, "$p is almost dead.", obj->carried_by, obj, nullptr, TO_CHAR);
            }
        }

        if ((obj->timer <= 0 || --obj->timer > 0))
            continue;

        /* if we get this far, object's timer has expired. */

        AT_TEMP = AT_PLAIN;
        switch (obj->item_type)
        {
        default:
            message = "$p has depleted itself.";
            AT_TEMP = AT_PLAIN;
            break;

        case ITEM_GRENADE:
        case ITEM_SHIPBOMB:
            explode(obj);
            return;
            break;

        case ITEM_PORTAL:
            message = "$p winks out of existence.";
            remove_portal(obj);
            obj->item_type = ITEM_TRASH; /* so extract_obj	 */
            AT_TEMP = AT_MAGIC;          /* doesn't remove_portal */
            break;
        case ITEM_FOUNTAIN:
            message = "$p dries up.";
            AT_TEMP = AT_BLUE;
            break;
        case ITEM_CORPSE_NPC:
            message = "$p decays into dust and blows away.";
            AT_TEMP = AT_OBJECT;
            break;
        case ITEM_DROID_CORPSE:
            message = "$p rusts away into oblivion.";
            AT_TEMP = AT_OBJECT;
            break;
        case ITEM_CORPSE_PC:
            message = "$p decays into dust and is blown away...";
            AT_TEMP = AT_MAGIC;
            break;
        case ITEM_FOOD:
            message = "$p is devoured by a swarm of maggots.";
            AT_TEMP = AT_HUNGRY;
            break;
        case ITEM_BLOOD:
            message = "$p slowly seeps into the ground.";
            AT_TEMP = AT_BLOOD;
            break;
        case ITEM_BLOODSTAIN:
            message = "$p dries up into flakes and blows away.";
            AT_TEMP = AT_BLOOD;
            break;
        case ITEM_SCRAPS:
            message = "$p crumbles and decays into nothing.";
            AT_TEMP = AT_OBJECT;
            break;
        case ITEM_FIRE:
            if (obj->in_room)
                --obj->in_room->light;
            message = "$p burns out.";
            AT_TEMP = AT_FIRE;
        }

        if (obj->carried_by)
        {
            act(AT_TEMP, message, obj->carried_by, obj, nullptr, TO_CHAR);
        }
        else if (obj->in_room && (rch = obj->in_room->first_person) != nullptr && !IS_OBJ_STAT(obj, ITEM_BURRIED))
        {
            act(AT_TEMP, message, rch, obj, nullptr, TO_ROOM);
            act(AT_TEMP, message, rch, obj, nullptr, TO_CHAR);
        }

        if (obj->serial == cur_obj)
            global_objcode = rOBJ_EXPIRED;
        extract_obj(obj);
    }
    return;
}

/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check(void)
{
    CHAR_DATA *ch, *ch_next;
    EXIT_DATA* pexit;
    static int cnt = 0;
    int door, retcode;

    cnt = (cnt + 1) % 2;

    for (ch = first_char; ch; ch = ch_next)
    {
        set_cur_char(ch);
        ch_next = ch->next;
        will_fall(ch, 0);

        if (char_died(ch))
            continue;

        if (IS_NPC(ch))
        {
            if (cnt != 0)
                continue;

            /* running mobs	-Thoric */
            if (IS_SET(ch->act, ACT_RUNNING))
            {
                if (!IS_SET(ch->act, ACT_SENTINEL) && !ch->fighting && ch->hunting)
                {
                    WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
                    hunt_victim(ch);
                    continue;
                }

                if (ch->spec_fun)
                {
                    if ((*ch->spec_fun)(ch))
                        continue;
                    if (char_died(ch))
                        continue;
                }
                if (ch->spec_2)
                {
                    if ((*ch->spec_2)(ch))
                        continue;
                    if (char_died(ch))
                        continue;
                }

                if (!IS_SET(ch->act, ACT_SENTINEL) && !IS_SET(ch->act, ACT_PROTOTYPE) && (door = number_bits(4)) <= 9 &&
                    (pexit = get_exit(ch->in_room, door)) != nullptr && pexit->to_room &&
                    !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) &&
                    ((!IS_SET(ch->act, ACT_STAY_AREA) && pexit->to_room->area) ||
                     pexit->to_room->area == ch->in_room->area))
                {
                    retcode = move_char(ch, pexit, 0);
                    if (char_died(ch))
                        continue;
                    if (retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL) || ch->position < POS_STANDING)
                        continue;
                }
            }
            continue;
        }
        else
        {
            if (ch->mount && ch->in_room != ch->mount->in_room)
            {
                REMOVE_BIT(ch->mount->act, ACT_MOUNTED);
                ch->mount = nullptr;
                ch->position = POS_STANDING;
                send_to_char("No longer upon your mount, you fall to the ground...\n\rOUCH!\n\r", ch);
            }

            if ((ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER) ||
                (ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR))
            {
                if (!IS_AFFECTED(ch, AFF_AQUA_BREATH))
                {
                    if (get_trust(ch) < LEVEL_IMMORTAL)
                    {
                        int dam;

                        dam = number_range(ch->max_hit / 50, ch->max_hit / 30);
                        dam = UMAX(1, dam);
                        if (ch->hit <= 0)
                            dam = UMIN(10, dam);
                        if (number_bits(3) == 0)
                            send_to_char("You cough and choke as you try to breathe water!\n\r", ch);
                        damage(ch, ch, dam, TYPE_UNDEFINED);
                    }
                }
            }

            if (char_died(ch))
                continue;

            if (ch->in_room &&
                ((ch->in_room->sector_type == SECT_WATER_NOSWIM) || (ch->in_room->sector_type == SECT_WATER_SWIM)))
            {
                if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING) &&
                    !IS_AFFECTED(ch, AFF_AQUA_BREATH) && !ch->mount)
                {
                    if (get_trust(ch) < LEVEL_IMMORTAL)
                    {
                        int dam;

                        if (ch->move > 0)
                            ch->move--;
                        else
                        {
                            dam = number_range(ch->max_hit / 50, ch->max_hit / 30);
                            dam = UMAX(1, dam);
                            if (ch->hit <= 0)
                                dam = UMIN(10, dam);
                            if (number_bits(3) == 0)
                                send_to_char("Struggling with exhaustion, you choke on a mouthful of water.\n\r", ch);
                            damage(ch, ch, dam, TYPE_UNDEFINED);
                        }
                    }
                }
            }
        }
    }
}

/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update(void)
{
    CHAR_DATA* wch = nullptr;
    CHAR_DATA* ch = nullptr;
    CHAR_DATA* ch_next = nullptr;
    CHAR_DATA* victim = nullptr;
    ACT_PROG_DATA* apdtmp = nullptr;

#ifdef UNDEFD
    /*
     *  GRUNT!  To do
     *
     */
    if (IS_NPC(wch) && wch->mpactnum > 0 && wch->in_room->area && &&wch->in_room->area->nplayer > 0)
    {
        MPROG_ACT_LIST *tmp_act, *tmp2_act;
        for (tmp_act = wch->mpact; tmp_act; tmp_act = tmp_act->next)
        {
            oprog_wordlist_check(tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG);
            DISPOSE(tmp_act->buf);
        }
        for (tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act)
        {
            tmp2_act = tmp_act->next;
            DISPOSE(tmp_act);
        }
        wch->mpactnum = 0;
        wch->mpact = nullptr;
    }
#endif

    /* check mobprog act queue */
    while ((apdtmp = mob_act_list) != nullptr)
    {
        wch = reinterpret_cast<CHAR_DATA*>(mob_act_list->vo);
        if (!char_died(wch) && wch->mpactnum > 0)
        {
            MPROG_ACT_LIST* tmp_act;

            while ((tmp_act = wch->mpact) != nullptr)
            {
                if (tmp_act->obj && obj_extracted(tmp_act->obj))
                    tmp_act->obj = nullptr;
                if (tmp_act->ch && !char_died(tmp_act->ch))
                    mprog_wordlist_check(tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG);
                wch->mpact = tmp_act->next;
                DISPOSE(tmp_act->buf);
                DISPOSE(tmp_act);
            }
            wch->mpactnum = 0;
            wch->mpact = nullptr;
        }
        mob_act_list = apdtmp->next;
        DISPOSE(apdtmp);
    }

    /*
     * Just check descriptors here for victims to aggressive mobs
     * We can check for linkdead victims to mobile_update	-Thoric
     */
    for (auto d : g_descriptors)
    {
        if (d->connected != CON_PLAYING || (wch = d->character) == nullptr)
            continue;

        if (char_died(wch) || IS_NPC(wch) || wch->top_level >= LEVEL_IMMORTAL || !wch->in_room)
            continue;

        for (ch = wch->in_room->first_person; ch; ch = ch_next)
        {
            int count = 0;

            ch_next = ch->next_in_room;

            if (!IS_NPC(ch) || ch->fighting || IS_AFFECTED(ch, AFF_CHARM) || !IS_AWAKE(ch) ||
                (IS_SET(ch->act, ACT_WIMPY)) || !can_see(ch, wch))
                continue;

            if (is_hating(ch, wch))
            {
                found_prey(ch, wch);
                continue;
            }

            if (!IS_SET(ch->act, ACT_AGGRESSIVE) || IS_SET(ch->act, ACT_MOUNTED) ||
                IS_SET(ch->in_room->room_flags, ROOM_SAFE))
                continue;

            victim = wch;

            if (!victim)
            {
                bug("Aggr_update: null victim.", count);
                continue;
            }

            if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
                continue;

            if (IS_NPC(ch) && IS_SET(ch->attacks, ATCK_BACKSTAB))
            {
                OBJ_DATA* obj;

                if (!ch->mount && (obj = get_eq_char(ch, WEAR_WIELD)) != nullptr && obj->value[3] == 11 &&
                    !victim->fighting && victim->hit >= victim->max_hit)
                {
                    WAIT_STATE(ch, skill_table[gsn_backstab]->beats);
                    if (!IS_AWAKE(victim) || number_percent() + 5 < ch->top_level)
                    {
                        global_retcode = multi_hit(ch, victim, gsn_backstab);
                        continue;
                    }
                    else
                    {
                        global_retcode = damage(ch, victim, 0, gsn_backstab);
                        continue;
                    }
                }
            }
            global_retcode = multi_hit(ch, victim, TYPE_UNDEFINED);
        }
    }

    return;
}

/* From interp.c */
bool check_social(CHAR_DATA* ch, const char* command, const char* argument);

/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms(CHAR_DATA* ch)
{
    CHAR_DATA* rvch = nullptr;
    CHAR_DATA* vch;
    sh_int drunk;
    sh_int position;

    if (IS_NPC(ch) || ch->pcdata->condition[COND_DRUNK] <= 0)
        return;

    if (number_percent() < 30)
        return;

    drunk = ch->pcdata->condition[COND_DRUNK];
    position = ch->position;
    ch->position = POS_STANDING;

    if (number_percent() < (2 * drunk / 20))
        check_social(ch, "burp", "");
    else if (number_percent() < (2 * drunk / 20))
        check_social(ch, "hiccup", "");
    else if (number_percent() < (2 * drunk / 20))
        check_social(ch, "drool", "");
    else if (number_percent() < (2 * drunk / 20))
        check_social(ch, "fart", "");
    else if (drunk > (10 + (get_curr_con(ch) / 5)) && number_percent() < (2 * drunk / 18))
    {
        for (vch = ch->in_room->first_person; vch; vch = vch->next_in_room)
            if (number_percent() < 10)
                rvch = vch;
        check_social(ch, "puke", (rvch ? rvch->name : ""));
    }

    ch->position = position;
    return;
}

void halucinations(CHAR_DATA* ch)
{
    if (ch->mental_state >= 30 && number_bits(5 - (ch->mental_state >= 50) - (ch->mental_state >= 75)) == 0)
    {
        const char* t;

        switch (number_range(1, UMIN(20, (ch->mental_state + 5) / 5)))
        {
        default:
        case 1:
            t = "You feel very restless... you can't sit still.\n\r";
            break;
        case 2:
            t = "You're tingling all over.\n\r";
            break;
        case 3:
            t = "Your skin is crawling.\n\r";
            break;
        case 4:
            t = "You suddenly feel that something is terribly wrong.\n\r";
            break;
        case 5:
            t = "Those damn little fairies keep laughing at you!\n\r";
            break;
        case 6:
            t = "You can hear your mother crying...\n\r";
            break;
        case 7:
            t = "Have you been here before, or not?  You're not sure...\n\r";
            break;
        case 8:
            t = "Painful childhood memories flash through your mind.\n\r";
            break;
        case 9:
            t = "You hear someone call your name in the distance...\n\r";
            break;
        case 10:
            t = "Your head is pulsating... you can't think straight.\n\r";
            break;
        case 11:
            t = "The ground... seems to be squirming...\n\r";
            break;
        case 12:
            t = "You're not quite sure what is real anymore.\n\r";
            break;
        case 13:
            t = "It's all a dream... or is it?\n\r";
            break;
        case 14:
            t = "They're coming to get you... coming to take you away...\n\r";
            break;
        case 15:
            t = "You begin to feel all powerful!\n\r";
            break;
        case 16:
            t = "You're light as air... the heavens are yours for the taking.\n\r";
            break;
        case 17:
            t = "Your whole life flashes by... and your future...\n\r";
            break;
        case 18:
            t = "You are everywhere and everything... you know all and are all!\n\r";
            break;
        case 19:
            t = "You feel immortal!\n\r";
            break;
        case 20:
            t = "Ahh... the power of a Supreme Entity... what to do...\n\r";
            break;
        }
        send_to_char(t, ch);
    }
    return;
}

void tele_update(void)
{
    TELEPORT_DATA *tele, *tele_next;

    if (!first_teleport)
        return;

    for (tele = first_teleport; tele; tele = tele_next)
    {
        tele_next = tele->next;
        if (--tele->timer <= 0)
        {
            if (tele->room->first_person)
            {
                teleport(tele->room->first_person, tele->room->tele_vnum, TELE_TRANSALL);
            }
            UNLINK(tele, first_teleport, last_teleport, next, prev);
            DISPOSE(tele);
        }
    }
}

#if false
/*
 * Write all outstanding authorization requests to Log channel - Gorog
 */
void auth_update(void)
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    char log_buf[MAX_INPUT_LENGTH];
    bool first_time = true; /* so titles are only done once */

    for (d = first_descriptor; d; d = d->next)
    {
        victim = d->character;
        if (victim && IS_WAITING_FOR_AUTH(victim))
        {
            if (first_time)
            {
                first_time = false;
                strcpy_s(log_buf, "Pending authorizations:");
                to_channel(log_buf, CHANNEL_MONITOR, "Monitor", 1);
            }
            sprintf_s(log_buf, " %s@%s new %s", victim->name, victim->desc->host, race_table[victim->race].race_name);
            to_channel(log_buf, CHANNEL_MONITOR, "Monitor", 1);
        }
    }
}
#endif

void auth_update(void)
{
    char buf[MAX_INPUT_LENGTH], log_buf[MAX_INPUT_LENGTH];
    bool found_hit = false; /* was at least one found? */

    strcpy_s(log_buf, "Pending authorizations:\n\r");
    for (auto d : g_descriptors)
    {
        auto victim = d->character;
        if (victim != nullptr && IS_WAITING_FOR_AUTH(victim))
        {
            found_hit = true;
            sprintf_s(buf, " %s@%s new %s\n\r", victim->name, victim->desc->connection->getHostname().c_str(),
                      race_table[victim->race].race_name);
            strcat_s(log_buf, buf);
        }
    }
    if (found_hit)
    {
        log_string(log_buf);
        to_channel(log_buf, CHANNEL_MONITOR, "Monitor", 1);
    }
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler(void)
{
    static int pulse_start_arena = PULSE_ARENA;
    static int pulse_arena = PULSE_ARENA;
    static int pulse_taxes;
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_second;
    static int pulse_space;
    static int pulse_ship;
    static int pulse_recharge;
    static int pulse_force;

    auto start_time = std::chrono::steady_clock::now();

    if (timechar)
    {
        set_char_color(AT_PLAIN, timechar);
        send_to_char("Starting update timer.\n\r", timechar);
    }

    if (--pulse_area <= 0)
    {
        pulse_area = number_range(PULSE_AREA / 2, 3 * PULSE_AREA / 2);
        area_update();
    }

    if (--pulse_force <= 0)
    {
        pulse_force = PULSE_FORCE;
        update_force();
    }

    if (--pulse_taxes <= 0)
    {
        pulse_taxes = PULSE_TAXES;
        update_taxes();
    }

    if (--pulse_mobile <= 0)
    {
        pulse_mobile = PULSE_MOBILE;
        mobile_update();
    }

    if (--pulse_space <= 0)
    {
        pulse_space = PULSE_SPACE;
        update_space();
        update_bus();
        // update_traffic ( );
    }

    if (--pulse_recharge <= 0)
    {
        pulse_recharge = PULSE_SPACE / 3;
        recharge_ships();
    }

    if (--pulse_ship <= 0)
    {
        pulse_ship = PULSE_SPACE / 10;
        move_ships();
    }

    if (--pulse_violence <= 0)
    {
        pulse_violence = PULSE_VIOLENCE;
        violence_update();
    }

    if (in_start_arena || ppl_challenged)
        if (--pulse_start_arena <= 0)
        {
            pulse_start_arena = PULSE_ARENA;
            start_arena();
        }

    if (ppl_in_arena)
        if ((--pulse_arena <= 0) || (num_in_arena() == 1))
        {
            pulse_arena = PULSE_ARENA;
            do_game();
        }

    if (--pulse_point <= 0)
    {
        pulse_point = number_range(PULSE_TICK * 0.75, PULSE_TICK * 1.25);

        auth_update(); /* Gorog */
        weather_update();
        char_update();
        obj_update();
        clear_vrooms(); /* remove virtual rooms */
    }

    if (--pulse_second <= 0)
    {
        pulse_second = PULSE_PER_SECOND;
        char_check();
        /*reboot_check( "" ); Disabled to check if its lagging a lot - Scryn*/
        /* Much faster version enabled by Altrag..
           although I dunno how it could lag too much, it was just a bunch
           of comparisons.. */
        check_pfiles(0);
        reboot_check(0);
    }

    if (auction->item && --auction->pulse <= 0)
    {
        auction->pulse = PULSE_AUCTION;
        auction_update();
    }

    mpsleep_update(); /* Check for sleeping mud progs -rkb */
    tele_update();
    aggr_update();
    obj_act_update();
    room_act_update();
    clean_obj_queue();  /* dispose of extracted objects */
    clean_char_queue(); /* dispose of dead mobs/quitting chars */
    if (timechar)
    {
        set_char_color(AT_PLAIN, timechar);
        send_to_char("Update timing complete.\n\r", timechar);
        auto time_taken = std::chrono::steady_clock::now() - start_time;
        ch_printf(timechar, "Timing took %d microseconds.\n\r",
                  std::chrono::duration_cast<std::chrono::microseconds>(time_taken));
        timechar = nullptr;
    }
    return;
}

void remove_portal(OBJ_DATA* portal)
{
    ROOM_INDEX_DATA *fromRoom, *toRoom;
    CHAR_DATA* ch;
    EXIT_DATA* pexit;
    bool found;

    if (!portal)
    {
        bug("remove_portal: portal is nullptr", 0);
        return;
    }

    fromRoom = portal->in_room;
    found = false;
    if (!fromRoom)
    {
        bug("remove_portal: portal->in_room is nullptr", 0);
        return;
    }

    for (pexit = fromRoom->first_exit; pexit; pexit = pexit->next)
        if (IS_SET(pexit->exit_info, EX_PORTAL))
        {
            found = true;
            break;
        }

    if (!found)
    {
        bug("remove_portal: portal not found in room %d!", fromRoom->vnum);
        return;
    }

    if (pexit->vdir != DIR_PORTAL)
        bug("remove_portal: exit in dir %d != DIR_PORTAL", pexit->vdir);

    if ((toRoom = pexit->to_room) == nullptr)
        bug("remove_portal: toRoom is nullptr", 0);

    extract_exit(fromRoom, pexit);
    /* rendunancy */
    /* send a message to fromRoom */
    /* ch = fromRoom->first_person; */
    /* if(ch!=nullptr) */
    /* act( AT_PLAIN, "A magical portal below winks from existence.", ch, nullptr, nullptr, TO_ROOM ); */

    /* send a message to toRoom */
    if (toRoom && (ch = toRoom->first_person) != nullptr)
        act(AT_PLAIN, "A magical portal above winks from existence.", ch, nullptr, nullptr, TO_ROOM);

    /* remove the portal obj: looks better to let update_obj do this */
    /* extract_obj(portal);  */

    return;
}

void reboot_check(time_t reset)
{
    static const char* tmsg[] = {
        "SYSTEM: Reboot in 10 seconds.", "SYSTEM: Reboot in 30 seconds.", "SYSTEM: Reboot in 1 minute.",
        "SYSTEM: Reboot in 2 minutes.",  "SYSTEM: Reboot in 3 minutes.",  "SYSTEM: Reboot in 4 minutes.",
        "SYSTEM: Reboot in 5 minutes.",  "SYSTEM: Reboot in 10 minutes.",
    };
    static const int times[] = {10, 30, 60, 120, 180, 240, 300, 600};
    static const int timesize = UMIN(sizeof(times) / sizeof(*times), sizeof(tmsg) / sizeof(*tmsg));
    char buf[MAX_STRING_LENGTH];
    static int trun;
    static bool init;

    if (!init || reset >= current_time)
    {
        for (trun = timesize - 1; trun >= 0; trun--)
            if (reset >= current_time + times[trun])
                break;
        init = true;
        return;
    }

    if ((current_time % 1800) == 0)
    {
        sprintf_s(buf, "%.24s: %d players", ctime(&current_time), -1); // TODO num_descriptors ded :(
        append_to_file(USAGE_FILE, buf);
    }

    if (new_boot_time_t - boot_time < 60 * 60 * 18 && !set_boot_time->manual)
        return;

    if (new_boot_time_t <= current_time)
    {
        CHAR_DATA* vch;
        extern bool mud_down;

        if (auction->item)
        {
            sprintf_s(buf, "Sale of %s has been stopped by mud.", auction->item->short_descr);
            talk_auction(buf);
            obj_to_char(auction->item, auction->seller);
            auction->item = nullptr;
            if (auction->buyer && auction->buyer != auction->seller)
            {
                auction->buyer->gold += auction->bet;
                send_to_char("Your money has been returned.\n\r", auction->buyer);
            }
        }
        echo_to_all(AT_AUCTION,
                    "You are forced from these realms by a strong "
                    "presence\n\ras life here is reconstructed.",
                    ECHOTAR_ALL);

        for (vch = first_char; vch; vch = vch->next)
            if (!IS_NPC(vch))
                save_char_obj(vch);
        mud_down = true;
        return;
    }

    if (trun != -1 && new_boot_time_t - current_time <= times[trun])
    {
        echo_to_all(AT_YELLOW, tmsg[trun], ECHOTAR_ALL);
        if (trun <= 5)
            sysdata.DENY_NEW_PLAYERS = true;
        --trun;
        return;
    }
    return;
}

#if 0
void reboot_check(char* arg)
{
	char buf[MAX_STRING_LENGTH];
	extern bool mud_down;
	/*tm *timestruct;
	int timecheck;*/
	CHAR_DATA* vch;

	/*Bools to show which pre-boot echoes we've done. */
	static bool thirty = false;
	static bool fifteen = false;
	static bool ten = false;
	static bool five = false;
	static bool four = false;
	static bool three = false;
	static bool two = false;
	static bool one = false;

	/* This function can be called by do_setboot when the reboot time
	   is being manually set to reset all the bools. */
	if (!str_cmp(arg, "reset"))
	{
		thirty = false;
		fifteen = false;
		ten = false;
		five = false;
		four = false;
		three = false;
		two = false;
		one = false;
		return;
	}

	/* If the mud has been up less than 18 hours and the boot time
	   wasn't set manually, forget it. */
	   /* Usage monitor */

	if ((current_time % 1800) == 0)
	{
		sprintf_s(buf, "%s: %d players", ctime(&current_time), num_descriptors);
		append_to_file(USAGE_FILE, buf);
	}

	/* Change by Scryn - if mud has not been up 18 hours at boot time - still
	 * allow for warnings even if not up 18 hours
	 */
	if (new_boot_time_t - boot_time < 60 * 60 * 18
		&& set_boot_time->manual == 0)
	{
		return;
	}
	/*
		timestruct = localtime( &current_time);

		if ( timestruct->tm_hour == set_boot_time->hour
			 && timestruct->tm_min  == set_boot_time->min )*/
	if (new_boot_time_t <= current_time)
	{
		/* Return auction item to seller */
		if (auction->item != nullptr)
		{
			sprintf_s(buf, "Sale of %s has been stopped by mud.",
				auction->item->short_descr);
			talk_auction(buf);
			obj_to_char(auction->item, auction->seller);
			auction->item = nullptr;
			if (auction->buyer != nullptr && auction->seller != auction->buyer) /* return money to the buyer */
			{
				auction->buyer->gold += auction->bet;
				send_to_char("Your money has been returned.\n\r", auction->buyer);
			}
		}

		sprintf_s(buf, "You are forced from these realms by a strong magical presence");
		echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
		sprintf_s(buf, "as life here is reconstructed.");
		echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);

		/* Save all characters before booting. */
		for (vch = first_char; vch; vch = vch->next)
		{
			if (!IS_NPC(vch))
				save_char_obj(vch);
		}
		mud_down = true;
	}

	/* How many minutes to the scheduled boot? */
  /*  timecheck = ( set_boot_time->hour * 60 + set_boot_time->min )
				- ( timestruct->tm_hour * 60 + timestruct->tm_min );

	if ( timecheck > 30  || timecheck < 0 ) return;

	if ( timecheck <= 1 ) */
	if (new_boot_time_t - current_time <= 60)
	{
		if (one == false)
		{
			sprintf_s(buf, "You feel the ground shake as the end comes near!");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			one = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 2 )*/
	if (new_boot_time_t - current_time <= 120)
	{
		if (two == false)
		{
			sprintf_s(buf, "Lightning crackles in the sky above!");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			two = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 3 )*/
	if (new_boot_time_t - current_time <= 180)
	{
		if (three == false)
		{
			sprintf_s(buf, "Crashes of thunder sound across the land!");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			three = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 4 )*/
	if (new_boot_time_t - current_time <= 240)
	{
		if (four == false)
		{
			sprintf_s(buf, "The sky has suddenly turned midnight black.");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			four = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 5 )*/
	if (new_boot_time_t - current_time <= 300)
	{
		if (five == false)
		{
			sprintf_s(buf, "You notice the life forms around you slowly dwindling away.");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			five = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 10 )*/
	if (new_boot_time_t - current_time <= 600)
	{
		if (ten == false)
		{
			sprintf_s(buf, "The seas across the realm have turned frigid.");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			ten = true;
		}
		return;
	}

	/*  if ( timecheck == 15 )*/
	if (new_boot_time_t - current_time <= 900)
	{
		if (fifteen == false)
		{
			sprintf_s(buf, "The aura of magic which once surrounded the realms seems slightly unstable.");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			fifteen = true;
		}
		return;
	}

	/*  if ( timecheck == 30 )*/
	if (new_boot_time_t - current_time <= 1800)
	{
		if (thirty == false)
		{
			sprintf_s(buf, "You sense a change in the magical forces surrounding you.");
			echo_to_all(AT_YELLOW, buf, ECHOTAR_ALL);
			thirty = true;
		}
		return;
	}

	return;
}
#endif

/* the auction update*/

void auction_update(void)
{
    int tax, pay;
    char buf[MAX_STRING_LENGTH];

    switch (++auction->going) /* increase the going state */
    {
    case 1: /* going once */
    case 2: /* going twice */
        if (auction->bet > auction->starting)
            sprintf_s(buf, "&W%s&w:&W going &w%s&W for &Y%d&W credits.", auction->item->short_descr,
                      ((auction->going == 1) ? "once" : "twice"), auction->bet);
        else
            sprintf_s(buf, "&W%s&w:&W going &w%s&W (No bid received).", auction->item->short_descr,
                      ((auction->going == 1) ? "once" : "twice"));

        talk_auction(buf);
        break;

    case 3: /* SOLD! */
        if (!auction->buyer && auction->bet)
        {
            bug("Auction code reached SOLD, with nullptr buyer, but %d gold bid", auction->bet);
            auction->bet = 0;
        }
        if (auction->bet > 0 && auction->buyer != auction->seller)
        {
            sprintf_s(buf, "&W%s&W is &wSOLD&W for &Y%d&W credits.", auction->item->short_descr, auction->bet);
            talk_auction(buf);

            act(AT_ACTION, "&wA courier arrives, and hands you $p.", auction->buyer, auction->item, nullptr, TO_CHAR);
            obj_to_char(auction->item, auction->buyer);

            pay = (int)auction->bet * 0.9;
            tax = (int)auction->bet * 0.1;
            if (auction->seller->in_room->area)
                boost_economy(auction->seller->in_room->area, tax);
            auction->seller->gold += pay; /* give him the money, tax 10 % */
            sprintf_s(buf, "&wYou are paid &Y%d&w credits, charging an auction fee of &Y%d&w.\n\r", pay, tax);
            send_to_char(buf, auction->seller);
            auction->item = nullptr; /* reset item */
            if (IS_SET(sysdata.save_flags, SV_AUCTION))
            {
                save_char_obj(auction->buyer);
                save_char_obj(auction->seller);
            }
        }
        else /* not sold */
        {
            sprintf_s(buf, "&WNo bids received for %s. Removed from auction.\n\r.", auction->item->short_descr);
            talk_auction(buf);
            act(AT_ACTION, "&wA courier arrives, and returns $p to you.", auction->seller, auction->item, nullptr,
                TO_CHAR);

            obj_to_char(auction->item, auction->seller);
            tax = (int)auction->item->cost * 0.05;
            boost_economy(auction->seller->in_room->area, tax);
            sprintf_s(buf, "&wYou are charged an auction fee of &Y%d&w credits.\n\r", tax);
            send_to_char(buf, auction->seller);
            if ((auction->seller->gold - tax) < 0)
                auction->seller->gold = 0;
            else
                auction->seller->gold -= tax;
            if (IS_SET(sysdata.save_flags, SV_AUCTION))
                save_char_obj(auction->seller);
        }                     /* else */
        auction->item = nullptr; /* clear auction */
    }                         /* switch */
} /* func */
