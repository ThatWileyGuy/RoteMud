/***********************************************************************************
 *                                                                                  *
 *          _______.____    __    ____       _______                 _______        *
 *         /       )\   \  /  \  /   /  _   |   ____)         __    |   ____)       *
 *        (   (----` \   \/    \/   /  (_)  |  |__    ___   _/  |_  |  |__          *
 *         \   \      \            /    _   |   __)  / _ \ (_   __) |   __)         *
 *     .----)   )      \    /\    /    (_)  |  |    ( (_) )  |  |   |  |____        *
 *    (________/        \__/  \__/          |__|     \___/   |__|   |_______)       *
 *                                                                                  *
 * SWFotE v2.0 (FotE v1.1 cleaned up and considerably modded) by                    *
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

/*
 *  Renumber Imm command
 *  Author: Cronel (cronel_kal@hotmail.com)
 *  of FrozenMUD (empire.digiunix.net 4000)
 *
 *  Permission to use and distribute this code is granted provided
 *  this header is retained and unaltered, and the distribution
 *  package contains all the original files unmodified.
 *  If you modify this code and use/distribute modified versions
 *  you must give credit to the original author(s).
 */

#include "mud.hxx"

#define NOT_FOUND (-1)
enum
{
    REN_ROOM,
    REN_OBJ,
    REN_MOB
};

struct RENUMBER_DATA
{
    int old_vnum;
    int new_vnum;

    RENUMBER_DATA* next;
};

struct RENUMBER_AREA
{
    int low_obj, hi_obj;
    RENUMBER_DATA* r_obj;
    int low_mob, hi_mob;
    RENUMBER_DATA* r_mob;
    int low_room, hi_room;
    RENUMBER_DATA* r_room;
};

void renumber_area(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_area, bool area_is_proto, bool verbose);

RENUMBER_AREA* gather_renumber_data(AREA_DATA* area, int new_base, bool fill_gaps);
RENUMBER_DATA* gather_one_list(short type, int low, int high, int new_base, bool fill_gaps, int* max_vnum);
void free_renumber_data(RENUMBER_DATA* r_data);

AREA_DATA* find_area(char* filename, bool* p_is_proto);
bool check_vnums(CHAR_DATA* ch, AREA_DATA* tarea, RENUMBER_AREA* r_area);

int find_translation(int vnum, RENUMBER_DATA* r_data);
void translate_reset(RESET_DATA* reset, RENUMBER_AREA* r_data);
void translate_objvals(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_data, bool verbose);
void translate_exits(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_area, bool verbose);
void warn_progs(CHAR_DATA* ch, int low, int high, AREA_DATA* area, RENUMBER_AREA* r_area);
void warn_in_prog(CHAR_DATA* ch, int low, int high, const char* where, int vnum, MPROG_DATA* mprog,
                  RENUMBER_AREA* r_area);

