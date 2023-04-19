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

#pragma once

#include <cctype>
#include <cstdio>
#include <cstdlib>

/*
 * Utility macros.
 */
#define UMIN(a, b) ((a) < (b) ? (a) : (b))
#define UMAX(a, b) ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c) ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c) ((c) >= 'A' && (c) <= 'Z' ? (c) + 'a' - 'A' : (c))
#define UPPER(c) ((c) >= 'a' && (c) <= 'z' ? (c) + 'A' - 'a' : (c))
#define IS_SET(flag, bit) ((flag) & (bit))
#define SET_BIT(var, bit) ((var) |= (bit))
#define REMOVE_BIT(var, bit) ((var) &= ~(bit))
#define TOGGLE_BIT(var, bit) ((var) ^= (bit))
#define CH(d) ((d)->original ? (d)->original : (d)->character)

/*
 * Memory allocation macros.
 */
#define CREATE(result, type, number)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!((result) = (type*)calloc((number), sizeof(type))))                                                       \
        {                                                                                                              \
            perror("malloc failure");                                                                                  \
            fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__);                                           \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

#define RECREATE(result, type, number)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!((result) = (type*)realloc((result), sizeof(type) * (number))))                                           \
        {                                                                                                              \
            perror("realloc failure");                                                                                 \
            fprintf(stderr, "Realloc failure @ %s:%d\n", __FILE__, __LINE__);                                          \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

#define DISPOSE(point)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((point))                                                                                                   \
        {                                                                                                              \
            if (in_hash_table((char*)(point)))                                                                         \
            {                                                                                                          \
                bug("&RDISPOSE called on STRALLOC pointer: %s, line %d", __FILE__, __LINE__);                          \
                log_string("Attempting to correct.");                                                                  \
                if (str_free((char*)(point)) == -1)                                                                    \
                    bug("&RSTRFREEing bad pointer: %s, line %d", __FILE__, __LINE__);                                  \
            }                                                                                                          \
            else                                                                                                       \
                free((point));                                                                                         \
            (point) = NULL;                                                                                            \
        }                                                                                                              \
    } while (0)

#define STRALLOC(point) str_alloc((point))
#define QUICKLINK(point) quick_link((point))
#define STRFREE(point)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((point))                                                                                                   \
        {                                                                                                              \
            if (!in_hash_table((point)))                                                                               \
            {                                                                                                          \
                bug("&RSTRFREE called on str_dup pointer: %s, line %d", __FILE__, __LINE__);                           \
                log_string("Attempting to correct.");                                                                  \
                free((point));                                                                                         \
            }                                                                                                          \
            else if (str_free((point)) == -1)                                                                          \
                bug("&RSTRFREEing bad pointer: %s, line %d", __FILE__, __LINE__);                                      \
            (point) = NULL;                                                                                            \
        }                                                                                                              \
    } while (0)

/* double-linked list handling macros -Thoric */
/* Updated by Scion 8/6/1999 */
#define LINK(link, first, last, next, prev)                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(first))                                                                                                  \
        {                                                                                                              \
            (first) = (link);                                                                                          \
            (last) = (link);                                                                                           \
        }                                                                                                              \
        else                                                                                                           \
            (last)->next = (link);                                                                                     \
        (link)->next = NULL;                                                                                           \
        if ((first) == (link))                                                                                         \
            (link)->prev = NULL;                                                                                       \
        else                                                                                                           \
            (link)->prev = (last);                                                                                     \
        (last) = (link);                                                                                               \
    } while (0)

#define INSERT(link, insert, first, next, prev)                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        (link)->prev = (insert)->prev;                                                                                 \
        if (!(insert)->prev)                                                                                           \
            (first) = (link);                                                                                          \
        else                                                                                                           \
            (insert)->prev->next = (link);                                                                             \
        (insert)->prev = (link);                                                                                       \
        (link)->next = (insert);                                                                                       \
    } while (0)

#define UNLINK(link, first, last, next, prev)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(link)->prev)                                                                                             \
        {                                                                                                              \
            (first) = (link)->next;                                                                                    \
            if ((first))                                                                                               \
                (first)->prev = NULL;                                                                                  \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (link)->prev->next = (link)->next;                                                                         \
        }                                                                                                              \
        if (!(link)->next)                                                                                             \
        {                                                                                                              \
            (last) = (link)->prev;                                                                                     \
            if ((last))                                                                                                \
                (last)->next = NULL;                                                                                   \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (link)->next->prev = (link)->prev;                                                                         \
        }                                                                                                              \
    } while (0)

