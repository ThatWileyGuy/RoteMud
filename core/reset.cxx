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
 * This file relies heavily on the fact that your linked lists are correct,
 * and that pArea->reset_first is the first reset in pArea.  Likewise,
 * pArea->reset_last *MUST* be the last reset in pArea.  Weird and
 * wonderful things will happen if any of your lists are messed up, none
 * of them good.  The most important are your pRoom->contents,
 * pRoom->people, rch->carrying, obj->contains, and pArea->reset_first ..
 * pArea->reset_last.  -- Altrag
 */

#include "mud.hxx"

/* Externals */
extern int top_reset;
extern const char* wear_locs[];

int get_trapflag(const char* flag);

/*
 * Find some object with a given index data.
 * Used by area-reset 'P', 'T' and 'H' commands.
 */
OBJ_DATA* get_obj_type(OBJ_INDEX_DATA* pObjIndex)
{
    OBJ_DATA* obj;

    for (obj = first_object; obj; obj = obj->next)
    {
        if (obj->pIndexData == pObjIndex)
            return obj;
    }
    return nullptr;
}

char* sprint_reset(RESET_DATA* pReset, short* num)
{
    RESET_DATA *tReset, *gReset;
    static char buf[MAX_STRING_LENGTH];
    char mobname[MAX_STRING_LENGTH] = {};
    char roomname[MAX_STRING_LENGTH] = {};
    char objname[MAX_STRING_LENGTH] = {};
    static ROOM_INDEX_DATA* room;
    static OBJ_INDEX_DATA *obj, *obj2;
    static MOB_INDEX_DATA* mob;

    switch (pReset->command)
    {
    default:
        snprintf(buf, MAX_STRING_LENGTH, "%2d) *** BAD RESET: %c %d %d %d %d ***\n\r", (*num), pReset->command,
                 pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3);
        break;

    case 'M':
        mob = get_mob_index(pReset->arg1);
        room = get_room_index(pReset->arg3);
        if (mob)
            strcpy_s(mobname, mob->player_name);
        else
            strcpy_s(mobname, "Mobile: *BAD VNUM*");
        if (room)
            strcpy_s(roomname, room->name);
        else
            strcpy_s(roomname, "Room: *BAD VNUM*");
        snprintf(buf, MAX_STRING_LENGTH, "%2d) %s (%d) -> %s Room: %d [%d]\n\r", (*num), mobname, pReset->arg1,
                 roomname, pReset->arg3, pReset->arg2);

        for (tReset = pReset->first_reset; tReset; tReset = tReset->next_reset)
        {
            (*num)++;
            switch (tReset->command)
            {
            case 'E':
                if (!mob)
                    strcpy_s(mobname, "* ERROR: NO MOBILE! *");
                if (!(obj = get_obj_index(tReset->arg1)))
                    strcpy_s(objname, "Object: *BAD VNUM*");
                else
                    strcpy_s(objname, obj->name);
                snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%2d) (equip) %s (%d) -> %s (%s) [%d]\n\r",
                         (*num), objname, tReset->arg1, mobname, wear_locs[tReset->arg3], tReset->arg2);
                break;

            case 'G':
                if (!mob)
                    strcpy_s(mobname, "* ERROR: NO MOBILE! *");
                if (!(obj = get_obj_index(tReset->arg1)))
                    strcpy_s(objname, "Object: *BAD VNUM*");
                else
                    strcpy_s(objname, obj->name);
                snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%2d) (carry) %s (%d) -> %s [%d]\n\r",
                         (*num), objname, tReset->arg1, mobname, tReset->arg2);
                break;
            }
            if (tReset->first_reset)
            {
                for (gReset = tReset->first_reset; gReset; gReset = gReset->next_reset)
                {
                    (*num)++;
                    switch (gReset->command)
                    {
                    case 'P':
                        if (!(obj2 = get_obj_index(gReset->arg1)))
                            strcpy_s(objname, "Object1: *BAD VNUM*");
                        else
                            strcpy_s(objname, obj2->name);
                        if (gReset->arg3 > 0 && (obj = get_obj_index(gReset->arg3)) == nullptr)
                            strcpy_s(roomname, "Object2: *BAD VNUM*");
                        else if (!obj)
                            strcpy_s(roomname, "Object2: *nullptr obj*");
                        else
                            strcpy_s(roomname, obj->name);
                        snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf),
                                 "%2d) (put) %s (%d) -> %s (%d) [%d]\n\r", (*num), objname, gReset->arg1, roomname,
                                 obj ? obj->vnum : gReset->arg3, gReset->arg2);
                        break;
                    }
                }
            }
        }
        break;

    case 'O':
        if (!(obj = get_obj_index(pReset->arg1)))
            strcpy_s(objname, "Object: *BAD VNUM*");
        else
            strcpy_s(objname, obj->name);
        room = get_room_index(pReset->arg3);
        if (!room)
            strcpy_s(roomname, "Room: *BAD VNUM*");
        else
            strcpy_s(roomname, room->name);
        snprintf(buf, MAX_STRING_LENGTH, "%2d) (object) %s (%d) -> %s Room: %d [%d]\n\r", (*num), objname, pReset->arg1,
                 roomname, pReset->arg3, pReset->arg2);

        for (tReset = pReset->first_reset; tReset; tReset = tReset->next_reset)
        {
            (*num)++;

            switch (tReset->command)
            {
            case 'P':
                if (!(obj2 = get_obj_index(tReset->arg1)))
                    strcpy_s(objname, "Object1: *BAD VNUM*");
                else
                    strcpy_s(objname, obj2->name);
                if (tReset->arg3 > 0 && (obj = get_obj_index(tReset->arg3)) == nullptr)
                    strcpy_s(roomname, "Object2: *BAD VNUM*");
                else if (!obj)
                    strcpy_s(roomname, "Object2: *nullptr obj*");
                else
                    strcpy_s(roomname, obj->name);
                snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%2d) (put) %s (%d) -> %s (%d) [%d]\n\r",
                         (*num), objname, tReset->arg1, roomname, obj ? obj->vnum : tReset->arg3, tReset->arg2);
                break;

            case 'T':
                snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf),
                         "%2d) (trap) %d %d %d %d (%s) -> %s (%d)\n\r", (*num), tReset->extra, tReset->arg1,
                         tReset->arg2, tReset->arg3, flag_string(tReset->extra, trap_flags), objname,
                         obj ? obj->vnum : 0);
                break;

            case 'H':
                snprintf(buf + strlen(buf), MAX_STRING_LENGTH - strlen(buf), "%2d) (hide) -> %s\n\r", (*num), objname);
                break;
            }
        }
        break;

    case 'D':
        if (pReset->arg2 < 0 || pReset->arg2 > MAX_DIR + 1)
            pReset->arg2 = 0;
        if (!(room = get_room_index(pReset->arg1)))
        {
            strcpy_s(roomname, "Room: *BAD VNUM*");
            snprintf(objname, MAX_STRING_LENGTH, "%s (no exit)", dir_name[pReset->arg2]);
        }
        else
        {
            strcpy_s(roomname, room->name);
            snprintf(objname, MAX_STRING_LENGTH, "%s%s", dir_name[pReset->arg2],
                     get_exit(room, pReset->arg2) ? "" : " (NO EXIT!)");
        }
        switch (pReset->arg3)
        {
        default:
            strcpy_s(mobname, "(* ERROR *)");
            break;
        case 0:
            strcpy_s(mobname, "Open");
            break;
        case 1:
            strcpy_s(mobname, "Close");
            break;
        case 2:
            strcpy_s(mobname, "Close and lock");
            break;
        }
        snprintf(buf, MAX_STRING_LENGTH, "%2d) %s [%d] the %s [%d] door %s (%d)\n\r", (*num), mobname, pReset->arg3,
                 objname, pReset->arg2, roomname, pReset->arg1);
        break;

    case 'R':
        if (!(room = get_room_index(pReset->arg1)))
            strcpy_s(roomname, "Room: *BAD VNUM*");
        else
            strcpy_s(roomname, room->name);
        snprintf(buf, MAX_STRING_LENGTH, "%2d) Randomize exits 0 to %d -> %s (%d)\n\r", (*num), pReset->arg2, roomname,
                 pReset->arg1);
        break;

    case 'T':
        if (!(room = get_room_index(pReset->arg3)))
            strcpy_s(roomname, "Room: *BAD VNUM*");
        else
            strcpy_s(roomname, room->name);
        snprintf(buf, MAX_STRING_LENGTH, "%2d) Trap: %d %d %d %d (%s) -> %s (%d)\n\r", (*num), pReset->extra,
                 pReset->arg1, pReset->arg2, pReset->arg3, flag_string(pReset->extra, trap_flags), roomname,
                 room ? room->vnum : 0);
        break;
    }
    return buf;
}