void do_renumber(CHAR_DATA* ch, char* argument)
{
    RENUMBER_AREA* r_area;
    AREA_DATA* area;
    bool is_proto;
    char arg1[MAX_INPUT_LENGTH];
    int new_base;
    bool fill_gaps, verbose;

    /* parse the first two parameters */
    /* first, area */
    argument = one_argument(argument, arg1);
    if (arg1[0] == '\0')
    {
        ch_printf(ch, "What area do you want to renumber?\n\r");
        return;
    }
    area = find_area(arg1, &is_proto);
    if (area == nullptr)
    {
        ch_printf(ch, "No such area '%s'.\n\r", arg1);
        return;
    }

    /* and new vnum base */
    argument = one_argument(argument, arg1);
    if (arg1[0] == '\0')
    {
        ch_printf(ch, "What will be the new vnum base for this area?\n\r");
        return;
    }
    if (!is_number(arg1))
    {
        ch_printf(ch, "Sorry, '%s' is not a valid vnum base number!\n\r", arg1);
        return;
    }
    new_base = atoi(arg1);

    /* parse the flags */
    fill_gaps = false;
    verbose = false;
    for (;;)
    {
        argument = one_argument(argument, arg1);
        if (arg1[0] == '\0')
            break;
        else if (!str_prefix(arg1, "fillgaps"))
            fill_gaps = true;
        else if (!str_prefix(arg1, "verbose"))
            verbose = true;
        else
        {
            ch_printf(ch, "Invalid flag '%s'.\n\r", arg1);
            return;
        }
    }

    /* sanity check */
    if (new_base == area->low_r_vnum && new_base == area->low_o_vnum && new_base == area->low_m_vnum && !fill_gaps)
    {
        ch_printf(ch, "You don't want to change the base vnum and you don't want to fill gaps...\n\rSo what DO you "
                      "wanna do?\n\r");
        return;
    }

    /* some restrictions */
    if (IS_NPC(ch))
    {
        ch_printf(ch, "Yeah, right.\n\r");
        return;
    }
    if (ch->top_level < LEVEL_SUB_IMPLEM)
    {
        ch_printf(ch, "You don't have enough privileges.\n\r");
        return;
    }
    if (ch->top_level == LEVEL_SUB_IMPLEM)
    {
        if (area->low_r_vnum < ch->pcdata->r_range_lo || area->hi_r_vnum > ch->pcdata->r_range_hi ||
            area->low_m_vnum < ch->pcdata->m_range_lo || area->hi_m_vnum > ch->pcdata->m_range_hi ||
            area->low_o_vnum < ch->pcdata->o_range_lo || area->hi_o_vnum > ch->pcdata->o_range_hi)
        {
            ch_printf(ch, "You can't renumber that area ('%s').\n\r", area->filename);
            return;
        }
    }

    /* get the renumber data */
    r_area = gather_renumber_data(area, new_base, fill_gaps);

    /* one more restriction */
    if (ch->top_level == LEVEL_SUB_IMPLEM)
    {
        if (r_area->low_room < ch->pcdata->r_range_lo || r_area->hi_room > ch->pcdata->r_range_hi ||
            r_area->low_obj < ch->pcdata->o_range_lo || r_area->hi_obj > ch->pcdata->o_range_hi ||
            r_area->low_mob < ch->pcdata->m_range_lo || r_area->hi_mob > ch->pcdata->m_range_hi)
        {
            ch_printf(ch, "The renumbered area would be outside your assigned vnum range.\n\r");
            return;
        }
    }
    else if (is_proto)
    {
        if (r_area->low_room < area->low_r_vnum || r_area->hi_room > area->hi_r_vnum ||
            r_area->low_obj < area->low_o_vnum || r_area->hi_obj > area->hi_o_vnum ||
            r_area->low_mob < area->low_m_vnum || r_area->hi_mob > area->hi_m_vnum)
        {
            ch_printf(ch, "Moving a proto area out of its range would create problems.\n\rWait till the area is "
                          "finished to move it.\n\r");
            return;
        }
    }

    /* no overwriting of dest vnums */
    if (check_vnums(ch, area, r_area))
        return;

    /* another sanity check :) */
    if (r_area == nullptr || (r_area->r_obj == nullptr && r_area->r_mob == nullptr && r_area->r_room == nullptr))
    {
        ch_printf(ch, "No changes to make.\n\r");
        if (r_area != nullptr)
            DISPOSE(r_area);
        return;
    }

    /* ok, do it! */
    pager_printf(ch, "Renumbering area '%s' to new base %d, filling gaps: %s\n\r", area->filename, new_base,
                 fill_gaps ? "yes" : "no");
    renumber_area(ch, area, r_area, is_proto, verbose);
    pager_printf(ch, "Done.\n\r");

    /* clean up and goodbye */
    if (r_area->r_room != nullptr)
        free_renumber_data(r_area->r_room);
    if (r_area->r_obj != nullptr)
        free_renumber_data(r_area->r_obj);
    if (r_area->r_mob != nullptr)
        free_renumber_data(r_area->r_mob);
    DISPOSE(r_area);
}

