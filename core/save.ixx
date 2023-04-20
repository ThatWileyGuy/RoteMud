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

module;

#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <SDKDDKVer.h>
#endif

#include <boost/asio.hpp> // TODO why is this needed here?
#include "mud.hxx"

export module save;

import std.filesystem;

import std.core;
import mud;
import connection;
import hashstr;
import comments;

/*
 * Increment with every major format change.
 */
#define SAVEVERSION 5

/*
 * Array to keep track of equipment temporarily.		-Thoric
 */
OBJ_DATA* save_equipment[MAX_WEAR][MAX_LAYERS];
export CHAR_DATA *quitting_char, *loading_char, *saving_char;

int file_ver;

/*
 * Array of containers read for proper re-nesting of objects.
 */
static OBJ_DATA* rgObjNest[MAX_NEST];

/*
 * Local functions.
 */
void fwrite_char(CHAR_DATA* ch, FILE* fp);
void fread_char(CHAR_DATA* ch, FILE* fp, bool preload);
export void write_corpses(CHAR_DATA* ch, char* name);

export void save_home(CHAR_DATA* ch)
{
    if (ch->plr_home)
    {
        FILE* fp;
        char filename[256];
        sh_int templvl;
        OBJ_DATA* contents;

        sprintf_s(filename, "%s%c/%s.home", PLAYER_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());
        if ((fp = fopen(filename, "w")) == nullptr)
        {
        }
        else
        {
            templvl = ch->top_level;
            ch->top_level = LEVEL_HERO; /* make sure EQ doesn't get lost */
            contents = ch->plr_home->last_content;
            if (contents)
                fwrite_obj(ch, contents, fp, 0, OS_CARRY);
            fprintf(fp, "#END\n");
            ch->top_level = templvl;
            fclose(fp);
        }
    }
}

/*
 * Un-equip character before saving to ensure proper	-Thoric
 * stats are saved in case of changes to or removal of EQ
 */
export void de_equip_char(CHAR_DATA* ch)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA* obj;
    int x, y;

    for (x = 0; x < MAX_WEAR; x++)
        for (y = 0; y < MAX_LAYERS; y++)
            save_equipment[x][y] = nullptr;
    for (obj = ch->first_carrying; obj; obj = obj->next_content)
        if (obj->wear_loc > -1 && obj->wear_loc < MAX_WEAR)
        {

            for (x = 0; x < MAX_LAYERS; x++)
                if (!save_equipment[obj->wear_loc][x])
                {
                    save_equipment[obj->wear_loc][x] = obj;
                    break;
                }
            if (x == MAX_LAYERS)
            {
                sprintf_s(buf, "%s had on more than %d layers of clothing in one location (%d): %s", ch->name,
                          MAX_LAYERS, obj->wear_loc, obj->name);
                bug(buf, 0);
            }

            unequip_char(ch, obj);
        }
}

/*
 * Re-equip character					-Thoric
 */
export void re_equip_char(CHAR_DATA* ch)
{
    int x, y;

    for (x = 0; x < MAX_WEAR; x++)
        for (y = 0; y < MAX_LAYERS; y++)
            if (save_equipment[x][y] != nullptr)
            {
                if (quitting_char != ch)
                    equip_char(ch, save_equipment[x][y], x);
                save_equipment[x][y] = nullptr;
            }
            else
                break;
}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
export void save_char_obj(CHAR_DATA* ch)
{
    char strsave[MAX_INPUT_LENGTH];
    char strback[MAX_INPUT_LENGTH];
    FILE* fp;

    if (!ch)
    {
        bug("Save_char_obj: null ch!", 0);
        return;
    }

    if (IS_NPC(ch) || NOT_AUTHED(ch))
        return;

    saving_char = ch;
    /* save pc's clan's data while we're at it to keep the data in sync */
    if (!IS_NPC(ch) && ch->pcdata->clan)
        save_clan(ch->pcdata->clan);

    if (ch->desc && ch->desc->original)
        ch = ch->desc->original;

    ch->save_time = current_time;
    sprintf_s(strsave, "%s%c/%s", PLAYER_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());

    /*
     * Auto-backup pfile (can cause lag with high disk access situtations
     */
    if (IS_SET(sysdata.save_flags, SV_BACKUP))
    {
        sprintf_s(strback, "%s%c/%s", BACKUP_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());
        rename(strsave, strback);
    }

    de_equip_char(ch);

    /*
     * Save immortal stats, level & vnums for wizlist		-Thoric
     * and do_vnums command
     *
     * Also save the player flags so we the wizlist builder can see
     * who is a guest and who is retired.
     */
    if (get_trust(ch) > LEVEL_IMMORTAL - 1)
    {
        sprintf_s(strback, "%s%s", GOD_DIR, capitalize(ch->name).c_str());

        if ((fp = fopen(strback, "w")) == nullptr)
        {
            bug("Save_god_level: fopen", 0);
            perror(strsave);
        }
        else
        {
            fprintf(fp, "Level        %d\n", ch->top_level);
            fprintf(fp, "Pcflags      %d\n", ch->pcdata->flags);
            if (ch->pcdata->r_range_lo && ch->pcdata->r_range_hi)
                fprintf(fp, "RoomRange    %d %d\n", ch->pcdata->r_range_lo, ch->pcdata->r_range_hi);
            if (ch->pcdata->o_range_lo && ch->pcdata->o_range_hi)
                fprintf(fp, "ObjRange     %d %d\n", ch->pcdata->o_range_lo, ch->pcdata->o_range_hi);
            if (ch->pcdata->m_range_lo && ch->pcdata->m_range_hi)
                fprintf(fp, "MobRange     %d %d\n", ch->pcdata->m_range_lo, ch->pcdata->m_range_hi);
            fclose(fp);
        }
    }

    if ((fp = fopen(strsave, "w")) == nullptr)
    {
        bug("Save_char_obj: fopen", 0);
        perror(strsave);
    }
    else
    {
        fwrite_char(ch, fp);
        if (ch->first_carrying)
            fwrite_obj(ch, ch->last_carrying, fp, 0, OS_CARRY);
        if (ch->comments)            /* comments */
            fwrite_comments(ch, fp); /* comments */
        fprintf(fp, "#END\n");
        fclose(fp);
    }

    re_equip_char(ch);

    write_corpses(ch, nullptr);
    quitting_char = nullptr;
    saving_char = nullptr;
    return;
}

export void save_clone(CHAR_DATA* ch)
{
    char strsave[MAX_INPUT_LENGTH];
    char strback[MAX_INPUT_LENGTH];
    FILE* fp;

    if (!ch)
    {
        bug("Save_char_obj: null ch!", 0);
        return;
    }

    if (IS_NPC(ch) || NOT_AUTHED(ch))
        return;

    if (ch->desc && ch->desc->original)
        ch = ch->desc->original;

    de_equip_char(ch);

    ch->save_time = current_time;
    SET_BIT(ch->pcdata->act2, ACT_EXEMPT);
    sprintf_s(strsave, "%s%c/%s.clone", PLAYER_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());

    /*
     * Auto-backup pfile (can cause lag with high disk access situtations
     */
    if (IS_SET(sysdata.save_flags, SV_BACKUP))
    {
        sprintf_s(strback, "%s%c/%s", BACKUP_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());
        rename(strsave, strback);
    }

    if ((fp = fopen(strsave, "w")) == nullptr)
    {
        bug("Save_char_obj: fopen", 0);
        perror(strsave);
    }
    else
    {
        fwrite_char(ch, fp);
        if (ch->comments)            /* comments */
            fwrite_comments(ch, fp); /* comments */
        fprintf(fp, "#END\n");
        fclose(fp);
    }

    re_equip_char(ch);

    write_corpses(ch, nullptr);
    quitting_char = nullptr;
    saving_char = nullptr;
    REMOVE_BIT(ch->pcdata->act2, ACT_EXEMPT);
    return;
}

/*
 * Write the char.
 */
