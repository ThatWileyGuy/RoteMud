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
#include "bet.hxx"

/*double sqrt( double x );*/

/*
 * External functions
 */
void write_corpses(CHAR_DATA* ch, char* name);
void show_list_to_char(OBJ_DATA* list, CHAR_DATA* ch, bool fShort, bool fShowNothing);
/*
 * Local functions.
 */
void get_obj(CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* container);
bool remove_obj(CHAR_DATA* ch, int iWear, bool fReplace);
void wear_obj(CHAR_DATA* ch, OBJ_DATA* obj, bool fReplace, sh_int wear_bit);

/*
 * how resistant an object is to damage				-Thoric
 */
sh_int get_obj_resistance(OBJ_DATA* obj)
{
    sh_int resist;

    resist = number_fuzzy(MAX_ITEM_IMPACT);

    /* magical items are more resistant */
    if (IS_OBJ_STAT(obj, ITEM_MAGIC))
        resist += number_fuzzy(12);
    /* blessed objects should have a little bonus */
    if (IS_OBJ_STAT(obj, ITEM_BLESS))
        resist += number_fuzzy(5);
    /* lets make store inventory pretty tough */
    if (IS_OBJ_STAT(obj, ITEM_INVENTORY))
        resist += 20;

    /* okay... let's add some bonus/penalty for item level... */
    resist += (obj->level / 10);

    /* and lasty... take armor or weapon's condition into consideration */
    if (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_WEAPON)
        resist += (obj->value[0]);

    return URANGE(10, resist, 99);
}

void get_obj(CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* container)
{
    CLAN_DATA* clan;
    int weight;

    if (!CAN_WEAR(obj, ITEM_TAKE) && (ch->top_level < sysdata.level_getobjnotake))
    {
        send_to_char("You can't take that.\n\r", ch);
        return;
    }

    if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE) && !can_take_proto(ch))
    {
        send_to_char("A godly force prevents you from getting close to it.\n\r", ch);
        return;
    }
    if (obj->item_type != ITEM_MONEY)
    {
        if ((ch->carry_number + get_obj_number(obj) > can_carry_n(ch)))
        {
            act(AT_PLAIN, "$d: you can't carry that many items.", ch, nullptr, obj->name, TO_CHAR);
            return;
        }
    }

    if (IS_OBJ_STAT(obj, ITEM_COVERING))
        weight = obj->weight;
    else
        weight = get_obj_weight(obj);

    if (ch->carry_weight + weight > can_carry_w(ch))
    {
        act(AT_PLAIN, "$d: you can't carry that much weight.", ch, nullptr, obj->name, TO_CHAR);
        return;
    }

    if (container)
    {
        act(AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? "You get $p from beneath $P." : "You get $p from $P", ch,
            obj, container, TO_CHAR);
        act(AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? "$n gets $p from beneath $P." : "$n gets $p from $P", ch,
            obj, container, TO_ROOM);
        obj_from_obj(obj);
    }
    else
    {
        act(AT_ACTION, "You get $p.", ch, obj, container, TO_CHAR);
        act(AT_ACTION, "$n gets $p.", ch, obj, container, TO_ROOM);
        obj_from_room(obj);
    }

    /* Clan storeroom checks */
    if (IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) && (!container || container->carried_by == nullptr))
        for (clan = first_clan; clan; clan = clan->next)
            if (clan->storeroom == ch->in_room->vnum)
                save_clan_storeroom(ch, clan);

    if (obj->item_type != ITEM_CONTAINER)
        check_for_trap(ch, obj, TRAP_GET);
    if (char_died(ch))
        return;

    if (obj->item_type == ITEM_MONEY)
    {
        ch->gold += obj->value[0];
        extract_obj(obj);
    }
    else
    {
        obj = obj_to_char(obj, ch);
    }

    if (char_died(ch) || obj_extracted(obj))
        return;
    oprog_get_trigger(ch, obj);
    return;
}

void do_get(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA* obj;
    OBJ_DATA* obj_next;
    OBJ_DATA* container;
    sh_int number;
    bool found;

    argument = one_argument(argument, arg1);
    if (is_number(arg1))
    {
        number = atoi(arg1);
        if (number < 1)
        {
            send_to_char("That was easy...\n\r", ch);
            return;
        }
        if ((ch->carry_number + number) > can_carry_n(ch))
        {
            send_to_char("You can't carry that many.\n\r", ch);
            return;
        }
        argument = one_argument(argument, arg1);
    }
    else
        number = 0;
    argument = one_argument(argument, arg2);
    /* munch optional words */
    if (!str_cmp(arg2, "from") && argument[0] != '\0')
        argument = one_argument(argument, arg2);

    /* Get type. */
    if (arg1[0] == '\0')
    {
        send_to_char("Get what?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (arg2[0] == '\0')
    {
        if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
        {
            /* 'get obj' */
            obj = get_obj_list(ch, arg1, ch->in_room->first_content);
            if (!obj)
            {
                act(AT_PLAIN, "I see no $T here.", ch, nullptr, arg1, TO_CHAR);
                return;
            }
            separate_obj(obj);
            get_obj(ch, obj, nullptr);
            if (char_died(ch))
                return;
            if (IS_SET(sysdata.save_flags, SV_GET))
                save_char_obj(ch);
        }
        else
        {
            sh_int cnt = 0;
            bool fAll;
            char* chk;

            if (IS_SET(ch->in_room->room_flags, ROOM_DONATION))
            {
                send_to_char("The gods frown upon such a display of greed!\n\r", ch);
                return;
            }
            if (!str_cmp(arg1, "all"))
                fAll = true;
            else
                fAll = false;
            if (number > 1)
                chk = arg1;
            else
                chk = &arg1[4];
            /* 'get all' or 'get all.obj' */
            found = false;
            for (obj = ch->in_room->first_content; obj; obj = obj_next)
            {
                obj_next = obj->next_content;
                if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj))
                {
                    found = true;
                    if (number && (cnt + obj->count) > number)
                        split_obj(obj, number - cnt);
                    cnt += obj->count;
                    get_obj(ch, obj, nullptr);
                    if (char_died(ch) || ch->carry_number >= can_carry_n(ch) || ch->carry_weight >= can_carry_w(ch) ||
                        (number && cnt >= number))
                    {
                        if (IS_SET(sysdata.save_flags, SV_GET) && !char_died(ch))
                            save_char_obj(ch);
                        return;
                    }
                }
            }

            if (!found)
            {
                if (fAll)
                    send_to_char("I see nothing here.\n\r", ch);
                else
                    act(AT_PLAIN, "I see no $T here.", ch, nullptr, chk, TO_CHAR);
            }
            else if (IS_SET(sysdata.save_flags, SV_GET))
                save_char_obj(ch);
        }
    }
    else
    {
        /* 'get ... container' */
        if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
        {
            send_to_char("You can't do that.\n\r", ch);
            return;
        }

        if ((container = get_obj_here(ch, arg2)) == nullptr)
        {
            act(AT_PLAIN, "I see no $T here.", ch, nullptr, arg2, TO_CHAR);
            return;
        }

        switch (container->item_type)
        {
        default:
            if (!IS_OBJ_STAT(container, ITEM_COVERING))
            {
                send_to_char("That's not a container.\n\r", ch);
                return;
            }
            if (ch->carry_weight + container->weight > can_carry_w(ch))
            {
                send_to_char("It's too heavy for you to lift.\n\r", ch);
                return;
            }
            break;

        case ITEM_CONTAINER:
        case ITEM_DROID_CORPSE:
        case ITEM_CORPSE_PC:
        case ITEM_CORPSE_NPC:
            break;
        }

        if (!IS_OBJ_STAT(container, ITEM_COVERING) && IS_SET(container->value[1], CONT_CLOSED))
        {
            act(AT_PLAIN, "The $d is closed.", ch, nullptr, container->name, TO_CHAR);
            return;
        }

        if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
        {
            /* 'get obj container' */
            obj = get_obj_list(ch, arg1, container->first_content);
            if (!obj)
            {
                act(AT_PLAIN,
                    IS_OBJ_STAT(container, ITEM_COVERING) ? "I see nothing like that beneath the $T."
                                                          : "I see nothing like that in the $T.",
                    ch, nullptr, arg2, TO_CHAR);
                return;
            }
            separate_obj(obj);
            get_obj(ch, obj, container);

            check_for_trap(ch, container, TRAP_GET);
            if (char_died(ch))
                return;
            if (IS_SET(sysdata.save_flags, SV_GET))
                save_char_obj(ch);
        }
        else
        {
            int cnt = 0;
            bool fAll;
            char* chk;

            /* 'get all container' or 'get all.obj container' */
            if (IS_OBJ_STAT(container, ITEM_DONATION))
            {
                send_to_char("The gods frown upon such an act of greed!\n\r", ch);
                return;
            }
            if (!str_cmp(arg1, "all"))
                fAll = true;
            else
                fAll = false;
            if (number > 1)
                chk = arg1;
            else
                chk = &arg1[4];
            found = false;
            for (obj = container->first_content; obj; obj = obj_next)
            {
                obj_next = obj->next_content;
                if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj))
                {
                    found = true;
                    if (number && (cnt + obj->count) > number)
                        split_obj(obj, number - cnt);
                    cnt += obj->count;
                    get_obj(ch, obj, container);
                    if (char_died(ch) || ch->carry_number >= can_carry_n(ch) || ch->carry_weight >= can_carry_w(ch) ||
                        (number && cnt >= number))
                    {
                        if (container->item_type == ITEM_CORPSE_PC)
                            write_corpses(nullptr, container->short_descr + 14);
                        if (found && IS_SET(sysdata.save_flags, SV_GET))
                            save_char_obj(ch);
                        return;
                    }
                }
            }

            if (!found)
            {
                if (fAll)
                    act(AT_PLAIN,
                        IS_OBJ_STAT(container, ITEM_COVERING) ? "I see nothing beneath the $T."
                                                              : "I see nothing in the $T.",
                        ch, nullptr, arg2, TO_CHAR);
                else
                    act(AT_PLAIN,
                        IS_OBJ_STAT(container, ITEM_COVERING) ? "I see nothing like that beneath the $T."
                                                              : "I see nothing like that in the $T.",
                        ch, nullptr, arg2, TO_CHAR);
            }
            else
                check_for_trap(ch, container, TRAP_GET);
            if (char_died(ch))
                return;
            if (found && IS_SET(sysdata.save_flags, SV_GET))
                save_char_obj(ch);
        }
    }
    return;
}