bool check_vnums(CHAR_DATA* ch, AREA_DATA* tarea, RENUMBER_AREA* r_area)
{
    int high, low;
    AREA_DATA* area;
    bool proto;

    /* this function assumes all the lows are allways gonna be
       lower or equal to all the highs .. */
    high = UMAX(r_area->hi_room, UMAX(r_area->hi_obj, r_area->hi_mob));
    low = UMIN(r_area->low_room, UMIN(r_area->low_obj, r_area->low_mob));

    /* in do_check_vnums they use first_bsort, first_asort but.. i dunno.. */
    area = first_area;
    proto = false;
    while (area)
    {
        if (tarea == area)
            ;
        else if (!(high < area->low_r_vnum || low > area->hi_r_vnum) ||
                 !(high < area->low_o_vnum || low > area->hi_o_vnum) ||
                 !(high < area->low_m_vnum || low > area->hi_m_vnum))
        {
            ch_printf(ch, "This operation would overwrite area %s! Use checkvnums first.\n\r", area->filename);
            return true;
        }

        area = area->next;
        if (area == nullptr && !proto)
        {
            area = first_build;
            proto = true;
        }
    }
    return false;
}

RENUMBER_AREA* gather_renumber_data(AREA_DATA* area, int new_base, bool fill_gaps)
/* this function actualy gathers all the renumber data for an area */
{
    RENUMBER_AREA* r_area;
    int max;

    CREATE(r_area, RENUMBER_AREA, 1);

    r_area->r_mob = gather_one_list(REN_MOB, area->low_m_vnum, area->hi_m_vnum, new_base, fill_gaps, &max);
    r_area->low_mob = new_base;
    r_area->hi_mob = max;

    r_area->r_obj = gather_one_list(REN_OBJ, area->low_o_vnum, area->hi_o_vnum, new_base, fill_gaps, &max);
    r_area->low_obj = new_base;
    r_area->hi_obj = max;

    r_area->r_room = gather_one_list(REN_ROOM, area->low_r_vnum, area->hi_r_vnum, new_base, fill_gaps, &max);
    r_area->low_room = new_base;
    r_area->hi_room = max;

    return r_area;
}

RENUMBER_DATA* gather_one_list(short type, int low, int high, int new_base, bool fill_gaps, int* max_vnum)
/* this function builds a list of renumber data for a type (obj, room, or mob) */
{
    int cur_vnum;
    RENUMBER_DATA *r_data, root;
    bool found;
    ROOM_INDEX_DATA* room;
    OBJ_INDEX_DATA* obj;
    MOB_INDEX_DATA* mob;
    int i;
    int highest;

    memset(&root, 0, sizeof(RENUMBER_DATA));
    r_data = &root;

    cur_vnum = new_base;
    highest = -1;
    for (i = low; i <= high; i++)
    {
        found = false;
        switch (type)
        {
        case REN_ROOM:
            room = get_room_index(i);
            if (room != nullptr)
                found = true;
            break;
        case REN_OBJ:
            obj = get_obj_index(i);
            if (obj != nullptr)
                found = true;
            break;
        case REN_MOB:
            mob = get_mob_index(i);
            if (mob != nullptr)
                found = true;
            break;
        }

        if (found)
        {
            if (cur_vnum > highest)
                highest = cur_vnum;
            if (cur_vnum != i)
            {
                CREATE(r_data->next, RENUMBER_DATA, 1);
                r_data = r_data->next;
                r_data->old_vnum = i;
                r_data->new_vnum = cur_vnum;
            }
            cur_vnum++;
        }
        else if (!fill_gaps)
            cur_vnum++;
    }
    *max_vnum = highest;
    return root.next;
}

void free_renumber_data(RENUMBER_DATA* r_data)
/* disposes of a list of renumber data items */
{
    RENUMBER_DATA* r_next;

    while (r_data != nullptr)
    {
        r_next = r_data->next;
        DISPOSE(r_data);
        r_data = r_next;
    }
}