/*
 * Create a new reset (for online building) - Thoric
 */
RESET_DATA* make_reset(char letter, int extra, int arg1, int arg2, int arg3)
{
    RESET_DATA* pReset;

    CREATE(pReset, RESET_DATA, 1);
    pReset->command = letter;
    pReset->extra = extra;
    pReset->arg1 = arg1;
    pReset->arg2 = arg2;
    pReset->arg3 = arg3;
    top_reset++;
    return pReset;
}

void add_obj_reset(ROOM_INDEX_DATA* room, char cm, OBJ_DATA* obj, int v2, int v3)
{
    OBJ_DATA* inobj;
    static int iNest;

    if ((cm == 'O' || cm == 'P') && obj->pIndexData->vnum == OBJ_VNUM_TRAP)
    {
        if (cm == 'O')
            add_reset(room, 'T', obj->value[3], obj->value[1], obj->value[0], v3);
        return;
    }
    add_reset(room, cm, (cm == 'P' ? iNest : 0), obj->pIndexData->vnum, v2, v3);
    if (cm == 'O' && IS_OBJ_STAT(obj, ITEM_HIDDEN) && !CAN_WEAR(obj, ITEM_TAKE))
        add_reset(room, 'H', 1, 0, 0, 0);
    for (inobj = obj->first_content; inobj; inobj = inobj->next_content)
    {
        if (inobj->pIndexData->vnum == OBJ_VNUM_TRAP)
            add_obj_reset(room, 'O', inobj, 0, 0);
    }
    if (cm == 'P')
        iNest++;
    for (inobj = obj->first_content; inobj; inobj = inobj->next_content)
        add_obj_reset(room, 'P', inobj, inobj->count, obj->pIndexData->vnum);
    if (cm == 'P')
        iNest--;
    return;
}