void do_put(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA* container;
    OBJ_DATA* obj;
    OBJ_DATA* obj_next;
    CLAN_DATA* clan;
    sh_int count;
    int number;
    bool save_char = false;

    argument = one_argument(argument, arg1);
    if (is_number(arg1))
    {
        number = atoi(arg1);
        if (number < 1)
        {
            send_to_char("That was easy...\n\r", ch);
            return;
        }
        argument = one_argument(argument, arg1);
    }
    else
        number = 0;
    argument = one_argument(argument, arg2);
    /* munch optional words */
    if ((!str_cmp(arg2, "into") || !str_cmp(arg2, "inside") || !str_cmp(arg2, "in")) && argument[0] != '\0')
        argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Put what in what?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
    {
        send_to_char("You can't do that.\n\r", ch);
        return;
    }

    if ((container = get_obj_here(ch, arg2)) == nullptr)
    {
        act(AT_PLAIN, "I see no $T here.", ch, nullptr, arg2, TO_CHAR);
        return;
    }

    if (!container->carried_by && IS_SET(sysdata.save_flags, SV_PUT))
        save_char = true;

    if (IS_OBJ_STAT(container, ITEM_COVERING))
    {
        if (ch->carry_weight + container->weight > can_carry_w(ch))
        {
            send_to_char("It's too heavy for you to lift.\n\r", ch);
            return;
        }
    }
    else
    {
        if (container->item_type != ITEM_CONTAINER)
        {
            send_to_char("That's not a container.\n\r", ch);
            return;
        }

        if (IS_SET(container->value[1], CONT_CLOSED))
        {
            act(AT_PLAIN, "The $d is closed.", ch, nullptr, container->name, TO_CHAR);
            return;
        }
    }

    if (number > 0)
    {
        /* 'put NNNN coins object' */

        if (!str_cmp(arg1, "credits") || !str_cmp(arg1, "credit"))
        {
            if (ch->gold < number)
            {
                send_to_char("You haven't got that many credits.\n\r", ch);
                return;
            }

            if (!IS_NPC(ch) && ch->top_level < 11)
            {
                send_to_char("Due to cheating, players under level 11 are not allowed to move credits.\n\r", ch);
                return;
            }

            ch->gold -= number;

            for (obj = container->first_content; obj; obj = obj_next)
            {
                obj_next = obj->next_content;

                switch (obj->pIndexData->vnum)
                {
                case OBJ_VNUM_MONEY_ONE:
                    number += 1;
                    extract_obj(obj);
                    break;

                case OBJ_VNUM_MONEY_SOME:
                    number += obj->value[0];
                    extract_obj(obj);
                    break;
                }
            }

            act(AT_ACTION, "$n puts some credits in $P.", ch, nullptr, container, TO_ROOM);
            obj_to_obj(create_money(number), container);
            send_to_char("OK.\n\r", ch);
            if (IS_SET(sysdata.save_flags, SV_DROP))
                save_char_obj(ch);
            return;
        }
    }

    if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
    {
        /* 'put obj container' */
        if ((obj = get_obj_carry(ch, arg1)) == nullptr)
        {
            send_to_char("You do not have that item.\n\r", ch);
            return;
        }

        if (obj == container)
        {
            send_to_char("You can't fold it into itself.\n\r", ch);
            return;
        }

        if (!can_drop_obj(ch, obj))
        {
            send_to_char("You can't let go of it.\n\r", ch);
            return;
        }

        if ((IS_OBJ_STAT(container, ITEM_COVERING) &&
             (get_obj_weight(obj) / obj->count) > ((get_obj_weight(container) / container->count) - container->weight)))
        {
            send_to_char("It won't fit under there.\n\r", ch);
            return;
        }

        if ((get_obj_weight(obj) / obj->count) + (get_obj_weight(container) / container->count) > container->value[0])
        {
            send_to_char("It won't fit.\n\r", ch);
            return;
        }

        if (obj->item_type == ITEM_GRENADE && obj->timer > 0)
        {
            send_to_char("Put an armed grenade in a bag? This ain't acme, kid.\n\r", ch);
            return;
        }

        separate_obj(obj);
        separate_obj(container);
        obj_from_char(obj);
        obj = obj_to_obj(obj, container);
        check_for_trap(ch, container, TRAP_PUT);
        if (char_died(ch))
            return;
        count = obj->count;
        obj->count = 1;
        if (!oprog_use_trigger(ch, container, nullptr, nullptr, nullptr))
        {
            act(AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? "$n hides $p beneath $P." : "$n puts $p in $P.", ch,
                obj, container, TO_ROOM);
            act(AT_ACTION, IS_OBJ_STAT(container, ITEM_COVERING) ? "You hide $p beneath $P." : "You put $p in $P.", ch,
                obj, container, TO_CHAR);
        }
        obj->count = count;

        if (save_char)
            save_char_obj(ch);
        /* Clan storeroom check */
        if (IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) && container->carried_by == nullptr)
            for (clan = first_clan; clan; clan = clan->next)
                if (clan->storeroom == ch->in_room->vnum)
                    save_clan_storeroom(ch, clan);
    }
    else
    {
        bool found = false;
        int cnt = 0;
        bool fAll;
        char* chk;

        if (!str_cmp(arg1, "all"))
            fAll = true;
        else
            fAll = false;
        if (number > 1)
            chk = arg1;
        else
            chk = &arg1[4];

        if (container->pIndexData->vnum == 1097 && fAll)
        {
            send_to_char("You can't put everything into the trash! Do it one at a time!\n\r", ch);
            return;
        }

        separate_obj(container);
        /* 'put all container' or 'put all.obj container' */
        for (obj = ch->first_carrying; obj; obj = obj_next)
        {
            obj_next = obj->next_content;

            if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE &&
                obj != container && can_drop_obj(ch, obj) &&
                get_obj_weight(obj) + get_obj_weight(container) <= container->value[0])
            {
                if (number && (cnt + obj->count) > number)
                    split_obj(obj, number - cnt);
                cnt += obj->count;
                obj_from_char(obj);
                if (!oprog_use_trigger(ch, container, nullptr, nullptr, nullptr))
                {
                    act(AT_ACTION, "$n puts $p in $P.", ch, obj, container, TO_ROOM);
                    act(AT_ACTION, "You put $p in $P.", ch, obj, container, TO_CHAR);
                }
                obj = obj_to_obj(obj, container);
                found = true;

                check_for_trap(ch, container, TRAP_PUT);
                if (char_died(ch))
                    return;
                if (number && cnt >= number)
                    break;
            }
        }

        /*
         * Don't bother to save anything if nothing was dropped   -Thoric
         */
        if (!found)
        {
            if (fAll)
                act(AT_PLAIN, "You are not carrying anything.", ch, nullptr, nullptr, TO_CHAR);
            else
                act(AT_PLAIN, "You are not carrying any $T.", ch, nullptr, chk, TO_CHAR);
            return;
        }

        if (save_char)
            save_char_obj(ch);
        /* Clan storeroom check */
        if (IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM) && container->carried_by == nullptr)
            for (clan = first_clan; clan; clan = clan->next)
                if (clan->storeroom == ch->in_room->vnum)
                    save_clan_storeroom(ch, clan);
    }

    return;
}