void renumber_area(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_area, bool area_is_proto, bool verbose)
/* this is the function that actualy does the renumbering of "area" according
   to the renumber data in "r_area". "ch" is to show messages. */
{
    RENUMBER_DATA* r_data;
    RESET_DATA *preset, *treset;
    int iHash;
    int low, high;

    high = UMAX(area->hi_r_vnum, UMIN(area->hi_o_vnum, area->hi_m_vnum));
    low = UMIN(area->low_r_vnum, UMIN(area->low_o_vnum, area->low_m_vnum));

    pager_printf(ch, "(Room) Renumbering...\n\r");

    /* what we do here is, for each list (room/obj/mob) first we
     * take each element out of the hash array, change the vnum,
     * and move it to our own list. after everything's moved out
     * we put it in again. this is to avoid problems in situations
     * where where room A is being moved to position B, but theres
     * already a room B wich is also being moved to position C.
     * a straightforward approach would result in us moving A to
     * position B first, and then again to position C, and room
     * B being lost inside the hash array, still there, but not
     * foundable (its "covered" by A because they'd have the same
     * vnum).
     */

    std::vector<ROOM_INDEX_DATA*> rooms;
    for (r_data = r_area->r_room; r_data; r_data = r_data->next)
    {
        if (verbose)
            pager_printf(ch, "(Room) %d -> %d\n\r", r_data->old_vnum, r_data->new_vnum);

        ROOM_INDEX_DATA* room = get_room_index(r_data->old_vnum);
        if (!room)
        {
            bug("renumber_area: nullptr room %d", r_data->old_vnum);
            continue;
        }

        /* remove it from the hash list */
        if (!g_roomIndex.erase(r_data->old_vnum))
        {
            bug("renumber_area: Couldn't find a room in the hash table! Skipping it.\n\r");
            continue;
        }

        /* change the vnum */
        room->vnum = r_data->new_vnum;

        /* move it to the temporary list */
        rooms.push_back(room);
    }
    /* now move everything back into the hash array */
    for (auto room : rooms)
    {
        g_roomIndex.insert({room->vnum, room});
    }
    /* if nothing was moved, or if the area is proto, dont change this */
    if (r_area->r_room != nullptr && !area_is_proto)
    {
        area->low_r_vnum = r_area->low_room;
        area->hi_r_vnum = r_area->hi_room;
    }

    pager_printf(ch, "(Mobs) Renumbering...\n\r");
    std::vector<MOB_INDEX_DATA*> mobs;
    for (r_data = r_area->r_mob; r_data; r_data = r_data->next)
    {
        if (verbose)
            pager_printf(ch, "(Mobs) %d -> %d\n\r", r_data->old_vnum, r_data->new_vnum);

        MOB_INDEX_DATA* mob = get_mob_index(r_data->old_vnum);
        if (!mob)
        {
            bug("renumber_area: nullptr mob %d", r_data->old_vnum);
            continue;
        }

        /* fix references to this mob from shops while renumbering this mob */
        if (mob->pShop)
        {
            if (verbose)
                pager_printf(ch, "(Mobs) Fixing shop for mob %d -> %d\n\r", r_data->old_vnum, r_data->new_vnum);
            mob->pShop->keeper = r_data->new_vnum;
        }
        if (mob->rShop)
        {
            if (verbose)
                pager_printf(ch, "(Mobs) Fixing repair shop for mob %d -> %d\n\r", r_data->old_vnum, r_data->new_vnum);
            mob->rShop->keeper = r_data->new_vnum;
        }

        /* remove it from the hash list */
        if (!g_mobIndex.erase(r_data->old_vnum))
        {
            bug("renumber_area: Couldn't find a mob in the hash table! Skipping it.\n\r");
            continue;
        }

        /* change the vnum */
        mob->vnum = r_data->new_vnum;

        /* move to private list */
        mobs.push_back(mob);
    }
    for (auto mob : mobs)
    {
        g_mobIndex.insert({mob->vnum, mob});
    }
    if (r_area->r_mob && !area_is_proto)
    {
        area->low_m_vnum = r_area->low_mob;
        area->hi_m_vnum = r_area->hi_mob;
    }

    pager_printf(ch, "(Objs) Renumbering...\n\r");
    std::vector<OBJ_INDEX_DATA*> objects;
    for (r_data = r_area->r_obj; r_data; r_data = r_data->next)
    {
        if (verbose)
            pager_printf(ch, "(Objs) %d -> %d\n\r", r_data->old_vnum, r_data->new_vnum);
        OBJ_INDEX_DATA* obj = get_obj_index(r_data->old_vnum);
        if (!obj)
        {
            bug("renumber_area: nullptr obj %d", r_data->old_vnum);
            continue;
        }

        /* remove it from the hash list */
        if (!g_objectIndex.erase(r_data->old_vnum))
        {
            bug("renumber_area: Couldn't find an obj in the hash table! Skipping it.\n\r");
            continue;
        }

        /* change the vnum */
        obj->vnum = r_data->new_vnum;

        /* to our list */
        objects.push_back(obj);
    }
    for (auto obj : objects)
    {
        /* add it to the hash list again */
        g_objectIndex.insert({obj->vnum, obj});
    }
    if (r_area->r_obj && !area_is_proto)
    {
        area->low_o_vnum = r_area->low_obj;
        area->hi_o_vnum = r_area->hi_obj;
    }

    pager_printf(ch, "Fixing references...\n\r");

    pager_printf(ch, "... fixing objvals...\n\r");
    translate_objvals(ch, area, r_area, verbose);

    pager_printf(ch, "... fixing exits...\n\r");
    translate_exits(ch, area, r_area, verbose);

    send_to_pager("... fixing resets...\n\r", ch);
    for (auto room = area->first_room; room; room = room->next_aroom)
    {
        for (preset = room->first_reset; preset; preset = preset->next)
        {
            translate_reset(preset, r_area);
            for (treset = preset->first_reset; treset; treset = preset->next_reset)
                translate_reset(treset, r_area);
        }
    }

    if (verbose)
    {
        pager_printf(ch, "Searching progs for references to renumbered vnums...\n\r");
        warn_progs(ch, low, high, area, r_area);
    }
}