void fwrite_char(CHAR_DATA* ch, FILE* fp)
{
    AFFECT_DATA* paf = nullptr;
    int sn, track, drug;
    BUG_DATA* bug = nullptr;
    SKILL_TYPE* skill = nullptr;
    SHIP_DATA* ship = nullptr;
    CONTRACT_DATA* contract = nullptr;
    FELLOW_DATA* fellow = nullptr;

    fprintf(fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER");

    fprintf(fp, "Name         %s~\n", ch->name);
    fprintf(fp, "Version      %d\n", SAVEVERSION);
    fprintf(fp, "Comfreq      %s~\n", ch->comfreq);
    if (ch->short_descr && ch->short_descr[0] != '\0')
        fprintf(fp, "ShortDescr   %s~\n", ch->short_descr);
    if (ch->long_descr && ch->long_descr[0] != '\0')
        fprintf(fp, "LongDescr    %s~\n", ch->long_descr);
    if (ch->description && ch->description[0] != '\0')
        fprintf(fp, "Description  %s~\n", ch->description);
    fprintf(fp, "Sex          %d\n", ch->sex);
    fprintf(fp, "Race         %d\n", ch->race);
    fprintf(fp, "MainAbility  %d\n", ch->main_ability);
    fprintf(fp, "SecondAbility  %d\n", ch->secondary_ability);
    fprintf(fp, "Languages    %d %d\n", ch->speaks, ch->speaking);
    fprintf(fp, "Toplevel     %d\n", ch->top_level);
    if (ch->trust)
        fprintf(fp, "Trust        %d\n", ch->trust);
    fprintf(fp, "Pheight      %d\n", ch->pheight);
    fprintf(fp, "Build        %d\n", ch->build);
    fprintf(fp, "Played       %d\n", ch->played + (int)(current_time - ch->logon));

    if (ch->in_room && (ship = ship_from_room(ch->in_room->vnum)))
    {
        fprintf(fp, "Ship          %s~\n", ship->name);
        /*        for ( sRoom = ship->first_room ; sRoom ; sRoom = sRoom->next_in_ship )
                  if ( ch->in_room == sRoom )*/
        fprintf(fp, "ShipNum          %d\n", ch->in_room->exvnum);
    }
    else
    {
        fprintf(fp, "Ship            ~\n");
        fprintf(fp, "ShipNum         0\n");
    }
    fprintf(fp, "Room         %d\n",
            (ch->in_room == get_room_index(ROOM_VNUM_LIMBO) && ch->was_in_room) ? ch->was_in_room->vnum
                                                                                : ch->in_room->vnum);
    if (ch->plr_home != nullptr)
        fprintf(fp, "PlrHome      %d\n", ch->plr_home->vnum);

    fprintf(fp, "HpManaMove   %d %d 0 0 %d %d\n", ch->hit, ch->max_hit, ch->move, ch->max_move);
    fprintf(fp, "Force        %d %d %d %d\n", ch->perm_frc, 0, ch->mana, ch->max_mana);
    fprintf(fp, "ForceControl %d\n", ch->force_control);
    fprintf(fp, "ForceSense   %d\n", ch->force_sense);
    fprintf(fp, "ForceAlter   %d\n", ch->force_alter);
    fprintf(fp, "ForceChance   %d\n", ch->force_chance);
    fprintf(fp, "ForceId      %d\n", ch->force_identified);
    fprintf(fp, "ForceAlign   %d\n", ch->force_align);
    fprintf(fp, "ForceConverted   %d\n", ch->force_converted);
    fprintf(fp, "ForceType      %d\n", ch->force_type);
    fprintf(fp, "ForceLvlStatus %d\n", ch->force_level_status);
    if (ch->force_master && ch->force_master[0] != '\0')
        fprintf(fp, "ForceMaster  %s~\n", ch->force_master);
    {
        int ability;
        for (ability = 0; ability < MAX_FORCE_SKILL; ability++)
            fprintf(fp, "ForceSkill   %d %d\n", ability, ch->force_skill[ability]);
    }
    fprintf(fp, "Gold         %d\n", ch->gold);
    fprintf(fp, "Bank         %ld\n", ch->pcdata->bank);
    fprintf(fp, "Salary       %d\n", ch->pcdata->salary);
    {
        int ability;
        for (ability = 0; ability < MAX_ABILITY; ability++)
            fprintf(fp, "Ability        %d %d %ld %d\n", ability, ch->skill_level[ability], ch->experience[ability],
                    ch->bonus[ability]);
    }
    if (ch->act)
        fprintf(fp, "Act          %d\n", ch->act);
    if (ch->pcdata->act2)
        fprintf(fp, "Act2          %d\n", ch->pcdata->act2);

    if (ch->affected_by)
        fprintf(fp, "AffectedBy   %d\n", ch->affected_by);
    fprintf(fp, "Position     %d\n", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

    fprintf(fp, "SavingThrows %d %d %d %d %d\n", ch->saving_poison_death, ch->saving_wand, ch->saving_para_petri,
            ch->saving_breath, ch->saving_spell_staff);
    fprintf(fp, "Alignment    %d\n", ch->alignment);
    fprintf(fp, "Glory        %d\n", ch->pcdata->quest_curr);
    fprintf(fp, "MGlory       %d\n", ch->pcdata->quest_accum);
    fprintf(fp, "Hitroll      %d\n", ch->hitroll);
    fprintf(fp, "Damroll      %d\n", ch->damroll);
    fprintf(fp, "Armor        %d\n", ch->armor);
    if (ch->rppoints)
        fprintf(fp, "Rppoints   %d\n", ch->rppoints);
    else
        fprintf(fp, "Rppoints   0\n");
    if (ch->wimpy)
        fprintf(fp, "Wimpy        %d\n", ch->wimpy);
    if (ch->deaf)
        fprintf(fp, "Deaf         %d\n", ch->deaf);
    if (ch->resistant)
        fprintf(fp, "Resistant    %d\n", ch->resistant);
    if (ch->immune)
        fprintf(fp, "Immune       %d\n", ch->immune);
    if (ch->susceptible)
        fprintf(fp, "Susceptible  %d\n", ch->susceptible);
    if (ch->pcdata && ch->pcdata->outcast_time)
        fprintf(fp, "Outcast_time %ld\n", ch->pcdata->outcast_time);
    if (ch->pcdata && ch->pcdata->restore_time)
        fprintf(fp, "Restore_time %ld\n", ch->pcdata->restore_time);
    if (ch->mental_state != -10)
        fprintf(fp, "Mentalstate  %d\n", ch->mental_state);

    if (IS_NPC(ch))
    {
        fprintf(fp, "Vnum         %d\n", ch->pIndexData->vnum);
        fprintf(fp, "Mobinvis     %d\n", ch->mobinvis);
    }
    else
    {
        fprintf(fp, "Password     %s~\n", ch->pcdata->pwd);
        fprintf(fp, "Lastplayed   %d\n", (int)current_time);
        if (ch->pcdata->bamfin && ch->pcdata->bamfin[0] != '\0')
            fprintf(fp, "Bamfin       %s~\n", ch->pcdata->bamfin);
        if (ch->pcdata->email && ch->pcdata->email[0] != '\0')
            fprintf(fp, "Email       %s~\n", ch->pcdata->email);
        if (ch->pcdata->bamfout && ch->pcdata->bamfout[0] != '\0')
            fprintf(fp, "Bamfout      %s~\n", ch->pcdata->bamfout);
        if (ch->rank && ch->rank[0] != '\0')
            fprintf(fp, "Rank         %s~\n", ch->rank);
        if (ch->pcdata->fiance && ch->pcdata->fiance[0] != '\0')
            fprintf(fp, "Fiance         %s~\n", ch->pcdata->fiance);

        /*** Now Defunct. Still loads if seen, does not save.
            if ( ch->pcdata->last_name && ch->pcdata->last_name[0] != '\0' )
              fprintf( fp, "Lastname         %s~\n",	ch->pcdata->last_name	);
        ***/
        if (ch->pcdata->spouse && ch->pcdata->spouse[0] != '\0')
            fprintf(fp, "Spouse         %s~\n", ch->pcdata->spouse);
        if (ch->pcdata->forcerank)
            fprintf(fp, "forcerank  %d\n", ch->pcdata->forcerank);
        if (ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0')
            fprintf(fp, "Bestowments  %s~\n", ch->pcdata->bestowments);
        fprintf(fp, "Title        %s~\n", ch->pcdata->title);
        if (ch->pcdata->disguise && ch->pcdata->disguise[0] != '\0')
            fprintf(fp, "Disguise     %s~\n", ch->pcdata->disguise);
        if (ch->pcdata->homepage && ch->pcdata->homepage[0] != '\0')
            fprintf(fp, "Homepage     %s~\n", ch->pcdata->homepage);
        if (ch->pcdata->screenname && ch->pcdata->screenname[0] != '\0')
            fprintf(fp, "Screenname   %s~\n", ch->pcdata->screenname);
        if (ch->pcdata->image && ch->pcdata->image[0] != '\0')
            fprintf(fp, "Image        %s~\n", ch->pcdata->image);
        if (ch->pcdata->bio && ch->pcdata->bio[0] != '\0')
            fprintf(fp, "Bio          %s~\n", ch->pcdata->bio);
        if (ch->pcdata->authed_by && ch->pcdata->authed_by[0] != '\0')
            fprintf(fp, "AuthedBy     %s~\n", ch->pcdata->authed_by);
        if (ch->pcdata->min_snoop)
            fprintf(fp, "Minsnoop     %d\n", ch->pcdata->min_snoop);
        if (ch->pcdata->prompt && *ch->pcdata->prompt)
            fprintf(fp, "Prompt       %s~\n", ch->pcdata->prompt);
        if (ch->pcdata->pagerlen != 24)
            fprintf(fp, "Pagerlen     %d\n", ch->pcdata->pagerlen);
        fprintf(fp, "Addiction   ");
        for (drug = 0; drug <= 9; drug++)
            fprintf(fp, " %d", ch->pcdata->addiction[drug]);
        fprintf(fp, "\n");
        fprintf(fp, "Druglevel   ");
        for (drug = 0; drug <= 9; drug++)
            fprintf(fp, " %d", ch->pcdata->drug_level[drug]);
        fprintf(fp, "\n");
        if (ch->pcdata->wanted_flags)
            fprintf(fp, "Wanted       %d\n", ch->pcdata->wanted_flags);

        if (IS_IMMORTAL(ch) || ch->pcdata->area)
        {
            fprintf(fp, "WizInvis     %d\n", ch->pcdata->wizinvis);
            if (ch->pcdata->r_range_lo && ch->pcdata->r_range_hi)
                fprintf(fp, "RoomRange    %d %d\n", ch->pcdata->r_range_lo, ch->pcdata->r_range_hi);
            if (ch->pcdata->o_range_lo && ch->pcdata->o_range_hi)
                fprintf(fp, "ObjRange     %d %d\n", ch->pcdata->o_range_lo, ch->pcdata->o_range_hi);
            if (ch->pcdata->m_range_lo && ch->pcdata->m_range_hi)
                fprintf(fp, "MobRange     %d %d\n", ch->pcdata->m_range_lo, ch->pcdata->m_range_hi);
        }
        if (ch->pcdata->clan_name && ch->pcdata->clan_name[0] != '\0')
            fprintf(fp, "Clan         %s~\n", ch->pcdata->clan_name);
        fprintf(fp, "Flags        %d\n", ch->pcdata->flags);
        if (ch->pcdata->release_date > current_time)
            fprintf(fp, "Helled       %d %s~\n", (int)ch->pcdata->release_date, ch->pcdata->helled_by);
        if (ch->pcdata->pkills)
            fprintf(fp, "PKills       %d\n", ch->pcdata->pkills);
        if (ch->pcdata->pdeaths)
            fprintf(fp, "PDeaths      %d\n", ch->pcdata->pdeaths);
        if (get_timer(ch, TIMER_PKILLED) && (get_timer(ch, TIMER_PKILLED) > 0))
            fprintf(fp, "PTimer       %d\n", get_timer(ch, TIMER_PKILLED));
        fprintf(fp, "MKills       %d\n", ch->pcdata->mkills);
        fprintf(fp, "MDeaths      %d\n", ch->pcdata->mdeaths);
        if (ch->pcdata->illegal_pk)
            fprintf(fp, "IllegalPK    %d\n", ch->pcdata->illegal_pk);
        fprintf(fp, "AttrPerm     %d %d %d %d %d %d %d\n", ch->perm_str, ch->perm_int, ch->perm_wis, ch->perm_dex,
                ch->perm_con, ch->perm_cha, ch->perm_lck);

        fprintf(fp, "AttrMod      %d %d %d %d %d %d %d\n", ch->mod_str, ch->mod_int, ch->mod_wis, ch->mod_dex,
                ch->mod_con, ch->mod_cha, ch->mod_lck);

        fprintf(fp, "Condition    %d %d %d %d\n", ch->pcdata->condition[0], ch->pcdata->condition[1],
                ch->pcdata->condition[2], ch->pcdata->condition[3]);
        if (ch->desc && ch->desc->connection->getHostname().c_str())
            fprintf(fp, "Site         %s\n", ch->desc->connection->getHostname().c_str());
        else
            fprintf(fp, "Site         (Link-Dead)\n");

        for (sn = 1; sn < top_sn; sn++)
        {
            if (skill_table[sn]->name && ch->pcdata->learned[sn] > 0)
                switch (skill_table[sn]->type)
                {
                default:
                    fprintf(fp, "Skill        %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn]->name);
                    break;
                case SKILL_SPELL:
                    fprintf(fp, "Spell        %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn]->name);
                    break;
                case SKILL_WEAPON:
                    fprintf(fp, "Weapon       %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn]->name);
                    break;
                case SKILL_TONGUE:
                    fprintf(fp, "Tongue       %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn]->name);
                    break;
                }
        }
    }
    for (bug = ch->first_bug; bug; bug = bug->next_in_bug)
        fprintf(fp, "Bugged	%s~\n", bug->name);

    for (contract = ch->first_contract; contract; contract = contract->next_in_contract)
        fprintf(fp, "Contract    %s %d\n", contract->target, contract->amount);

    for (fellow = ch->first_fellow; fellow; fellow = fellow->next)
        fprintf(fp, "Fellow        %s %s~\n", fellow->victim, fellow->knownas);

    for (paf = ch->first_affect; paf; paf = paf->next)
    {
        if (paf->type >= 0 && (skill = get_skilltype(paf->type)) == nullptr)
            continue;

        if (paf->type >= 0 && paf->type < TYPE_PERSONAL)
            fprintf(fp, "AFFECT_DATA   '%s' %3d %3d %3d %10d\n", skill->name, paf->duration, paf->modifier,
                    paf->location, paf->bitvector);
        else
            fprintf(fp, "Affect       %3d %3d %3d %3d %10d\n", paf->type, paf->duration, paf->modifier, paf->location,
                    paf->bitvector);
    }

    track = URANGE(2, ((ch->top_level + 3) * MAX_KILLTRACK) / LEVEL_AVATAR, MAX_KILLTRACK);
    for (sn = 0; sn < track; sn++)
    {
        if (ch->pcdata->killed[sn].vnum == 0)
            break;
        fprintf(fp, "Killed       %d %d\n", ch->pcdata->killed[sn].vnum, ch->pcdata->killed[sn].count);
    }
    /* Save color values - Samson 9-29-98 */
    {
        int x;
        fprintf(fp, "MaxColors    %d\n", MAX_COLORS);
        fprintf(fp, "Colors       ");
        for (x = 0; x < MAX_COLORS; x++)
            fprintf(fp, "%d ", ch->colors[x]);
        fprintf(fp, "\n");
    }

    fprintf(fp, "End\n\n");
    return;
}

/*
 * Write an object and its contents.
 */
export void fwrite_obj(CHAR_DATA* ch, OBJ_DATA* obj, FILE* fp, int iNest, sh_int os_type)
{
    EXTRA_DESCR_DATA* ed;
    AFFECT_DATA* paf;
    sh_int wear, wear_loc, x;

    if (iNest >= MAX_NEST)
    {
        bug("fwrite_obj: iNest hit MAX_NEST %d", iNest);
        return;
    }

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if (obj->prev_content && os_type != OS_CORPSE)
        fwrite_obj(ch, obj->prev_content, fp, iNest, OS_CARRY);

    /*
     * Catch deleted objects					-Thoric
     */
    if (obj_extracted(obj))
        return;

    /*
     * Do NOT save prototype items!				-Thoric
     */
    if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
        return;

    /* Corpse saving. -- Altrag */
    fprintf(fp, (os_type == OS_CORPSE ? "#CORPSE\n" : "#OBJECT\n"));

    if (iNest)
        fprintf(fp, "Nest         %d\n", iNest);
    if (obj->count > 1)
        fprintf(fp, "Count        %d\n", obj->count);
    if (obj->name && obj->pIndexData->name && str_cmp(obj->name, obj->pIndexData->name))
        fprintf(fp, "Name         %s~\n", obj->name);
    if (obj->short_descr && obj->pIndexData->short_descr && str_cmp(obj->short_descr, obj->pIndexData->short_descr))
        fprintf(fp, "ShortDescr   %s~\n", obj->short_descr);
    if (obj->description && obj->pIndexData->description && str_cmp(obj->description, obj->pIndexData->description))
        fprintf(fp, "Description  %s~\n", obj->description);
    if (obj->action_desc && obj->pIndexData->action_desc && str_cmp(obj->action_desc, obj->pIndexData->action_desc))
        fprintf(fp, "ActionDesc   %s~\n", obj->action_desc);

    fprintf(fp, "Vnum         %d\n", obj->pIndexData->vnum);
    if (os_type == OS_CORPSE && obj->in_room)
        fprintf(fp, "Room         %d\n", obj->in_room->vnum);
    if (obj->extra_flags != obj->pIndexData->extra_flags)
        fprintf(fp, "ExtraFlags   %d\n", obj->extra_flags);
    if (obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf(fp, "WearFlags    %d\n", obj->wear_flags);
    wear_loc = -1;
    for (wear = 0; wear < MAX_WEAR; wear++)
        for (x = 0; x < MAX_LAYERS; x++)
            if (obj == save_equipment[wear][x])
            {
                wear_loc = wear;
                break;
            }
            else if (!save_equipment[wear][x])
                break;
    if (wear_loc != -1)
        fprintf(fp, "WearLoc      %d\n", wear_loc);
    if (obj->item_type != obj->pIndexData->item_type)
        fprintf(fp, "ItemType     %d\n", obj->item_type);
    if (obj->weight != obj->pIndexData->weight)
        fprintf(fp, "Weight       %d\n", obj->weight);
    if (obj->level)
        fprintf(fp, "Level        %d\n", obj->level);
    if (obj->timer)
        fprintf(fp, "Timer        %d\n", obj->timer);
    if (obj->cost != obj->pIndexData->cost)
        fprintf(fp, "Cost         %d\n", obj->cost);
    if (obj->value[0] || obj->value[1] || obj->value[2] || obj->value[3] || obj->value[4] || obj->value[5])
        fprintf(fp, "Values       %d %d %d %d %d %d\n", obj->value[0], obj->value[1], obj->value[2], obj->value[3],
                obj->value[4], obj->value[5]);

    switch (obj->item_type)
    {
    case ITEM_PILL: /* was down there with staff and wand, wrongly - Scryn */
    case ITEM_POTION:
        if (IS_VALID_SN(obj->value[1]))
            fprintf(fp, "Spell 1      '%s'\n", skill_table[obj->value[1]]->name);

        if (IS_VALID_SN(obj->value[2]))
            fprintf(fp, "Spell 2      '%s'\n", skill_table[obj->value[2]]->name);

        if (IS_VALID_SN(obj->value[3]))
            fprintf(fp, "Spell 3      '%s'\n", skill_table[obj->value[3]]->name);

        break;

    case ITEM_DEVICE:
        if (IS_VALID_SN(obj->value[3]))
            fprintf(fp, "Spell 3      '%s'\n", skill_table[obj->value[3]]->name);

        break;
    case ITEM_SALVE:
        if (IS_VALID_SN(obj->value[4]))
            fprintf(fp, "Spell 4      '%s'\n", skill_table[obj->value[4]]->name);

        if (IS_VALID_SN(obj->value[5]))
            fprintf(fp, "Spell 5      '%s'\n", skill_table[obj->value[5]]->name);
        break;
    }

    for (paf = obj->first_affect; paf; paf = paf->next)
    {
        /*
         * Save extra object affects				-Thoric
         */
        if (paf->type < 0 || paf->type >= top_sn)
        {
            fprintf(fp, "Affect       %d %d %d %d %d\n", paf->type, paf->duration,
                    ((paf->location == APPLY_WEAPONSPELL || paf->location == APPLY_WEARSPELL ||
                      paf->location == APPLY_REMOVESPELL || paf->location == APPLY_STRIPSN) &&
                     IS_VALID_SN(paf->modifier))
                        ? skill_table[paf->modifier]->slot
                        : paf->modifier,
                    paf->location, paf->bitvector);
        }
        else
            fprintf(fp, "AFFECT_DATA   '%s' %d %d %d %d\n", skill_table[paf->type]->name, paf->duration,
                    ((paf->location == APPLY_WEAPONSPELL || paf->location == APPLY_WEARSPELL ||
                      paf->location == APPLY_REMOVESPELL || paf->location == APPLY_STRIPSN) &&
                     IS_VALID_SN(paf->modifier))
                        ? skill_table[paf->modifier]->slot
                        : paf->modifier,
                    paf->location, paf->bitvector);
    }

    for (ed = obj->first_extradesc; ed; ed = ed->next)
        fprintf(fp, "ExtraDescr   %s~ %s~\n", ed->keyword, ed->description);

    fprintf(fp, "End\n\n");

    if (obj->first_content)
        fwrite_obj(ch, obj->last_content, fp, iNest + 1, OS_CARRY);

    return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
export bool load_char_obj(DESCRIPTOR_DATA& d, const char* name, bool preload)
{
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA* ch = nullptr;
    FILE* fp = nullptr;
    bool found = false;
    struct stat fst = {};
    int i = 0;
    int x = 0;
    extern FILE* fpArea;
    extern char strArea[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];

    CREATE(ch, CHAR_DATA, 1);
    for (x = 0; x < MAX_WEAR; x++)
        for (i = 0; i < MAX_LAYERS; i++)
            save_equipment[x][i] = nullptr;
    clear_char(ch);
    loading_char = ch;

    CREATE(ch->pcdata, PC_DATA, 1);
    d.character = ch;
    ch->desc = &d;
    ch->name = STRALLOC(name);
    ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
    ch->perm_str = 10;
    ch->perm_int = 10;
    ch->perm_wis = 10;
    ch->perm_dex = 10;
    ch->perm_con = 10;
    ch->perm_cha = 10;
    ch->perm_lck = 10;
    ch->pcdata->condition[COND_THIRST] = 48;
    ch->pcdata->condition[COND_FULL] = 48;
    ch->pcdata->condition[COND_BLOODTHIRST] = 10;
    ch->pcdata->wizinvis = 0;
    ch->mental_state = -10;
    ch->mobinvis = 0;
    for (i = 0; i < MAX_SKILL; i++)
        ch->pcdata->learned[i] = 0;
    ch->pcdata->release_date = 0;
    ch->pcdata->helled_by = nullptr;
    ch->saving_poison_death = 0;
    ch->saving_wand = 0;
    ch->saving_para_petri = 0;
    ch->saving_breath = 0;
    ch->saving_spell_staff = 0;
    ch->comments = nullptr; /* comments */
    ch->pcdata->pagerlen = 24;
    ch->mob_clan = STRALLOC("");
    ch->was_sentinel = nullptr;
    ch->plr_home = nullptr;
    ch->pheight = 0;
    ch->build = 0;
    found = false;
    sprintf_s(strsave, "%s%c/%s", PLAYER_DIR, tolower(name[0]), capitalize(name).c_str());
    if (stat(strsave, &fst) != -1)
    {
        if (fst.st_size == 0)
        {
            sprintf_s(strsave, "%s%c/%s", BACKUP_DIR, tolower(name[0]), capitalize(name).c_str());
            send_to_char("Restoring your backup player file...", ch);
        }
        else
        {
            sprintf_s(buf, "%s player data for: %s (%dK)", preload ? "Preloading" : "Loading", ch->name,
                      (int)fst.st_size / 1024);
            // log_string_plus( buf, LOG_COMM, LEVEL_GREATER );
        }
    }
    /* else no player file */

    if ((fp = fopen(strsave, "r")) != nullptr)
    {
        int iNest;

        for (iNest = 0; iNest < MAX_NEST; iNest++)
            rgObjNest[iNest] = nullptr;

        found = true;
        /* Cheat so that bug will show line #'s -- Altrag */
        fpArea = fp;
        strcpy_s(strArea, strsave);
        for (;;)
        {
            char letter;
            char* word;

            letter = fread_letter(fp);
            if (letter == '*')
            {
                fread_to_eol(fp);
                continue;
            }

            if (letter != '#')
            {
                bug("Load_char_obj: # not found.", 0);
                bug(name, 0);
                break;
            }

            word = fread_word(fp);
            if (!str_cmp(word, "PLAYER"))
            {
                fread_char(ch, fp, preload);
                if (preload)
                    break;
            }
            else if (!str_cmp(word, "OBJECT")) /* Objects	*/
                fread_obj(ch, fp, OS_CARRY);
            else if (!str_cmp(word, "COMMENT"))
                fread_comment(ch, fp);      /* Comments	*/
            else if (!str_cmp(word, "END")) /* Done		*/
                break;
            else
            {
                bug("Load_char_obj: bad section.", 0);
                bug(name, 0);
                break;
            }
        }
        fclose(fp);
        fpArea = nullptr;
        strcpy_s(strArea, "$");
    }

    if (!found)
    {
        ch->short_descr = STRALLOC("");
        ch->long_descr = STRALLOC("");
        ch->description = STRALLOC("");
        ch->editor = nullptr;
        ch->pcdata->clan_name = STRALLOC("");
        ch->pcdata->clan = nullptr;
        ch->pcdata->pwd = str_dup("");
        ch->pcdata->email = str_dup("");
        ch->pcdata->bamfin = str_dup("");
        ch->pcdata->bamfout = str_dup("");
        ch->rank = str_dup("");
        ch->pcdata->shipname = str_dup("");
        ch->pcdata->bestowments = str_dup("");
        ch->pcdata->title = STRALLOC("");
        ch->pcdata->disguise = STRALLOC("");
        ch->pcdata->homepage = str_dup("");
        ch->pcdata->screenname = str_dup("");
        ch->pcdata->image = str_dup("");
        ch->pcdata->bio = STRALLOC("");
        ch->pcdata->authed_by = STRALLOC("");
        ch->pcdata->prompt = STRALLOC("");
        ch->pcdata->r_range_lo = 0;
        ch->pcdata->r_range_hi = 0;
        ch->pcdata->m_range_lo = 0;
        ch->pcdata->m_range_hi = 0;
        ch->pcdata->o_range_lo = 0;
        ch->pcdata->o_range_hi = 0;
        ch->pcdata->wizinvis = 0;
        ch->pcdata->wanted_flags = 0;
    }
    else
    {
        if (!ch->pcdata->clan_name)
        {
            ch->pcdata->clan_name = STRALLOC("");
            ch->pcdata->clan = nullptr;
        }
        if (!ch->pcdata->bio)
            ch->pcdata->bio = STRALLOC("");

        if (!ch->pcdata->authed_by)
            ch->pcdata->authed_by = STRALLOC("");

        if (!IS_NPC(ch) && get_trust(ch) > LEVEL_AVATAR)
        {
            if (ch->pcdata->wizinvis < 2)
                ch->pcdata->wizinvis = ch->top_level;
            assign_area(ch);
        }
        if (file_ver > 1)
        {
            for (i = 0; i < MAX_WEAR; i++)
                for (x = 0; x < MAX_LAYERS; x++)
                    if (save_equipment[i][x])
                    {
                        equip_char(ch, save_equipment[i][x], i);
                        save_equipment[i][x] = nullptr;
                    }
                    else
                        break;
        }
    }

    loading_char = nullptr;
    return found;
}

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY(literal, field, value)                                                                                     \
    if (!str_cmp(word, literal))                                                                                       \
    {                                                                                                                  \
        field = value;                                                                                                 \
        fMatch = true;                                                                                                 \
        break;                                                                                                         \
    }

void fread_char(CHAR_DATA* ch, FILE* fp, bool preload)
{
    char buf[MAX_STRING_LENGTH];
    char* line;
    const char* word;
    int x1, x2, x3, x4, x5, x6, x7, x8, x9, x0;
    sh_int killcnt;
    bool fMatch;
    int max_colors = 0; /* Color code */
    time_t lastplayed;
    int sn, extra;
    char acrobuf[MSL];

    file_ver = 0;
    killcnt = 0;
    /* Setup color values in case player has none set - Samson */
    memcpy(&ch->colors, &default_set, sizeof(default_set));

    for (;;)
    {
        word = feof(fp) ? "End" : fread_word(fp);
        if (word == nullptr)
        {
            bug("fread_word error on character %s.", ch);
            return;
        }
        fMatch = false;

        switch (UPPER(word[0]))
        {
        case '*':
            fMatch = true;
            fread_to_eol(fp);
            break;

        case 'A':
            KEY("Act", ch->act, fread_number(fp));
            if (IS_SET(ch->act, ACT_MOUNTED))
                REMOVE_BIT(ch->act, ACT_MOUNTED);
            KEY("Act2", ch->pcdata->act2, fread_number(fp));
            KEY("AffectedBy", ch->affected_by, fread_number(fp));
            KEY("Alignment", ch->alignment, fread_number(fp));
            KEY("Armor", ch->armor, fread_number(fp));

            if (!str_cmp(word, "Addiction"))
            {
                line = fread_line(fp);
                x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = 0;
                sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &x0, &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8, &x9);
                ch->pcdata->addiction[0] = x0;
                ch->pcdata->addiction[1] = x1;
                ch->pcdata->addiction[2] = x2;
                ch->pcdata->addiction[3] = x3;
                ch->pcdata->addiction[4] = x4;
                ch->pcdata->addiction[5] = x5;
                ch->pcdata->addiction[6] = x6;
                ch->pcdata->addiction[7] = x7;
                ch->pcdata->addiction[8] = x8;
                ch->pcdata->addiction[9] = x9;
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Ability"))
            {
                line = fread_line(fp);
                x0 = x1 = x2 = x3 = 0;
                sscanf(line, "%d %d %d %d", &x0, &x1, &x2, &x3);
                if (x0 >= 0 && x0 < MAX_ABILITY)
                {
                    ch->skill_level[x0] = x1;
                    ch->experience[x0] = x2;
                    ch->bonus[x0] = x3;
                }
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Affect") || !str_cmp(word, "AFFECT_DATA"))
            {
                AFFECT_DATA* paf;

                if (preload)
                {
                    fMatch = true;
                    fread_to_eol(fp);
                    break;
                }
                CREATE(paf, AFFECT_DATA, 1);
                if (!str_cmp(word, "Affect"))
                {
                    paf->type = fread_number(fp);
                }
                else
                {
                    int sn;
                    char* sname = fread_word(fp);

                    if ((sn = skill_lookup(sname)) < 0)
                    {
                        if ((sn = herb_lookup(sname)) < 0)
                            bug("Fread_char: unknown skill.", 0);
                        else
                            sn += TYPE_HERB;
                    }
                    paf->type = sn;
                }

                paf->duration = fread_number(fp);
                paf->modifier = fread_number(fp);
                paf->location = fread_number(fp);
                paf->bitvector = fread_number(fp);
                LINK(paf, ch->first_affect, ch->last_affect, next, prev);
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "AttrMod"))
            {
                line = fread_line(fp);
                x1 = x2 = x3 = x4 = x5 = x6 = x7 = 13;
                sscanf(line, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);
                ch->mod_str = x1;
                ch->mod_int = x2;
                ch->mod_wis = x3;
                ch->mod_dex = x4;
                ch->mod_con = x5;
                ch->mod_cha = x6;
                ch->mod_lck = x7;
                if (!x7)
                    ch->mod_lck = 0;
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "AttrPerm"))
            {
                line = fread_line(fp);
                x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
                sscanf(line, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);
                ch->perm_str = x1;
                ch->perm_int = x2;
                ch->perm_wis = x3;
                ch->perm_dex = x4;
                ch->perm_con = x5;
                ch->perm_cha = x6;
                ch->perm_lck = x7;
                if (!x7 || x7 == 0)
                    ch->perm_lck = 13;
                fMatch = true;
                break;
            }
            KEY("AuthedBy", ch->pcdata->authed_by, fread_string(fp));
            break;

        case 'B':
            if (!str_cmp(word, "Bugged"))
            {
                BUG_DATA* bug;
                CREATE(bug, BUG_DATA, 1);
                bug->name = fread_string(fp);
                LINK(bug, ch->first_bug, ch->last_bug, next_in_bug, prev_in_bug);
                fMatch = true;
                break;
            }
            KEY("Bamfin", ch->pcdata->bamfin, fread_string_nohash(fp));
            KEY("Bamfout", ch->pcdata->bamfout, fread_string_nohash(fp));
            KEY("Bestowments", ch->pcdata->bestowments, fread_string_nohash(fp));
            KEY("Bio", ch->pcdata->bio, fread_string(fp));
            KEY("Bank", ch->pcdata->bank, fread_number(fp));
            KEY("Build", ch->build, fread_number(fp));
            break;

        case 'C':
            if (!str_cmp(word, "Comfreq"))
            {
                ch->comfreq = fread_string(fp);
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Clan"))
            {
                ch->pcdata->clan_name = fread_string(fp);

                if (!preload && ch->pcdata->clan_name[0] != '\0' &&
                    (ch->pcdata->clan = get_clan(ch->pcdata->clan_name)) == nullptr)
                {
                    sprintf_s(buf,
                              "Warning: the organization %s no longer exists, and therefore you no longer\n\rbelong to "
                              "that organization.\n\r",
                              ch->pcdata->clan_name);
                    send_to_char(buf, ch);
                    STRFREE(ch->pcdata->clan_name);
                    ch->pcdata->clan_name = STRALLOC("");
                }
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Condition"))
            {
                line = fread_line(fp);
                sscanf(line, "%d %d %d %d", &x1, &x2, &x3, &x4);
                ch->pcdata->condition[0] = x1;
                ch->pcdata->condition[1] = x2;
                ch->pcdata->condition[2] = x3;
                ch->pcdata->condition[3] = x4;
                fMatch = true;
                break;
            }

            /* Load color values - Samson 9-29-98 */
            {
                int x;
                if (!str_cmp(word, "Colors"))
                {
                    for (x = 0; x < max_colors; x++)
                        ch->colors[x] = fread_number(fp);
                    fMatch = true;
                    break;
                }
            }

            if (!str_cmp(word, "Contract"))
            {
                char* s1;
                int s2;
                CONTRACT_DATA* contract;
                s1 = fread_word(fp);
                s2 = fread_number(fp);
                CREATE(contract, CONTRACT_DATA, 1);
                contract->target = str_dup(s1);
                contract->amount = s2;
                LINK(contract, ch->first_contract, ch->last_contract, next_in_contract, prev_in_contract);
                fMatch = true;
                break;
            }
            break;

        case 'D':
            KEY("Damroll", ch->damroll, fread_number(fp));
            KEY("Deaf", ch->deaf, fread_number(fp));
            KEY("Description", ch->description, fread_string(fp));
            KEY("Disguise", ch->pcdata->disguise, fread_string(fp));
            if (!str_cmp(word, "Druglevel"))
            {
                line = fread_line(fp);
                x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = 0;
                sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &x0, &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8, &x9);
                ch->pcdata->drug_level[0] = x0;
                ch->pcdata->drug_level[1] = x1;
                ch->pcdata->drug_level[2] = x2;
                ch->pcdata->drug_level[3] = x3;
                ch->pcdata->drug_level[4] = x4;
                ch->pcdata->drug_level[5] = x5;
                ch->pcdata->drug_level[6] = x6;
                ch->pcdata->drug_level[7] = x7;
                ch->pcdata->drug_level[8] = x8;
                ch->pcdata->drug_level[9] = x9;
                fMatch = true;
                break;
            }
            break;

            /* 'E' was moved to after 'S' */
        case 'F':
            KEY("Flags", ch->pcdata->flags, fread_number(fp));
            KEY("ForceControl", ch->force_control, fread_number(fp));
            KEY("ForceSense", ch->force_sense, fread_number(fp));
            KEY("ForceAlter", ch->force_alter, fread_number(fp));
            KEY("ForceChance", ch->force_chance, fread_number(fp));
            KEY("ForceId", ch->force_identified, fread_number(fp));
            KEY("ForceType", ch->force_type, fread_number(fp));
            KEY("ForceAlign", ch->force_align, fread_number(fp));
            KEY("ForceConverted", ch->force_converted, fread_number(fp));
            KEY("ForceLvlStatus", ch->force_level_status, fread_number(fp));
            KEY("ForceMaster", ch->force_master, fread_string(fp));
            if (!str_cmp(word, "Force"))
            {
                line = fread_line(fp);
                x1 = x2 = x3 = x4 = x5 = x6 = 0;
                sscanf(line, "%d %d %d %d", &x1, &x2, &x3, &x4);
                ch->perm_frc = x1;
                ch->mana = x3;
                ch->max_mana = x4;
                fMatch = true;
                break;
            }
            if (!str_cmp(word, "ForceSkill"))
            {
                line = fread_line(fp);
                sscanf(line, "%d %d", &x1, &x2);
                if (x1 < MAX_FORCE_SKILL)
                    ch->force_skill[x1] = x2;
                fMatch = true;
                break;
            }
            KEY("forcerank", ch->pcdata->forcerank, fread_number(fp));
            KEY("Fiance", ch->pcdata->fiance, fread_string(fp));
            if (!str_cmp(word, "Fellow"))
            {
                char* victim;
                char* knownas;
                FELLOW_DATA* fellow;
                victim = fread_word(fp);
                knownas = fread_string(fp);
                // sprintf_s(buf, "Player %s knows %s as %s.", ch->name, victim, knownas);
                // log_string(buf);

                CREATE(fellow, FELLOW_DATA, 1);
                fellow->victim = STRALLOC(victim);
                fellow->knownas = knownas;
                LINK(fellow, ch->first_fellow, ch->last_fellow, next, prev);
                fMatch = true;
                break;
            }

            break;

        case 'G':
            KEY("Glory", ch->pcdata->quest_curr, fread_number(fp));
            KEY("Gold", ch->gold, fread_number(fp));
            /* temporary measure */
            if (!str_cmp(word, "Guild"))
            {
                ch->pcdata->clan_name = fread_string(fp);

                if (!preload && ch->pcdata->clan_name[0] != '\0' &&
                    (ch->pcdata->clan = get_clan(ch->pcdata->clan_name)) == nullptr)
                {
                    sprintf_s(buf,
                              "Warning: the organization %s no longer exists, and therefore you no longer\n\rbelong to "
                              "that organization.\n\r",
                              ch->pcdata->clan_name);
                    send_to_char(buf, ch);
                    STRFREE(ch->pcdata->clan_name);
                    ch->pcdata->clan_name = STRALLOC("");
                }
                fMatch = true;
                break;
            }
            break;

        case 'H':
            if (!str_cmp(word, "Helled"))
            {
                ch->pcdata->release_date = fread_number(fp);
                ch->pcdata->helled_by = fread_string(fp);
                if (ch->pcdata->release_date < current_time)
                {
                    STRFREE(ch->pcdata->helled_by);
                    ch->pcdata->helled_by = nullptr;
                    ch->pcdata->release_date = 0;
                }
                fMatch = true;
                break;
            }

            KEY("Hitroll", ch->hitroll, fread_number(fp));
            KEY("Homepage", ch->pcdata->homepage, fread_string_nohash(fp));

            if (!str_cmp(word, "HpManaMove"))
            {
                line = fread_line(fp);
                x1 = x2 = x3 = x4 = x5 = x6 = 0;
                sscanf(line, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);
                ch->hit = x1;
                ch->max_hit = x2;
                ch->move = x5;
                ch->max_move = x6;
                if (x4 >= 100)
                {
                    ch->perm_frc = number_range(1, 20);
                    ch->max_mana = x4;
                    ch->mana = x4;
                }
                else if (x4 >= 10)
                {
                    ch->perm_frc = 1;
                    ch->max_mana = x4;
                }
                fMatch = true;
                break;
            }

            break;

        case 'I':
            KEY("IllegalPK", ch->pcdata->illegal_pk, fread_number(fp));
            KEY("Image", ch->pcdata->image, fread_string_nohash(fp));
            KEY("Immune", ch->immune, fread_number(fp));
            break;

        case 'K':
            if (!str_cmp(word, "Killed"))
            {
                fMatch = true;
                if (killcnt >= MAX_KILLTRACK)
                    bug("fread_char: killcnt (%d) >= MAX_KILLTRACK", killcnt);
                else
                {
                    ch->pcdata->killed[killcnt].vnum = fread_number(fp);
                    ch->pcdata->killed[killcnt++].count = fread_number(fp);
                }
            }
            break;

        case 'L':
            KEY("Lastname", ch->pcdata->last_name, fread_string(fp));
            if (!str_cmp(word, "Lastplayed"))
            {
                lastplayed = fread_number(fp);
                fMatch = true;
                break;
            }
            KEY("LongDescr", ch->long_descr, fread_string(fp));
            if (!str_cmp(word, "Languages"))
            {
                ch->speaks = fread_number(fp);
                ch->speaking = fread_number(fp);
                fMatch = true;
            }
            break;

        case 'M':
            KEY("MainAbility", ch->main_ability, fread_number(fp));
            KEY("MaxColors", max_colors, fread_number(fp));
            KEY("MDeaths", ch->pcdata->mdeaths, fread_number(fp));
            KEY("Mentalstate", ch->mental_state, fread_number(fp));
            KEY("MGlory", ch->pcdata->quest_accum, fread_number(fp));
            KEY("Minsnoop", ch->pcdata->min_snoop, fread_number(fp));
            KEY("MKills", ch->pcdata->mkills, fread_number(fp));
            KEY("Mobinvis", ch->mobinvis, fread_number(fp));
            if (!str_cmp(word, "MobRange"))
            {
                ch->pcdata->m_range_lo = fread_number(fp);
                ch->pcdata->m_range_hi = fread_number(fp);
                fMatch = true;
            }
            break;

        case 'N':
            if (!str_cmp(word, "Name"))
            {
                /*
                 * Name already set externally.
                 */
                fread_to_eol(fp);
                fMatch = true;
                break;
            }
            break;

        case 'O':
            KEY("Outcast_time", ch->pcdata->outcast_time, fread_number(fp));
            if (!str_cmp(word, "ObjRange"))
            {
                ch->pcdata->o_range_lo = fread_number(fp);
                ch->pcdata->o_range_hi = fread_number(fp);
                fMatch = true;
            }
            break;

        case 'P':
            KEY("Pagerlen", ch->pcdata->pagerlen, fread_number(fp));
            KEY("Password", ch->pcdata->pwd, fread_string_nohash(fp));
            KEY("PDeaths", ch->pcdata->pdeaths, fread_number(fp));
            KEY("Pheight", ch->pheight, fread_number(fp));
            KEY("PKills", ch->pcdata->pkills, fread_number(fp));
            KEY("Played", ch->played, fread_number(fp));
            KEY("Position", ch->position, fread_number(fp));
            KEY("Practice", extra, fread_number(fp));
            KEY("Prompt", ch->pcdata->prompt, fread_string(fp));
            if (!str_cmp(word, "PTimer"))
            {
                add_timer(ch, TIMER_PKILLED, fread_number(fp), nullptr, 0);
                fMatch = true;
                break;
            }
            if (!str_cmp(word, "PlrHome"))
            {
                ch->plr_home = get_room_index(fread_number(fp));
                if (!ch->plr_home)
                    ch->plr_home = nullptr;
                fMatch = true;
                break;
            }
            break;

        case 'R':
            KEY("Race", ch->race, fread_number(fp));
            KEY("Rank", ch->rank, fread_string_nohash(fp));
            KEY("Resistant", ch->resistant, fread_number(fp));
            KEY("Restore_time", ch->pcdata->restore_time, fread_number(fp));
            KEY("Rppoints", ch->rppoints, fread_number(fp));

            if (!str_cmp(word, "Room"))
            {
                ch->in_room = get_room_index(fread_number(fp));
                if (!ch->in_room)
                    ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
                fMatch = true;
                break;
            }
            if (!str_cmp(word, "RoomRange"))
            {
                ch->pcdata->r_range_lo = fread_number(fp);
                ch->pcdata->r_range_hi = fread_number(fp);
                fMatch = true;
            }
            break;

        case 'S':
            KEY("Salary", ch->pcdata->salary, fread_number(fp));
            KEY("Screenname", ch->pcdata->screenname, fread_string_nohash(fp));
            KEY("Sex", ch->sex, fread_number(fp));
            KEY("ShipNum", ch->pcdata->shipnum, fread_number(fp));
            KEY("Ship", ch->pcdata->shipname, fread_string(fp));
            KEY("SecondAbility", ch->secondary_ability, fread_number(fp));
            KEY("ShortDescr", ch->short_descr, fread_string(fp));
            KEY("Spouse", ch->pcdata->spouse, fread_string(fp));
            KEY("Susceptible", ch->susceptible, fread_number(fp));
            if (!str_cmp(word, "SavingThrow"))
            {
                ch->saving_wand = fread_number(fp);
                ch->saving_poison_death = ch->saving_wand;
                ch->saving_para_petri = ch->saving_wand;
                ch->saving_breath = ch->saving_wand;
                ch->saving_spell_staff = ch->saving_wand;
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "SavingThrows"))
            {
                ch->saving_poison_death = fread_number(fp);
                ch->saving_wand = fread_number(fp);
                ch->saving_para_petri = fread_number(fp);
                ch->saving_breath = fread_number(fp);
                ch->saving_spell_staff = fread_number(fp);
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Site"))
            {
                if (!preload)
                {
                    sprintf_s(buf, "Last connected from: %s\n\r", fread_word(fp));
                    send_to_char(buf, ch);
                    if (file_ver < 4)
                    {
                        send_to_char("&R-=-=-=-=-=&WPLEASE READ&R=-=-=-=-=-\n\r", ch);

                        sprintf_s(acrobuf,
                                  "&WIn an effort to boost player interactivity, we at %s have decided to provide each "
                                  "player with 20,000 credits. This will only be given once, and if you wish the muds "
                                  "economy to be stronger, remember that if the credits are kept with the players, "
                                  "there is a larger chance of you getting credits back when you're selling a product "
                                  "or 'service' for you assassins and bounty hunters. So enjoy, and don't let the "
                                  "money go to your head. :)\n\r",
                                  sysdata.mud_acronym);
                        send_to_pager(acrobuf, ch);
                        send_to_pager("&R-=-=-=-=-=&WPLEASE READ&R=-=-=-=-=-\n\r", ch);
                        ch->gold = ch->gold + 20000;
                        WAIT_STATE(ch, 20 * PULSE_PER_SECOND);
                    }
                }
                else
                    fread_to_eol(fp);
                fMatch = true;
                if (preload)
                    word = "End";
                else
                    break;
            }

            if (!str_cmp(word, "Skill"))
            {
                int value;

                if (preload)
                    word = "End";
                else
                {
                    value = fread_number(fp);
                    if (file_ver < 3)
                        sn = skill_lookup(fread_word(fp));
                    else
                        sn = bsearch_skill_exact(fread_word(fp), gsn_first_skill, gsn_first_weapon - 1);
                    if (sn < 0)
                        bug("Fread_char: unknown skill.", 0);
                    else
                    {
                        ch->pcdata->learned[sn] = value;
                    }
                    fMatch = true;
                    break;
                }
            }

            if (!str_cmp(word, "Spell"))
            {
                int sn;
                int value;

                if (preload)
                    word = "End";
                else
                {
                    value = fread_number(fp);

                    sn = bsearch_skill_exact(fread_word(fp), gsn_first_spell, gsn_first_skill - 1);
                    if (sn < 0)
                        bug("Fread_char: unknown spell.", 0);
                    else
                    {
                        ch->pcdata->learned[sn] = value;
                    }
                    fMatch = true;
                    break;
                }
            }
            if (str_cmp(word, "End"))
                break;

        case 'E':
            if (!str_cmp(word, "End"))
            {
                if (!ch->short_descr)
                    ch->short_descr = STRALLOC("");
                if (!ch->long_descr)
                    ch->long_descr = STRALLOC("");
                if (!ch->description)
                    ch->description = STRALLOC("");
                if (!ch->pcdata->pwd)
                    ch->pcdata->pwd = str_dup("");
                if (!ch->pcdata->shipname)
                    ch->pcdata->shipname = str_dup("");
                if (!ch->pcdata->email)
                    ch->pcdata->email = str_dup("");
                if (!ch->pcdata->bamfin)
                    ch->pcdata->bamfin = str_dup("");
                if (!ch->pcdata->bamfout)
                    ch->pcdata->bamfout = str_dup("");
                if (!ch->pcdata->bio)
                    ch->pcdata->bio = STRALLOC("");
                if (!ch->rank)
                    ch->rank = str_dup("");
                if (!ch->pcdata->bestowments)
                    ch->pcdata->bestowments = str_dup("");
                if (!ch->pcdata->title)
                    ch->pcdata->title = STRALLOC("");
                if (!ch->pcdata->disguise)
                    ch->pcdata->disguise = STRALLOC("");
                if (!ch->pcdata->homepage)
                    ch->pcdata->homepage = str_dup("");
                if (!ch->pcdata->screenname)
                    ch->pcdata->screenname = str_dup("");
                if (!ch->pcdata->image)
                    ch->pcdata->image = str_dup("");
                if (!ch->pcdata->authed_by)
                    ch->pcdata->authed_by = STRALLOC("");
                if (!ch->pcdata->prompt)
                    ch->pcdata->prompt = STRALLOC("");
                ch->editor = nullptr;
                killcnt = URANGE(2, ((ch->top_level + 3) * MAX_KILLTRACK) / LEVEL_AVATAR, MAX_KILLTRACK);
                if (killcnt < MAX_KILLTRACK)
                    ch->pcdata->killed[killcnt].vnum = 0;
                {
                    int ability;
                    for (ability = 0; ability < MAX_ABILITY; ability++)
                    {
                        if (ch->skill_level[ability] == 0 && ability != FORCE_ABILITY)
                            ch->skill_level[ability] = 1;
                    }
                }
                if (!IS_IMMORTAL(ch) && !ch->speaking)
                    /*	ch->speaking = LANG_BASIC;      */
                    ch->speaking = race_table[ch->race].language;
                if (IS_IMMORTAL(ch))
                {
                    ch->speaks = ~0;
                    if (ch->speaking == 0)
                        ch->speaking = ~0;
                }
                if (!ch->pcdata->prompt)
                    ch->pcdata->prompt = STRALLOC("");

                if (lastplayed != 0)
                {
                    int hitgain;
                    hitgain = ((int)(current_time - lastplayed) / 60);
                    ch->hit = URANGE(1, ch->hit + hitgain, ch->max_hit);
                    ch->move = URANGE(1, ch->move + hitgain, ch->max_move);
                    if (ch->skill_level[FORCE_ABILITY] > 1)
                        ch->mana = URANGE(0, ch->mana + hitgain, ch->max_mana);
                    better_mental_state(ch, hitgain);
                }
                for (sn = 0; sn < top_sn; sn++)
                {
                    if (!skill_table[sn]->name)
                        break;

                    if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
                        continue;

                    if (ch->pcdata->learned[sn] > 0 &&
                        ch->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level)
                        ch->pcdata->learned[sn] = 0;
                }
                return;
            }
            KEY("Email", ch->pcdata->email, fread_string_nohash(fp));
            break;

        case 'T':
            KEY("Toplevel", ch->top_level, fread_number(fp));
            if (!str_cmp(word, "Tongue"))
            {
                int sn;
                int value;

                if (preload)
                    word = "End";
                else
                {
                    value = fread_number(fp);

                    sn = bsearch_skill_exact(fread_word(fp), gsn_first_tongue, gsn_top_sn - 1);
                    /* temp fix -Tawnos */
                    if (sn < 0)
                        sn = bsearch_skill_exact("basic", gsn_first_tongue, gsn_top_sn - 1);

                    if (sn < 0)
                        bug("Fread_char: unknown tongue.", 0);
                    else
                    {
                        ch->pcdata->learned[sn] = value;
                    }
                    fMatch = true;
                }
                break;
            }
            KEY("Trust", ch->trust, fread_number(fp));
            /* Let no character be trusted higher than one below maxlevel -- Narn */
            ch->trust = UMIN(ch->trust, MAX_LEVEL - 1);

            if (!str_cmp(word, "Title"))
            {
                ch->pcdata->title = fread_string(fp);
                if (isalpha(ch->pcdata->title[0]) || isdigit(ch->pcdata->title[0]))
                {
                    sprintf_s(buf, " %s", ch->pcdata->title);
                    if (ch->pcdata->title)
                        STRFREE(ch->pcdata->title);
                    ch->pcdata->title = STRALLOC(buf);
                }
                fMatch = true;
                break;
            }

            break;

        case 'V':
            if (!str_cmp(word, "Vnum"))
            {
                ch->pIndexData = get_mob_index(fread_number(fp));
                fMatch = true;
                break;
            }
            if (!str_cmp(word, "Version"))
            {
                file_ver = fread_number(fp);
                ch->pcdata->version = file_ver;
                fMatch = true;
                break;
            }
            break;

        case 'W':
            if (!str_cmp(word, "Weapon"))
            {
                int sn;
                int value;

                if (preload)
                    word = "End";
                else
                {
                    value = fread_number(fp);

                    sn = bsearch_skill_exact(fread_word(fp), gsn_first_weapon, gsn_first_tongue - 1);
                    if (sn < 0)
                        bug("Fread_char: unknown weapon.", 0);
                    else
                    {
                        ch->pcdata->learned[sn] = value;
                    }
                    fMatch = true;
                }
                break;
            }
            KEY("Wimpy", ch->wimpy, fread_number(fp));
            KEY("WizInvis", ch->pcdata->wizinvis, fread_number(fp));
            KEY("Wanted", ch->pcdata->wanted_flags, fread_number(fp));
            break;
        }

        if (!fMatch)
        {
            sprintf_s(buf, "Fread_char: no match: %s", word);
            bug(buf, 0);
        }
    }
}

export void fread_obj(CHAR_DATA* ch, FILE* fp, sh_int os_type)
{
    OBJ_DATA* obj = nullptr;
    const char* word = nullptr;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    ROOM_INDEX_DATA* room = nullptr;

    CREATE(obj, OBJ_DATA, 1);
    obj->count = 1;
    obj->wear_loc = -1;
    obj->weight = 1;

    fNest = true; /* Requiring a Nest 0 is a waste */
    fVnum = true;
    iNest = 0;

    for (;;)
    {
        word = feof(fp) ? "End" : fread_word(fp);
        fMatch = false;

        switch (UPPER(word[0]))
        {
        case '*':
            fMatch = true;
            fread_to_eol(fp);
            break;

        case 'A':
            if (!str_cmp(word, "Affect") || !str_cmp(word, "AFFECT_DATA"))
            {
                AFFECT_DATA* paf;
                int pafmod;

                CREATE(paf, AFFECT_DATA, 1);
                if (!str_cmp(word, "Affect"))
                {
                    paf->type = fread_number(fp);
                }
                else
                {
                    int sn;

                    sn = skill_lookup(fread_word(fp));
                    if (sn < 0)
                        bug("Fread_obj: unknown skill.", 0);
                    else
                        paf->type = sn;
                }
                paf->duration = fread_number(fp);
                pafmod = fread_number(fp);
                paf->location = fread_number(fp);
                paf->bitvector = fread_number(fp);
                if (paf->location == APPLY_WEAPONSPELL || paf->location == APPLY_WEARSPELL ||
                    paf->location == APPLY_REMOVESPELL)
                    paf->modifier = slot_lookup(pafmod);
                else
                    paf->modifier = pafmod;
                LINK(paf, obj->first_affect, obj->last_affect, next, prev);
                fMatch = true;
                break;
            }
            KEY("Actiondesc", obj->action_desc, fread_string(fp));
            break;

        case 'C':
            KEY("Cost", obj->cost, fread_number(fp));
            KEY("Count", obj->count, fread_number(fp));
            break;

        case 'D':
            KEY("Description", obj->description, fread_string(fp));
            break;

        case 'E':
            KEY("ExtraFlags", obj->extra_flags, fread_number(fp));

            if (!str_cmp(word, "ExtraDescr"))
            {
                EXTRA_DESCR_DATA* ed;

                CREATE(ed, EXTRA_DESCR_DATA, 1);
                ed->keyword = fread_string(fp);
                ed->description = fread_string(fp);
                LINK(ed, obj->first_extradesc, obj->last_extradesc, next, prev);
                fMatch = true;
            }

            if (!str_cmp(word, "End"))
            {
                if (!fNest || !fVnum)
                {
                    bug("Fread_obj: incomplete object.", 0);
                    if (obj->name)
                        STRFREE(obj->name);
                    if (obj->description)
                        STRFREE(obj->description);
                    if (obj->short_descr)
                        STRFREE(obj->short_descr);
                    DISPOSE(obj);
                    return;
                }
                else
                {
                    sh_int wear_loc = obj->wear_loc;

                    if (!obj->name)
                        obj->name = QUICKLINK(obj->pIndexData->name);
                    if (!obj->description)
                        obj->description = QUICKLINK(obj->pIndexData->description);
                    if (!obj->short_descr)
                        obj->short_descr = QUICKLINK(obj->pIndexData->short_descr);
                    if (!obj->action_desc)
                        obj->action_desc = QUICKLINK(obj->pIndexData->action_desc);
                    LINK(obj, first_object, last_object, next, prev);
                    obj->pIndexData->count += obj->count;
                    if (!obj->serial)
                    {
                        cur_obj_serial = UMAX((cur_obj_serial + 1) & (BV30 - 1), 1);
                        obj->serial = obj->pIndexData->serial = cur_obj_serial;
                    }
                    if (fNest)
                        rgObjNest[iNest] = obj;
                    numobjsloaded += obj->count;
                    ++physicalobjects;
                    if (file_ver > 1 || obj->wear_loc < -1 || obj->wear_loc >= MAX_WEAR)
                        obj->wear_loc = -1;
                    /* Corpse saving. -- Altrag */
                    if (os_type == OS_CORPSE)
                    {
                        if (!room)
                        {
                            bug("Fread_obj: Corpse without room", 0);
                            room = get_room_index(ROOM_VNUM_LIMBO);
                        }
                        obj = obj_to_room(obj, room);
                    }
                    else if (iNest == 0 || rgObjNest[iNest] == nullptr)
                    {
                        int slot;
                        bool reslot = false;

                        if (file_ver > 1 && wear_loc > -1 && wear_loc < MAX_WEAR)
                        {
                            int x;

                            for (x = 0; x < MAX_LAYERS; x++)
                                if (!save_equipment[wear_loc][x])
                                {
                                    save_equipment[wear_loc][x] = obj;
                                    slot = x;
                                    reslot = true;
                                    break;
                                }
                            if (x == MAX_LAYERS)
                                bug("Fread_obj: too many layers %d", wear_loc);
                        }
                        obj = obj_to_char(obj, ch);
                        if (reslot)
                            save_equipment[wear_loc][slot] = obj;
                    }
                    else
                    {
                        if (rgObjNest[iNest - 1])
                        {
                            separate_obj(rgObjNest[iNest - 1]);
                            obj = obj_to_obj(obj, rgObjNest[iNest - 1]);
                        }
                        else
                            bug("Fread_obj: nest layer missing %d", iNest - 1);
                    }
                    if (fNest)
                        rgObjNest[iNest] = obj;
                    return;
                }
            }
            break;

        case 'I':
            KEY("ItemType", obj->item_type, fread_number(fp));
            break;

        case 'L':
            KEY("Level", obj->level, fread_number(fp));
            break;

        case 'N':
            KEY("Name", obj->name, fread_string(fp));

            if (!str_cmp(word, "Nest"))
            {
                iNest = fread_number(fp);
                if (iNest < 0 || iNest >= MAX_NEST)
                {
                    bug("Fread_obj: bad nest %d.", iNest);
                    iNest = 0;
                    fNest = false;
                }
                fMatch = true;
            }
            break;

        case 'R':
            KEY("Room", room, get_room_index(fread_number(fp)));

        case 'S':
            KEY("ShortDescr", obj->short_descr, fread_string(fp));

            if (!str_cmp(word, "Spell"))
            {
                int iValue;
                int sn;

                iValue = fread_number(fp);
                sn = skill_lookup(fread_word(fp));
                if (iValue < 0 || iValue > 5)
                    bug("Fread_obj: bad iValue %d.", iValue);
                else if (sn < 0)
                    bug("Fread_obj: unknown skill.", 0);
                else
                    obj->value[iValue] = sn;
                fMatch = true;
                break;
            }

            break;

        case 'T':
            KEY("Timer", obj->timer, fread_number(fp));
            break;

        case 'V':
            if (!str_cmp(word, "Values"))
            {
                int x1, x2, x3, x4, x5, x6;
                char* ln = fread_line(fp);

                x1 = x2 = x3 = x4 = x5 = x6 = 0;
                sscanf(ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);

                obj->value[0] = x1;
                obj->value[1] = x2;
                obj->value[2] = x3;
                obj->value[3] = x4;
                obj->value[4] = x5;
                obj->value[5] = x6;
                fMatch = true;
                break;
            }

            if (!str_cmp(word, "Vnum"))
            {
                int vnum;

                vnum = fread_number(fp);
                if ((obj->pIndexData = get_obj_index(vnum)) == nullptr)
                {
                    fVnum = false;
                    bug("Fread_obj: bad vnum %d.", vnum);
                }
                else
                {
                    fVnum = true;
                    obj->cost = obj->pIndexData->cost;
                    obj->weight = obj->pIndexData->weight;
                    obj->item_type = obj->pIndexData->item_type;
                    obj->wear_flags = obj->pIndexData->wear_flags;
                    obj->extra_flags = obj->pIndexData->extra_flags;
                }
                fMatch = true;
                break;
            }
            break;

        case 'W':
            KEY("WearFlags", obj->wear_flags, fread_number(fp));
            KEY("WearLoc", obj->wear_loc, fread_number(fp));
            KEY("Weight", obj->weight, fread_number(fp));
            break;
        }

        if (!fMatch)
        {
            EXTRA_DESCR_DATA* ed;
            AFFECT_DATA* paf;

            bug("Fread_obj: no match.", 0);
            bug(word, 0);
            fread_to_eol(fp);
            if (obj->name)
                STRFREE(obj->name);
            if (obj->description)
                STRFREE(obj->description);
            if (obj->short_descr)
                STRFREE(obj->short_descr);
            while ((ed = obj->first_extradesc) != nullptr)
            {
                STRFREE(ed->keyword);
                STRFREE(ed->description);
                UNLINK(ed, obj->first_extradesc, obj->last_extradesc, next, prev);
                DISPOSE(ed);
            }
            while ((paf = obj->first_affect) != nullptr)
            {
                UNLINK(paf, obj->first_affect, obj->last_affect, next, prev);
                DISPOSE(paf);
            }
            DISPOSE(obj);
            return;
        }
    }
}

// TODO this causes SIGALRM to be delivered but signal handling is dead
/*
void set_alarm(long seconds)
{
    alarm(seconds);
}
*/

void do_last(CHAR_DATA* ch, char* argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    struct stat fst;

    argument = one_argument(argument, arg);
    if (arg[0] == '\0')
    {
        send_to_char("Usage: last <playername>\n\r", ch);
        send_to_char("Usage: last <# of entries OR \'-1\' for all entries OR \'today\' for all of today's entries>\n\r",
                     ch);
        send_to_char("Usage: last <playername> <count>\n\r", ch);
        return;
    }
    if (get_trust(ch) < LEVEL_ASCENDANT)
    {
        set_char_color(AT_IMMORT, ch);
        send_to_char("Their godly glow prevents you from getting a good look.\n\r", ch);
        return;
    }
    if (isdigit(arg[0]) || atoi(arg) == -1 || !str_cmp(arg, "today")) // View list instead of players
    {
        send_to_char("&w&RName                     Time                        "
                     "Host/Ip\n\r&c&w---------------------------------------------------------------------------\n\r",
                     ch);
        if (!str_cmp(arg, "today"))
            read_last_file(ch, -2, nullptr);
        else
            read_last_file(ch, atoi(arg), nullptr);
        return;
    }
    strcpy_s(name, capitalize(arg).c_str());
    if (argument[0] != '\0')
    {
        send_to_char("&w&RName                     Time                        "
                     "Host/Ip\n\r&c&w---------------------------------------------------------------------------\n\r",
                     ch);
        read_last_file(ch, atoi(argument), name);
        return;
    }
    sprintf_s(buf, "%s%c/%s", PLAYER_DIR, tolower(arg[0]), name);

    if (stat(buf, &fst) != -1)
        sprintf_s(buf, "%s was last on: %s\r", name, ctime(&fst.st_mtime));
    else
        sprintf_s(buf, "%s was not found.\n\r", name);
    send_to_char(buf, ch);
}

void write_corpses(CHAR_DATA* ch, char* name)
{
    OBJ_DATA* corpse;
    FILE* fp = nullptr;

    /* Name and ch support so that we dont have to have a char to save their
       corpses.. (ie: decayed corpses while offline) */
    if (ch && IS_NPC(ch))
    {
        bug("Write_corpses: writing NPC corpse.", 0);
        return;
    }
    if (ch)
        name = ch->name;
    /* Go by vnum, less chance of screwups. -- Altrag */
    for (corpse = first_object; corpse; corpse = corpse->next)
        if (corpse->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && corpse->in_room != nullptr &&
            !str_cmp(corpse->short_descr + 14, name))
        {
            if (!fp)
            {
                char buf[127];

                sprintf_s(buf, "%s%s", CORPSE_DIR, capitalize(name).c_str());
                if (!(fp = fopen(buf, "w")))
                {
                    bug("Write_corpses: Cannot open file.", 0);
                    perror(buf);
                    return;
                }
            }
            fwrite_obj(ch, corpse, fp, 0, OS_CORPSE);
        }
    if (fp)
    {
        fprintf(fp, "#END\n\n");
        fclose(fp);
    }
    else
    {
        char buf[127];

        sprintf_s(buf, "%s%s", CORPSE_DIR, capitalize(name).c_str());
        remove(buf);
    }
    return;
}

export void load_corpses(void)
{
    // TODO why are these extern? What?!?
    extern FILE* fpArea;
    extern char strArea[MAX_INPUT_LENGTH];
    extern int falling;

    fpArea = nullptr;
    // TODO make sure this directory exists
    /*
    if (!(dp = opendir(CORPSE_DIR)))
    {
        bug("Load_corpses: can't open CORPSE_DIR", 0);
        perror(CORPSE_DIR);
        return;
    }
    */

    falling = 1; /* Arbitrary, must be >0 though. */
    for (const auto& entry : std::filesystem::directory_iterator(CORPSE_DIR))
    {
        auto name = entry.path().filename().string();
        if (name[0] != '.')
        {
            sprintf_s(strArea, "%s%s", CORPSE_DIR, name.c_str());
            fprintf(stderr, "Corpse -> %s\n", strArea);
            if (!(fpArea = fopen(strArea, "r")))
            {
                perror(strArea);
                continue;
            }
            for (;;)
            {
                char letter;
                char* word;

                letter = fread_letter(fpArea);
                if (letter == '*')
                {
                    fread_to_eol(fpArea);
                    continue;
                }
                if (letter != '#')
                {
                    bug("Load_corpses: # not found.", 0);
                    break;
                }
                word = fread_word(fpArea);
                if (!str_cmp(word, "CORPSE"))
                    fread_obj(nullptr, fpArea, OS_CORPSE);
                else if (!str_cmp(word, "OBJECT"))
                    fread_obj(nullptr, fpArea, OS_CARRY);
                else if (!str_cmp(word, "END"))
                    break;
                else
                {
                    bug("Load_corpses: bad section.", 0);
                    break;
                }
            }
            fclose(fpArea);
            fpArea = nullptr;
        }
    }
    fpArea = nullptr;
    strcpy_s(strArea, "$");
    falling = 0;
    return;
}