void do_drop(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    OBJ_DATA* obj;
    OBJ_DATA* obj_next;
    bool found;
    CLAN_DATA* clan;
    int number;

    argument = one_argument(argument, arg);
    if (is_number(arg))
    {
        number = atoi(arg);
        if (number < 1)
        {
            send_to_char("That was easy...\n\r", ch);
            return;
        }
        argument = one_argument(argument, arg);
    }
    else
        number = 0;

    if (arg[0] == '\0')
    {
        send_to_char("Drop what?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (IS_SET(ch->in_room->room_flags, ROOM_NODROP) || (!IS_NPC(ch) && IS_SET(ch->act, PLR_LITTERBUG)))
    {
        set_char_color(AT_MAGIC, ch);
        send_to_char("A magical force stops you!\n\r", ch);
        set_char_color(AT_TELL, ch);
        send_to_char("Someone tells you, 'No littering here!'\n\r", ch);
        return;
    }

    if (number > 0)
    {
        /* 'drop NNNN coins' */

        if (!str_cmp(arg, "credits") || !str_cmp(arg, "credit"))
        {
            if (ch->gold < number)
            {
                send_to_char("You haven't got that many credits.\n\r", ch);
                return;
            }

            if (!IS_NPC(ch) && ch->top_level < 11)
            {
                send_to_char("Due to cheating, players under level 11 are not allowed to move credits.\n\r", ch);
                return;
            }

            ch->gold -= number;

            for (obj = ch->in_room->first_content; obj; obj = obj_next)
            {
                obj_next = obj->next_content;

                switch (obj->pIndexData->vnum)
                {
                case OBJ_VNUM_MONEY_ONE:
                    number += 1;
                    extract_obj(obj);
                    break;

                case OBJ_VNUM_MONEY_SOME:
                    number += obj->value[0];
                    extract_obj(obj);
                    break;
                }
            }

            act(AT_ACTION, "$n drops some credits.", ch, nullptr, nullptr, TO_ROOM);
            obj_to_room(create_money(number), ch->in_room);
            send_to_char("OK.\n\r", ch);
            if (!IS_NPC(ch) && IS_IMMORTAL(ch))
            {
                sprintf_s(logbuf, "%s dropped %d credits.", ch->name, number);
                log_string(logbuf);
            }
            if (IS_SET(sysdata.save_flags, SV_DROP))
                save_char_obj(ch);
            return;
        }
    }

    if (number <= 1 && str_cmp(arg, "all") && str_prefix("all.", arg))
    {
        /* 'drop obj' */
        if ((obj = get_obj_carry(ch, arg)) == nullptr)
        {
            send_to_char("You do not have that item.\n\r", ch);
            return;
        }

        if (!can_drop_obj(ch, obj))
        {
            send_to_char("You can't let go of it.\n\r", ch);
            return;
        }

        separate_obj(obj);
        act(AT_ACTION, "$n drops $p.", ch, obj, nullptr, TO_ROOM);
        act(AT_ACTION, "You drop $p.", ch, obj, nullptr, TO_CHAR);
        if (!IS_NPC(ch) && IS_IMMORTAL(ch))
        {
            sprintf_s(logbuf, "%s dropped %s.", ch->name, obj->short_descr);
            log_string(logbuf);
        }

        obj_from_char(obj);
        obj = obj_to_room(obj, ch->in_room);
        oprog_drop_trigger(ch, obj); /* mudprogs */

        if (char_died(ch) || obj_extracted(obj))
            return;

        /* Clan storeroom saving */
        if (IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM))
            for (clan = first_clan; clan; clan = clan->next)
                if (clan->storeroom == ch->in_room->vnum)
                    save_clan_storeroom(ch, clan);
    }
    else
    {
        int cnt = 0;
        char* chk;
        bool fAll;

        if (!str_cmp(arg, "all"))
            fAll = true;
        else
            fAll = false;
        if (number > 1)
            chk = arg;
        else
            chk = &arg[4];
        /* 'drop all' or 'drop all.obj' */
        if (IS_SET(ch->in_room->room_flags, ROOM_NODROPALL))
        {
            send_to_char("You can't seem to do that here...\n\r", ch);
            return;
        }
        found = false;
        for (obj = ch->first_carrying; obj; obj = obj_next)
        {
            obj_next = obj->next_content;

            if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE &&
                can_drop_obj(ch, obj))
            {
                found = true;
                if (obj->pIndexData->progtypes & DROP_PROG && obj->count > 1)
                {
                    ++cnt;
                    separate_obj(obj);
                    obj_from_char(obj);
                    if (!obj_next)
                        obj_next = ch->first_carrying;
                }
                else
                {
                    if (number && (cnt + obj->count) > number)
                        split_obj(obj, number - cnt);
                    cnt += obj->count;
                    obj_from_char(obj);
                }
                act(AT_ACTION, "$n drops $p.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You drop $p.", ch, obj, nullptr, TO_CHAR);
                obj = obj_to_room(obj, ch->in_room);
                if (!IS_NPC(ch) && IS_IMMORTAL(ch))
                {
                    sprintf_s(logbuf, "%s dropped %s.", ch->name, obj->short_descr);
                    log_string(logbuf);
                }
                oprog_drop_trigger(ch, obj); /* mudprogs */
                if (char_died(ch))
                    return;
                if (number && cnt >= number)
                    break;
            }
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_CLANSTOREROOM))
            for (clan = first_clan; clan; clan = clan->next)
                if (clan->storeroom == ch->in_room->vnum)
                    save_clan_storeroom(ch, clan);

        if (!found)
        {
            if (fAll)
                act(AT_PLAIN, "You are not carrying anything.", ch, nullptr, nullptr, TO_CHAR);
            else
                act(AT_PLAIN, "You are not carrying any $T.", ch, nullptr, chk, TO_CHAR);
        }
    }
    if (IS_SET(sysdata.save_flags, SV_DROP))
        save_char_obj(ch); /* duping protector */
    return;
}