void translate_exits(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_area, bool verbose)
{
    int i, new_vnum;
    EXIT_DATA *exit, *rev_exit;
    ROOM_INDEX_DATA* room;
    int old_vnum;

    for (i = area->low_r_vnum; i <= area->hi_r_vnum; i++)
    {
        room = get_room_index(i);
        if (!room)
            continue;
        for (exit = room->first_exit; exit; exit = exit->next)
        {
            /* translate the exit destination, if it was moved */
            new_vnum = find_translation(exit->vnum, r_area->r_room);
            if (new_vnum != NOT_FOUND)
                exit->vnum = new_vnum;
            /* if this room was moved */
            if (exit->rvnum != i)
            {
                old_vnum = exit->rvnum;
                exit->rvnum = i;
                /* all reverse exits in other areas will be wrong */
                rev_exit = get_exit_to(exit->to_room, rev_dir[exit->vdir], old_vnum);
                if (rev_exit && exit->to_room->area != area)
                {
                    if (rev_exit->vnum != i)
                    {
                        pager_printf(ch, "...    fixing reverse exit in area %s.\n\r", exit->to_room->area->filename);
                        rev_exit->vnum = i;
                    }
                }
            }

            /* translate the key */
            if (exit->key != -1)
            {
                new_vnum = find_translation(exit->key, r_area->r_obj);
                if (new_vnum == NOT_FOUND)
                    continue;
                exit->key = new_vnum;
            }
        }
    }
}