void delete_reset(RESET_DATA* pReset)
{
    RESET_DATA *tReset, *tReset_next;

    for (tReset = pReset->first_reset; tReset; tReset = tReset_next)
    {
        tReset_next = tReset->next_reset;

        UNLINK(tReset, pReset->first_reset, pReset->last_reset, next_reset, prev_reset);
        delete_reset(tReset);
    }
    pReset->first_reset = pReset->last_reset = nullptr;
    DISPOSE(pReset);
    return;
}

void instaroom(ROOM_INDEX_DATA* pRoom, bool dodoors)
{
    CHAR_DATA* rch;
    OBJ_DATA* obj;

    for (rch = pRoom->first_person; rch; rch = rch->next_in_room)
    {
        if (!IS_NPC(rch))
            continue;

        add_reset(pRoom, 'M', 1, rch->pIndexData->vnum, rch->pIndexData->count, pRoom->vnum);

        for (obj = rch->first_carrying; obj; obj = obj->next_content)
        {
            if (obj->wear_loc == WEAR_NONE)
                add_obj_reset(pRoom, 'G', obj, 1, 0);
            else
                add_obj_reset(pRoom, 'E', obj, 1, obj->wear_loc);
        }
    }
    for (obj = pRoom->first_content; obj; obj = obj->next_content)
        add_obj_reset(pRoom, 'O', obj, obj->count, pRoom->vnum);
    if (dodoors)
    {
        EXIT_DATA* pexit;

        for (pexit = pRoom->first_exit; pexit; pexit = pexit->next)
        {
            int state = 0;

            if (!IS_SET(pexit->exit_info, EX_ISDOOR))
                continue;

            if (IS_SET(pexit->exit_info, EX_CLOSED))
            {
                if (IS_SET(pexit->exit_info, EX_LOCKED))
                    state = 2;
                else
                    state = 1;
            }
            add_reset(pRoom, 'D', 0, pRoom->vnum, pexit->vdir, state);
        }
    }
    return;
}

void wipe_resets(ROOM_INDEX_DATA* room)
{
    RESET_DATA *pReset, *pReset_next;

    for (pReset = room->first_reset; pReset; pReset = pReset_next)
    {
        pReset_next = pReset->next;

        UNLINK(pReset, room->first_reset, room->last_reset, next, prev);
        delete_reset(pReset);
    }
    room->first_reset = room->last_reset = nullptr;
    return;
}

void wipe_area_resets(AREA_DATA* area)
{
    ROOM_INDEX_DATA* room;
    extern bool mud_down;

    if (!mud_down)
    {
        for (room = area->first_room; room; room = room->next_aroom)
            wipe_resets(room);
    }
    return;
}

/* Function modified from original form - Samson */
void do_instaroom(CHAR_DATA* ch, char* argument)
{
    bool dodoors;

    if (IS_NPC(ch) || get_trust(ch) < LEVEL_SAVIOR || !ch->pcdata->area)
    {
        send_to_char("You don't have an assigned area to create resets for.\n\r", ch);
        return;
    }

    if (!str_cmp(argument, "nodoors"))
        dodoors = false;
    else
        dodoors = true;

    if (!can_rmodify(ch, ch->in_room))
        return;
    if (ch->in_room->area != ch->pcdata->area && get_trust(ch) < LEVEL_GREATER)
    {
        send_to_char("You cannot reset this room.\n\r", ch);
        return;
    }
    if (ch->in_room->first_reset)
        wipe_resets(ch->in_room);
    instaroom(ch->in_room, dodoors);
    send_to_char("Room resets installed.\n\r", ch);
}

/* Function modified from original form - Samson */
void do_instazone(CHAR_DATA* ch, char* argument)
{
    AREA_DATA* pArea;
    ROOM_INDEX_DATA* pRoom;
    bool dodoors;

    if (IS_NPC(ch) || get_trust(ch) < LEVEL_SAVIOR || !ch->pcdata->area)
    {
        send_to_char("You don't have an assigned area to create resets for.\n\r", ch);
        return;
    }
    if (!str_cmp(argument, "nodoors"))
        dodoors = false;
    else
        dodoors = true;
    pArea = ch->pcdata->area;
    wipe_area_resets(pArea);
    for (pRoom = pArea->first_room; pRoom; pRoom = pRoom->next_aroom)
        instaroom(pRoom, dodoors);
    send_to_char("Area resets installed.\n\r", ch);
    return;
}