void do_give(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA* victim;
    OBJ_DATA* obj;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if (!str_cmp(arg2, "to") && argument[0] != '\0')
        argument = one_argument(argument, arg2);

    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Give what to whom?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (is_number(arg1))
    {
        /* 'give NNNN coins victim' */
        int amount;

        amount = atoi(arg1);
        if (amount <= 0 || (str_cmp(arg2, "credits") && str_cmp(arg2, "credit")))
        {
            send_to_char("Sorry, you can't do that.\n\r", ch);
            return;
        }

        argument = one_argument(argument, arg2);
        if (!str_cmp(arg2, "to") && argument[0] != '\0')
            argument = one_argument(argument, arg2);
        if (arg2[0] == '\0')
        {
            send_to_char("Give what to whom?\n\r", ch);
            return;
        }

        if ((victim = get_char_room(ch, arg2)) == nullptr)
        {
            send_to_char("They aren't here.\n\r", ch);
            return;
        }

        if (ch->gold < amount)
        {
            send_to_char("Very generous of you, but you haven't got that many credits.\n\r", ch);
            return;
        }

        if (!IS_NPC(ch) && ch->top_level < 11)
        {
            send_to_char("Due to cheating, players under level 11 are not allowed to move credits.\n\r", ch);
            return;
        }

        ch->gold -= amount;
        victim->gold += amount;
        sprintf_s(buf, "You receive %d %s from $n.", amount, (amount > 1) ? "credits" : "credit");
        /*
                strcpy_s(buf, "$n gives you ");
                strcat_s(buf, arg1);
                strcat_s(buf, (amount > 1) ? " credits." : " credit.");
        */
        act(AT_ACTION, buf, ch, nullptr, victim, TO_VICT);
        sprintf_s(buf, "You give $N %d %s", amount, (amount > 1) ? "credits." : "credit.");
        act(AT_ACTION, "$n gives $N some credits.", ch, nullptr, victim, TO_NOTVICT);
        act(AT_ACTION, buf, ch, nullptr, victim, TO_CHAR);
        send_to_char("OK.\n\r", ch);
        if (!IS_NPC(ch) && !IS_NPC(victim) && IS_IMMORTAL(ch))
        {
            sprintf_s(logbuf, "%s gives %s %d credits.", ch->name, victim->name, amount);
            log_string(logbuf);
        }
        mprog_bribe_trigger(victim, ch, amount);
        if (IS_SET(sysdata.save_flags, SV_GIVE) && !char_died(ch))
            save_char_obj(ch);
        if (IS_SET(sysdata.save_flags, SV_RECEIVE) && !char_died(victim))
            save_char_obj(victim);
        return;
    }

    if ((obj = get_obj_carry(ch, arg1)) == nullptr)
    {
        send_to_char("You do not have that item.\n\r", ch);
        return;
    }

    if (obj->wear_loc != WEAR_NONE)
    {
        send_to_char("You must remove it first.\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg2)) == nullptr)
    {
        send_to_char("They aren't here.\n\r", ch);
        return;
    }

    if (!can_drop_obj(ch, obj))
    {
        send_to_char("You can't let go of it.\n\r", ch);
        return;
    }

    if (victim->carry_number + (get_obj_number(obj) / obj->count) > can_carry_n(victim) && !IS_NPC(victim))
    {
        act(AT_PLAIN, "$N has $S hands full.", ch, nullptr, victim, TO_CHAR);
        return;
    }

    if (victim->carry_weight + (get_obj_weight(obj) / obj->count) > can_carry_w(victim))
    {
        act(AT_PLAIN, "$N can't carry that much weight.", ch, nullptr, victim, TO_CHAR);
        return;
    }

    if (!can_see_obj(victim, obj))
    {
        act(AT_PLAIN, "$N can't see it.", ch, nullptr, victim, TO_CHAR);
        return;
    }

    if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE) && !can_take_proto(victim))
    {
        act(AT_PLAIN, "You cannot give that to $N!", ch, nullptr, victim, TO_CHAR);
        return;
    }

    separate_obj(obj);
    obj_from_char(obj);
    act(AT_ACTION, "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
    act(AT_ACTION, "$n gives you $p.", ch, obj, victim, TO_VICT);
    act(AT_ACTION, "You give $p to $N.", ch, obj, victim, TO_CHAR);
    obj = obj_to_char(obj, victim);
    if (!IS_NPC(ch) && !IS_NPC(victim) && IS_IMMORTAL(ch))
    {
        sprintf_s(logbuf, "%s gives %s to %s.", ch->name, obj->short_descr, victim->name);
        log_string(logbuf);
    }

    mprog_give_trigger(victim, ch, obj);
    if (IS_SET(sysdata.save_flags, SV_GIVE) && !char_died(ch))
        save_char_obj(ch);
    if (IS_SET(sysdata.save_flags, SV_RECEIVE) && !char_died(victim))
        save_char_obj(victim);
    return;
}

/*
 * Damage an object.						-Thoric
 * Affect player's AC if necessary.
 * Make object into scraps if necessary.
 * Send message about damaged object.
 */
obj_ret damage_obj(OBJ_DATA* obj)
{
    CHAR_DATA* ch;
    obj_ret objcode;

    ch = obj->carried_by;
    objcode = rNONE;

    separate_obj(obj);
    if (ch)
        act(AT_OBJECT, "($p gets damaged)", ch, obj, nullptr, TO_CHAR);
    else if (obj->in_room && (ch = obj->in_room->first_person) != nullptr)
    {
        act(AT_OBJECT, "($p gets damaged)", ch, obj, nullptr, TO_ROOM);
        act(AT_OBJECT, "($p gets damaged)", ch, obj, nullptr, TO_CHAR);
        ch = nullptr;
    }

    oprog_damage_trigger(ch, obj);
    if (obj_extracted(obj))
        return global_objcode;

    switch (obj->item_type)
    {
    default:
        make_scraps(obj);
        objcode = rOBJ_SCRAPPED;
        break;
    case ITEM_CONTAINER:
        if (--obj->value[3] <= 0)
        {
            make_scraps(obj);
            objcode = rOBJ_SCRAPPED;
        }
        break;
    case ITEM_ARMOR:
        if (ch && obj->value[0] >= 1)
            ch->armor += apply_ac(obj, obj->wear_loc);
        if (--obj->value[0] <= 0)
        {
            make_scraps(obj);
            objcode = rOBJ_SCRAPPED;
        }
        else if (ch && obj->value[0] >= 1)
            ch->armor -= apply_ac(obj, obj->wear_loc);
        break;
    case ITEM_WEAPON:
        if (--obj->value[0] <= 0)
        {
            make_scraps(obj);
            objcode = rOBJ_SCRAPPED;
        }
        break;
    }
    if (ch != nullptr)
        save_char_obj(ch); /* Stop scrap duping - Samson 1-2-00 */

    return objcode;
}

/*
 * Remove an object.
 */
bool remove_obj(CHAR_DATA* ch, int iWear, bool fReplace)
{
    OBJ_DATA *obj, *tmpobj;

    if ((obj = get_eq_char(ch, iWear)) == nullptr)
        return true;

    if (!fReplace && ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
    {
        act(AT_PLAIN, "$d: you can't carry that many items.", ch, nullptr, obj->name, TO_CHAR);
        return false;
    }

    if (!fReplace)
        return false;

    if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
    {
        act(AT_PLAIN, "You can't remove $p.", ch, obj, nullptr, TO_CHAR);
        return false;
    }

    if (obj == get_eq_char(ch, WEAR_WIELD) && (tmpobj = get_eq_char(ch, WEAR_DUAL_WIELD)) != nullptr)
        tmpobj->wear_loc = WEAR_WIELD;

    unequip_char(ch, obj);

    act(AT_ACTION, "$n stops using $p.", ch, obj, nullptr, TO_ROOM);
    act(AT_ACTION, "You stop using $p.", ch, obj, nullptr, TO_CHAR);
    oprog_remove_trigger(ch, obj);
    return true;
}

/*
 * See if char could be capable of dual-wielding		-Thoric
 */
bool could_dual(CHAR_DATA* ch)
{
    if (IS_NPC(ch))
        return true;
    if (ch->pcdata->learned[gsn_dual_wield])
        return true;

    return false;
}

/*
 * Check for the ability to dual-wield under all conditions.  -Orion
 *
 * Original version by Thoric.
 */
bool can_dual(CHAR_DATA* ch)
{
    /*
     * We must assume that when they come in, they are NOT wielding something. We
     * take care of the actual value later. -Orion
     */
    bool wielding[2], alreadyWielding = false;
    wielding[0] = false;
    wielding[1] = false;

    /*
     * If they don't have the ability to dual-wield, why should we allow them to
     * do so? -Orion
     */
    if (!could_dual(ch))
        return false;

    /*
     * Get that true wielding value I mentioned earlier. If they're wielding and
     * missile wielding, we can simply return false. If not, set the values. -Orion
     */
    if (get_eq_char(ch, WEAR_WIELD) && get_eq_char(ch, WEAR_MISSILE_WIELD))
    {
        send_to_char("You are already wielding two weapons... grow some more arms!\n\r", ch);
        return false;
    }
    else
    {
        /*
         * Wield position. -Orion
         */
        wielding[0] = get_eq_char(ch, WEAR_WIELD) ? true : false;
        /*
         * Missile wield position. -Orion
         */
        wielding[1] = get_eq_char(ch, WEAR_MISSILE_WIELD) ? true : false;
    }

    /*
     * Save some extra typing farther down. -Orion
     */
    if (wielding[0] || wielding[1])
        alreadyWielding = true;

    /*
     * If wielding and dual wielding, then they can't wear another weapon. Return
     * false. We can assume that dual wield will not be full if there is no wield
     * slot already filled. -Orion
     */
    if (wielding[0] && get_eq_char(ch, WEAR_DUAL_WIELD))
    {
        send_to_char("You are already wielding two weapons... grow some more arms!\n\r", ch);
        return false;
    }

    /*
     * If wielding or missile wielding and holding a shield, then we can return
     * false. -Orion
     */
    if (alreadyWielding && get_eq_char(ch, WEAR_SHIELD))
    {
        send_to_char("You cannot dual wield, you're already holding a shield!\n\r", ch);
        return false;
    }

    /*
     * If wielding or missile wielding and holding something, then we can return
     * false. -Orion
     */
    if (alreadyWielding && get_eq_char(ch, WEAR_HOLD))
    {
        send_to_char("You cannot hold another weapon, you're already holding something in that hand!\n\r", ch);
        return false;
    }

    return true;
}

/*
 * Check to see if there is room to wear another object on this location
 * (Layered clothing support)
 */
bool can_layer(CHAR_DATA* ch, OBJ_DATA* obj, sh_int wear_loc)
{
    OBJ_DATA* otmp;
    sh_int bitlayers = 0;
    sh_int objlayers = obj->pIndexData->layers;

    for (otmp = ch->first_carrying; otmp; otmp = otmp->next_content)
        if (otmp->wear_loc == wear_loc)
        {
            if (!otmp->pIndexData->layers)
                return false;
            else
                bitlayers |= otmp->pIndexData->layers;
        }
    if ((bitlayers && !objlayers) || bitlayers > objlayers)
        return false;
    if (!bitlayers || ((bitlayers & ~objlayers) == bitlayers))
        return true;
    return false;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 * Restructured a bit to allow for specifying body location	-Thoric
 */
void wear_obj(CHAR_DATA* ch, OBJ_DATA* obj, bool fReplace, sh_int wear_bit)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA* tmpobj;
    sh_int bit, tmp;
    bool check_size;

    separate_obj(obj);

    if (wear_bit > -1)
    {
        bit = wear_bit;
        if (!CAN_WEAR(obj, 1 << bit))
        {
            if (fReplace)
            {
                switch (1 << bit)
                {
                case ITEM_HOLD:
                    send_to_char("You cannot hold that.\n\r", ch);
                    break;
                case ITEM_WIELD:
                    send_to_char("You cannot wield that.\n\r", ch);
                    break;
                default:
                    sprintf_s(buf, "You cannot wear that on your %s.\n\r", w_flags[bit]);
                    send_to_char(buf, ch);
                }
            }
            return;
        }
    }
    else
    {
        for (bit = -1, tmp = 1; tmp < 31; tmp++)
        {
            if (CAN_WEAR(obj, 1 << tmp))
            {
                bit = tmp;
                break;
            }
        }
    }

    check_size = false;

    if (1 << bit == ITEM_WIELD || 1 << bit == ITEM_HOLD || obj->item_type == ITEM_LIGHT || 1 << bit == ITEM_WEAR_SHIELD)
        check_size = false;
    else if (ch->race == RACE_DEFEL)
        check_size = true;
    else if (!IS_NPC(ch))
        switch (ch->race)
        {
        default:
        case RACE_TRANDOSHAN:
        case RACE_VERPINE:
        case RACE_HUMAN:
        case RACE_ADARIAN:
        case RACE_RODIAN:
        case RACE_TWI_LEK:

            if (!IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE))
                check_size = true;
            break;

        case RACE_HUTT:

            if (!IS_OBJ_STAT(obj, ITEM_HUTT_SIZE))
                check_size = true;
            break;

        case RACE_GAMORREAN:
        case RACE_MON_CALAMARI:
        case RACE_QUARREN:
        case RACE_WOOKIEE:

            if (!IS_OBJ_STAT(obj, ITEM_LARGE_SIZE))
                check_size = true;
            break;

        case RACE_EWOK:
        case RACE_NOGHRI:
        case RACE_JAWA:

            if (!IS_OBJ_STAT(obj, ITEM_SMALL_SIZE))
                check_size = true;
            break;
        }

    /*
       this seems redundant but it enables both multiple sized objects to be
       used as well as objects with no size flags at all
    */

    if (check_size)
    {
        if (ch->race == RACE_DEFEL)
        {
            act(AT_MAGIC, "It is against your nature to wear anything that might make you visible.", ch, nullptr, nullptr,
                TO_CHAR);
            act(AT_ACTION, "$n wants to use $p, but doesn't.", ch, obj, nullptr, TO_ROOM);
            return;
        }

        if (IS_OBJ_STAT(obj, ITEM_HUTT_SIZE))
        {
            act(AT_MAGIC, "That item is too big for you.", ch, nullptr, nullptr, TO_CHAR);
            act(AT_ACTION, "$n tries to use $p, but it is too big.", ch, obj, nullptr, TO_ROOM);
            return;
        }

        if (IS_OBJ_STAT(obj, ITEM_LARGE_SIZE) || IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE))
        {
            act(AT_MAGIC, "That item is the wrong size for you.", ch, nullptr, nullptr, TO_CHAR);
            act(AT_ACTION, "$n tries to use $p, but can't.", ch, obj, nullptr, TO_ROOM);
            return;
        }

        if (IS_OBJ_STAT(obj, ITEM_SMALL_SIZE))
        {
            act(AT_MAGIC, "That item is too small for you.", ch, nullptr, nullptr, TO_CHAR);
            act(AT_ACTION, "$n tries to use $p, but it is too small.", ch, obj, nullptr, TO_ROOM);
            return;
        }
    }

    /* currently cannot have a light in non-light position */
    if (obj->item_type == ITEM_LIGHT)
    {
        if (!remove_obj(ch, WEAR_LIGHT, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n holds $p as a light.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You hold $p as your light.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_LIGHT);
        oprog_wear_trigger(ch, obj);
        return;
    }

    if (bit == -1)
    {
        if (fReplace)
            send_to_char("You can't wear, wield, or hold that.\n\r", ch);
        return;
    }

    switch (1 << bit)
    {
    default:
        bug("wear_obj: uknown/unused item_wear bit %d", bit);
        if (fReplace)
            send_to_char("You can't wear, wield, or hold that.\n\r", ch);
        return;

    case ITEM_WEAR_FINGER:
        if (get_eq_char(ch, WEAR_FINGER_L) && get_eq_char(ch, WEAR_FINGER_R) &&
            !remove_obj(ch, WEAR_FINGER_L, fReplace) && !remove_obj(ch, WEAR_FINGER_R, fReplace))
            return;

        if (!get_eq_char(ch, WEAR_FINGER_L))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n slips $s left finger into $p.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You slip your left finger into $p.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_FINGER_L);
            oprog_wear_trigger(ch, obj);
            return;
        }

        if (!get_eq_char(ch, WEAR_FINGER_R))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n slips $s right finger into $p.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You slip your right finger into $p.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_FINGER_R);
            oprog_wear_trigger(ch, obj);
            return;
        }

        bug("Wear_obj: no free finger.", 0);
        send_to_char("You already wear something on both fingers.\n\r", ch);
        return;

    case ITEM_WEAR_NECK:
        if (get_eq_char(ch, WEAR_NECK_1) != nullptr && get_eq_char(ch, WEAR_NECK_2) != nullptr &&
            !remove_obj(ch, WEAR_NECK_1, fReplace) && !remove_obj(ch, WEAR_NECK_2, fReplace))
            return;

        if (!get_eq_char(ch, WEAR_NECK_1))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n wears $p around $s neck.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You wear $p around your neck.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_NECK_1);
            oprog_wear_trigger(ch, obj);
            return;
        }

        if (!get_eq_char(ch, WEAR_NECK_2))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n wears $p around $s neck.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You wear $p around your neck.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_NECK_2);
            oprog_wear_trigger(ch, obj);
            return;
        }

        bug("Wear_obj: no free neck.", 0);
        send_to_char("You already wear two neck items.\n\r", ch);
        return;

    case ITEM_WEAR_BODY:
        /*
            if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
              return;
        */
        if (!can_layer(ch, obj, WEAR_BODY))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n fits $p on $s body.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You fit $p on your body.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_BODY);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_HEAD:
        if (ch->race == RACE_VERPINE || ch->race == RACE_TWI_LEK)
        {
            send_to_char("You cant wear anything on your head.\n\r", ch);
            return;
        }
        if (!remove_obj(ch, WEAR_HEAD, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n dons $p upon $s head.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You don $p upon your head.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_HEAD);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_EYES:
        if (!remove_obj(ch, WEAR_EYES, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n places $p on $s eyes.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You place $p on your eyes.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_EYES);
        oprog_wear_trigger(ch, obj);
        return;
    case ITEM_WEAR_BACK:
        if (!can_layer(ch, obj, WEAR_BACK))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n fits $p on $s back.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You fit $p on your back.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_BACK);
        oprog_wear_trigger(ch, obj);
        return;
    case ITEM_WEAR_EARS:
        if (ch->race == RACE_VERPINE)
        {
            send_to_char("What ears?.\n\r", ch);
            return;
        }
        if (!remove_obj(ch, WEAR_EARS, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p on $s ears.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on your ears.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_EARS);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_LEGS:
        /*
                if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
                  return;
        */
        if (ch->race == RACE_HUTT)
        {
            send_to_char("Hutts don't have legs.\n\r", ch);
            return;
        }
        if (!can_layer(ch, obj, WEAR_LEGS))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n slips into $p.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You slip into $p.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_LEGS);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_FEET:
        /*
                if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
                  return;
        */
        if (ch->race == RACE_HUTT)
        {
            send_to_char("Hutts don't have feet!\n\r", ch);
            return;
        }
        if (!can_layer(ch, obj, WEAR_FEET))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p on $s feet.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on your feet.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_FEET);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_HOLSTER1:
        /*
                if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
                  return;
        */
        if (!can_layer(ch, obj, WEAR_HOLSTER_L))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n straps $p on $s left hip.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on left hip.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_HOLSTER_L);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_HOLSTER2:
        /*
                if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
                  return;
        */
        if (!can_layer(ch, obj, WEAR_HOLSTER_R))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n straps $p on $s right hip.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on right hip.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_HOLSTER_R);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_HANDS:
        /*
                if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
                  return;
        */
        if (!can_layer(ch, obj, WEAR_HANDS))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p on $s hands.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on your hands.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_HANDS);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_ARMS:
        /*
                if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
                  return;
        */
        if (!can_layer(ch, obj, WEAR_ARMS))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p on $s arms.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on your arms.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_ARMS);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_ABOUT:
        /*
            if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
              return;
        */
        if (!can_layer(ch, obj, WEAR_ABOUT))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p about $s body.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p about your body.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_ABOUT);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_WAIST:
        /*
                if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
                  return;
        */
        if (!can_layer(ch, obj, WEAR_WAIST))
        {
            send_to_char("It won't fit overtop of what you're already wearing.\n\r", ch);
            return;
        }
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p about $s waist.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p about your waist.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_WAIST);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_WRIST:
        if (get_eq_char(ch, WEAR_WRIST_L) && get_eq_char(ch, WEAR_WRIST_R) && !remove_obj(ch, WEAR_WRIST_L, fReplace) &&
            !remove_obj(ch, WEAR_WRIST_R, fReplace))
            return;

        if (!get_eq_char(ch, WEAR_WRIST_L))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n fits $p around $s left wrist.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You fit $p around your left wrist.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_WRIST_L);
            oprog_wear_trigger(ch, obj);
            return;
        }

        if (!get_eq_char(ch, WEAR_WRIST_R))
        {
            if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
            {
                if (!obj->action_desc || obj->action_desc[0] == '\0')
                {
                    act(AT_ACTION, "$n fits $p around $s right wrist.", ch, obj, nullptr, TO_ROOM);
                    act(AT_ACTION, "You fit $p around your right wrist.", ch, obj, nullptr, TO_CHAR);
                }
                else
                    actiondesc(ch, obj, nullptr);
            }
            equip_char(ch, obj, WEAR_WRIST_R);
            oprog_wear_trigger(ch, obj);
            return;
        }

        bug("Wear_obj: no free wrist.", 0);
        send_to_char("You already wear two wrist items.\n\r", ch);
        return;

    case ITEM_WEAR_SHIELD:
        if (!remove_obj(ch, WEAR_SHIELD, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n uses $p as an energy shield.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You use $p as an energy shield.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_SHIELD);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WIELD:
        if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != nullptr && !could_dual(ch))
        {
            send_to_char("You're already wielding something.\n\r", ch);
            return;
        }

        if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != nullptr && could_dual(ch) &&
            tmpobj->value[3] == WEAPON_DUAL_LIGHTSABER)
        {
            send_to_char("You are wielding a dual sided lightsaber. This requires both hands.\n\r", ch);
            return;
        }

        if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != nullptr && could_dual(ch) && tmpobj->value[3] == WEAPON_FORCE_PIKE)
        {
            send_to_char("Force pikes require both hands.\n\r", ch);
            return;
        }

        if (obj->value[3] == WEAPON_BOWCASTER && get_curr_str(ch) <= 22)
        {
            send_to_char("You are too weak to wield this weapon.\n\r", ch);
            return;
        }

        if (tmpobj)
        {
            if (can_dual(ch))
            {

                if (obj->value[3] == WEAPON_DUAL_LIGHTSABER)
                {
                    send_to_char(
                        "You need two hands to wield a dual lightsaber and your already wielding a weapon.\n\r", ch);
                    return;
                }

                if (obj->value[3] == WEAPON_FORCE_PIKE)
                {
                    send_to_char("You need two hands to wield a force pike and you're already wielding a weapon.\n\r",
                                 ch);
                    return;
                }

                if (get_obj_weight(obj) + get_obj_weight(tmpobj) > str_app[get_curr_str(ch)].wield)
                {
                    send_to_char("It is too heavy for you to wield.\n\r", ch);
                    return;
                }
                if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
                {
                    if (!obj->action_desc || obj->action_desc[0] == '\0')
                    {
                        act(AT_ACTION, "$n dual-wields $p.", ch, obj, nullptr, TO_ROOM);
                        act(AT_ACTION, "You dual-wield $p.", ch, obj, nullptr, TO_CHAR);
                    }
                    else
                        actiondesc(ch, obj, nullptr);
                }
                equip_char(ch, obj, WEAR_DUAL_WIELD);
                oprog_wear_trigger(ch, obj);
            }
            return;
        }

        if (get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
        {
            send_to_char("It is too heavy for you to wield.\n\r", ch);
            return;
        }

        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wields $p.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wield $p.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_WIELD);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_HOLD:
        if (get_eq_char(ch, WEAR_DUAL_WIELD))
        {
            send_to_char("You cannot hold something AND two weapons!\n\r", ch);
            return;
        }
        if (!remove_obj(ch, WEAR_HOLD, fReplace))
            return;
        if (obj->item_type == ITEM_DEVICE || obj->item_type == ITEM_GRENADE || obj->item_type == ITEM_FOOD ||
            obj->item_type == ITEM_PILL || obj->item_type == ITEM_POTION || obj->item_type == ITEM_DRINK_CON ||
            obj->item_type == ITEM_SALVE || obj->item_type == ITEM_KEY || !oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            act(AT_ACTION, "$n holds $p in $s hands.", ch, obj, nullptr, TO_ROOM);
            act(AT_ACTION, "You hold $p in your hands.", ch, obj, nullptr, TO_CHAR);
        }
        equip_char(ch, obj, WEAR_HOLD);
        oprog_wear_trigger(ch, obj);
        return;

    case ITEM_WEAR_BOTHWRISTS:
        if (!remove_obj(ch, WEAR_BOTH_WRISTS, fReplace))
            return;
        if (!oprog_use_trigger(ch, obj, nullptr, nullptr, nullptr))
        {
            if (!obj->action_desc || obj->action_desc[0] == '\0')
            {
                act(AT_ACTION, "$n wears $p on both wrists.", ch, obj, nullptr, TO_ROOM);
                act(AT_ACTION, "You wear $p on both wrists.", ch, obj, nullptr, TO_CHAR);
            }
            else
                actiondesc(ch, obj, nullptr);
        }
        equip_char(ch, obj, WEAR_BOTH_WRISTS);
        oprog_wear_trigger(ch, obj);
        return;
    }
}