void translate_objvals(CHAR_DATA* ch, AREA_DATA* area, RENUMBER_AREA* r_area, bool verbose)
{
    int i, new_vnum;
    OBJ_INDEX_DATA* obj;

    for (i = area->low_o_vnum; i <= area->hi_o_vnum; i++)
    {
        obj = get_obj_index(i);
        if (!obj)
            continue;

        if (obj->item_type == ITEM_CONTAINER)
        {
            new_vnum = find_translation(obj->value[2], r_area->r_obj);
            if (new_vnum != NOT_FOUND)
            {
                if (verbose)
                    pager_printf(ch, "...    container %d; fixing objval2 (key vnum) %d -> %d\n\r", i, obj->value[2],
                                 new_vnum);
                obj->value[2] = new_vnum;
            }
            else if (verbose)
                pager_printf(ch, "...    container %d; no need to fix.\n\r", i);
        }
        else if (obj->item_type == ITEM_SWITCH || obj->item_type == ITEM_LEVER || obj->item_type == ITEM_PULLCHAIN ||
                 obj->item_type == ITEM_BUTTON)
        {
            /* levers might have room vnum references in their objvals */
            if (IS_SET(obj->value[0], TRIG_TELEPORT) || IS_SET(obj->value[0], TRIG_TELEPORTALL) ||
                IS_SET(obj->value[0], TRIG_TELEPORTPLUS) || IS_SET(obj->value[0], TRIG_RAND4) ||
                IS_SET(obj->value[0], TRIG_RAND6) || IS_SET(obj->value[0], TRIG_DOOR))
            {
                new_vnum = find_translation(obj->value[1], r_area->r_room);
                if (new_vnum != NOT_FOUND)
                {
                    if (verbose)
                        pager_printf(ch, "...    lever %d: fixing source room (%d -> %d)\n\r", i, obj->value[1],
                                     new_vnum);
                    obj->value[1] = new_vnum;
                }
                if (IS_SET(obj->value[0], TRIG_DOOR) && IS_SET(obj->value[0], TRIG_PASSAGE))
                {
                    new_vnum = find_translation(obj->value[2], r_area->r_room);
                    if (new_vnum != NOT_FOUND)
                    {
                        if (verbose)
                            pager_printf(ch, "...    lever %d: fixing dest room (passage) (%d -> %d)\n\r", i,
                                         obj->value[2], new_vnum);
                        obj->value[2] = new_vnum;
                    }
                }
            }
        }
    }
}

void warn_progs(CHAR_DATA* ch, int low, int high, AREA_DATA* area, RENUMBER_AREA* r_area)
{
    ROOM_INDEX_DATA* room;
    OBJ_INDEX_DATA* obj;
    MOB_INDEX_DATA* mob;
    MPROG_DATA* mprog;
    int i;

    for (i = area->low_r_vnum; i <= area->hi_r_vnum; i++)
    {
        room = get_room_index(i);
        if (!room)
            continue;
        mprog = room->mudprogs;
        while (mprog)
        {
            warn_in_prog(ch, low, high, "room", i, mprog, r_area);
            mprog = mprog->next;
        }
    }

    for (i = area->low_o_vnum; i <= area->hi_o_vnum; i++)
    {
        obj = get_obj_index(i);
        if (!obj)
            continue;
        mprog = obj->mudprogs;
        while (mprog)
        {
            warn_in_prog(ch, low, high, "obj", i, mprog, r_area);
            mprog = mprog->next;
        }
    }

    for (i = area->low_m_vnum; i <= area->hi_m_vnum; i++)
    {
        mob = get_mob_index(i);
        if (!mob)
            continue;
        mprog = mob->mudprogs;
        while (mprog)
        {
            warn_in_prog(ch, low, high, "mob", i, mprog, r_area);
            mprog = mprog->next;
        }
    }
}

void warn_in_prog(CHAR_DATA* ch, int low, int high, const char* where, int vnum, MPROG_DATA* mprog,
                  RENUMBER_AREA* r_area)
{
    char *p, *start_number, cTmp;
    int num;

    p = mprog->comlist;
    while (*p)
    {
        if (isdigit(*p))
        {
            start_number = p;
            while (isdigit(*p) && *p)
                p++;
            cTmp = *p;
            *p = 0;
            num = atoi(start_number);
            *p = cTmp;
            if (num >= low && num <= high)
            {
                pager_printf(ch,
                             "Warning! %s prog in %s vnum %d might contain a reference to %d.\n\r(Translation: Room "
                             "%d, Obj %d, Mob %d)\n\r",
                             mprog_type_to_name(mprog->type), where, vnum, num, find_translation(num, r_area->r_room),
                             find_translation(num, r_area->r_obj), find_translation(num, r_area->r_mob));
            }
            if (*p == '\0')
                break;
        }
        p++;
    }
}