int generate_itemlevel(AREA_DATA* pArea, OBJ_INDEX_DATA* pObjIndex)
{
    int olevel;
    int min = UMAX(pArea->low_soft_range, 1);
    int max = UMIN(pArea->hi_soft_range, min + 15);

    if (pObjIndex->level > 0)
        olevel = UMIN(pObjIndex->level, MAX_LEVEL);
    else
        switch (pObjIndex->item_type)
        {
        default:
            olevel = 0;
            break;
        case ITEM_PILL:
            olevel = number_range(min, max);
            break;
        case ITEM_POTION:
            olevel = number_range(min, max);
            break;
        case ITEM_SCROLL:
            olevel = pObjIndex->value[0];
            break;
        case ITEM_WAND:
            olevel = number_range(min + 4, max + 1);
            break;
        case ITEM_STAFF:
            olevel = number_range(min + 9, max + 5);
            break;
        case ITEM_ARMOR:
            olevel = number_range(min + 4, max + 1);
            break;
        case ITEM_WEAPON:
            olevel = number_range(min + 4, max + 1);
            break;
        }
    return olevel;
}

/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list(RESET_DATA* pReset, OBJ_INDEX_DATA* pObjIndex, OBJ_DATA* list)
{
    OBJ_DATA* obj;
    int nMatch = 0;

    for (obj = list; obj; obj = obj->next_content)
    {
        if (obj->pIndexData == pObjIndex)
        {
            if (obj->count > 1)
                nMatch += obj->count;
            else
                nMatch++;
        }
    }
    return nMatch;
}

/*
 * Reset one room.
 */