void do_wear(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA* obj;
    sh_int wear_bit;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if ((!str_cmp(arg2, "on") || !str_cmp(arg2, "upon") || !str_cmp(arg2, "around")) && argument[0] != '\0')
        argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("Wear, wield, or hold what?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (!str_cmp(arg1, "all"))
    {
        OBJ_DATA* obj_next;

        for (obj = ch->first_carrying; obj; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->item_type == ITEM_BINDERS)
            {
                send_to_char("You're into that S&&M stuff, eh?\n\r", ch);
                return;
            }
            if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
                wear_obj(ch, obj, false, -1);
        }
        return;
    }
    else
    {
        if ((obj = get_obj_carry(ch, arg1)) == nullptr)
        {
            send_to_char("You do not have that item.\n\r", ch);
            return;
        }
        if (obj->item_type == ITEM_BINDERS)
        {
            send_to_char("You're into that S&&M stuff, eh?\n\r", ch);
            return;
        }
        if (arg2[0] != '\0')
            wear_bit = get_wflag(arg2);
        else
            wear_bit = -1;
        wear_obj(ch, obj, true, wear_bit);
    }

    return;
}

void do_remove(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj, *obj_next;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("Remove what?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    if (!str_cmp(arg, "all")) /* SB Remove all */
    {
        for (obj = ch->first_carrying; obj != nullptr; obj = obj_next)
        {
            obj_next = obj->next_content;
            if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj) && obj->item_type != ITEM_BINDERS)
                remove_obj(ch, obj->wear_loc, true);
        }
        return;
    }

    if ((obj = get_obj_wear(ch, arg)) == nullptr)
    {
        send_to_char("You are not using that item.\n\r", ch);
        return;
    }
    if ((obj_next = get_eq_char(ch, obj->wear_loc)) != obj)
    {
        act(AT_PLAIN, "You must remove $p first.", ch, obj_next, nullptr, TO_CHAR);
        return;
    }
    if (obj->item_type == ITEM_BINDERS)
    {
        send_to_char("You find it very difficult to remove your bindings.\n\r", ch);
        return;
    }

    remove_obj(ch, obj->wear_loc, true);
    return;
}