#define CHECK_LINKS(first, last, next, prev, type)                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        type *ptr, *pptr = NULL;                                                                                       \
        if (!(first) && !(last))                                                                                       \
            break;                                                                                                     \
        if (!(first))                                                                                                  \
        {                                                                                                              \
            bug("CHECK_LINKS: last with NULL first!  %s.", __STRING(first));                                           \
            for (ptr = (last); ptr->prev; ptr = ptr->prev)                                                             \
                ;                                                                                                      \
            (first) = ptr;                                                                                             \
        }                                                                                                              \
        else if (!(last))                                                                                              \
        {                                                                                                              \
            bug("CHECK_LINKS: first with NULL last!  %s.", __STRING(first));                                           \
            for (ptr = (first); ptr->next; ptr = ptr->next)                                                            \
                ;                                                                                                      \
            (last) = ptr;                                                                                              \
        }                                                                                                              \
        if ((first))                                                                                                   \
        {                                                                                                              \
            for (ptr = (first); ptr; ptr = ptr->next)                                                                  \
            {                                                                                                          \
                if (ptr->prev != pptr)                                                                                 \
                {                                                                                                      \
                    bug("CHECK_LINKS(%s): %p:->prev != %p.  Fixing.", __STRING(first), ptr, pptr);                     \
                    ptr->prev = pptr;                                                                                  \
                }                                                                                                      \
                if (ptr->prev && ptr->prev->next != ptr)                                                               \
                {                                                                                                      \
                    bug("CHECK_LINKS(%s): %p:->prev->next != %p.  Fixing.", __STRING(first), ptr, ptr);                \
                    ptr->prev->next = ptr;                                                                             \
                }                                                                                                      \
                pptr = ptr;                                                                                            \
            }                                                                                                          \
            pptr = NULL;                                                                                               \
        }                                                                                                              \
        if ((last))                                                                                                    \
        {                                                                                                              \
            for (ptr = (last); ptr; ptr = ptr->prev)                                                                   \
            {                                                                                                          \
                if (ptr->next != pptr)                                                                                 \
                {                                                                                                      \
                    bug("CHECK_LINKS (%s): %p:->next != %p.  Fixing.", __STRING(first), ptr, pptr);                    \
                    ptr->next = pptr;                                                                                  \
                }                                                                                                      \
                if (ptr->next && ptr->next->prev != ptr)                                                               \
                {                                                                                                      \
                    bug("CHECK_LINKS(%s): %p:->next->prev != %p.  Fixing.", __STRING(first), ptr, ptr);                \
                    ptr->next->prev = ptr;                                                                             \
                }                                                                                                      \
                pptr = ptr;                                                                                            \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define ASSIGN_GSN(gsn, skill)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (((gsn) = skill_lookup((skill))) == -1)                                                                     \
            fprintf(stderr, "ASSIGN_GSN: Skill %s not found.\n", (skill));                                             \
    } while (0)

#define CHECK_SUBRESTRICTED(ch)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((ch)->substate == SUB_RESTRICTED)                                                                          \
        {                                                                                                              \
            send_to_char("You cannot use this command from within another command.\n\r", ch);                          \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

/*
 * Character macros.
 */
#define IS_NPC(ch) (!(ch)->pcdata)
#define IS_IMMORTAL(ch) (get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_DROID(ch)                                                                                                   \
    (ch->race == RACE_DROID || ch->race == RACE_PROTOCAL_DROID || ch->race == RACE_ASSASSIN_DROID ||                   \
     ch->race == RACE_GLADIATOR_DROID || ch->race == RACE_ASTROMECH_DROID || ch->race == RACE_INTERROGATION_DROID)
#define IS_HERO(ch) (get_trust((ch)) >= LEVEL_HERO)
#define IS_AFFECTED(ch, sn) (IS_SET((ch)->affected_by, (sn)))
#define HAS_BODYPART(ch, part) ((ch)->xflags == 0 || IS_SET((ch)->xflags, (part)))

#define IS_GOOD(ch) ((ch)->alignment >= 350)
#define IS_EVIL(ch) ((ch)->alignment <= -350)
#define IS_NEUTRAL(ch) (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch) ((ch)->position > POS_SLEEPING)
#define GET_AC(ch)                                                                                                     \
    ((ch)->armor + (IS_AWAKE(ch) ? dex_app[get_curr_dex(ch)].defensive : 0) -                                          \
     ((ch)->race == RACE_DEFEL ? (ch)->skill_level[COMBAT_ABILITY] * 7 + 5 : (ch)->skill_level[COMBAT_ABILITY] / 2))
#define GET_HITROLL(ch) ((ch)->hitroll + str_app[get_curr_str(ch)].tohit + (2 - (abs((ch)->mental_state) / 10)))
#define GET_DAMROLL(ch)                                                                                                \
    ((ch)->damroll + str_app[get_curr_str(ch)].todam + (((ch)->mental_state > 5 && (ch)->mental_state < 15) ? 1 : 0))

#define IS_OUTSIDE(ch)                                                                                                 \
    (!IS_SET((ch)->in_room->room_flags, ROOM_INDOORS) && !IS_SET((ch)->in_room->room_flags, ROOM_SPACECRAFT))

#define IS_DRUNK(ch, drunk) (number_percent() < ((ch)->pcdata->condition[COND_DRUNK] * 2 / (drunk)))

#define IS_CLANNED(ch) (!IS_NPC((ch)) && (ch)->pcdata->clan)

#define WAIT_STATE(ch, npulse) ((ch)->wait = UMAX((ch)->wait, (IS_IMMORTAL(ch) ? 0 : (npulse))))

#define EXIT(ch, door) (get_exit((ch)->in_room, door))

#define CAN_GO(ch, door)                                                                                               \
    (EXIT((ch), (door)) && (EXIT((ch), (door))->to_room != NULL) && !IS_SET(EXIT((ch), (door))->exit_info, EX_CLOSED))

#define IS_VALID_SN(sn) ((sn) >= 0 && (sn) < MAX_SKILL && skill_table[(sn)] && skill_table[(sn)]->name)

#define IS_VALID_HERB(sn) ((sn) >= 0 && (sn) < MAX_HERB && herb_table[(sn)] && herb_table[(sn)]->name)

#define SPELL_FLAG(skill, flag) (IS_SET((skill)->flags, (flag)))
#define SPELL_DAMAGE(skill) (((skill)->flags) & 7)
#define SPELL_ACTION(skill) (((skill)->flags >> 3) & 7)
#define SPELL_CLASS(skill) (((skill)->flags >> 6) & 7)
#define SPELL_POWER(skill) (((skill)->flags >> 9) & 3)
#define SET_SDAM(skill, val) ((skill)->flags = ((skill)->flags & SDAM_MASK) + ((val)&7))
#define SET_SACT(skill, val) ((skill)->flags = ((skill)->flags & SACT_MASK) + (((val)&7) << 3))
#define SET_SCLA(skill, val) ((skill)->flags = ((skill)->flags & SCLA_MASK) + (((val)&7) << 6))
#define SET_SPOW(skill, val) ((skill)->flags = ((skill)->flags & SPOW_MASK) + (((val)&3) << 9))

/* Retired and guest imms. */
#define IS_RETIRED(ch) (ch->pcdata && IS_SET(ch->pcdata->flags, PCFLAG_RETIRED))
#define IS_GUEST(ch) (ch->pcdata && IS_SET(ch->pcdata->flags, PCFLAG_GUEST))

/* RIS by gsn lookups. -- Altrag.
   Will need to add some || stuff for spells that need a special GSN. */

#define IS_FIRE(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_FIRE)
#define IS_COLD(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_COLD)
#define IS_ACID(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_ACID)
#define IS_ELECTRICITY(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_ELECTRICITY)
#define IS_ENERGY(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_ENERGY)

#define IS_DRAIN(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_DRAIN)

#define IS_POISON(dt) (IS_VALID_SN(dt) && SPELL_DAMAGE(skill_table[(dt)]) == SD_POISON)

#define NOT_AUTHED(ch) (!IS_NPC(ch) && ch->pcdata->auth_state <= 3 && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED))

#define HAS_SLUG(ch) (!IS_NPC(ch) && IS_SET(ch->pcdata->flags, PCFLAG_HASSLUG))
#define IS_WAITING_FOR_AUTH(ch)                                                                                        \
    (!IS_NPC(ch) && ch->desc && ch->pcdata->auth_state == 1 && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED))

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part) (IS_SET((obj)->wear_flags, (part)))
#define IS_OBJ_STAT(obj, stat) (IS_SET((obj)->extra_flags, (stat)))

/*
 * Description macros.
 */
/* PERS WILL NOW BE HANDLED IN COMM.C
#define PERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
                ( IS_NPC(ch) ? (ch)->short_descr	\
                : (ch)->name ) : IS_IMMORTAL(ch) ? "Immortal" : "someone" )

*/

#define log_string(txt) (log_string_plus((txt), LOG_NORMAL, LEVEL_LOG))

#define MAKE_TEMP_STRING(s) TempString<sizeof(s)>(s)

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define DECLARE_DO_FUN(fun) extern "C" EXPORT DO_FUN fun
#define DECLARE_SPEC_FUN(fun) extern "C" EXPORT SPEC_FUN fun
#define DECLARE_SPELL_FUN(fun) extern "C" EXPORT SPELL_FUN fun