void translate_reset(RESET_DATA* reset, RENUMBER_AREA* r_data)
/* this function translates a reset according to the renumber data in r_data */
{
    /* a list based approach to fixing the resets. instead
       of having a bunch of several instances of very
       similar code, i just made this array that tells the
       code what to do. it's pretty straightforward */
    const char* action_table[] = {"Mm1r3", "Oo1r3", "Ho1", "Po1o3", "Go1", "Eo1", "Dr1", "Rr1", nullptr};
    const char* p;
    RENUMBER_DATA* r_table;
    int *parg, new_vnum, i;

    /* T is a special case */
    if (reset->command == 'T')
    {
        if (IS_SET(reset->extra, TRAP_ROOM))
            r_table = r_data->r_room;
        else if (IS_SET(reset->extra, TRAP_OBJ))
            r_table = r_data->r_obj;
        else
        {
            bug("translate_reset: Invalid 'T' reset found.\n\r");
            return;
        }
        new_vnum = find_translation(reset->arg3, r_table);
        if (new_vnum != NOT_FOUND)
            reset->arg3 = new_vnum;
        return;
    }

    /* B is another special case */
    if (reset->command == 'B')
    {
        bug("translate_reset: B command found.");
        if ((reset->arg2 & BIT_RESET_TYPE_MASK) == BIT_RESET_DOOR ||
            (reset->arg2 & BIT_RESET_TYPE_MASK) == BIT_RESET_ROOM)
        {
            new_vnum = find_translation(reset->arg1, r_data->r_obj);
            if (new_vnum != NOT_FOUND)
                reset->arg1 = new_vnum;
        }
        return;
    }

    for (i = 0; action_table[i] != nullptr; i++)
    {
        if (reset->command == action_table[i][0])
        {
            p = action_table[i] + 1;
            while (*p)
            {
                if (*p == 'm')
                    r_table = r_data->r_mob;
                else if (*p == 'o')
                    r_table = r_data->r_obj;
                else if (*p == 'r')
                    r_table = r_data->r_room;
                else
                {
                    bug("translate_reset: Invalid action found in action table.\n\r");
                    p += 2;
                    continue;
                }
                p++;

                if (*p == '1')
                    parg = &(reset->arg1);
                else if (*p == '2')
                    parg = &(reset->arg2);
                else if (*p == '3')
                    parg = &(reset->arg3);
                else
                {
                    bug("translate_reset: Invalid argument number found in action table.\n\r");
                    p++;
                    continue;
                }
                p++;

                new_vnum = find_translation(*parg, r_table);
                if (new_vnum != NOT_FOUND)
                    *parg = new_vnum;
            }
            return;
        }
    }

    if (action_table[i] == nullptr)
        bug("translate_reset: Invalid reset '%c' found.\n\r", reset->command);
}

int find_translation(int vnum, RENUMBER_DATA* r_data)
/* returns the new vnum for the old vnum "vnum" according to the info in
 * r_data
 */
{
    RENUMBER_DATA* r_temp;

    for (r_temp = r_data; r_temp; r_temp = r_temp->next)
    {
        if (r_temp->old_vnum == vnum)
            return r_temp->new_vnum;
    }
    return NOT_FOUND;
}

AREA_DATA* find_area(char* filename, bool* p_is_proto)
/* simply returns a pointer to a "filename" or nullptr if no such area. stores
   true in *p_is_proto if the area is proto */
{
    bool found;
    AREA_DATA* area;

    found = false;

    for (area = first_area; area; area = area->next)
    {
        if (!str_cmp(area->filename, filename))
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        *p_is_proto = false;
        return area;
    }

    for (area = first_build; area; area = area->next)
    {
        if (!str_cmp(area->filename, filename))
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        *p_is_proto = true;
        return area;
    }
    else
        return nullptr;
}