void do_bury(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA* obj;
    bool shovel;
    sh_int move;

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char("What do you wish to bury?\n\r", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    shovel = false;
    for (obj = ch->first_carrying; obj; obj = obj->next_content)
        if (obj->item_type == ITEM_SHOVEL)
        {
            shovel = true;
            break;
        }

    obj = get_obj_list_rev(ch, arg, ch->in_room->last_content);
    if (!obj)
    {
        send_to_char("You can't find it.\n\r", ch);
        return;
    }

    separate_obj(obj);
    if (!CAN_WEAR(obj, ITEM_TAKE))
    {
        act(AT_PLAIN, "You cannot bury $p.", ch, obj, 0, TO_CHAR);
        return;
    }

    switch (ch->in_room->sector_type)
    {
    case SECT_CITY:
    case SECT_INSIDE:
        send_to_char("The floor is too hard to dig through.\n\r", ch);
        return;
    case SECT_WATER_SWIM:
    case SECT_WATER_NOSWIM:
    case SECT_UNDERWATER:
        send_to_char("You cannot bury something here.\n\r", ch);
        return;
    case SECT_AIR:
        send_to_char("What?  In the air?!\n\r", ch);
        return;
    }

    if (obj->weight > (UMAX(5, (can_carry_w(ch) / 10))) && !shovel)
    {
        send_to_char("You'd need a shovel to bury something that big.\n\r", ch);
        return;
    }

    move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
    move = URANGE(2, move, 1000);
    if (move > ch->move)
    {
        send_to_char("You don't have the energy to bury something of that size.\n\r", ch);
        return;
    }
    ch->move -= move;

    act(AT_ACTION, "You solemnly bury $p...", ch, obj, nullptr, TO_CHAR);
    act(AT_ACTION, "$n solemnly buries $p...", ch, obj, nullptr, TO_ROOM);
    SET_BIT(obj->extra_flags, ITEM_BURRIED);
    WAIT_STATE(ch, URANGE(10, move / 2, 100));
    return;
}

void do_sacrifice(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA* obj;

    one_argument(argument, arg);

    if (arg[0] == '\0' || !str_cmp(arg, ch->name))
    {
        act(AT_ACTION, "$n offers $mself to $s deity, who graciously declines.", ch, nullptr, nullptr, TO_ROOM);
        send_to_char("Your deity appreciates your offer and may accept it later.", ch);
        return;
    }

    if (ms_find_obj(ch))
        return;

    obj = get_obj_list_rev(ch, arg, ch->in_room->last_content);
    if (!obj)
    {
        send_to_char("You can't find it.\n\r", ch);
        return;
    }

    separate_obj(obj);
    if (!CAN_WEAR(obj, ITEM_TAKE))
    {
        act(AT_PLAIN, "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR);
        return;
    }

    oprog_sac_trigger(ch, obj);
    if (obj_extracted(obj))
        return;
    if (cur_obj == obj->serial)
        global_objcode = rOBJ_SACCED;
    separate_obj(obj);
    extract_obj(obj);
    return;
}

void do_brandish(CHAR_DATA* ch, char* argument)
{
    CHAR_DATA* vch;
    CHAR_DATA* vch_next;
    OBJ_DATA* staff;
    ch_ret retcode;
    int sn;

    if ((staff = get_eq_char(ch, WEAR_HOLD)) == nullptr)
    {
        send_to_char("You hold nothing in your hand.\n\r", ch);
        return;
    }

    if (staff->item_type != ITEM_STAFF)
    {
        send_to_char("You can brandish only with a staff.\n\r", ch);
        return;
    }

    if ((sn = staff->value[3]) < 0 || sn >= top_sn || skill_table[sn]->spell_fun == nullptr)
    {
        bug("Do_brandish: bad sn %d.", sn);
        return;
    }

    WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

    if (staff->value[2] > 0)
    {
        if (!oprog_use_trigger(ch, staff, nullptr, nullptr, nullptr))
        {
            act(AT_MAGIC, "$n brandishes $p.", ch, staff, nullptr, TO_ROOM);
            act(AT_MAGIC, "You brandish $p.", ch, staff, nullptr, TO_CHAR);
        }
        for (vch = ch->in_room->first_person; vch; vch = vch_next)
        {
            vch_next = vch->next_in_room;
            if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WIZINVIS) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL)
                continue;
            else
                switch (skill_table[sn]->target)
                {
                default:
                    bug("Do_brandish: bad target for sn %d.", sn);
                    return;

                case TAR_IGNORE:
                    if (vch != ch)
                        continue;
                    break;

                case TAR_CHAR_OFFENSIVE:
                    if (IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch))
                        continue;
                    break;

                case TAR_CHAR_DEFENSIVE:
                    if (IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch))
                        continue;
                    break;

                case TAR_CHAR_SELF:
                    if (vch != ch)
                        continue;
                    break;
                }

            retcode = obj_cast_spell(staff->value[3], staff->value[0], ch, vch, nullptr);
            if (retcode == rCHAR_DIED || retcode == rBOTH_DIED)
            {
                bug("do_brandish: char died", 0);
                return;
            }
        }
    }

    if (--staff->value[2] <= 0)
    {
        act(AT_MAGIC, "$p blazes bright and vanishes from $n's hands!", ch, staff, nullptr, TO_ROOM);
        act(AT_MAGIC, "$p blazes bright and is gone!", ch, staff, nullptr, TO_CHAR);
        if (staff->serial == cur_obj)
            global_objcode = rOBJ_USED;
        extract_obj(staff);
    }

    return;
}