void reset_room(ROOM_INDEX_DATA* room)
{
    RESET_DATA *pReset, *tReset, *gReset;
    OBJ_DATA* nestmap[MAX_NEST];
    CHAR_DATA* mob;
    OBJ_DATA *obj, *lastobj, *to_obj;
    ROOM_INDEX_DATA* pRoomIndex = nullptr;
    MOB_INDEX_DATA* pMobIndex = nullptr;
    OBJ_INDEX_DATA *pObjIndex = nullptr, *pObjToIndex;
    EXIT_DATA* pexit;
    char* filename = room->area->filename;
    int level = 0, n, num = 0, lastnest;

    mob = nullptr;
    obj = nullptr;
    lastobj = nullptr;
    if (!room->first_reset)
        return;
    level = 0;
    for (pReset = room->first_reset; pReset; pReset = pReset->next)
    {
        switch (pReset->command)
        {
        default:
            bug("%s: %s: bad command %c.", __FUNCTION__, filename, pReset->command);
            break;

        case 'M':
            if (!(pMobIndex = get_mob_index(pReset->arg1)))
            {
                bug("%s: %s: 'M': bad mob vnum %d.", __FUNCTION__, filename, pReset->arg1);
                continue;
            }
            if (!(pRoomIndex = get_room_index(pReset->arg3)))
            {
                bug("%s: %s: 'M': bad room vnum %d.", __FUNCTION__, filename, pReset->arg3);
                continue;
            }
            if (pMobIndex->count >= pReset->arg2)
            {
                mob = nullptr;
                break;
            }
            mob = create_mobile(pMobIndex);
            {
                ROOM_INDEX_DATA* pRoomPrev = get_room_index(pReset->arg3 - 1);

                if (pRoomPrev && IS_SET(pRoomPrev->room_flags, ROOM_PET_SHOP))
                    SET_BIT(mob->act, ACT_PET);
            }
            if (room_is_dark(pRoomIndex))
                SET_BIT(mob->affected_by, AFF_INFRARED);
            char_to_room(mob, pRoomIndex);
            level = URANGE(0, mob->top_level - 2, LEVEL_AVATAR);

            if (pReset->first_reset)
            {
                for (tReset = pReset->first_reset; tReset; tReset = tReset->next_reset)
                {
                    switch (tReset->command)
                    {
                    case 'G':
                    case 'E':
                        if (!(pObjIndex = get_obj_index(tReset->arg1)))
                        {
                            bug("%s: %s: 'E' or 'G': bad obj vnum %d.", __FUNCTION__, filename, tReset->arg1);
                            continue;
                        }
                        if (!mob)
                        {
                            lastobj = nullptr;
                            break;
                        }

                        if (mob->pIndexData->pShop)
                        {
                            int olevel = generate_itemlevel(room->area, pObjIndex);
                            obj = create_object(pObjIndex, olevel);
                            SET_BIT(obj->extra_flags, ITEM_INVENTORY);
                        }
                        else
                            obj = create_object(pObjIndex, number_fuzzy(level));
                        obj->level = URANGE(0, obj->level, LEVEL_AVATAR);
                        obj = obj_to_char(obj, mob);
                        if (tReset->command == 'E')
                        {
                            if (obj->carried_by != mob)
                            {
                                bug("'E' reset: can't give object %d to mob %d.", obj->pIndexData->vnum,
                                    mob->pIndexData->vnum);
                                break;
                            }
                            equip_char(mob, obj, tReset->arg3);
                        }
                        for (n = 0; n < MAX_NEST; n++)
                            nestmap[n] = nullptr;
                        nestmap[0] = obj;
                        lastobj = nestmap[0];
                        lastnest = 0;

                        if (tReset->first_reset)
                        {
                            for (gReset = tReset->first_reset; gReset; gReset = gReset->next_reset)
                            {
                                int iNest;
                                to_obj = lastobj;

                                switch (gReset->command)
                                {
                                case 'H':
                                    if (!lastobj)
                                        break;
                                    SET_BIT(lastobj->extra_flags, ITEM_HIDDEN);
                                    break;

                                case 'P':
                                    if (!(pObjIndex = get_obj_index(gReset->arg1)))
                                    {
                                        bug("%s: %s: 'P': bad obj vnum %d.", __FUNCTION__, filename, gReset->arg1);
                                        continue;
                                    }
                                    iNest = gReset->extra;

                                    if (!(pObjToIndex = get_obj_index(gReset->arg3)))
                                    {
                                        bug("%s: %s: 'P': bad objto vnum %d.", __FUNCTION__, filename, gReset->arg3);
                                        continue;
                                    }
                                    if (iNest >= MAX_NEST)
                                    {
                                        bug("%s: %s: 'P': Exceeded nesting limit of %d", __FUNCTION__, filename,
                                            MAX_NEST);
                                        obj = nullptr;
                                        break;
                                    }
                                    if (count_obj_list(gReset, pObjIndex, to_obj->first_content) > 0)
                                    {
                                        obj = nullptr;
                                        break;
                                    }

                                    if (iNest < lastnest)
                                        to_obj = nestmap[iNest];
                                    else if (iNest == lastnest)
                                        to_obj = nestmap[lastnest];
                                    else
                                        to_obj = lastobj;

                                    obj = create_object(
                                        pObjIndex,
                                        number_fuzzy(UMAX(generate_itemlevel(room->area, pObjIndex), to_obj->level)));
                                    if (num > 1)
                                        pObjIndex->count += (num - 1);
                                    obj->count = gReset->arg2;
                                    obj->level = UMIN(obj->level, LEVEL_AVATAR);
                                    obj->count = gReset->arg2;
                                    obj_to_obj(obj, to_obj);
                                    if (iNest > lastnest)
                                    {
                                        nestmap[iNest] = to_obj;
                                        lastnest = iNest;
                                    }
                                    lastobj = obj;
                                    // Hackish fix for nested puts
                                    if (gReset->arg3 == OBJ_VNUM_MONEY_ONE)
                                        gReset->arg3 = to_obj->pIndexData->vnum;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            break;

        case 'O':
            if (!(pObjIndex = get_obj_index(pReset->arg1)))
            {
                bug("%s: %s: 'O': bad obj vnum %d.", __FUNCTION__, filename, pReset->arg1);
                continue;
            }
            if (!(pRoomIndex = get_room_index(pReset->arg3)))
            {
                bug("%s: %s: 'O': bad room vnum %d.", __FUNCTION__, filename, pReset->arg3);
                continue;
            }
            /*
             * Rent item limits here
             */
            if (count_obj_list(pReset, pObjIndex, pRoomIndex->first_content) > 0)
            {
                obj = nullptr;
                lastobj = nullptr;
                break;
            }

            obj = create_object(pObjIndex, number_fuzzy(generate_itemlevel(room->area, pObjIndex)));
            if (num > 1)
                pObjIndex->count += (num - 1);
            obj->count = pReset->arg2;
            obj->level = UMIN(obj->level, LEVEL_AVATAR);
            obj->cost = 0;
            obj_to_room(obj, pRoomIndex);
            for (n = 0; n < MAX_NEST; n++)
                nestmap[n] = nullptr;
            nestmap[0] = obj;
            lastobj = nestmap[0];
            lastnest = 0;
            if (pReset->first_reset)
            {
                for (tReset = pReset->first_reset; tReset; tReset = tReset->next_reset)
                {
                    int iNest;

                    to_obj = lastobj;

                    switch (tReset->command)
                    {
                    case 'H':
                        if (!lastobj)
                            break;
                        SET_BIT(lastobj->extra_flags, ITEM_HIDDEN);
                        break;

                    case 'T':
                        if (!IS_SET(tReset->extra, TRAP_OBJ))
                        {
                            bug("%s: Room reset found on object reset list", __FUNCTION__);
                            break;
                        }
                        else
                        {
                            /*
                             * We need to preserve obj for future 'T' checks
                             */
                            OBJ_DATA* pobj;

                            if (tReset->arg3 > 0)
                            {
                                if (!(pObjToIndex = get_obj_index(tReset->arg3)))
                                {
                                    bug("%s: %s: 'T': bad objto vnum %d.", __FUNCTION__, filename, tReset->arg3);
                                    continue;
                                }
                                if (room->area->nplayer > 0 || !(to_obj = get_obj_type(pObjToIndex)) ||
                                    (to_obj->carried_by && !IS_NPC(to_obj->carried_by)) || is_trapped(to_obj))
                                    break;
                            }
                            else
                            {
                                if (!lastobj || !obj)
                                    break;
                                to_obj = obj;
                            }
                            pobj = make_trap(tReset->arg2, tReset->arg1, number_fuzzy(to_obj->level), tReset->extra);
                            obj_to_obj(pobj, to_obj);
                        }
                        break;

                    case 'P':
                        if (!(pObjIndex = get_obj_index(tReset->arg1)))
                        {
                            bug("%s: %s: 'P': bad obj vnum %d.", __FUNCTION__, filename, tReset->arg1);
                            continue;
                        }
                        iNest = tReset->extra;

                        if (!(pObjToIndex = get_obj_index(tReset->arg3)))
                        {
                            bug("%s: %s: 'P': bad objto vnum %d.", __FUNCTION__, filename, tReset->arg3);
                            continue;
                        }

                        if (iNest >= MAX_NEST)
                        {
                            bug("%s: %s: 'P': Exceeded nesting limit of %d. Room %d.", __FUNCTION__, filename, MAX_NEST,
                                room->vnum);
                            obj = nullptr;
                            break;
                        }

                        if (count_obj_list(tReset, pObjIndex, to_obj->first_content) > 0)
                        {
                            obj = nullptr;
                            break;
                        }
                        if (iNest < lastnest)
                            to_obj = nestmap[iNest];
                        else if (iNest == lastnest)
                            to_obj = nestmap[lastnest];
                        else
                            to_obj = lastobj;

                        obj = create_object(
                            pObjIndex, number_fuzzy(UMAX(generate_itemlevel(room->area, pObjIndex), to_obj->level)));
                        if (num > 1)
                            pObjIndex->count += (num - 1);
                        obj->count = tReset->arg2;
                        obj->level = UMIN(obj->level, LEVEL_AVATAR);
                        obj->count = tReset->arg2;
                        obj_to_obj(obj, to_obj);
                        if (iNest > lastnest)
                        {
                            nestmap[iNest] = to_obj;
                            lastnest = iNest;
                        }
                        lastobj = obj;
                        // Hackish fix for nested puts
                        if (tReset->arg3 == OBJ_VNUM_MONEY_ONE)
                            tReset->arg3 = to_obj->pIndexData->vnum;
                        break;
                    }
                }
            }
            break;

        case 'T':
            if (IS_SET(pReset->extra, TRAP_OBJ))
            {
                bug("%s: Object trap found in room %d reset list", __FUNCTION__, room->vnum);
                break;
            }
            else
            {
                if (!(pRoomIndex = get_room_index(pReset->arg3)))
                {
                    bug("%s: %s: 'T': bad room %d.", __FUNCTION__, filename, pReset->arg3);
                    continue;
                }
                if (room->area->nplayer > 0 ||
                    count_obj_list(pReset, get_obj_index(OBJ_VNUM_TRAP), pRoomIndex->first_content) > 0)
                    break;
                to_obj = make_trap(pReset->arg1, pReset->arg1, 10, pReset->extra);
                obj_to_room(to_obj, pRoomIndex);
            }
            break;

        case 'D':
            if (!(pRoomIndex = get_room_index(pReset->arg1)))
            {
                bug("%s: %s: 'D': bad room vnum %d.", __FUNCTION__, filename, pReset->arg1);
                continue;
            }
            if (!(pexit = get_exit(pRoomIndex, pReset->arg2)))
                break;
            switch (pReset->arg3)
            {
            case 0:
                REMOVE_BIT(pexit->exit_info, EX_CLOSED);
                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                break;
            case 1:
                SET_BIT(pexit->exit_info, EX_CLOSED);
                REMOVE_BIT(pexit->exit_info, EX_LOCKED);
                if (IS_SET(pexit->exit_info, EX_xSEARCHABLE))
                    SET_BIT(pexit->exit_info, EX_SECRET);
                break;
            case 2:
                SET_BIT(pexit->exit_info, EX_CLOSED);
                SET_BIT(pexit->exit_info, EX_LOCKED);
                if (IS_SET(pexit->exit_info, EX_xSEARCHABLE))
                    SET_BIT(pexit->exit_info, EX_SECRET);
                break;
            }
            break;

        case 'R':
            if (!(pRoomIndex = get_room_index(pReset->arg1)))
            {
                bug("%s: %s: 'R': bad room vnum %d.", __FUNCTION__, filename, pReset->arg1);
                continue;
            }
            randomize_exits(pRoomIndex, pReset->arg2 - 1);
            break;
        }
    }
    return;
}

void reset_area(AREA_DATA* area)
{
    ROOM_INDEX_DATA* room;

    if (!area->first_room)
        return;

    for (room = area->first_room; room; room = room->next_aroom)
        reset_room(room);
}

/* Setup put nesting levels, regardless of whether or not the resets will
   actually reset, or if they're bugged. */
void renumber_put_resets(ROOM_INDEX_DATA* room)
{
    RESET_DATA *pReset, *tReset, *lastobj = nullptr;

    for (pReset = room->first_reset; pReset; pReset = pReset->next)
    {
        switch (pReset->command)
        {
        default:
            break;

        case 'O':
            lastobj = pReset;
            for (tReset = pReset->first_reset; tReset; tReset = tReset->next_reset)
            {
                switch (tReset->command)
                {
                case 'P':
                    if (tReset->arg3 == 0)
                    {
                        if (!lastobj)
                            tReset->extra = 1000000;
                        else if (lastobj->command != 'P' || lastobj->arg3 > 0)
                            tReset->extra = 0;
                        else
                            tReset->extra = lastobj->extra + 1;
                        lastobj = tReset;
                    }
                    break;
                }
            }
            break;
        }
    }
    return;
}

/*
 * Add a reset to an area -Thoric
 */
RESET_DATA* add_reset(ROOM_INDEX_DATA* room, char letter, int extra, int arg1, int arg2, int arg3)
{
    RESET_DATA* pReset;

    if (!room)
    {
        bug("%s: nullptr room!", __FUNCTION__);
        return nullptr;
    }

    letter = UPPER(letter);
    pReset = make_reset(letter, extra, arg1, arg2, arg3);
    switch (letter)
    {
    case 'M':
        room->last_mob_reset = pReset;
        break;

    case 'E':
    case 'G':
        if (!room->last_mob_reset)
        {
            bug("%s: Can't add '%c' reset to room: last_mob_reset is nullptr.", __FUNCTION__, letter);
            return nullptr;
        }
        room->last_obj_reset = pReset;
        LINK(pReset, room->last_mob_reset->first_reset, room->last_mob_reset->last_reset, next_reset, prev_reset);
        return pReset;

    case 'P':
        if (!room->last_obj_reset)
        {
            bug("%s: Can't add '%c' reset to room: last_obj_reset is nullptr.", __FUNCTION__, letter);
            return nullptr;
        }
        LINK(pReset, room->last_obj_reset->first_reset, room->last_obj_reset->last_reset, next_reset, prev_reset);
        return pReset;

    case 'O':
        room->last_obj_reset = pReset;
        break;

    case 'T':
        if (IS_SET(extra, TRAP_OBJ))
        {
            pReset->prev_reset = nullptr;
            pReset->next_reset = room->last_obj_reset->first_reset;
            if (room->last_obj_reset->first_reset)
                room->last_obj_reset->first_reset->prev_reset = pReset;
            room->last_obj_reset->first_reset = pReset;
            if (!room->last_obj_reset->last_reset)
                room->last_obj_reset->last_reset = pReset;
            return pReset;
        }
        break;

    case 'H':
        pReset->prev_reset = nullptr;
        pReset->next_reset = room->last_obj_reset->first_reset;
        if (room->last_obj_reset->first_reset)
            room->last_obj_reset->first_reset->prev_reset = pReset;
        room->last_obj_reset->first_reset = pReset;
        if (!room->last_obj_reset->last_reset)
            room->last_obj_reset->last_reset = pReset;
        return pReset;
    }
    LINK(pReset, room->first_reset, room->last_reset, next, prev);
    return pReset;
}

RESET_DATA* find_oreset(ROOM_INDEX_DATA* room, char* oname)
{
    RESET_DATA* pReset;
    OBJ_INDEX_DATA* pobj;
    char arg[MAX_INPUT_LENGTH];
    int cnt = 0, num = number_argument(oname, arg);

    for (pReset = room->first_reset; pReset; pReset = pReset->next)
    {
        // Only going to allow traps/hides on room reset objects. Unless someone can come up with a better way to do
        // this.
        if (pReset->command != 'O')
            continue;

        if (!(pobj = get_obj_index(pReset->arg1)))
            continue;

        if (is_name(arg, pobj->name) && ++cnt == num)
            return pReset;
    }
    return nullptr;
}

void do_reset(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];

    if (!argument || argument[0] == '\0')
    {
        send_to_char("Usage: reset area\n\r", ch);
        send_to_char("Usage: reset randomize <direction>\n\r", ch);
        send_to_char("Usage: reset delete <number>\n\r", ch);
        send_to_char("Usage: reset hide <objname>\n\r", ch);
        send_to_char("Usage: reset trap room <type> <charges> [flags]\n\r", ch);
        send_to_char("Usage: reset trap obj <name> <type> <charges> [flags]\n\r", ch);
        return;
    }

    argument = one_argument(argument, arg);
    if (!str_cmp(arg, "area"))
    {
        reset_area(ch->in_room->area);
        send_to_char("Area has been reset.\n\r", ch);
        return;
    }

    // Yeah, I know, this function is mucho ugly... but...
    if (!str_cmp(arg, "delete"))
    {
        RESET_DATA *pReset, *tReset, *pReset_next, *tReset_next, *gReset, *gReset_next;
        int num, nfind = 0;

        if (!argument || argument[0] == '\0')
        {
            send_to_char("You must specify a reset # in this room to delete one.\n\r", ch);
            return;
        }

        if (!is_number(argument))
        {
            send_to_char("Specified reset must be designated by number. See &Wredit rlist&D.\n\r", ch);
            return;
        }
        num = atoi(argument);

        for (pReset = ch->in_room->first_reset; pReset; pReset = pReset_next)
        {
            pReset_next = pReset->next;

            nfind++;
            if (nfind == num)
            {
                UNLINK(pReset, ch->in_room->first_reset, ch->in_room->last_reset, next, prev);
                delete_reset(pReset);
                send_to_char("Reset deleted.\n\r", ch);
                return;
            }

            for (tReset = pReset->first_reset; tReset; tReset = tReset_next)
            {
                tReset_next = tReset->next_reset;

                nfind++;
                if (nfind == num)
                {
                    UNLINK(tReset, pReset->first_reset, pReset->last_reset, next_reset, prev_reset);
                    delete_reset(tReset);
                    send_to_char("Reset deleted.\n\r", ch);
                    return;
                }

                for (gReset = tReset->first_reset; gReset; gReset = gReset_next)
                {
                    gReset_next = gReset->next_reset;

                    nfind++;
                    if (nfind == num)
                    {
                        UNLINK(gReset, tReset->first_reset, tReset->last_reset, next_reset, prev_reset);
                        delete_reset(gReset);
                        send_to_char("Reset deleted.\n\r", ch);
                        return;
                    }
                }
            }
        }
        send_to_char("No reset matching that number was found in this room.\n\r", ch);
        return;
    }

    if (!str_cmp(arg, "random"))
    {
        RESET_DATA* pReset;
        int vnum = get_dir(arg);
        argument = one_argument(argument, arg);

        if (vnum < 0 || vnum > 9)
        {
            send_to_char("Reset which random doors?\n\r", ch);
            return;
        }

        if (vnum == 0)
        {
            send_to_char("There is no point in randomizing one door.\n\r", ch);
            return;
        }

        if (!get_room_index(vnum))
        {
            send_to_char("Target room does not exist.\n\r", ch);
            return;
        }

        pReset = make_reset('R', 0, ch->in_room->vnum, vnum, 0);
        pReset->prev = nullptr;
        pReset->next = ch->in_room->first_reset;
        if (ch->in_room->first_reset->prev)
            ch->in_room->first_reset->prev = pReset;
        ch->in_room->first_reset = pReset;
        if (!ch->in_room->last_reset)
            ch->in_room->last_reset = pReset;
        send_to_char("Reset random doors created.\n\r", ch);
        return;
    }

    if (!str_cmp(arg, "trap"))
    {
        RESET_DATA *pReset = nullptr, *tReset;
        char oname[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
        int num, chrg, value, extra = 0, vnum;

        argument = one_argument(argument, arg2);

        if (!str_cmp(arg2, "room"))
        {
            vnum = ch->in_room->vnum;
            extra = TRAP_ROOM;

            argument = one_argument(argument, arg);
            num = is_number(arg) ? atoi(arg) : -1;
            argument = one_argument(argument, arg);
            chrg = is_number(arg) ? atoi(arg) : -1;
        }
        else if (!str_cmp(arg2, "obj"))
        {
            argument = one_argument(argument, oname);
            if (!(pReset = find_oreset(ch->in_room, oname)))
            {
                send_to_char("No matching reset found to set a trap on.\n\r", ch);
                return;
            }
            vnum = 0;
            extra = TRAP_OBJ;

            argument = one_argument(argument, arg);
            num = is_number(arg) ? atoi(arg) : -1;
            argument = one_argument(argument, arg);
            chrg = is_number(arg) ? atoi(arg) : -1;
        }
        else
        {
            send_to_char("Trap reset must be on 'room' or 'obj'\n\r", ch);
            return;
        }

        if (num < 1 || num > MAX_TRAPTYPE)
        {
            send_to_char("Invalid trap type.\n\r", ch);
            return;
        }

        if (chrg < 0 || chrg > 10000)
        {
            send_to_char("Invalid trap charges. Must be between 1 and 10000.\n\r", ch);
            return;
        }

        while (*argument)
        {
            argument = one_argument(argument, arg);
            value = get_trapflag(arg);
            if (value < 0 || value > 31)
            {
                ch_printf(ch, "Bad trap flag: %s\n\r", arg);
                continue;
            }
            SET_BIT(extra, 1 << value);
        }
        tReset = make_reset('T', extra, num, chrg, vnum);
        if (pReset)
        {
            tReset->prev_reset = nullptr;
            tReset->next_reset = pReset->first_reset;
            if (pReset->first_reset->prev_reset)
                pReset->first_reset->prev_reset = tReset;
            pReset->first_reset = tReset;
            if (!pReset->last_reset)
                pReset->last_reset = tReset;
        }
        else
        {
            tReset->prev = nullptr;
            tReset->next = ch->in_room->first_reset;
            if (ch->in_room->first_reset->prev)
                ch->in_room->first_reset->prev = tReset;
            ch->in_room->first_reset = tReset;
            if (!ch->in_room->last_reset)
                ch->in_room->last_reset = tReset;
        }
        send_to_char("Trap created.\n\r", ch);
        return;
    }

    if (!str_cmp(arg, "hide"))
    {
        RESET_DATA *pReset = nullptr, *tReset;

        if (!(pReset = find_oreset(ch->in_room, argument)))
        {
            send_to_char("No such object to hide in this room.\n\r", ch);
            return;
        }
        tReset = make_reset('H', 1, 0, 0, 0);
        if (pReset)
        {
            tReset->prev_reset = nullptr;
            tReset->next_reset = pReset->first_reset;
            if (pReset->first_reset->prev_reset)
                pReset->first_reset->prev_reset = tReset;
            pReset->first_reset = tReset;
            if (!pReset->last_reset)
                pReset->last_reset = tReset;
        }
        else
        {
            tReset->prev = nullptr;
            tReset->next = ch->in_room->first_reset;
            if (ch->in_room->first_reset->prev)
                ch->in_room->first_reset->prev = tReset;
            ch->in_room->first_reset = tReset;
            if (!ch->in_room->last_reset)
                ch->in_room->last_reset = tReset;
        }
        send_to_char("Hide reset created.\n\r", ch);
        return;
    }
    do_reset(ch, MAKE_TEMP_STRING(""));
    return;
}