void do_zap(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA* victim;
    OBJ_DATA* wand;
    OBJ_DATA* obj;
    ch_ret retcode;

    one_argument(argument, arg);
    if (arg[0] == '\0' && !ch->fighting)
    {
        send_to_char("Zap whom or what?\n\r", ch);
        return;
    }

    if ((wand = get_eq_char(ch, WEAR_HOLD)) == nullptr)
    {
        send_to_char("You hold nothing in your hand.\n\r", ch);
        return;
    }

    if (wand->item_type != ITEM_WAND)
    {
        send_to_char("You can zap only with a wand.\n\r", ch);
        return;
    }

    obj = nullptr;
    if (arg[0] == '\0')
    {
        if (ch->fighting)
        {
            victim = who_fighting(ch);
        }
        else
        {
            send_to_char("Zap whom or what?\n\r", ch);
            return;
        }
    }
    else
    {
        if ((victim = get_char_room(ch, arg)) == nullptr && (obj = get_obj_here(ch, arg)) == nullptr)
        {
            send_to_char("You can't find it.\n\r", ch);
            return;
        }
    }

    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);

    if (wand->value[2] > 0)
    {
        if (victim)
        {
            if (!oprog_use_trigger(ch, wand, victim, nullptr, nullptr))
            {
                act(AT_MAGIC, "$n aims $p at $N.", ch, wand, victim, TO_ROOM);
                act(AT_MAGIC, "You aim $p at $N.", ch, wand, victim, TO_CHAR);
            }
        }
        else
        {
            if (!oprog_use_trigger(ch, wand, nullptr, obj, nullptr))
            {
                act(AT_MAGIC, "$n aims $p at $P.", ch, wand, obj, TO_ROOM);
                act(AT_MAGIC, "You aim $p at $P.", ch, wand, obj, TO_CHAR);
            }
        }

        retcode = obj_cast_spell(wand->value[3], wand->value[0], ch, victim, obj);
        if (retcode == rCHAR_DIED || retcode == rBOTH_DIED)
        {
            bug("do_zap: char died", 0);
            return;
        }
    }

    if (--wand->value[2] <= 0)
    {
        act(AT_MAGIC, "$p explodes into fragments.", ch, wand, nullptr, TO_ROOM);
        act(AT_MAGIC, "$p explodes into fragments.", ch, wand, nullptr, TO_CHAR);
        if (wand->serial == cur_obj)
            global_objcode = rOBJ_USED;
        extract_obj(wand);
    }

    return;
}

/*
 * Save items in a clan storage room			-Scryn & Thoric
 */
void save_clan_storeroom(CHAR_DATA* ch, CLAN_DATA* clan)
{
    FILE* fp = nullptr;
    char filename[256];
    sh_int templvl;
    OBJ_DATA* contents;

    if (!clan)
    {
        bug("save_clan_storeroom: Null clan pointer!", 0);
        return;
    }

    if (!ch)
    {
        bug("save_clan_storeroom: Null ch pointer!", 0);
        return;
    }

    sprintf_s(filename, "%s%s.vault", CLAN_DIR, clan->filename);
    fp = fopen(filename, "w");
    if (fp == nullptr)
    {
        bug("save_clan_storeroom: fopen", 0);
        perror(filename);
    }
    else
    {
        templvl = ch->top_level;
        ch->top_level = LEVEL_HERO; /* make sure EQ doesn't get lost */
        contents = ch->in_room->last_content;
        if (contents)
            fwrite_obj(ch, contents, fp, 0, OS_CARRY);
        fprintf(fp, "#END\n");
        ch->top_level = templvl;
        fclose(fp);
        return;
    }
    return;
}

/* put an item on auction, or see the stats on the current item or bet */
void do_auction(CHAR_DATA* ch, char* argument)
{
    OBJ_DATA* obj;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA* paf;

    argument = one_argument(argument, arg1);

    if (IS_NPC(ch)) /* NPC can be extracted at any time and thus can't auction! */
        return;

    /*    if ( ( time_info.hour > 18 || time_info.hour < 9 ) && auction->item == nullptr )
        {
            set_char_color ( AT_LBLUE, ch );
            send_to_char ( "\n\rThe auctioneer has retired for the evening...\n\r", ch );
            return;
        }
    */
    if (arg1[0] == '\0')
    {
        if (auction->item != nullptr)
        {
            if (ch == auction->seller && !IS_IMMORTAL(ch))
            {
                send_to_char("You can't check on the stats of your own item!\n\r", ch);
                return;
            }

            obj = auction->item;

            /* show item data here */
            if (auction->bet > 0)
                sprintf_s(buf, "%d", auction->bet);
            else
                sprintf_s(buf, "zero");
            ch_printf(ch, "\n\r&W++\n\r&z||&w Item:&G %s  &wType:&G %s\n\r", obj->short_descr,
                      aoran(item_type_name(obj)).c_str());
            ch_printf(ch, "&z|| &wCurrent bid on this item is &Y%s&w credits.\n\r", buf);
            ch_printf(ch, "&z||&w Cost:&G %d  &wWeight:&G %d  &wWorn on:&G %s\n\r", obj->cost, obj->weight,
                      flag_string(obj->wear_flags - 1, w_flags));
            ch_printf(ch, "&W++\n\r");

            switch (obj->item_type)
            {

            case ITEM_RLAUNCHER:
                if (obj->value[5] == 0)
                    ch_printf(ch, "&z|| &wIt isn't loaded with anything.\n\r");
                else
                    ch_printf(ch, "&z|| &wIt is loaded with an &R%s&w missile.\n\r",
                              obj->value[1] == 1 ? "incendiary" : "explosive");
                if (obj->value[2] == 1)
                    ch_printf(ch, "&z|| &wIt is equipped with a guidance system.\n\r");
                ch_printf(ch, "&W++\n\r");

                break;
            case ITEM_ARMOR:
                ch_printf(ch, "&z|| &wCurrent Armor Class: &G%d&w  Maximum: &G%d\n\r", obj->value[0], obj->value[1]);
                ch_printf(ch, "&W++\n\r");
                break;
            case ITEM_WEAPON:
                ch_printf(ch, "&z|| &wIt is a &G%s&w.  Average Damage: &G%d&w\n\r",
                          obj->value[3] == WEAPON_VIBRO_BLADE  ? "vibro blade"
                          : obj->value[3] == WEAPON_BOWCASTER  ? "bowcaster"
                          : obj->value[3] == WEAPON_FORCE_PIKE ? "force pike"
                          : obj->value[3] == WEAPON_BLASTER    ? "blaster"
                          : obj->value[3] == WEAPON_LIGHTSABER ? "lightsaber"
                                                               : "weapon",
                          (obj->value[1] + obj->value[2]) / 2);
                if (obj->value[3] == WEAPON_BLASTER || obj->value[3] == WEAPON_VIBRO_BLADE ||
                    obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_FORCE_PIKE)
                    ch_printf(ch, "&z|| &wEnergy cell rating: &G%d\n\r", obj->value[5]);
                ch_printf(ch, "&W++\n\r");
                break;
            }
            set_char_color(AT_WHITE, ch);
            for (paf = obj->pIndexData->first_affect; paf; paf = paf->next)
                showaffect(ch, paf);

            for (paf = obj->first_affect; paf; paf = paf->next)
                showaffect(ch, paf);
            if ((obj->item_type == ITEM_CONTAINER) && (obj->first_content))
            {
                ch_printf(ch, "&z||&w Contents of &G%s&w:\n\r\n\r", obj->short_descr);
                set_char_color(AT_LBLUE, ch);
                show_list_to_char(obj->first_content, ch, true, false);
                set_char_color(AT_WHITE, ch);
                ch_printf(ch, "\n\r&z||&w\n\r&W++\n\r", ch);
            }

            if (IS_IMMORTAL(ch))
            {
                sprintf_s(buf, "Seller: %s.  Bidder: %s.  Round: %d.\n\r", auction->seller->name, auction->buyer->name,
                          (auction->going + 1));
                send_to_char(buf, ch);
                sprintf_s(buf, "Time left in round: %d seconds\n\r", auction->pulse / 4);
                send_to_char(buf, ch);
            }
            return;
        }
        else
        {
            set_char_color(AT_AUCTION, ch);
            send_to_char("\n\rThere is nothing being auctioned right now.  What would you like to auction?\n\r", ch);
            return;
        }
    }

    if (IS_IMMORTAL(ch) && !str_cmp(arg1, "stop"))
    {
        if (auction->item == nullptr)
        {
            send_to_char("There is no auction to stop.\n\r", ch);
            return;
        }
        else /* stop the auction */
        {
            set_char_color(AT_AUCTION, ch);
            sprintf_s(buf, "Sale of %s has been stopped by an Immortal.", auction->item->short_descr);
            talk_auction(buf);
            obj_to_char(auction->item, auction->seller);
            if (IS_SET(sysdata.save_flags, SV_AUCTION))
                save_char_obj(auction->seller);
            auction->item = nullptr;
            if (auction->buyer != nullptr && auction->buyer != auction->seller) /* return money to the buyer */
            {
                auction->buyer->gold += auction->bet;
                send_to_char("Your money has been returned.\n\r", auction->buyer);
            }
            return;
        }
    }
    if (!str_cmp(arg1, "bid"))
    {
        if (auction->item != nullptr)
        {
            int newbet;

            if (ch == auction->seller)
            {
                send_to_char("You can't bid on your own item!\n\r", ch);
                return;
            }

            /* make - perhaps - a bet now */
            if (argument[0] == '\0')
            {
                send_to_char("Bid how much?\n\r", ch);
                return;
            }

            newbet = parsebet(auction->bet, argument);
            /*	    ch_printf( ch, "Bid: %d\n\r",newbet);	*/

            if (newbet < auction->starting)
            {
                send_to_char("You must place a bid that is higher than the starting bet.\n\r", ch);
                return;
            }

            /* to avoid slow auction, use a bigger amount than 100 if the bet
               is higher up - changed to 10000 for our high economy
                */

            if (newbet < (auction->bet + 100))
            {
                send_to_char("You must at least bid 100 credits over the current bid.\n\r", ch);
                return;
            }

            if (newbet > ch->gold)
            {
                send_to_char("You don't have that much money!\n\r", ch);
                return;
            }

            if (newbet > 2000000000)
            {
                send_to_char("You can't bid over 2 billion credits.\n\r", ch);
                return;
            }

            /* the actual bet is OK! */

            /* return the gold to the last buyer, if one exists */
            if (auction->buyer != nullptr && auction->buyer != auction->seller)
                auction->buyer->gold += auction->bet;

            ch->gold -= newbet; /* substract the gold - important :) */
            if (IS_SET(sysdata.save_flags, SV_AUCTION))
                save_char_obj(ch);
            auction->buyer = ch;
            auction->bet = newbet;
            auction->going = 0;
            auction->pulse = PULSE_AUCTION; /* start the auction over again */

            sprintf_s(buf, "&WNew bidder: &Y%d &Wcredits for %s.\n\r", newbet, auction->item->short_descr);
            talk_auction(buf);
            return;
        }
        else
        {
            send_to_char("There isn't anything being auctioned right now.\n\r", ch);
            return;
        }
    }
    /* finally... */
    if (ms_find_obj(ch))
        return;

    obj = get_obj_carry(ch, arg1); /* does char have the item ? */

    if (obj == nullptr)
    {
        send_to_char("You aren't carrying that.\n\r", ch);
        return;
    }

    if (obj->timer > 0)
    {
        send_to_char("You can't auction objects that are decaying.\n\r", ch);
        return;
    }

    argument = one_argument(argument, arg2);

    if (arg2[0] == '\0')
    {
        auction->starting = 0;
        strcpy_s(arg2, "0");
    }

    if (!is_number(arg2))
    {
        send_to_char("You must input a number at which to start the auction.\n\r", ch);
        return;
    }

    if (atoi(arg2) < 0)
    {
        send_to_char("You can't auction something for less than 0 credits!\n\r", ch);
        return;
    }

    if (auction->item == nullptr)
        switch (obj->item_type)
        {

        default:
            act(AT_TELL, "You cannot auction $Ts.", ch, nullptr, item_type_name(obj).c_str(), TO_CHAR);
            return;

            /* insert any more item types here... items with a timer MAY NOT BE
               AUCTIONED!
            */
        case ITEM_LIGHT:
        case ITEM_TREASURE:
        case ITEM_RARE_METAL:
        case ITEM_CRYSTAL:
        case ITEM_BOOK:
        case ITEM_FABRIC:
        case ITEM_PAPER:
        case ITEM_ARMOR:
        case ITEM_COMLINK:
        case ITEM_WEAPON:
        case ITEM_GLAUNCHER:
        case ITEM_RLAUNCHER:
        case ITEM_GRENADE:
        case ITEM_SHIPBOMB:

        case ITEM_MISSILE:
        case ITEM_CONTAINER:
        case ITEM_GOGGLES:
            separate_obj(obj);
            obj_from_char(obj);
            if (IS_SET(sysdata.save_flags, SV_AUCTION))
                save_char_obj(ch);
            auction->item = obj;
            auction->bet = 0;
            auction->buyer = ch;
            auction->seller = ch;
            auction->pulse = PULSE_AUCTION;
            auction->going = 0;
            auction->starting = atoi(arg2);

            if (auction->starting > 0)
                auction->bet = auction->starting;

            sprintf_s(buf, "&WNew item: %s&W at &Y%d&W credits.", obj->short_descr, auction->starting);
            talk_auction(buf);

            return;

        } /* switch */
    else
    {
        act(AT_TELL, "Try again later - $p is being auctioned right now!", ch, auction->item, nullptr, TO_CHAR);
        WAIT_STATE(ch, 1.5 * PULSE_VIOLENCE);
        return;
    }
}

/* Make objects in rooms that are nofloor fall - Scryn 1/23/96 */

void obj_fall(OBJ_DATA* obj, bool through)
{
    EXIT_DATA* pexit;
    ROOM_INDEX_DATA* to_room;
    static int fall_count;
    char buf[MAX_STRING_LENGTH];
    static bool is_falling; /* Stop loops from the call to obj_to_room()  -- Altrag */

    if (!obj->in_room || is_falling)
        return;

    if (fall_count > 30)
    {
        bug("object falling in loop more than 30 times", 0);
        extract_obj(obj);
        fall_count = 0;
        return;
    }

    if (IS_SET(obj->in_room->room_flags, ROOM_NOFLOOR) && CAN_GO(obj, DIR_DOWN) && !IS_OBJ_STAT(obj, ITEM_MAGIC))
    {

        pexit = get_exit(obj->in_room, DIR_DOWN);
        to_room = pexit->to_room;

        if (through)
            fall_count++;
        else
            fall_count = 0;

        if (obj->in_room == to_room)
        {
            sprintf_s(buf, "Object falling into same room, room %d", to_room->vnum);
            bug(buf, 0);
            extract_obj(obj);
            return;
        }

        if (obj->in_room->first_person)
        {
            act(AT_PLAIN, "$p falls far below...", obj->in_room->first_person, obj, nullptr, TO_ROOM);
            act(AT_PLAIN, "$p falls far below...", obj->in_room->first_person, obj, nullptr, TO_CHAR);
        }
        obj_from_room(obj);
        is_falling = true;
        obj = obj_to_room(obj, to_room);
        is_falling = false;

        if (obj->in_room->first_person)
        {
            act(AT_PLAIN, "$p falls from above...", obj->in_room->first_person, obj, nullptr, TO_ROOM);
            act(AT_PLAIN, "$p falls from above...", obj->in_room->first_person, obj, nullptr, TO_CHAR);
        }

        if (!IS_SET(obj->in_room->room_flags, ROOM_NOFLOOR) && through)
        {
            /*		int dam = (int)9.81*sqrt(fall_count*2/9.81)*obj->weight/2;
             */
            int dam = fall_count * obj->weight / 2;
            /* Damage players */
            if (obj->in_room->first_person && number_percent() > 15)
            {
                CHAR_DATA* rch;
                CHAR_DATA* vch = nullptr;
                int chcnt = 0;

                for (rch = obj->in_room->first_person; rch; rch = rch->next_in_room, chcnt++)
                    if (number_range(0, chcnt) == 0)
                        vch = rch;
                act(AT_WHITE, "$p falls on $n!", vch, obj, nullptr, TO_ROOM);
                act(AT_WHITE, "$p falls on you!", vch, obj, nullptr, TO_CHAR);
                damage(vch, vch, dam * vch->top_level, TYPE_UNDEFINED);
            }
            /* Damage objects */
            switch (obj->item_type)
            {
            case ITEM_WEAPON:
            case ITEM_ARMOR:
                if ((obj->value[0] - dam) <= 0)
                {
                    if (obj->in_room->first_person)
                    {
                        act(AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, nullptr, TO_ROOM);
                        act(AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, nullptr, TO_CHAR);
                    }
                    make_scraps(obj);
                }
                else
                    obj->value[0] -= dam;
                break;
            default:
                if ((dam * 15) > get_obj_resistance(obj))
                {
                    if (obj->in_room->first_person)
                    {
                        act(AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, nullptr, TO_ROOM);
                        act(AT_PLAIN, "$p is destroyed by the fall!", obj->in_room->first_person, obj, nullptr, TO_CHAR);
                    }
                    make_scraps(obj);
                }
                break;
            }
        }
        obj_fall(obj, true);
    }
    return;
}
