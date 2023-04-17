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

#ifdef WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <SDKDDKVer.h>
#endif

#include <limits>
#include <cmath>
#include <chrono>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdarg.h>
#include <boost/asio/ip/tcp.hpp>
#include <boost/circular_buffer.hpp>

typedef int ch_ret;
typedef int obj_ret;

#ifndef WIN32
// Linux doesn't define the safe CRT functions that use template deduction, but it's easy enough
template <size_t dest_size> int sprintf_s(char (&dest)[dest_size], const char* format, ...)
{
    va_list args;

    va_start(args, format);
    int ret = vsnprintf(&dest[0], dest_size, format, args);
    va_end(args);

    return ret;
}

int sprintf_s(char* dest, size_t dest_size, const char* format, ...);

template <size_t dest_size> char* strcpy_s(char (&dest)[dest_size], const char* source)
{
    if (strlen(source) >= dest_size)
    {
        assert(0);
        return nullptr;
    }

    return strcpy(&dest[0], source);
}

char* strcpy_s(char* dest, size_t dest_size, const char* source);

template <size_t dest_size> char* strncpy_s(char (&dest)[dest_size], const char* source, size_t num)
{
    if (num >= dest_size)
    {
        assert(0);
        return nullptr;
    }

    return strncpy(&dest[0], source, num);
}

template <size_t dest_size> char* strcat_s(char (&dest)[dest_size], const char* source)
{
    if (strlen(source) + strlen(dest) >= dest_size)
    {
        assert(0);
        return nullptr;
    }

    return strcat(dest, source);
}

char* strcat_s(char* dest, size_t dest_size, const char* source);
#endif

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define DECLARE_DO_FUN(fun) extern "C" EXPORT DO_FUN fun
#define DECLARE_SPEC_FUN(fun) extern "C" EXPORT SPEC_FUN fun
#define DECLARE_SPELL_FUN(fun) extern "C" EXPORT SPELL_FUN fun

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */

// TODO this needs to go away very rapidly
#if !defined(BERR)
#define BERR 255
#endif

#if defined(_AIX)
#if !defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef short int sh_int;
#endif

/*
 * Structure types.
 */
struct AREA_DATA;
struct BUG_DATA;
struct CHAR_DATA;
struct CLAN_DATA;
struct CONTRACT_DATA;
struct EDITOR_DATA;
struct FELLOW_DATA;
struct GUARD_DATA;
struct HHF_DATA;
struct HOUR_MIN_SEC;
struct MAP_DATA;
struct MISSILE_DATA;
struct OBJ_DATA;
struct PC_DATA;
struct ROOM_INDEX_DATA;
struct SKILL_TYPE;
struct SHIP_DATA;
struct SHIP_PROTOTYPE;
struct SPACE_DATA;

/*
 * Class types.
 */
class Connection;

/*
 * Function types.
 */
typedef void DO_FUN(CHAR_DATA* ch, char* argument);
typedef bool SPEC_FUN(CHAR_DATA* ch);
typedef ch_ret SPELL_FUN(int sn, int level, CHAR_DATA* ch, void* vo);

/*
 * Template types.
 */
template <size_t N> struct TempString
{
    char contents[N];

    TempString(const char data[N])
    {
        for (size_t i = 0; i < N; i++)
        {
            contents[i] = data[i];
        }
    }

    operator char*()
    {
        return &contents[0];
    }

    size_t getSize()
    {
        return sizeof(contents) / sizeof(char);
    }
};

#define MAKE_TEMP_STRING(s) TempString<sizeof(s)>(s)

#define DUR_CONV 23.333333333333333333333333
#define HIDDEN_TILDE '*'

#define BV00 (1 << 0)
#define BV01 (1 << 1)
#define BV02 (1 << 2)
#define BV03 (1 << 3)
#define BV04 (1 << 4)
#define BV05 (1 << 5)
#define BV06 (1 << 6)
#define BV07 (1 << 7)
#define BV08 (1 << 8)
#define BV09 (1 << 9)
#define BV10 (1 << 10)
#define BV11 (1 << 11)
#define BV12 (1 << 12)
#define BV13 (1 << 13)
#define BV14 (1 << 14)
#define BV15 (1 << 15)
#define BV16 (1 << 16)
#define BV17 (1 << 17)
#define BV18 (1 << 18)
#define BV19 (1 << 19)
#define BV20 (1 << 20)
#define BV21 (1 << 21)
#define BV22 (1 << 22)
#define BV23 (1 << 23)
#define BV24 (1 << 24)
#define BV25 (1 << 25)
#define BV26 (1 << 26)
#define BV27 (1 << 27)
#define BV28 (1 << 28)
#define BV29 (1 << 29)
#define BV30 (1 << 30)
#define BV31 (1 << 31)
/* 32 USED! DO NOT ADD MORE! SB */

/*
 * String and memory management parameters.
 */
#define MAX_STRING_LENGTH 4096 /* buf */ // TODO this needs to go away
#define MAX_INPUT_LENGTH 1024            /* arg */
#define MAX_INBUF_SIZE 1024
#define MAX_OUTPUT_SIZE 4096
#define LAST_FILE_SIZE 500 // max entries in the last file
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH
#define MAX_MOB_COUNT 10

#define HASHSTR /* use string hashing */

#define MAX_LAYERS 8 /* maximum clothing layers */
#define MAX_NEST 100 /* maximum container nesting */

#define MAX_KILLTRACK 20 /* track mob vnums killed */

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_EXP_WORTH 500000
#define MIN_EXP_WORTH 25

#define MAX_REXITS 20 /* Maximum exits allowed in 1 room */
#define MAX_SKILL 282
#define MAX_ABILITY 10
#define MAX_RL_ABILITY 8
#define MAX_RACE 41
#define MAX_NPC_RACE 91
#define MAX_VNUMS 500000
#define MAX_LEVEL 36
#define MAX_CLAN 50
#define MAX_PLANET 100
#define MAX_SHIP 1000
#define MAX_SHIP_ROOMS 25
#define MAX_BOUNTY 255
#define MAX_GOV 255

#define MAX_HERB 20

#define LEVEL_HERO (MAX_LEVEL - 4)
#define LEVEL_IMMORTAL (MAX_LEVEL - 4)
#define LEVEL_SUPREME MAX_LEVEL
#define LEVEL_INFINITE (MAX_LEVEL - 4)
#define LEVEL_ETERNAL (MAX_LEVEL - 4)
#define LEVEL_IMPLEMENTOR (MAX_LEVEL)
#define LEVEL_SUB_IMPLEM (MAX_LEVEL - 1)
#define LEVEL_ASCENDANT (MAX_LEVEL - 2)
#define LEVEL_GREATER (MAX_LEVEL - 3)
#define LEVEL_LESSER (MAX_LEVEL - 4)
#define LEVEL_RETIRED (MAX_LEVEL - 5)
#define LEVEL_GOD (MAX_LEVEL - 4)
#define LEVEL_TRUEIMM (MAX_LEVEL - 4)
#define LEVEL_DEMI (MAX_LEVEL - 4)
#define LEVEL_SAVIOR (MAX_LEVEL - 4)
#define LEVEL_CREATOR (MAX_LEVEL - 4)
#define LEVEL_ACOLYTE (MAX_LEVEL - 4)
#define LEVEL_NEOPHYTE (MAX_LEVEL - 4)
#define LEVEL_AVATAR (MAX_LEVEL - 5)
#define LEVEL_NOHUNGER 30

#include "do_fun.hxx"
#include "color.hxx"

#define LEVEL_LOG LEVEL_LESSER
#define LEVEL_HIGOD LEVEL_GOD

#define OBJ_VNUM_DEED 67 /* vnum of deed */
#define VNUM_DEBIT_CARD 87
#define PULSE_PER_SECOND 4
#define PULSE_MINUTE (60 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE (3 * PULSE_PER_SECOND)
#define PULSE_MOBILE (4 * PULSE_PER_SECOND)
#define PULSE_TICK (70 * PULSE_PER_SECOND)
#define PULSE_AREA (60 * PULSE_PER_SECOND)
#define PULSE_AUCTION (10 * PULSE_PER_SECOND)
#define PULSE_SPACE (10 * PULSE_PER_SECOND)
#define PULSE_TAXES (60 * PULSE_MINUTE)
#define PULSE_ARENA (30 * PULSE_PER_SECOND)
#define PULSE_FORCE PULSE_MINUTE

/*
 * Command logging types.
 */
typedef enum
{
    LOG_NORMAL,
    LOG_ALWAYS,
    LOG_NEVER,
    LOG_BUILD,
    LOG_HIGH,
    LOG_COMM,
    LOG_ALL
} log_types;

/*
 * Return types for move_char, damage, greet_trigger, etc, etc
 * Added by Thoric to get rid of bugs
 */
typedef enum
{
    rNONE,
    rCHAR_DIED,
    rVICT_DIED,
    rBOTH_DIED,
    rCHAR_QUIT,
    rVICT_QUIT,
    rBOTH_QUIT,
    rSPELL_FAILED,
    rOBJ_SCRAPPED,
    rOBJ_EATEN,
    rOBJ_EXPIRED,
    rOBJ_TIMER,
    rOBJ_SACCED,
    rOBJ_QUAFFED,
    rOBJ_USED,
    rOBJ_EXTRACTED,
    rOBJ_DRUNK,
    rCHAR_IMMUNE,
    rVICT_IMMUNE,
    rCHAR_AND_OBJ_EXTRACTED = 128,
    rERROR = 255
} ret_types;

/* Begin new force defines */
typedef enum
{
    FORCE_INROOM,
    FORCE_ANYWHERE
} force_locations;

typedef enum
{
    FORCE_SKILL_REFRESH,
    FORCE_SKILL_FINFO,
    FORCE_SKILL_STUDENT,
    FORCE_SKILL_MASTER,
    FORCE_SKILL_IDENTIFY,
    FORCE_SKILL_PROMOTE,
    FORCE_SKILL_INSTRUCT,
    FORCE_SKILL_HEAL,
    FORCE_SKILL_PROTECT,
    FORCE_SKILL_SHIELD,
    FORCE_SKILL_WHIRLWIND,
    FORCE_SKILL_STRIKE,
    FORCE_SKILL_SQUEEZE,
    FORCE_SKILL_FORCE_LIGHTNING,
    FORCE_SKILL_DISGUISE,
    FORCE_SKILL_MAKELIGHTSABER,
    FORCE_SKILL_PARRY,
    FORCE_SKILL_FINISH,
    FORCE_SKILL_FHELP,
    FORCE_SKILL_DUALLIGHTSABER,
    FORCE_SKILL_REFLECT,
    FORCE_SKILL_CONVERT,
    FORCE_SKILL_MAKEDUALSABER,
    FORCE_SKILL_AWARENESS
} force_skills_type;

typedef enum
{
    FORCE_NONCOMBAT,
    FORCE_COMBAT,
    FORCE_NORESTRICT
} force_skill_types;

typedef enum
{
    FORCE_NONE,
    FORCE_APPRENTICE,
    FORCE_KNIGHT,
    FORCE_MASTER
} force_level_type;

typedef enum
{
    FORCE_GENERAL,
    FORCE_JEDI,
    FORCE_SITH
} force_skills_class;

#define MAX_FORCE_SKILL 24

struct FORCE_SKILL
{
    int type;
    int index;
    char* name;
    char* room_effect[5];
    char* victim_effect[5];
    char* ch_effect[5];
    int cost;
    int control;
    int alter;
    int sense;
    char* code;
    int status;
    int wait_state;
    int disabled;
    int notskill;
    int mastertrain;
    DO_FUN* do_fun;
    FORCE_SKILL* next;
    FORCE_SKILL* prev;
};

extern FORCE_SKILL* first_force_skill;
extern FORCE_SKILL* last_force_skill;

#define MAX_FORCE_ALIGN 100
#define MIN_FORCE_ALIGN -100

struct FORCE_HELP
{
    char* name;
    int status;
    int type;
    char* desc;
    int skill;
    FORCE_HELP* next;
    FORCE_HELP* prev;
};

extern FORCE_HELP* first_force_help;
extern FORCE_HELP* last_force_help;

/* End force defines */

/* Echo types for echo_to_all */
#define ECHOTAR_ALL 0
#define ECHOTAR_PC 1
#define ECHOTAR_IMM 2

/* defines for new do_who */
#define WT_MORTAL 0
#define WT_IMM 2
#define WT_AVATAR 1
#define WT_NEWBIE 3

/*
 * do_who output structure -- Narn
 */
struct WHO_DATA
{
    WHO_DATA* prev;
    WHO_DATA* next;
    char* text;
    int type;
};

/*
 * Site ban structure.
 */
struct BAN_DATA
{
    BAN_DATA* next;
    BAN_DATA* prev;
    char* name;
    int level;
    char* ban_time;
};

/*
 * Time and weather stuff.
 */
typedef enum
{
    SUN_DARK,
    SUN_RISE,
    SUN_LIGHT,
    SUN_SET
} sun_positions;

typedef enum
{
    SKY_CLOUDLESS,
    SKY_CLOUDY,
    SKY_RAINING,
    SKY_LIGHTNING
} sky_conditions;

struct TIME_INFO_DATA
{
    int hour;
    int day;
    int month;
    int year;
};

struct HOUR_MIN_SEC
{
    int hour;
    int min;
    int sec;
    int manual;
};

struct WEATHER_DATA
{
    int mmhg;
    int change;
    int sky;
    int sunlight;
};

/*
 * Structure used to build wizlist
 */
struct WIZENT
{
    WIZENT* next;
    WIZENT* last;
    char* name;
    sh_int level;
};

/*
 * Connected state for a channel.
 */
typedef enum
{
    CON_PLAYING,
    CON_GET_NAME,
    CON_GET_OLD_PASSWORD,
    CON_CONFIRM_NEW_NAME,
    CON_GET_NEW_PASSWORD,
    CON_CONFIRM_NEW_PASSWORD,
    CON_GET_NEW_SEX,
    CON_READ_MOTD,
    CON_GET_NEW_RACE,
    CON_GET_EMULATION,
    CON_EDITING,
    CON_MAIL_BEGIN,
    CON_MAIN_MAIL_MENU,
    CON_MAIL_DISPLAY,
    CON_MAIL_WRITE_START,
    CON_MAIL_WRITE_SUBJECT,
    CON_MAIL_WRITE_TO,
    CON_GET_WANT_RIPANSI,
    CON_TITLE,
    CON_PRESS_ENTER,
    CON_WAIT_1,
    CON_WAIT_2,
    CON_WAIT_3,
    CON_ACCEPTED,
    CON_GET_PKILL,
    CON_READ_IMOTD,
    CON_GET_NEW_EMAIL,
    CON_GET_MSP,
    CON_GET_NEW_CLASS,
    CON_GET_NEW_SECOND,
    CON_ROLL_STATS,
    CON_STATS_OK,
    CON_COPYOVER_RECOVER,
    CON_GET_PUEBLO,
    CON_GET_HEIGHT,
    CON_GET_BUILD,
    CON_GET_DROID
} connection_types;

/*
 * Character substates
 */
typedef enum
{
    SUB_NONE,
    SUB_PAUSE,
    SUB_PERSONAL_DESC,
    SUB_OBJ_SHORT,
    SUB_OBJ_LONG,
    SUB_OBJ_EXTRA,
    SUB_MOB_LONG,
    SUB_MOB_DESC,
    SUB_ROOM_DESC,
    SUB_ROOM_EXTRA,
    SUB_ROOM_EXIT_DESC,
    SUB_WRITING_NOTE,
    SUB_MPROG_EDIT,
    SUB_HELP_EDIT,
    SUB_WRITING_MAP,
    SUB_PERSONAL_BIO,
    SUB_REPEATCMD,
    SUB_RESTRICTED,
    SUB_DEITYDESC,
    SUB_SHIPDESC,
    SUB_FORCE_CH0,
    SUB_FORCE_CH1,
    SUB_FORCE_CH2,
    SUB_FORCE_CH3,
    SUB_FORCE_CH4,
    SUB_FORCE_ROOM0,
    SUB_FORCE_ROOM1,
    SUB_FORCE_ROOM2,
    SUB_FORCE_ROOM3,
    SUB_FORCE_ROOM4,
    SUB_FORCE_VICTIM0,
    SUB_FORCE_VICTIM1,
    SUB_FORCE_VICTIM2,
    SUB_FORCE_VICTIM3,
    SUB_FORCE_VICTIM4,
    SUB_FORCE_HELP,
    SUB_SLAYCMSG,
    SUB_SLAYVMSG,
    SUB_SLAYRMSG,

    /* timer types ONLY below this point */
    SUB_TIMER_DO_ABORT = 128,
    SUB_TIMER_CANT_ABORT
} char_substates;

/*
 * Descriptor (channel) structure.
 */
struct DESCRIPTOR_DATA
{
    DESCRIPTOR_DATA* snoop_by;
    std::shared_ptr<Connection> connection;
    CHAR_DATA* character;
    CHAR_DATA* original;
    sh_int connected;
    sh_int idle;
    sh_int lines;
    sh_int scrlen;
    bool fcommand;                 // true if the user ran a command this pulse
    char inlast[MAX_INPUT_LENGTH]; // the last line that was run as a command
    int repeat;
    char* pagebuf;
    unsigned long pagesize;
    int pagetop;
    char* pagepoint;
    char pagecolor;
    char* user;
    int atimes;
    int newstate;
    unsigned char prevcolor;

    ~DESCRIPTOR_DATA();
};

/*
 * Attribute bonus structures.
 */
struct STR_APP_TYPE
{
    sh_int tohit;
    sh_int todam;
    sh_int carry;
    sh_int wield;
};

struct INT_APP_TYPE
{
    sh_int learn;
};

struct WIS_APP_TYPE
{
    sh_int practice;
};

struct DEX_APP_TYPE
{
    sh_int defensive;
};

struct CON_APP_TYPE
{
    sh_int hitp;
    sh_int shock;
};

struct CHA_APP_TYPE
{
    sh_int charm;
};

struct LCK_APP_TYPE
{
    sh_int luck;
};

struct FRC_APP_TYPE
{
    sh_int force;
};

/* ability classes */

#define ABILITY_NONE -1
#define COMBAT_ABILITY 0
#define PILOTING_ABILITY 1
#define ENGINEERING_ABILITY 2
#define HUNTING_ABILITY 3
#define SMUGGLING_ABILITY 4
/*#define DIPLOMACY_ABILITY	5
#define LEADERSHIP_ABILITY	6*/  /* Gonna replace the diplomacy and leadership abilities and make them POLITICIANs */
#define POLITICIAN_ABILITY 5
#define FORCE_ABILITY 6
#define SLICER_ABILITY 7
#define ASSASSIN_ABILITY 8
#define TECHNICIAN_ABILITY 9

/* the races */
#define RACE_HUMAN 0
#define RACE_WOOKIEE 1
#define RACE_TWI_LEK 2
#define RACE_RODIAN 3
#define RACE_HUTT 4
#define RACE_MON_CALAMARI 5
#define RACE_NOGHRI 6
#define RACE_GAMORREAN 7
#define RACE_JAWA 8
#define RACE_ADARIAN 9
#define RACE_EWOK 10
#define RACE_VERPINE 11
#define RACE_DEFEL 12
#define RACE_TRANDOSHAN 13
#define RACE_HAPAN 14
#define RACE_QUARREN 15
#define RACE_SHISTAVANEN 16
#define RACE_FALLEEN 17
#define RACE_ITHORIAN 18
#define RACE_DEVARONIAN 19
#define RACE_GOTAL 20
#define RACE_DROID 21
#define RACE_FIRRERREO 22
#define RACE_BARABEL 23
#define RACE_BOTHAN 24
#define RACE_TOGORIAN 25
#define RACE_DUG 26
#define RACE_KUBAZ 27
#define RACE_SELONIAN 28
#define RACE_GRAN 29
#define RACE_YEVETHA 30
#define RACE_GAND 31
#define RACE_DUROS 32
#define RACE_COYNITE 33
#define RACE_SULLUSTAN 34
#define RACE_PROTOCAL_DROID 35
#define RACE_ASSASSIN_DROID 36
#define RACE_GLADIATOR_DROID 37
#define RACE_ASTROMECH_DROID 38
#define RACE_INTERROGATION_DROID 39

/*
 * Languages -- Altrag
 */
#define LANG_BASIC BV00 /* Human base language */
#define LANG_WOOKIEE BV01
#define LANG_TWI_LEK BV02
#define LANG_RODIAN BV03
#define LANG_HUTT BV04
#define LANG_MON_CALAMARI BV05
#define LANG_NOGHRI BV06
#define LANG_EWOK BV07
#define LANG_ITHORIAN BV08
#define LANG_GOTAL BV09
#define LANG_DEVARONIAN BV10
#define LANG_BINARY BV11
#define LANG_FIRRERREO BV12
#define LANG_CLAN BV13
#define LANG_GAMORREAN BV14
#define LANG_TOGORIAN BV15
#define LANG_SHISTAVANEN BV16
#define LANG_JAWA BV17
#define LANG_KUBAZ BV18
#define LANG_ADARIAN BV19
#define LANG_VERPINE BV20
#define LANG_DEFEL BV21
#define LANG_TRANDOSHAN BV22
#define LANG_HAPAN BV23
#define LANG_QUARREN BV24
#define LANG_SULLUSTAN BV25
#define LANG_FALLEEN BV26
#define LANG_BARABEL BV27
#define LANG_YEVETHAN BV28
#define LANG_GAND BV29
#define LANG_DUROS BV30
#define LANG_COYNITE BV31
#define LANG_UNKNOWN 0 /* Anything that doesnt fit a category */
#define VALID_LANGS                                                                                                    \
    (LANG_BASIC | LANG_WOOKIEE | LANG_TWI_LEK | LANG_RODIAN | LANG_HUTT | LANG_MON_CALAMARI | LANG_NOGHRI |            \
     LANG_GAMORREAN | LANG_JAWA | LANG_ADARIAN | LANG_EWOK | LANG_VERPINE | LANG_DEFEL | LANG_TRANDOSHAN |             \
     LANG_HAPAN | LANG_QUARREN | LANG_SULLUSTAN | LANG_BINARY | LANG_FIRRERREO | LANG_CLAN | LANG_TOGORIAN |           \
     LANG_SHISTAVANEN | LANG_KUBAZ | LANG_YEVETHAN | LANG_GAND | LANG_DUROS | LANG_COYNITE | LANG_GOTAL |              \
     LANG_DEVARONIAN | LANG_FALLEEN | LANG_ITHORIAN | LANG_BARABEL)
/*  32 Languages */

/*
 * TO types for act.
 */
#define TO_ROOM 0
#define TO_NOTVICT 1
#define TO_VICT 2
#define TO_CHAR 3
#define TO_MUD 4

#define INIT_WEAPON_CONDITION 12
#define MAX_ITEM_IMPACT 30

/*
 * Help table types.
 */
struct HELP_DATA
{
    HELP_DATA* next;
    HELP_DATA* prev;
    sh_int level;
    char* keyword;
    char* text;
};

/*
 * Shop types.
 */
#define MAX_TRADE 5

struct SHOP_DATA
{
    SHOP_DATA* next;            /* Next shop in list		*/
    SHOP_DATA* prev;            /* Previous shop in list	*/
    int keeper;                 /* Vnum of shop keeper mob	*/
    sh_int buy_type[MAX_TRADE]; /* Item types shop will buy	*/
    sh_int profit_buy;          /* Cost multiplier for buying	*/
    sh_int profit_sell;         /* Cost multiplier for selling	*/
    sh_int open_hour;           /* First opening hour		*/
    sh_int close_hour;          /* First closing hour		*/
};

#define MAX_FIX 3
#define SHOP_FIX 1
#define SHOP_RECHARGE 2

struct REPAIR_DATA
{
    REPAIR_DATA* next;        /* Next shop in list		*/
    REPAIR_DATA* prev;        /* Previous shop in list	*/
    int keeper;               /* Vnum of shop keeper mob	*/
    sh_int fix_type[MAX_FIX]; /* Item types shop will fix	*/
    sh_int profit_fix;        /* Cost multiplier for fixing	*/
    sh_int shop_type;         /* Repair shop type		*/
    sh_int open_hour;         /* First opening hour		*/
    sh_int close_hour;        /* First closing hour		*/
};

/* Mob program structures */

/* Mob program structures and defines */
/* Moved these defines here from mud_prog.c as I need them -rkb */
#define MAX_IFS 20 /* should always be generous */
#define IN_IF 0
#define IN_ELSE 1
#define DO_IF 2
#define DO_ELSE 3

#define MAX_PROG_NEST 20

struct ACT_PROG_DATA
{
    ACT_PROG_DATA* next;
    void* vo;
};

struct MPROG_ACT_LIST
{
    MPROG_ACT_LIST* next;
    char* buf;
    CHAR_DATA* ch;
    OBJ_DATA* obj;
    void* vo;
};

struct MPROG_DATA
{
    MPROG_DATA* next;
    int type;
    bool triggered;
    int resetdelay;
    char* arglist;
    char* comlist;
};

/* Used to store sleeping mud progs. -rkb */
typedef enum
{
    MP_MOB,
    MP_ROOM,
    MP_OBJ
} mp_types;

struct MPSLEEP_DATA
{
    MPSLEEP_DATA* next;
    MPSLEEP_DATA* prev;

    int timer;             /* Pulses to sleep */
    mp_types type;         /* Mob, Room or Obj prog */
    ROOM_INDEX_DATA* room; /* Room when type is MP_ROOM */

    /* mprog_driver state variables */
    int ignorelevel;
    int iflevel;
    bool ifstate[MAX_IFS][DO_ELSE];

    /* mprog_driver arguments */
    char* com_list;
    CHAR_DATA* mob;
    CHAR_DATA* actor;
    OBJ_DATA* obj;
    void* vo;
    bool single_step;
};

extern bool MOBtrigger;

/* race dedicated stuff */
struct RACE_TYPE
{
    char race_name[16]; /* Race name			*/
    int affected;       /* Default affect bitvectors	*/
    sh_int str_plus;    /* Str bonus/penalty		*/
    sh_int dex_plus;    /* Dex      "			*/
    sh_int wis_plus;    /* Wis      "			*/
    sh_int int_plus;    /* Int      "			*/
    sh_int con_plus;    /* Con      "			*/
    sh_int cha_plus;    /* Cha      "			*/
    sh_int lck_plus;    /* Lck 	    "			*/
    sh_int frc_plus;    /* Frc 	    "			*/
    sh_int hit;
    sh_int mana;
    sh_int resist;
    sh_int suscept;
    int class_restriction; /* Flags for illegal classes	*/
    int language;          /* Default racial language      */
};

typedef enum
{
    CLAN_PLAIN,
    CLAN_CRIME,
    CLAN_GUILD,
    CLAN_SUBCLAN,
    CLAN_CORPORATION
} clan_types;

typedef enum
{
    PLAYER_SHIP,
    MOB_SHIP,
    CLAN_SHIP
} ship_types;
typedef enum
{
    SHIP_DOCKED,
    SHIP_READY,
    SHIP_BUSY,
    SHIP_BUSY_2,
    SHIP_BUSY_3,
    SHIP_REFUEL,
    SHIP_LAUNCH,
    SHIP_LAUNCH_2,
    SHIP_LAND,
    SHIP_LAND_2,
    SHIP_HYPERSPACE,
    SHIP_DISABLED,
    SHIP_FLYING
} ship_states;
typedef enum
{
    MISSILE_READY,
    MISSILE_FIRED,
    MISSILE_RELOAD,
    MISSILE_RELOAD_2,
    MISSILE_DAMAGED
} missile_states;

typedef enum
{
    LAND_VEHICLE,
    SHIP_FIGHTER,
    SHIP_BOMBER,
    SHIP_SHUTTLE,
    SHIP_FREIGHTER,
    SHIP_FRIGATE,
    SHIP_TT,
    SHIP_CORVETTE,
    SHIP_CRUISER,
    SHIP_DREADNAUGHT,
    SHIP_DESTROYER,
    SHIP_SPACE_STATION
} ship_classes;

typedef enum
{
    // 0         1            2           3          4            5
    B_NONE,
    SINGLE_LASER,
    DUAL_LASER,
    TRI_LASER,
    QUAD_LASER,
    AUTOBLASTER,

    //   6           7           8            9
    HEAVY_LASER,
    LIGHT_ION,
    REPEATING_ION,
    HEAVY_ION
} beam_types;

typedef enum
{
    CONCUSSION_MISSILE,
    PROTON_TORPEDO,
    HEAVY_ROCKET,
    HEAVY_BOMB
} missile_types;

#define LASER_DAMAGED -1
#define LASER_READY 0

#define CARGO_NONE 0
#define CARGO_FOOD 1
#define CARGO_WATER 2
#define CARGO_MEDICAL 3
#define CARGO_METALS 4
#define CARGO_RARE_METALS 5
#define CARGO_ELECTRONICS 6
#define CARGO_PRODUCTS 7
#define CARGO_MAX 8

struct PLANET_DATA
{
    PLANET_DATA* next;
    PLANET_DATA* prev;
    PLANET_DATA* next_in_system;
    PLANET_DATA* prev_in_system;
    AREA_DATA* next_in_area;
    AREA_DATA* prev_in_area;
    GUARD_DATA* first_guard;
    AREA_DATA* first_area;
    AREA_DATA* last_area;
    GUARD_DATA* last_guard;
    SPACE_DATA* starsystem;
    AREA_DATA* area;
    char* name;
    char* filename;
    CLAN_DATA* governed_by;
    int population;
    float pop_support;
    int sector;
    int x, y, z;
    int size;
    bool flags;
    long base_value;
    int citysize;
    int wilderness;
    int wildlife;
    int farmland;
    int barracks;

    int price[CARGO_MAX];

    int controls;
};

struct CARGO_DATA
{
    int cargo0;
    int cargo1;
    int cargo2;
    int cargo3;
    int cargo4;
    int cargo5;
    int cargo6;
    int cargo7;
    int cargo8;
    int cargo9;
    int orgcargo0;
    int orgcargo1;
    int orgcargo2;
    int orgcargo3;
    int orgcargo4;
    int orgcargo5;
    int orgcargo6;
    int orgcargo7;
    int orgcargo8;
    int orgcargo9;
    int price0;
    int price1;
    int price2;
    int price3;
    int price4;
    int price5;
    int price6;
    int price7;
    int price8;
    int price9;
    bool smug;
};

struct SPACE_DATA
{
    SPACE_DATA* next;
    SPACE_DATA* prev;
    SHIP_DATA* first_ship;
    SHIP_DATA* last_ship;
    MISSILE_DATA* first_missile;
    MISSILE_DATA* last_missile;
    PLANET_DATA* first_planet;
    PLANET_DATA* last_planet;
    char* filename;
    char* name;
    char* star1;
    char* star2;
    char* planet1;
    char* planet2;
    char* planet3;
    char* location1a;
    char* location2a;
    char* location3a;
    char* location1b;
    char* location2b;
    char* location3b;
    char* location1c;
    char* location2c;
    char* location3c;
    int xpos;
    int ypos;
    int zpos;
    int s1x;
    int s1y;
    int s1z;
    int s2x;
    int s2y;
    int s2z;
    int doc1a;
    int doc2a;
    int doc3a;
    int doc1b;
    int doc2b;
    int doc3b;
    int doc1c;
    int doc2c;
    int doc3c;
    bool seca;
    bool secb;
    bool secc;
    int p1x;
    bool trainer;
    int p1y;
    int p1z;
    int p2x;
    int p2y;
    int p2z;
    int p3x;
    int p3y;
    int p3z;
    int gravitys1;
    int gravitys2;
    int gravityp1;
    int gravityp2;
    int gravityp3;
    int p1_low;
    int p1_high;
    int p2_low;
    int p2_high;
    int p3_low;
    int p3_high;
    int crash;
};

struct BOUNTY_DATA
{
    BOUNTY_DATA* next;
    BOUNTY_DATA* prev;
    char* target;
    long int amount;
};

struct BMARKET_DATA
{
    BMARKET_DATA* next;
    BMARKET_DATA* prev;
    char* filename;
    int quantity;
};

struct GUARD_DATA
{
    GUARD_DATA* next;
    GUARD_DATA* prev;
    GUARD_DATA* next_on_planet;
    GUARD_DATA* prev_on_planet;
    CHAR_DATA* mob;
    ROOM_INDEX_DATA* reset_loc;
    PLANET_DATA* planet;
};

struct SENATE_DATA
{
    SENATE_DATA* next;
    SENATE_DATA* prev;
    char* name;
};

#define PLANET_NOCAPTURE BV00

struct CLAN_DATA
{
    CLAN_DATA* next; /* next clan in list			*/
    CLAN_DATA* prev; /* previous clan in list		*/
    CLAN_DATA* next_subclan;
    CLAN_DATA* prev_subclan;
    CLAN_DATA* first_subclan;
    CLAN_DATA* last_subclan;
    CLAN_DATA* mainclan;
    char* acro;
    char* filename;    /* Clan filename			*/
    char* shortname;   /* Clan shortname - used in member lists*/
    char* name;        /* Clan name				*/
    char* description; /* A brief description of the clan	*/
    char* adjective; // the adjective to show on 'shiplist' ex. "Rebel" Starfighter, "Imperial" Death Cruiser, that sort
                     // of thing
    char* leader;    /* Head clan leader			*/
    char* number1;   /* First officer			*/
    char* number2;   /* Second officer			*/
    int pkills;      /* Number of pkills on behalf of clan	*/
    int pdeaths;     /* Number of pkills against clan	*/
    int mkills;      /* Number of mkills on behalf of clan	*/
    int mdeaths;     /* Number of clan deaths due to mobs	*/
    sh_int clan_type; /* See clan type defines		*/
    char* atwar;      /* Clan name				*/
    sh_int members;   /* Number of clan members		*/
    int board;        /* Vnum of clan board			*/
    int storeroom;    /* Vnum of clan's store room		*/
    int guard1;       /* Vnum of clan guard type 1		*/
    int guard2;       /* Vnum of clan guard type 2		*/
    int patrol1;      /* vnum of patrol */
    int patrol2;      /* vnum of patrol */
    int trooper1;     /* vnum of reinforcements */
    int trooper2;     /* vnum of elite troopers */
    long int funds;
    int spacecraft;
    int troops;
    int vehicles;
    int jail;
    char* tmpstr;
};

struct TURRET_DATA
{
    TURRET_DATA* next;
    TURRET_DATA* prev;
    ROOM_INDEX_DATA* room;
    SHIP_DATA* target;
    sh_int laserstate;
};

struct HANGER_DATA
{
    HANGER_DATA* next;
    HANGER_DATA* prev;
    ROOM_INDEX_DATA* room;
    bool bayopen;
    int type;
};

struct MODULE_DATA
{
    MODULE_DATA* next;
    MODULE_DATA* prev;
    int affect;  // What item is it going to affect.
    int ammount; // How much is it going to affect it.
};
struct SHIP_DATA
{
    SHIP_DATA* next;
    SHIP_DATA* prev;
    SHIP_DATA* next_in_starsystem;
    SHIP_DATA* prev_in_starsystem;
    SHIP_DATA* next_in_room;
    SHIP_DATA* prev_in_room;
    ROOM_INDEX_DATA* in_room;
    SPACE_DATA* starsystem;
    SHIP_DATA* inship;
    char* filename;
    char* name;
    char* protoname;
    char* clanowner;
    char* home;
    char* description;
    char* owner;
    char* pilot;
    char* copilot;
    char* dest;
    char* pbeacon;
    sh_int type;
    sh_int clazz;
    sh_int comm;
    int cost;
    sh_int sensor;
    sh_int astro_array;
    sh_int hyperspeed;
    int hyperdistance;
    sh_int realspeed;
    sh_int currspeed;
    sh_int shipstate;
    sh_int hyperstate;
    sh_int slave;

    /* New ship shit by || && Tawnos for FotE */

    bool juking;
    bool rolling;
    sh_int primaryState;   // (was statet0) Primary beam state   (Damaged/charging)
    sh_int secondaryState; // (was statet0i) Secondary beam state

    sh_int primaryType;   // Primary weapon type, defined in beam_types
    sh_int secondaryType; // Secondary weapon type, defined in beam_types

    bool primaryLinked;   // Linked fire, if !single can fire all available (up to 4 at once)
    bool secondaryLinked; // Linked fire, if !single will fire all available
    bool warheadLinked;   // Linked fire, if !single will fire all available

    sh_int primaryCount;   // (was lasers) Number of primaries
    sh_int secondaryCount; // (was ions) Number of secondaries

    sh_int statet1; // Begin turbolaser turret states
    sh_int statet2;
    sh_int statet3;
    sh_int statet4;
    sh_int statet5;
    sh_int statet6;
    sh_int statet7;
    sh_int statet8;
    sh_int statet9;
    sh_int statet10; // End turbolaser turret states

    sh_int missiletype;

    sh_int missilestate;
    sh_int torpedostate;
    sh_int rocketstate;

    sh_int bombs;
    sh_int maxbombs;
    sh_int alarm;
    sh_int missiles;
    sh_int maxmissiles;
    sh_int torpedos;
    sh_int maxtorpedos;
    sh_int rockets;
    sh_int maxrockets;
    sh_int maxmod;
    sh_int tractorbeam;
    sh_int manuever;
    bool bayopen;
    bool hatchopen;
    bool autorecharge;
    bool autotrack;
    bool autospeed;
    float vx, vy, vz;
    float hx, hy, hz;
    float jx, jy, jz;
    int maxenergy;
    int energy;
    int shield;
    int maxshield;
    int hull;
    int maxhull;
    int cockpit;
    int turret1;
    int turret2;
    int turret3;
    int turret4;
    int turret5;
    int turret6;
    int turret7;
    int turret8;
    int turret9;
    int turret10;
    int location;
    int lastdoc;
    int shipyard;
    int entrance;
    int engineroom;
    int firstroom;
    int lastroom;
    int navseat;
    int pilotseat;
    int coseat;
    int gunseat;
    long collision;
    SHIP_DATA* target0;
    SHIP_DATA* target1;
    SHIP_DATA* target2;
    SHIP_DATA* target3;
    SHIP_DATA* target4;
    SHIP_DATA* target5;
    SHIP_DATA* target6;
    SHIP_DATA* target7;
    SHIP_DATA* target8;
    SHIP_DATA* target9;
    SHIP_DATA* target10;
    SPACE_DATA* currjump;
    sh_int chaff;
    sh_int maxchaff;
    unsigned char chaff_released;
    bool autopilot;
    int channel;
    int password;
    int flags;
    MODULE_DATA* first_module;
    MODULE_DATA* last_module;
    sh_int maxmods;
    TURRET_DATA* first_turret;
    TURRET_DATA* last_turret;
    int hanger1;
    int hanger2;
    int hanger3;
    int hanger4;
    int exlocation;
    int sim_vnum;
    int max_modules;
    int baycode;
    int hibombstr;
    int lowbombstr;
    SHIP_DATA* tractored_by;
    SHIP_DATA* tractoring;
    int upeng;
    int maxupeng;
    int upengint;
    int hyperinstallable;
    int uphull;
    int uphullint;
    int uphullmax;
    int uppcount;
    int uppcountmax;
    int upptype;
    int upptypemax;
    int upscount;
    int upscountmax;
    int upstype;
    int upstypemax;
    int upengcost;
    int hypercost;
    int uphullcost;
    int uppcountcost;
    int upptypecost;
    int upscountcost;
    int upstypecost;
    int tractorinstallable;
    int tractorcost;
    int upshieldmax;
    int upshieldint;
    int upshieldcost;
    int upshield;
    int upenergymax;
    int upenergyint;
    int upenergycost;
    int upenergy;

    int maxcargo;
    int cargo;
    int cargotype;
};

struct MISSILE_DATA
{
    MISSILE_DATA* next;
    MISSILE_DATA* prev;
    MISSILE_DATA* next_in_starsystem;
    MISSILE_DATA* prev_in_starsystem;
    SPACE_DATA* starsystem;
    SHIP_DATA* target;
    SHIP_DATA* fired_from;
    char* fired_by;
    sh_int missiletype;
    sh_int age;
    int speed;
    int mx, my, mz;
};

struct TOURNEY_DATA
{
    int open;
    int low_level;
    int hi_level;
};

/*
 * Data structure for notes.
 */
struct NOTE_DATA
{
    NOTE_DATA* next;
    NOTE_DATA* prev;
    char* sender;
    char* date;
    char* to_list;
    char* subject;
    int voting;
    char* yesvotes;
    char* novotes;
    char* abstentions;
    char* text;
};

struct BOARD_DATA
{
    BOARD_DATA* next;        /* Next board in list		   */
    BOARD_DATA* prev;        /* Previous board in list	   */
    NOTE_DATA* first_note;   /* First note on board		   */
    NOTE_DATA* last_note;    /* Last note on board		   */
    char* note_file;         /* Filename to save notes to	   */
    char* read_group;        /* Can restrict a board to a       */
    char* post_group;        /* council, clan, guild etc        */
    char* extra_readers;     /* Can give read rights to players */
    char* extra_removers;    /* Can give remove rights to players */
    int board_obj;           /* Vnum of board object		   */
    sh_int num_posts;        /* Number of notes on this board   */
    sh_int min_read_level;   /* Minimum level to read a note	   */
    sh_int min_post_level;   /* Minimum level to post a note    */
    sh_int min_remove_level; /* Minimum level to remove a note  */
    sh_int max_posts;        /* Maximum amount of notes allowed */
    int type;                /* Normal board or mail board? */
};

/*
 * An affect.
 */
struct AFFECT_DATA // TODO effect?
{
    AFFECT_DATA* next;
    AFFECT_DATA* prev;
    sh_int type;
    int duration;
    sh_int location;
    int modifier;
    int bitvector;
};

/*
 * A SMAUG spell
 */
struct SMAUG_AFF
{
    SMAUG_AFF* next;
    char* duration;
    sh_int location;
    char* modifier;
    int bitvector;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_ANIMATED_CORPSE 5
#define MOB_VNUM_POLY_WOLF 10

#define MOB_VNUM_STORMTROOPER 20
#define MOB_VNUM_IMP_GUARD 21
#define MOB_VNUM_NR_GUARD 22
#define MOB_VNUM_NR_TROOPER 23
#define MOB_VNUM_MERCINARY 24
#define MOB_VNUM_BOUNCER 25
#define MOB_VNUM_IMP_ELITE 26
#define MOB_VNUM_IMP_PATROL 27
#define MOB_VNUM_IMP_FORCES 28
#define MOB_VNUM_NR_ELITE 29
#define MOB_VNUM_NR_PATROL 30
#define MOB_VNUM_NR_FORCES 31
#define MOB_VNUM_MERC_ELITE 32
#define MOB_VNUM_MERC_PATROL 33
#define MOB_VNUM_MERC_FORCES 34
#define MOB_VNUM_SHIP_GUARD 35

/* Ship Flags */
#define SHIP_NOHIJACK BV00
#define SHIP_SHIELD_BOOST BV01
#define SHIP_TORP_BOOST BV02
#define SHIP_CHAFF_BOOST BV03
#define SHIP_HULL_BOOST BV04
#define SHIP_LASER_BOOST BV05
#define SHIP_MISSILE_BOOST BV06
#define SHIP_ROCKET_BOOST BV07
#define SHIP_SIMULATOR BV08
#define SHIP_NODESTROY BV09
#define SHIP_NOSLICER BV10
#define XSHIP_ION_LASERS BV11
#define XSHIP_ION_DRIVE BV12
#define XSHIP_ION_ION BV13
#define XSHIP_ION_TURRET1 BV14
#define XSHIP_ION_TURRET2 BV15
#define XSHIP_ION_TURRET3 BV16
#define XSHIP_ION_TURRET4 BV17
#define XSHIP_ION_TURRET5 BV18
#define XSHIP_ION_TURRET6 BV19
#define XSHIP_ION_TURRET7 BV20
#define XSHIP_ION_TURRET8 BV21
#define XSHIP_ION_TURRET9 BV22
#define XSHIP_ION_TURRET10 BV23
#define SHIP_RESPAWN BV24
#define XSHIP_ION_HYPER BV25
#define XSHIP_ION_MISSILES BV26
#define SHIP_CLOAK BV27

#define SHIP_DAMAGE_DRIVE BV00
#define SHIP_DAMAGE_HYPERDRIVE BV01
#define SHIP_DAMAGE_LASER BV02
#define SHIP_DAMAGE_ION BV03
#define SHIP_DAMAGE_TURRET1 BV04
#define SHIP_DAMAGE_TURRET2 BV05
#define SHIP_DAMAGE_TURRET3 BV06
#define SHIP_DAMAGE_TURRET4 BV07
#define SHIP_DAMAGE_TURRET5 BV08
#define SHIP_DAMAGE_TURRET6 BV09
#define SHIP_DAMAGE_TURRET7 BV10
#define SHIP_DAMAGE_TURRET8 BV11
#define SHIP_DAMAGE_TURRET9 BV12
#define SHIP_DAMAGE_TURRET10 BV13
#define SHIP_DAMAGE_SHIELD BV14
#define SHIP_DAMAGE_PLASMASHIELD BV15
#define SHIP_DAMAGE_LIFESUPPORT BV16
#define SHIP_DAMAGE_MISSILE BV17

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC BV00     /* Auto set for mobs	*/
#define ACT_SENTINEL BV01   /* Stays in one room	*/
#define ACT_SCAVENGER BV02  /* Picks up objects	*/
#define ACT_NOFLEE BV03     /* Mobs don't flee. -T  */
#define ACT_AGGRESSIVE BV05 /* Attacks PC's		*/
#define ACT_STAY_AREA BV06  /* Won't leave area	*/
#define ACT_WIMPY BV07      /* Flees when hurt	*/
#define ACT_PET BV08        /* Auto set for pets	*/
#define ACT_TRAIN BV09      /* Can train PC's	*/
#define ACT_PRACTICE BV10   /* Can practice PC's	*/
#define ACT_IMMORTAL BV11   /* Cannot be killed	*/
#define ACT_DEADLY BV12     /* Has a deadly poison  */
#define ACT_POLYSELF BV13
#define ACT_META_AGGR BV14   /* Extremely aggressive */
#define ACT_GUARDIAN BV15    /* Protects master	*/
#define ACT_RUNNING BV16     /* Hunts quickly	*/
#define ACT_NOWANDER BV17    /* Doesn't wander	*/
#define ACT_MOUNTABLE BV18   /* Can be mounted	*/
#define ACT_MOUNTED BV19     /* Is mounted		*/
#define ACT_SCHOLAR BV20     /* Can teach languages  */
#define ACT_SECRETIVE BV21   /* actions aren't seen	*/
#define ACT_POLYMORPHED BV22 /* Mob is a ch		*/
#define ACT_MOBINVIS BV23    /* Like wizinvis	*/
#define ACT_NOASSIST BV24    /* Doesn't assist mobs	*/
#define ACT_NOKILL BV25      /* Mob can't die */
#define ACT_DROID BV26       /* mob is a droid */
#define ACT_NOCORPSE BV27
#define ACT_PUEBLO BV28    /* This is the pueblo flag */
#define ACT_PROTOTYPE BV30 /* A prototype mob	*/

/* Act2 Flags */
#define ACT_BOUND BV00  /* This is the bind flag */
#define ACT_EXEMPT BV01 /* Makes a player exampt from pfile deletion */
#define ACT_JEDI BV02   /* This is a light jedi */
#define ACT_SITH BV03   /* This is a dark jedi */
#define ACT_GAGGED BV04 /* This is a gagged flag */
/* 21 acts */

/* bits for vip flags */

#define VIP_CORUSCANT BV00
#define VIP_YAVIN_IV BV01
#define VIP_TATOOINE BV02
#define VIP_KASHYYYK BV03
#define VIP_MON_CALAMARI BV04
#define VIP_ENDOR BV05
#define VIP_ORD_MANTELL BV06
#define VIP_NAL_HUTTA BV07
#define VIP_CORELLIA BV08
#define VIP_BAKURA BV09

/* player wanted bits */

#define WANTED_CORUSCANT VIP_CORUSCANT
#define WANTED_YAVIN_IV VIP_YAVIN_IV
#define WANTED_TATOOINE VIP_TATOOINE
#define WANTED_KASHYYYK VIP_KASHYYYK
#define WANTED_MON_CALAMARI VIP_MON_CALAMARI
#define WANTED_ENDOR VIP_ENDOR
#define WANTED_ORD_MANTELL VIP_ORD_MANTELL
#define WANTED_NAL_HUTTA VIP_NAL_HUTTA
#define WANTED_CORELLIA VIP_CORELLIA
#define WANTED_BAKURA VIP_BAKURA

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_NONE 0

#define AFF_BLIND BV00
#define AFF_INVISIBLE BV01
#define AFF_DETECT_EVIL BV02
#define AFF_DETECT_INVIS BV03
#define AFF_DETECT_MAGIC BV04
#define AFF_DETECT_HIDDEN BV05
#define AFF_WEAKEN BV06
#define AFF_SANCTUARY BV07
#define AFF_FAERIE_FIRE BV08
#define AFF_INFRARED BV09
#define AFF_CURSE BV10
#define AFF_COVER_TRAIL BV11
#define AFF_POISON BV12
#define AFF_PROTECT BV13
#define AFF_PARALYSIS BV14
#define AFF_SNEAK BV15
#define AFF_HIDE BV16
#define AFF_SLEEP BV17
#define AFF_CHARM BV18
#define AFF_FLYING BV19
#define AFF_PASS_DOOR BV20
#define AFF_FLOATING BV21
#define AFF_TRUESIGHT BV22
#define AFF_DETECTTRAPS BV23
#define AFF_SCRYING BV24
#define AFF_FIRESHIELD BV25
#define AFF_SHOCKSHIELD BV26
#define AFF_FASTHEAL BV27
#define AFF_ICESHIELD BV28
#define AFF_POSSESS BV29
#define AFF_BERSERK BV30
#define AFF_AQUA_BREATH BV31

/* 31 aff's (1 left.. :P) */
/* make that none - ugh - time for another field? :P */
/*
 * Resistant Immune Susceptible flags
 */
#define RIS_FIRE BV00
#define RIS_COLD BV01
#define RIS_ELECTRICITY BV02
#define RIS_ENERGY BV03
#define RIS_BLUNT BV04
#define RIS_PIERCE BV05
#define RIS_SLASH BV06
#define RIS_ACID BV07
#define RIS_POISON BV08
#define RIS_DRAIN BV09
#define RIS_SLEEP BV10
#define RIS_CHARM BV11
#define RIS_HOLD BV12
#define RIS_NONMAGIC BV13
#define RIS_PLUS1 BV14
#define RIS_PLUS2 BV15
#define RIS_PLUS3 BV16
#define RIS_PLUS4 BV17
#define RIS_PLUS5 BV18
#define RIS_PLUS6 BV19
#define RIS_MAGIC BV20
#define RIS_PARALYSIS BV21
/* 21 RIS's*/

/*
 * Attack types
 */
#define ATCK_BITE BV00
#define ATCK_CLAWS BV01
#define ATCK_TAIL BV02
#define ATCK_STING BV03
#define ATCK_PUNCH BV04
#define ATCK_KICK BV05
#define ATCK_TRIP BV06
#define ATCK_BACKSTAB BV10

/*
 * Defense types
 */
#define DFND_PARRY BV00
#define DFND_DODGE BV01
#define DFND_DISARM BV19
#define DFND_GRIP BV21

/*
 * Body parts
 */
#define PART_HEAD BV00
#define PART_ARMS BV01
#define PART_LEGS BV02
#define PART_HEART BV03
#define PART_BRAINS BV04
#define PART_GUTS BV05
#define PART_HANDS BV06
#define PART_FEET BV07
#define PART_FINGERS BV08
#define PART_EAR BV09
#define PART_EYE BV10
#define PART_LONG_TONGUE BV11
#define PART_EYESTALKS BV12
#define PART_TENTACLES BV13
#define PART_FINS BV14
#define PART_WINGS BV15
#define PART_TAIL BV16
#define PART_SCALES BV17
/* for combat */
#define PART_CLAWS BV18
#define PART_FANGS BV19
#define PART_HORNS BV20
#define PART_TUSKS BV21
#define PART_TAILATTACK BV22
#define PART_SHARPSCALES BV23
#define PART_BEAK BV24

#define PART_HAUNCH BV25
#define PART_HOOVES BV26
#define PART_PAWS BV27
#define PART_FORELEGS BV28
#define PART_FEATHERS BV29

/*
 * Autosave flags
 */
#define SV_DEATH BV00
#define SV_KILL BV01
#define SV_PASSCHG BV02
#define SV_DROP BV03
#define SV_PUT BV04
#define SV_GIVE BV05
#define SV_AUTO BV06
#define SV_ZAPDROP BV07
#define SV_AUCTION BV08
#define SV_GET BV09
#define SV_RECEIVE BV10
#define SV_IDLE BV11
#define SV_BACKUP BV12

/*
 * Pipe flags
 */
#define PIPE_TAMPED BV01
#define PIPE_LIT BV02
#define PIPE_HOT BV03
#define PIPE_DIRTY BV04
#define PIPE_FILTHY BV05
#define PIPE_GOINGOUT BV06
#define PIPE_BURNT BV07
#define PIPE_FULLOFASH BV08

/*
 * Skill/Spell flags	The minimum BV *MUST* be 11!
 */
#define SF_WATER BV11
#define SF_EARTH BV12
#define SF_AIR BV13
#define SF_ASTRAL BV14
#define SF_AREA BV15    /* is an area spell		*/
#define SF_DISTANT BV16 /* affects something far away	*/
#define SF_REVERSE BV17
#define SF_SAVE_HALF_DAMAGE BV18 /* save for half damage		*/
#define SF_SAVE_NEGATES BV19     /* save negates affect		*/
#define SF_ACCUMULATIVE BV20     /* is accumulative		*/
#define SF_RECASTABLE BV21       /* can be refreshed		*/
#define SF_NOSCRIBE BV22         /* cannot be scribed		*/
#define SF_NOBREW BV23           /* cannot be brewed		*/
#define SF_GROUPSPELL BV24       /* only affects group members	*/
#define SF_OBJECT BV25           /* directed at an object	*/
#define SF_CHARACTER BV26        /* directed at a character	*/
#define SF_SECRETSKILL BV27      /* hidden unless learned	*/
#define SF_PKSENSITIVE BV28      /* much harder for plr vs. plr	*/
#define SF_STOPONFAIL BV29       /* stops spell on first failure */

typedef enum
{
    SS_NONE,
    SS_POISON_DEATH,
    SS_ROD_WANDS,
    SS_PARA_PETRI,
    SS_BREATH,
    SS_SPELL_STAFF
} save_types;

#define ALL_BITS INT_MAX
#define SDAM_MASK ALL_BITS & ~(BV00 | BV01 | BV02)
#define SACT_MASK ALL_BITS & ~(BV03 | BV04 | BV05)
#define SCLA_MASK ALL_BITS & ~(BV06 | BV07 | BV08)
#define SPOW_MASK ALL_BITS & ~(BV09 | BV10)

typedef enum
{
    SD_NONE,
    SD_FIRE,
    SD_COLD,
    SD_ELECTRICITY,
    SD_ENERGY,
    SD_ACID,
    SD_POISON,
    SD_DRAIN
} spell_dam_types;

typedef enum
{
    SA_NONE,
    SA_CREATE,
    SA_DESTROY,
    SA_RESIST,
    SA_SUSCEPT,
    SA_DIVINATE,
    SA_OBSCURE,
    SA_CHANGE
} spell_act_types;

typedef enum
{
    SP_NONE,
    SP_MINOR,
    SP_GREATER,
    SP_MAJOR
} spell_power_types;

typedef enum
{
    SC_NONE,
    SC_LUNAR,
    SC_SOLAR,
    SC_TRAVEL,
    SC_SUMMON,
    SC_LIFE,
    SC_DEATH,
    SC_ILLUSION
} spell_class_types;

/*
 * Sex.
 * Used in #MOBILES.
 */
typedef enum
{
    SEX_NEUTRAL,
    SEX_MALE,
    SEX_FEMALE
} sex_types;

typedef enum
{
    TRAP_TYPE_POISON_GAS = 1,
    TRAP_TYPE_POISON_DART,
    TRAP_TYPE_POISON_NEEDLE,
    TRAP_TYPE_POISON_DAGGER,
    TRAP_TYPE_POISON_ARROW,
    TRAP_TYPE_BLINDNESS_GAS,
    TRAP_TYPE_SLEEPING_GAS,
    TRAP_TYPE_FLAME,
    TRAP_TYPE_EXPLOSION,
    TRAP_TYPE_ACID_SPRAY,
    TRAP_TYPE_ELECTRIC_SHOCK,
    TRAP_TYPE_BLADE,
    TRAP_TYPE_SEX_CHANGE
} trap_types;

#define MAX_TRAPTYPE TRAP_TYPE_SEX_CHANGE

#define TRAP_ROOM BV00
#define TRAP_OBJ BV01
#define TRAP_ENTER_ROOM BV02
#define TRAP_LEAVE_ROOM BV03
#define TRAP_OPEN BV04
#define TRAP_CLOSE BV05
#define TRAP_GET BV06
#define TRAP_PUT BV07
#define TRAP_PICK BV08
#define TRAP_UNLOCK BV09
#define TRAP_N BV10
#define TRAP_S BV11
#define TRAP_E BV12
#define TRAP_W BV13
#define TRAP_U BV14
#define TRAP_D BV15
#define TRAP_EXAMINE BV16
#define TRAP_NE BV17
#define TRAP_NW BV18
#define TRAP_SE BV19
#define TRAP_SW BV20

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE 2
#define OBJ_VNUM_MONEY_SOME 3

#define COMMSYS_VNUM 62
#define DATAPAD_VNUM 63

#define MODULE_VNUM 73
#define SABER_VNUM 72

#define OBJ_VNUM_DROID_CORPSE 9
#define OBJ_VNUM_CORPSE_NPC 10
#define OBJ_VNUM_CORPSE_PC 11
#define OBJ_VNUM_SEVERED_HEAD 12
#define OBJ_VNUM_TORN_HEART 13
#define OBJ_VNUM_SLICED_ARM 14
#define OBJ_VNUM_SLICED_LEG 15
#define OBJ_VNUM_SPILLED_GUTS 16
#define OBJ_VNUM_BLOOD 17
#define OBJ_VNUM_BLOODSTAIN 18
#define OBJ_VNUM_SCRAPS 19

#define OBJ_VNUM_MUSHROOM 20
#define OBJ_VNUM_LIGHT_BALL 21
#define OBJ_VNUM_SPRING 22

#define OBJ_VNUM_SLICE 24
#define OBJ_VNUM_SHOPPING_BAG 25

#define OBJ_VNUM_FIRE 30
#define OBJ_VNUM_TRAP 31
#define OBJ_VNUM_PORTAL 32

#define OBJ_VNUM_BLACK_POWDER 33
#define OBJ_VNUM_SCROLL_SCRIBING 34
#define OBJ_VNUM_FLASK_BREWING 35
#define OBJ_VNUM_NOTE 36

/* Academy eq */
#define OBJ_VNUM_SCHOOL_MACE 10315
#define OBJ_VNUM_SCHOOL_DAGGER 10312
#define OBJ_VNUM_SCHOOL_SWORD 10313
#define OBJ_VNUM_SCHOOL_VEST 10308
#define OBJ_VNUM_SCHOOL_SHIELD 10310
#define OBJ_VNUM_SCHOOL_BANNER 10311
#define OBJ_VNUM_SCHOOL_DIPLOMA 10321

#define OBJ_VNUM_BLASTECH_E11 50
#define OBJ_VNUM_SHIPBOMB 68

/* These are some defines for modules */
#define AFFECT_PRIMARY 1
#define AFFECT_SECONDARY 2
#define AFFECT_MISSILE 3
#define AFFECT_ROCKET 4
#define AFFECT_TORPEDO 5
#define AFFECT_HULL 6
#define AFFECT_SHIELD 7
#define AFFECT_SPEED 8
#define AFFECT_HYPER 9
#define AFFECT_ENERGY 10
#define AFFECT_MANUEVER 11
#define AFFECT_CHAFF 12
#define AFFECT_ALARM 13
#define AFFECT_SLAVE 14
#define AFFECT_TRACTOR 15

/*
 * Item types.
 * Used in #OBJECTS.
 */
typedef enum
{
    ITEM_NONE,
    ITEM_LIGHT,
    ITEM_SCROLL,
    ITEM_WAND,
    ITEM_STAFF,
    ITEM_WEAPON,
    ITEM_FIREWEAPON,
    ITEM_MISSILE,
    ITEM_TREASURE,
    ITEM_ARMOR,
    ITEM_POTION,
    ITEM_WORN,
    ITEM_FURNITURE,
    ITEM_TRASH,
    ITEM_OLDTRAP,
    ITEM_CONTAINER,
    ITEM_NOTE,
    ITEM_DRINK_CON,
    ITEM_KEY,
    ITEM_FOOD,
    ITEM_MONEY,
    ITEM_PEN,
    ITEM_BOAT,
    ITEM_CORPSE_NPC,
    ITEM_CORPSE_PC,
    ITEM_FOUNTAIN,
    ITEM_PILL,
    ITEM_BLOOD,
    ITEM_BLOODSTAIN,
    ITEM_SCRAPS,
    ITEM_PIPE,
    ITEM_HERB_CON,
    ITEM_HERB,
    ITEM_INCENSE,
    ITEM_FIRE,
    ITEM_BOOK,
    ITEM_SWITCH,
    ITEM_LEVER,
    ITEM_PULLCHAIN,
    ITEM_BUTTON,
    ITEM_DIAL,
    ITEM_RUNE,
    ITEM_RUNEPOUCH,
    ITEM_MATCH,
    ITEM_TRAP,
    ITEM_MAP,
    ITEM_PORTAL,
    ITEM_PAPER,
    ITEM_TINDER,
    ITEM_LOCKPICK,
    ITEM_SPIKE,
    ITEM_DISEASE,
    ITEM_OIL,
    ITEM_FUEL,
    ITEM_DEBIT_CARD,
    ITEM_LONG_BOW,
    ITEM_CROSSBOW,
    ITEM_AMMO,
    ITEM_QUIVER,
    ITEM_SHOVEL,
    ITEM_SALVE,
    ITEM_RAWSPICE,
    ITEM_LENS,
    ITEM_CRYSTAL,
    ITEM_DURAPLAST,
    ITEM_BATTERY,
    ITEM_TOOLKIT,
    ITEM_DURASTEEL,
    ITEM_OVEN,
    ITEM_MIRROR,
    ITEM_CIRCUIT,
    ITEM_SUPERCONDUCTOR,
    ITEM_COMLINK,
    ITEM_MEDPAC,
    ITEM_FABRIC,
    ITEM_RARE_METAL,
    ITEM_MAGNET,
    ITEM_THREAD,
    ITEM_SPICE,
    ITEM_SMUT,
    ITEM_DEVICE,
    ITEM_SPACECRAFT,
    ITEM_GRENADE,
    ITEM_LANDMINE,
    ITEM_GOVERNMENT,
    ITEM_DROID_CORPSE,
    ITEM_BOLT,
    ITEM_CHEMICAL,
    ITEM_COMMSYSTEM,
    ITEM_DATAPAD,
    ITEM_MODULE,
    ITEM_BUG,
    ITEM_BEACON,
    ITEM_GLAUNCHER,
    ITEM_RLAUNCHER,
    ITEM_BINDERS,
    ITEM_GOGGLES,
    ITEM_SHIPBOMB,
    ITEM_EMP_GRENADE
} item_types;

#define MAX_ITEM_TYPE ITEM_EMP_GRENADE
/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW BV00
#define ITEM_HUM BV01
#define ITEM_DARK BV02
#define ITEM_HUTT_SIZE BV03
#define ITEM_CONTRABAND BV04
#define ITEM_INVIS BV05
#define ITEM_MAGIC BV06
#define ITEM_NODROP BV07
#define ITEM_BLESS BV08
#define ITEM_ANTI_GOOD BV09
#define ITEM_ANTI_EVIL BV10
#define ITEM_ANTI_NEUTRAL BV11
#define ITEM_NOREMOVE BV12
#define ITEM_INVENTORY BV13
#define ITEM_ANTI_SOLDIER BV14
#define ITEM_ANTI_THIEF BV15
#define ITEM_ANTI_HUNTER BV16
#define ITEM_ANTI_JEDI BV17
#define ITEM_SMALL_SIZE BV18
#define ITEM_LARGE_SIZE BV19
#define ITEM_DONATION BV20
#define ITEM_CLANOBJECT BV21
#define ITEM_ANTI_CITIZEN BV22
#define ITEM_ANTI_SITH BV23
#define ITEM_ANTI_PILOT BV24
#define ITEM_HIDDEN BV25
#define ITEM_POISONED BV26
#define ITEM_COVERING BV27
#define ITEM_DEATHROT BV28
#define ITEM_BURRIED BV29 /* item is underground */
#define ITEM_PROTOTYPE BV30
#define ITEM_HUMAN_SIZE BV31

/* Magic flags - extra extra_flags for objects that are used in spells */
#define ITEM_RETURNING BV00
#define ITEM_BACKSTABBER BV01
#define ITEM_BANE BV02
#define ITEM_LOYAL BV03
#define ITEM_HASTE BV04
#define ITEM_DRAIN BV05
#define ITEM_LIGHTNING_BLADE BV06

/* Blaster settings - only saves on characters */
#define BLASTER_NORMAL 0
#define BLASTER_HALF 2
#define BLASTER_FULL 5
#define BLASTER_LOW 1
#define BLASTER_STUN 3
#define BLASTER_HIGH 4

/* Weapon Types */

#define WEAPON_NONE 0
#define WEAPON_VIBRO_AXE 1
#define WEAPON_VIBRO_BLADE 2
#define WEAPON_LIGHTSABER 3
#define WEAPON_WHIP 4
#define WEAPON_CLAW 5
#define WEAPON_BLASTER 6
#define WEAPON_BLUDGEON 8
#define WEAPON_BOWCASTER 9
#define WEAPON_FORCE_PIKE 11
#define WEAPON_DUAL_LIGHTSABER 12

/* Lever/dial/switch/button/pullchain flags */
#define TRIG_UP BV00
#define TRIG_UNLOCK BV01
#define TRIG_LOCK BV02
#define TRIG_D_NORTH BV03
#define TRIG_D_SOUTH BV04
#define TRIG_D_EAST BV05
#define TRIG_D_WEST BV06
#define TRIG_D_UP BV07
#define TRIG_D_DOWN BV08
#define TRIG_DOOR BV09
#define TRIG_CONTAINER BV10
#define TRIG_OPEN BV11
#define TRIG_CLOSE BV12
#define TRIG_PASSAGE BV13
#define TRIG_OLOAD BV14
#define TRIG_MLOAD BV15
#define TRIG_TELEPORT BV16
#define TRIG_TELEPORTALL BV17
#define TRIG_TELEPORTPLUS BV18
#define TRIG_DEATH BV19
#define TRIG_CAST BV20
#define TRIG_FAKEBLADE BV21
#define TRIG_RAND4 BV22
#define TRIG_RAND6 BV23
#define TRIG_TRAPDOOR BV24
#define TRIG_ANOTHEROOM BV25
#define TRIG_USEDIAL BV26
#define TRIG_ABSOLUTEVNUM BV27
#define TRIG_SHOWROOMDESC BV28
#define TRIG_AUTORETURN BV29

#define TELE_SHOWDESC BV00
#define TELE_TRANSALL BV01
#define TELE_TRANSALLPLUS BV02

/* drug types */
#define SPICE_GLITTERSTIM 0
#define SPICE_CARSANUM 1
#define SPICE_RYLL 2
#define SPICE_ANDRIS 3

/* crystal types */
#define GEM_NON_ADEGEN 0
#define GEM_KATHRACITE 1
#define GEM_RELACITE 2
#define GEM_DANITE 3
#define GEM_MEPHITE 4
#define GEM_PONITE 5
#define GEM_ILLUM 6
#define GEM_CORUSCA 7

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE BV00
#define ITEM_WEAR_FINGER BV01
#define ITEM_WEAR_NECK BV02
#define ITEM_WEAR_BODY BV03
#define ITEM_WEAR_HEAD BV04
#define ITEM_WEAR_LEGS BV05
#define ITEM_WEAR_FEET BV06
#define ITEM_WEAR_HANDS BV07
#define ITEM_WEAR_ARMS BV08
#define ITEM_WEAR_SHIELD BV09
#define ITEM_WEAR_ABOUT BV10
#define ITEM_WEAR_WAIST BV11
#define ITEM_WEAR_WRIST BV12
#define ITEM_WIELD BV13
#define ITEM_HOLD BV14
#define ITEM_DUAL_WIELD BV15
#define ITEM_WEAR_EARS BV16
#define ITEM_WEAR_EYES BV17
#define ITEM_MISSILE_WIELD BV18
#define ITEM_WEAR_BACK BV19
#define ITEM_WEAR_HOLSTER1 BV20
#define ITEM_WEAR_HOLSTER2 BV21
#define ITEM_WEAR_BOTHWRISTS BV22

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
typedef enum
{
    APPLY_NONE,
    APPLY_STR,
    APPLY_DEX,
    APPLY_INT,
    APPLY_WIS,
    APPLY_CON,
    APPLY_SEX,
    APPLY_NULL,
    APPLY_LEVEL,
    APPLY_AGE,
    APPLY_HEIGHT,
    APPLY_WEIGHT,
    APPLY_MANA,
    APPLY_HIT,
    APPLY_MOVE,
    APPLY_GOLD,
    APPLY_EXP,
    APPLY_AC,
    APPLY_HITROLL,
    APPLY_DAMROLL,
    APPLY_SAVING_POISON,
    APPLY_SAVING_ROD,
    APPLY_SAVING_PARA,
    APPLY_SAVING_BREATH,
    APPLY_SAVING_SPELL,
    APPLY_CHA,
    APPLY_AFFECT,
    APPLY_RESISTANT,
    APPLY_IMMUNE,
    APPLY_SUSCEPTIBLE,
    APPLY_WEAPONSPELL,
    APPLY_LCK,
    APPLY_BACKSTAB,
    APPLY_PICK,
    APPLY_TRACK,
    APPLY_STEAL,
    APPLY_SNEAK,
    APPLY_HIDE,
    APPLY_PALM,
    APPLY_DETRAP,
    APPLY_DODGE,
    APPLY_PEEK,
    APPLY_SCAN,
    APPLY_GOUGE,
    APPLY_SEARCH,
    APPLY_MOUNT,
    APPLY_DISARM,
    APPLY_KICK,
    APPLY_PARRY,
    APPLY_BASH,
    APPLY_STUN,
    APPLY_PUNCH,
    APPLY_CLIMB,
    APPLY_GRIP,
    APPLY_SCRIBE,
    APPLY_COVER_TRAIL,
    APPLY_WEARSPELL,
    APPLY_REMOVESPELL,
    APPLY_EMOTION,
    APPLY_MENTALSTATE,
    APPLY_STRIPSN,
    APPLY_REMOVE,
    APPLY_DIG,
    APPLY_FULL,
    APPLY_THIRST,
    APPLY_DRUNK,
    APPLY_BLOOD,
    MAX_APPLY_TYPE
} apply_types;

#define REVERSE_APPLY 1000

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE 1
#define CONT_PICKPROOF 2
#define CONT_CLOSED 4
#define CONT_LOCKED 8

/*
 * Sitting/Standing/Sleeping/Sitting on/in/at Objects - Xerves
 * Used for furniture (value[2]) in the #OBJECTS Section
 */
#define SIT_ON BV00
#define SIT_IN BV01
#define SIT_AT BV02

#define STAND_ON BV03
#define STAND_IN BV04
#define STAND_AT BV05

#define SLEEP_ON BV06
#define SLEEP_IN BV07
#define SLEEP_AT BV08

#define REST_ON BV09
#define REST_IN BV10
#define REST_AT BV11

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO 2
#define ROOM_VNUM_POLY 3
#define ROOM_VNUM_CHAT 32144
#define ROOM_VNUM_TEMPLE 32144
#define ROOM_VNUM_ALTAR 32144
#define ROOM_VNUM_SCHOOL 115
#define ROOM_AUTH_START 10300
#define ROOM_START_HUMAN 211
#define ROOM_START_WOOKIEE 28600
#define ROOM_START_TWILEK 32148
#define ROOM_START_RODIAN 32148
#define ROOM_START_HUTT 32148
#define ROOM_START_MON_CALAMARIAN 21069
#define ROOM_START_NOGHRI 1015
#define ROOM_START_GAMORREAN 28100
#define ROOM_START_JAWA 31819
#define ROOM_START_ADARIAN 29000
#define ROOM_START_EWOK 32148
#define ROOM_START_VERPINE 32148
#define ROOM_START_DEFEL 32148
#define ROOM_START_TRANDOSHAN 32148
#define ROOM_START_HAPAN 32148
#define ROOM_START_DUINUOGWUIN 32148
#define ROOM_START_QUARREN 21069
#define ROOM_START_IMMORTAL 100
#define ROOM_LIMBO_SHIPYARD 45
#define ROOM_DEFAULT_CRASH 28025

#define ROOM_PLUOGUS_QUIT 905

#define ROOM_SHUTTLE_BUS 907   /* Sol */
#define ROOM_SHUTTLE_BUS_2 914 /* Monir*/
#define ROOM_SHUTTLE_BUS_3 921 /* Fau */
#define ROOM_SHUTTLE_BUS_4 928 /* Taw */
#define ROOM_CORUSCANT_SHUTTLE 199
#define ROOM_SENATE_SHUTTLE 10197
#define ROOM_CORUSCANT_TURBOCAR 226

#define SHIP_AREA "shipvnum.are"

/*
 * Room flags.           Holy cow!  Talked about stripped away..
 * Used in #ROOMS.       Those merc guys know how to strip code down.
 *			 Lets put it all back... ;)
 */

#define ROOM_DARK BV00
/* BV01 now reserved for track  BV01  and hunt */
#define ROOM_NO_MOB BV02
#define ROOM_INDOORS BV03
#define ROOM_CAN_LAND BV04
#define ROOM_CAN_FLY BV05
#define ROOM_NO_DRIVING BV06
#define ROOM_NO_MAGIC BV07
#define ROOM_BANK BV08
#define ROOM_PRIVATE BV09
#define ROOM_SAFE BV10
#define ROOM_SOLITARY BV11
#define ROOM_PET_SHOP BV12
#define ROOM_NO_RECALL BV13
#define ROOM_DONATION BV14
#define ROOM_NODROPALL BV15
#define ROOM_SILENCE BV16
#define ROOM_LOGSPEECH BV17
#define ROOM_NODROP BV18
#define ROOM_CLANSTOREROOM BV19
#define ROOM_PLR_HOME BV20
#define ROOM_EMPTY_HOME BV21
#define ROOM_TELEPORT BV22
#define ROOM_HOTEL BV23
#define ROOM_NOFLOOR BV24
#define ROOM_REFINERY BV25
#define ROOM_FACTORY BV26
#define ROOM_R_RECRUIT BV27
#define ROOM_E_RECRUIT BV28
#define ROOM_SPACECRAFT BV29
#define ROOM_PROTOTYPE BV30
#define ROOM_AUCTION BV31

/* Second Set of Room Flags */
#define ROOM_EMPTY_SHOP BV00
#define ROOM_PLR_SHOP BV01
#define ROOM_SHIPYARD BV02
#define ROOM_GARAGE BV03
#define ROOM_BARRACKS BV04
#define ROOM_CONTROL BV05
#define ROOM_CLANLAND BV06
#define ROOM_ARENA BV07
#define ROOM_CLANJAIL BV08
#define ROOM_BLACKMARKET BV09
#define ROOM_HIDDENPAD BV10
#define ROOM_SLOTS BV11
#define ROOM_IMPORT BV12
#define ROOM_STORAGEDOCK BV13
/*
 * Directions.
 * Used in #ROOMS.
 */
typedef enum
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_UP,
    DIR_DOWN,
    DIR_NORTHEAST,
    DIR_NORTHWEST,
    DIR_SOUTHEAST,
    DIR_SOUTHWEST,
    DIR_SOMEWHERE
} dir_types;

#define MAX_DIR DIR_SOUTHWEST    /* max for normal walking */
#define DIR_PORTAL DIR_SOMEWHERE /* portal direction	  */

/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR BV00
#define EX_CLOSED BV01
#define EX_LOCKED BV02
#define EX_SECRET BV03
#define EX_SWIM BV04
#define EX_PICKPROOF BV05
#define EX_FLY BV06
#define EX_CLIMB BV07
#define EX_DIG BV08
#define EX_RES1 BV09 /* are these res[1-4] important? */
#define EX_NOPASSDOOR BV10
#define EX_HIDDEN BV11
#define EX_PASSAGE BV12
#define EX_PORTAL BV13
#define EX_RES2 BV14
#define EX_RES3 BV15
#define EX_xCLIMB BV16
#define EX_xENTER BV17
#define EX_xLEAVE BV18
#define EX_xAUTO BV19
#define EX_RES4 BV20
#define EX_xSEARCHABLE BV21
#define EX_BASHED BV22
#define EX_BASHPROOF BV23
#define EX_NOMOB BV24
#define EX_WINDOW BV25
#define EX_xLOOK BV26
#define MAX_EXFLAG 26

/*
 * Sector types.
 * Used in #ROOMS.
 */
typedef enum
{
    SECT_INSIDE,
    SECT_CITY,
    SECT_FIELD,
    SECT_FOREST,
    SECT_HILLS,
    SECT_MOUNTAIN,
    SECT_WATER_SWIM,
    SECT_WATER_NOSWIM,
    SECT_UNDERWATER,
    SECT_AIR,
    SECT_DESERT,
    SECT_DUNNO,
    SECT_OCEANFLOOR,
    SECT_UNDERGROUND,
    SECT_SCRUB,
    SECT_ROCKY,
    SECT_SAVANNA,
    SECT_TUNDRA,
    SECT_GLACIAL,
    SECT_RAINFOREST,
    SECT_JUNGLE,
    SECT_SWAMP,
    SECT_WETLANDS,
    SECT_BRUSH,
    SECT_STEPPE,
    SECT_FARMLAND,
    SECT_VOLCANIC,
    SECT_MAX
} sector_types;

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
typedef enum
{
    WEAR_NONE = -1,
    WEAR_LIGHT = 0,
    WEAR_FINGER_L,
    WEAR_FINGER_R,
    WEAR_NECK_1,
    WEAR_NECK_2,
    WEAR_BODY,
    WEAR_HEAD,
    WEAR_LEGS,
    WEAR_FEET,
    WEAR_HANDS,
    WEAR_ARMS,
    WEAR_SHIELD,
    WEAR_ABOUT,
    WEAR_WAIST,
    WEAR_WRIST_L,
    WEAR_WRIST_R,
    WEAR_WIELD,
    WEAR_HOLD,
    WEAR_DUAL_WIELD,
    WEAR_EARS,
    WEAR_EYES,
    WEAR_MISSILE_WIELD,
    WEAR_BACK,
    WEAR_HOLSTER_L,
    WEAR_HOLSTER_R,
    WEAR_BOTH_WRISTS,
    MAX_WEAR
} wear_locations;

/* Board Types */
typedef enum
{
    BOARD_NOTE,
    BOARD_MAIL
} board_types;

/* Auth Flags */
#define FLAG_WRAUTH 1
#define FLAG_AUTH 2

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
typedef enum
{
    COND_DRUNK,
    COND_FULL,
    COND_THIRST,
    COND_BLOODTHIRST,
    MAX_CONDS
} conditions;

/*
 * Positions.
 */
typedef enum
{
    POS_DEAD,
    POS_MORTAL,
    POS_INCAP,
    POS_STUNNED,
    POS_SLEEPING,
    POS_RESTING,
    POS_SITTING,
    POS_FIGHTING,
    POS_STANDING,
    POS_MOUNTED,
    POS_SHOVE,
    POS_DRAG
} positions;

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC BV00 /* Don't EVER set.	*/
#define PLR_BOUGHT_PET BV01
#define PLR_SHOVEDRAG BV02
#define PLR_AUTOEXIT BV03
#define PLR_AUTOLOOT BV04
#define PLR_AUTOSAC BV05
#define PLR_BLANK BV06
#define PLR_OUTCAST BV07
#define PLR_BRIEF BV08
#define PLR_COMBINE BV09
#define PLR_PROMPT BV10
#define PLR_TELNET_GA BV11

#define PLR_HOLYLIGHT BV12
#define PLR_WIZINVIS BV13
#define PLR_ROOMVNUM BV14

#define PLR_SILENCE BV15
#define PLR_NO_EMOTE BV16
#define PLR_ATTACKER BV17
#define PLR_NO_TELL BV18
#define PLR_LOG BV19
#define PLR_DENY BV20
#define PLR_FREEZE BV21
#define PLR_KILLER BV22
#define PLR_WHOINVIS BV23
#define PLR_LITTERBUG BV24
#define PLR_ANSI BV25
#define PLR_SOUND BV26
#define PLR_NICE BV27
#define PLR_FLEE BV28
#define PLR_AUTOGOLD BV29
#define PLR_SLOG BV30
#define PLR_AFK BV31

/* Bits for pc_data->flags. */
#define PCFLAG_R1 BV00
/*
#define PCFLAG_                    BV01     extra flag
*/
#define PCFLAG_UNAUTHED BV02
#define PCFLAG_NORECALL BV03
#define PCFLAG_NOINTRO BV04
#define PCFLAG_GAG BV05
#define PCFLAG_RETIRED BV06
#define PCFLAG_GUEST BV07
#define PCFLAG_HASSLUG BV08
#define PCFLAG_PAGERON BV09
#define PCFLAG_NOTITLE BV10
#define PCFLAG_ROOM BV11
#define PCFLAG_MAP BV12

typedef enum
{
    TIMER_NONE,
    TIMER_RECENTFIGHT,
    TIMER_SHOVEDRAG,
    TIMER_DO_FUN,
    TIMER_APPLIED,
    TIMER_PKILLED
} timer_types;

struct TIMER
{
    TIMER* prev;
    TIMER* next;
    DO_FUN* do_fun;
    int value;
    sh_int type;
    sh_int count;
};

/*
 * Channel bits.
 */
#define CHANNEL_AUCTION BV00
#define CHANNEL_CHAT BV01
#define CHANNEL_QUEST BV02
#define CHANNEL_IMMTALK BV03
#define CHANNEL_MUSIC BV04
#define CHANNEL_ASK BV05
#define CHANNEL_SHOUT BV06
#define CHANNEL_YELL BV07
#define CHANNEL_MONITOR BV08
#define CHANNEL_LOG BV09
#define CHANNEL_104 BV10
#define CHANNEL_CLAN BV11
#define CHANNEL_BUILD BV12
#define CHANNEL_105 BV13
#define CHANNEL_AVTALK BV14
#define CHANNEL_PRAY BV15
#define CHANNEL_COUNCIL BV16
#define CHANNEL_GUILD BV17
#define CHANNEL_COMM BV18
#define CHANNEL_TELLS BV19
#define CHANNEL_ORDER BV20
#define CHANNEL_NEWBIE BV21
#define CHANNEL_WARTALK BV22
#define CHANNEL_OOC BV23
#define CHANNEL_SHIP BV24
#define CHANNEL_SYSTEM BV25
#define CHANNEL_SPACE BV26
#define CHANNEL_103 BV27
#define CHANNEL_SPORTS BV27
#define CHANNEL_HOLONET BV31

#define CHANNEL_CLANTALK CHANNEL_CLAN

/* Area defines - Scryn 8/11
 *
 */
#define AREA_DELETED BV00
#define AREA_LOADED BV01

/* Area flags - Narn Mar/96 */
#define AFLAG_NOPKILL BV00

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct MOB_INDEX_DATA
{
    SPEC_FUN* spec_fun;
    SPEC_FUN* spec_2;
    SHOP_DATA* pShop;
    REPAIR_DATA* rShop;
    MPROG_DATA* mudprogs;
    int progtypes;
    char* player_name;
    char* short_descr;
    char* long_descr;
    char* description;
    int vnum;
    sh_int count;
    sh_int killed;
    sh_int sex;
    sh_int level;
    int act;
    int affected_by;
    sh_int alignment;
    sh_int mobthac0; /* Unused */
    sh_int ac;
    sh_int hitnodice;
    sh_int hitsizedice;
    sh_int hitplus;
    sh_int damnodice;
    sh_int damsizedice;
    sh_int damplus;
    sh_int numattacks;
    int gold;
    int exp;
    int xflags;
    int resistant;
    int immune;
    int susceptible;
    int attacks;
    int defenses;
    int speaks;
    int speaking;
    sh_int position;
    sh_int defposition;
    sh_int height;
    sh_int weight;
    sh_int race;
    sh_int hitroll;
    sh_int damroll;
    sh_int perm_str;
    sh_int perm_int;
    sh_int perm_wis;
    sh_int perm_dex;
    sh_int perm_con;
    sh_int perm_cha;
    sh_int perm_lck;
    sh_int perm_frc;
    sh_int saving_poison_death;
    sh_int saving_wand;
    sh_int saving_para_petri;
    sh_int saving_breath;
    sh_int saving_spell_staff;
    int vip_flags;
};

// hunt-hate-fear data
struct HHF_DATA
{
    char* name;
    CHAR_DATA* who;
};

struct FIGHT_DATA
{
    CHAR_DATA* who;
    int xp;
    sh_int align;
    sh_int duration;
    sh_int timeskilled;
};

struct EXTRACT_CHAR_DATA
{
    EXTRACT_CHAR_DATA* next;
    CHAR_DATA* ch;
    ROOM_INDEX_DATA* room;
    ch_ret retcode;
    bool extract;
};

/*
 * One character (PC or NPC).
 * (Shouldn't most of that build interface stuff use substate, dest_buf,
 * spare_ptr and tempnum?  Seems a little redundant)
 */
struct CHAR_DATA
{
    CHAR_DATA* next;
    CHAR_DATA* prev;
    CHAR_DATA* next_in_room;
    CHAR_DATA* prev_in_room;
    CHAR_DATA* master;
    CHAR_DATA* leader;
    FIGHT_DATA* fighting;
    CHAR_DATA* reply;
    char* owner;
    ROOM_INDEX_DATA* home;
    CHAR_DATA* switched;
    BUG_DATA* first_bug;
    BUG_DATA* last_bug;
    CONTRACT_DATA* first_contract;
    CONTRACT_DATA* last_contract;
    FELLOW_DATA* first_fellow;
    FELLOW_DATA* last_fellow;
    CHAR_DATA* mount;
    HHF_DATA* hunting;
    HHF_DATA* fearing;
    HHF_DATA* hating;
    SPEC_FUN* spec_fun;
    SPEC_FUN* spec_2;
    MPROG_ACT_LIST* mpact;
    int mpactnum;
    int buzzed;
    int buzzedfrom;
    sh_int mpscriptpos;
    sh_int colors[MAX_COLORS];
    MOB_INDEX_DATA* pIndexData;
    DESCRIPTOR_DATA* desc;
    AFFECT_DATA* first_affect;
    AFFECT_DATA* last_affect;
    NOTE_DATA* pnote;
    NOTE_DATA* comments;
    OBJ_DATA* first_carrying;
    OBJ_DATA* last_carrying;
    OBJ_DATA* on;
    ROOM_INDEX_DATA* in_room;
    ROOM_INDEX_DATA* was_in_room;
    ROOM_INDEX_DATA* was_sentinel;
    ROOM_INDEX_DATA* plr_home;
    PC_DATA* pcdata;
    DO_FUN* last_cmd;
    DO_FUN* prev_cmd; /* mapping */
    CHAR_DATA* challenged;
    CHAR_DATA* betted_on;
    int bet_amt;
    void* dest_buf; // TODO this is hilariously unsafe and needs to go away
    void* dest_buf_2;
    void* spare_ptr;
    int tempnum;
    EDITOR_DATA* editor;
    TIMER* first_timer;
    TIMER* last_timer;
    char* name;
    char* short_descr;
    char* long_descr;
    char* description;
    sh_int num_fighting;
    sh_int substate;
    sh_int sex;
    sh_int race;
    sh_int top_level;
    sh_int skill_level[MAX_ABILITY];
    sh_int bonus[MAX_ABILITY];
    sh_int trust;
    int played;
    time_t logon;
    time_t save_time;
    sh_int timer;
    sh_int wait;
    sh_int hit;
    sh_int max_hit;
    int force_skill[MAX_FORCE_SKILL];
    sh_int force_control;
    sh_int force_sense;
    sh_int force_alter;
    sh_int force_chance;
    sh_int force_identified;
    sh_int force_level_status;
    sh_int force_align;
    sh_int force_converted;
    sh_int force_type;
    char* force_master;
    char* force_temp_master;
    char* force_disguise;
    int force_disguise_count;
    int wait_state;
    sh_int mana;
    sh_int max_mana;
    sh_int move;
    sh_int max_move;
    sh_int numattacks;
    int gold;
    long experience[MAX_ABILITY];
    int act;
    int affected_by;
    int carry_weight;
    int carry_number;
    int xflags;
    int resistant;
    int immune;
    int susceptible;
    int attacks;
    int defenses;
    int speaks;
    int speaking;
    sh_int saving_poison_death;
    sh_int saving_wand;
    sh_int saving_para_petri;
    sh_int saving_breath;
    sh_int saving_spell_staff;
    sh_int alignment;
    sh_int barenumdie;
    sh_int baresizedie;
    sh_int mobthac0;
    sh_int hitroll;
    sh_int damroll;
    sh_int hitplus;
    sh_int damplus;
    sh_int position;
    sh_int defposition;
    sh_int height;
    sh_int weight;
    sh_int armor;
    sh_int wimpy;
    int deaf;
    sh_int perm_str;
    sh_int perm_int;
    sh_int perm_wis;
    sh_int perm_dex;
    sh_int perm_con;
    sh_int perm_cha;
    sh_int perm_lck;
    sh_int perm_frc;
    sh_int mod_str;
    sh_int mod_int;
    sh_int mod_wis;
    sh_int mod_dex;
    sh_int mod_con;
    sh_int mod_cha;
    sh_int mod_lck;
    sh_int mod_frc;
    sh_int mental_state;    /* simplified */
    sh_int emotional_state; /* simplified */
    int pagelen;            /* BUILD INTERFACE */
    sh_int inter_page;      /* BUILD INTERFACE */
    sh_int inter_type;      /* BUILD INTERFACE */
    char* inter_editing;    /* BUILD INTERFACE */
    int inter_editing_vnum; /* BUILD INTERFACE */
    sh_int inter_substate;  /* BUILD INTERFACE */
    int retran;
    int regoto;
    sh_int mobinvis; /* Mobinvis level SB */
    int vip_flags;
    sh_int backup_wait; /* reinforcements */
    int backup_mob;     /* reinforcements */
    sh_int was_stunned;
    char* mob_clan; /* for spec_clan_guard.. set by postguard */
    GUARD_DATA* guard_data;
    sh_int main_ability;
    sh_int secondary_ability;
    sh_int rppoints;
    char* comfreq;
    char* rank;
    int pheight, build;
    CHAR_DATA* aiming_at;
};

struct KILLED_DATA
{
    int vnum;
    char count;
};

struct CHANGE_DATA
{
    char* change;
    char* coder;
    char* date;
    time_t mudtime;
};

struct BUG_DATA
{
    char* name;
    BUG_DATA* next_in_bug;
    BUG_DATA* prev_in_bug;
};

struct CONTRACT_DATA
{
    char* target;
    int amount;
    CONTRACT_DATA* next_in_contract;
    CONTRACT_DATA* prev_in_contract;
};

struct FELLOW_DATA
{

    char* victim;
    char* knownas;
    FELLOW_DATA* next;
    FELLOW_DATA* prev;
};

/*
 * Data which only PC's have.
 */
struct PC_DATA
{
    CLAN_DATA* clan;
    AREA_DATA* area;
    ROOM_INDEX_DATA* roomarena;
    char* homepage;
    char* screenname;
    char* image;
    char* clan_name;
    char* pwd;
    char* email;
    char* bamfin;
    char* bamfout;
    int lost_attacks;
    char* rank;
    int shipnum;
    sh_int version;
    char* shipname;
    char* title;
    char* disguise;
    char* bestowments; /* Special bestowed commands	   */
    int act2;
    int flags;      /* Whether the player is deadly and whatever else we add.      */
    int pkills;     /* Number of pkills on behalf of clan */
    int pdeaths;    /* Number of times pkilled (legally)  */
    int mkills;     /* Number of mobs killed		   */
    int mdeaths;    /* Number of deaths due to mobs       */
    int illegal_pk; /* Number of illegal pk's committed   */
    char* fiance;
    char* propose;
    char* proposed;
    char* spouse;
    int forcerank;
    char* last_name;
    long int outcast_time; /* The time at which the char was outcast */
    long int restore_time; /* The last time the char did a restore all */
    int r_range_lo;        /* room range */
    int r_range_hi;
    int m_range_lo; /* mob range  */
    int m_range_hi;
    int o_range_lo; /* obj range  */
    int o_range_hi;
    char* tell_snoop; /* Tell snoop */
    sh_int wizinvis;  /* wizinvis level */
    sh_int min_snoop; /* minimum snoop level */
    sh_int condition[MAX_CONDS];
    sh_int learned[MAX_SKILL];
    KILLED_DATA killed[MAX_KILLTRACK];
    sh_int quest_number; /* current *QUEST BEING DONE* DON'T REMOVE! */
    sh_int quest_curr;   /* current number of quest points */
    int quest_accum;     /* quest points accumulated in players life */
    int auth_state;
    time_t release_date; /* Auto-helling.. Altrag */
    char* helled_by;
    char* bio;                     /* Personal Bio */
    char* authed_by;               /* what crazy imm authed this name ;) */
    SKILL_TYPE* special_skills[5]; /* personalized skills/spells */
    char* prompt;                  /* User config prompts */
    char* subprompt;               /* Substate prompt */
    sh_int pagerlen;               /* For pager (NOT menus) */
    bool openedtourney;
    sh_int addiction[10];
    sh_int drug_level[10];
    char* store_title;
    bool is_hacking;
    int wanted_flags;
    long bank;
    int salary;
};

/*
 * Liquids.
 */
#define LIQ_WATER 0
#define LIQ_MAX 19

struct LIQ_TYPE
{
    const char* liq_name;
    const char* liq_color;
    sh_int liq_affect[3];
};

/*
 * Extra description data for a room or object.
 */
struct EXTRA_DESCR_DATA
{
    EXTRA_DESCR_DATA* next; /* Next in list                     */
    EXTRA_DESCR_DATA* prev; /* Previous in list                 */
    char* keyword;          /* Keyword in look/examine          */
    char* description;      /* What to see                      */
};

/*
 * Prototype for an object.
 */
struct OBJ_INDEX_DATA
{
    EXTRA_DESCR_DATA* first_extradesc;
    EXTRA_DESCR_DATA* last_extradesc;
    AFFECT_DATA* first_affect;
    AFFECT_DATA* last_affect;
    MPROG_DATA* mudprogs; /* objprogs */
    int progtypes;        /* objprogs */
    char* name;
    char* short_descr;
    char* description;
    char* action_desc;
    int vnum;
    sh_int level;
    sh_int item_type;
    int extra_flags;
    int magic_flags; /*Need more bitvectors for spells - Scryn*/
    int wear_flags;
    sh_int count;
    sh_int weight;
    int cost;
    int value[6];
    int serial;
    sh_int layers;
    int rent; /* Unused */
};

/*
 * One object.
 */
struct OBJ_DATA
{
    OBJ_DATA* next;
    OBJ_DATA* prev;
    OBJ_DATA* next_content;
    OBJ_DATA* prev_content;
    OBJ_DATA* first_content;
    OBJ_DATA* last_content;
    OBJ_DATA* in_obj;
    CHAR_DATA* carried_by;
    EXTRA_DESCR_DATA* first_extradesc;
    EXTRA_DESCR_DATA* last_extradesc;
    AFFECT_DATA* first_affect;
    AFFECT_DATA* last_affect;
    OBJ_INDEX_DATA* pIndexData;
    ROOM_INDEX_DATA* in_room;
    char* armed_by;
    char* name;
    char* short_descr;
    char* description;
    char* action_desc;
    sh_int item_type;
    sh_int mpscriptpos;
    int extra_flags;
    int magic_flags; /*Need more bitvectors for spells - Scryn*/
    int wear_flags;
    int blaster_setting;
    MPROG_ACT_LIST* mpact; /* mudprogs */
    int mpactnum;          /* mudprogs */
    sh_int wear_loc;
    sh_int weight;
    char* killer; /* This serves one real purpose. When making a corpse we assign the killers name to it. */
    int cost;
    sh_int level;
    sh_int timer;
    int value[6];
    sh_int count; /* support for object grouping */
    int serial;   /* serial number	       */
};

/*
 * Exit data.
 */
struct EXIT_DATA
{
    EXIT_DATA* prev;          /* previous exit in linked list	*/
    EXIT_DATA* next;          /* next exit in linked list	*/
    EXIT_DATA* rexit;         /* Reverse exit pointer		*/
    ROOM_INDEX_DATA* to_room; /* Pointer to destination room	*/
    char* keyword;            /* Keywords for exit or door	*/
    char* description;        /* Description of exit		*/
    int vnum;                 /* Vnum of room exit leads to	*/
    int rvnum;                /* Vnum of room in opposite dir	*/
    int exit_info;            /* door states & other flags	*/
    int key;                  /* Key vnum			*/
    sh_int vdir;              /* Physical "direction"		*/
    sh_int distance;          /* how far to the next room	*/
    int keypad;               /* Keypad Password -Riketsu     */
};

/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'H': hide an object
 *   'B': set a bitvector
 *   'T': trap an object
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct RESET_DATA
{
    RESET_DATA* next;
    RESET_DATA* prev;
    RESET_DATA* first_reset;
    RESET_DATA* last_reset;
    RESET_DATA* next_reset;
    RESET_DATA* prev_reset;
    char command;
    int extra;
    int arg1;
    int arg2;
    int arg3;
};

/* Constants for arg2 of 'B' resets. */
#define BIT_RESET_DOOR 0
#define BIT_RESET_OBJECT 1
#define BIT_RESET_MOBILE 2
#define BIT_RESET_ROOM 3
#define BIT_RESET_TYPE_MASK 0xFF /* 256 should be enough */
#define BIT_RESET_DOOR_THRESHOLD 8
#define BIT_RESET_DOOR_MASK 0xFF00 /* 256 should be enough */
#define BIT_RESET_SET BV30
#define BIT_RESET_TOGGLE BV31
#define BIT_RESET_FREEBITS 0x3FFF0000 /* For reference */

/*
 * Area definition.
 */
struct AREA_DATA
{
    AREA_DATA* next;
    AREA_DATA* prev;
    AREA_DATA* next_sort;
    AREA_DATA* prev_sort;
    ROOM_INDEX_DATA* first_room;
    ROOM_INDEX_DATA* last_room;
    PLANET_DATA* planet;
    AREA_DATA* next_on_planet;
    AREA_DATA* prev_on_planet;
    char* name;
    char* filename;
    int flags;
    sh_int status; /* h, 8/11 */
    sh_int age;
    sh_int nplayer;
    sh_int reset_frequency;
    int low_r_vnum;
    int hi_r_vnum;
    int low_o_vnum;
    int hi_o_vnum;
    int low_m_vnum;
    int hi_m_vnum;
    int low_soft_range;
    int hi_soft_range;
    int low_hard_range;
    int hi_hard_range;
    char* author;   /* Scryn */
    char* resetmsg; /* Rennard */
    sh_int max_players;
    int mkills;
    int mdeaths;
    int pkills;
    int pdeaths;
    int gold_looted;
    int illegal_pk;
    int high_economy;
    int low_economy;
};

/*
 * Load in the gods building data. -- Altrag
 */
struct GOD_DATA
{
    GOD_DATA* next;
    GOD_DATA* prev;
    int level;
    int low_r_vnum;
    int hi_r_vnum;
    int low_o_vnum;
    int hi_o_vnum;
    int low_m_vnum;
    int hi_m_vnum;
};

/*
 * Used to keep track of system settings and statistics		-Thoric
 */
struct SYSTEM_DATA
{
    int maxplayers;                /* Maximum players this boot   */
    int alltimemax;                /* Maximum players ever	  */
    char* mudname;                 /* Name of the mud */
    char* mud_acronym;             /* Acronym of the mud */
    char* time_of_max;             /* Time of max ever */
    bool NO_NAME_RESOLVING;        /* Hostnames are not resolved  */
    bool DENY_NEW_PLAYERS;         /* New players cannot connect  */
    bool WAIT_FOR_AUTH;            /* New players must be auth'ed */
    sh_int newbie_purge;           /* Level to auto-purge newbies at - Samson 12-27-98 */
    sh_int regular_purge;          /* Level to purge normal players at - Samson 12-27-98 */
    bool CLEANPFILES;              /* Should the mud clean up pfiles daily? - Samson 12-27-98 */
    sh_int read_all_mail;          /* Read all player mail(was 54)*/
    sh_int read_mail_free;         /* Read mail for free (was 51) */
    sh_int write_mail_free;        /* Write mail for free(was 51) */
    sh_int take_others_mail;       /* Take others mail (was 54)   */
    sh_int muse_level;             /* Level of muse channel */
    sh_int think_level;            /* Level of think channel LEVEL_HIGOD*/
    sh_int build_level;            /* Level of build channel LEVEL_BUILD*/
    sh_int log_level;              /* Level of log channel LEVEL LOG*/
    sh_int level_modify_proto;     /* Level to modify prototype stuff LEVEL_LESSER */
    sh_int level_override_private; /* override private flag */
    sh_int level_mset_player;      /* Level to mset a player */
    sh_int stun_plr_vs_plr;        /* Stun mod player vs. player */
    sh_int stun_regular;           /* Stun difficult */
    sh_int dam_plr_vs_plr;         /* Damage mod player vs. player */
    sh_int dam_plr_vs_mob;         /* Damage mod player vs. mobile */
    sh_int dam_mob_vs_plr;         /* Damage mod mobile vs. player */
    sh_int dam_mob_vs_mob;         /* Damage mod mobile vs. mobile */
    sh_int level_getobjnotake;     /* Get objects without take flag */
    sh_int level_forcepc;          /* The level at which you can use force on players. */
    sh_int wizlock;
    sh_int max_sn;         /* Max skills */
    char* guild_overseer;  /* Pointer to char containing the name of the */
    char* guild_advisor;   /* guild overseer and advisor. */
    int save_flags;        /* Toggles for saving conditions */
    sh_int save_frequency; /* How old to autosave someone */
    sh_int privwoverride;  /*Level of Imms who when walking dont get hte private message */
};

/*
 * Room type.
 */
struct ROOM_INDEX_DATA
{
    CHAR_DATA* first_person;
    CHAR_DATA* last_person;
    OBJ_DATA* first_content;
    OBJ_DATA* last_content;
    EXTRA_DESCR_DATA* first_extradesc;
    EXTRA_DESCR_DATA* last_extradesc;
    AREA_DATA* area;
    EXIT_DATA* first_exit;
    EXIT_DATA* last_exit;
    ROOM_INDEX_DATA* next_in_area;
    ROOM_INDEX_DATA* prev_in_area;
    ROOM_INDEX_DATA* next_in_ship;
    ROOM_INDEX_DATA* prev_in_ship;
    char* name;
    int exvnum;
    MAP_DATA* map; /* maps */
    SHIP_DATA* first_ship;
    SHIP_DATA* last_ship;
    char* description;
    int vnum;
    int room_flags;
    int room_flags2;
    MPROG_ACT_LIST* mpact; /* mudprogs */
    int mpactnum;          /* mudprogs */
    MPROG_DATA* mudprogs;  /* mudprogs */
    sh_int mpscriptpos;
    int progtypes; /* mudprogs */
    sh_int light;
    sh_int sector_type;
    int tele_vnum;
    sh_int tele_delay;
    sh_int tunnel; /* max people that will fit */
    RESET_DATA* first_reset;
    RESET_DATA* last_reset;
    RESET_DATA* last_mob_reset;
    RESET_DATA* last_obj_reset;
    ROOM_INDEX_DATA* next_aroom; /* Rooms within an area */
    ROOM_INDEX_DATA* prev_aroom;
};

/*
 * Delayed teleport type.
 */
struct TELEPORT_DATA
{
    TELEPORT_DATA* next;
    TELEPORT_DATA* prev;
    ROOM_INDEX_DATA* room;
    sh_int timer;
};

/*
 * Types of skill numbers.  Used to keep separate lists of sn's
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED -1
#define TYPE_MISSILE 111
#define TYPE_HIT 1000      /* allows for 1000 skills/spells */
#define TYPE_HERB 2000     /* allows for 1000 attack types  */
#define TYPE_PERSONAL 3000 /* allows for 1000 herb types    */

/*
 *  Target types.
 */
typedef enum
{
    TAR_IGNORE,
    TAR_CHAR_OFFENSIVE,
    TAR_CHAR_DEFENSIVE,
    TAR_CHAR_SELF,
    TAR_OBJ_INV
} target_types;

typedef enum
{
    SKILL_UNKNOWN,
    SKILL_SPELL,
    SKILL_SKILL,
    SKILL_WEAPON,
    SKILL_TONGUE,
    SKILL_HERB
} skill_types;

struct TIMERSET
{
    int num_uses;
    std::chrono::steady_clock::duration total_time;
    std::chrono::steady_clock::duration min_time;
    std::chrono::steady_clock::duration max_time;
};

/*
 * Skills include spells as a particular case.
 */
struct SKILL_TYPE
{
    char* name;              /* Name of skill		*/
    SPELL_FUN* spell_fun;    /* Spell pointer (for spells)	*/
    char* spell_fun_name;    /* Spell function name - Trax */
    DO_FUN* skill_fun;       /* Skill pointer (for skills)	*/
    char* skill_fun_name;    /* Skill function name - Trax */
    sh_int target;           /* Legal targets		*/
    sh_int minimum_position; /* Position for caster / user	*/
    sh_int slot;             /* Slot for #OBJECT loading	*/
    sh_int min_mana;         /* Minimum mana used		*/
    sh_int beats;            /* Rounds required to use skill	*/
    char* noun_damage;       /* Damage message		*/
    char* msg_off;           /* Wear off message		*/
    sh_int guild;            /* Which guild the skill belongs to */
    sh_int min_level;        /* Minimum level to be able to cast */
    sh_int type;             /* Spell/Skill/Weapon/Tongue	*/
    int flags;               /* extra stuff			*/
    char* hit_char;          /* Success message to caster	*/
    char* hit_vict;          /* Success message to victim	*/
    char* hit_room;          /* Success message to room	*/
    char* miss_char;         /* Failure message to caster	*/
    char* miss_vict;         /* Failure message to victim	*/
    char* miss_room;         /* Failure message to room	*/
    char* die_char;          /* Victim death msg to caster	*/
    char* die_vict;          /* Victim death msg to victim	*/
    char* die_room;          /* Victim death msg to room	*/
    char* imm_char;          /* Victim immune msg to caster	*/
    char* imm_vict;          /* Victim immune msg to victim	*/
    char* imm_room;          /* Victim immune msg to room	*/
    char* dice;              /* Dice roll			*/
    int value;               /* Misc value			*/
    char saves;              /* What saving spell applies	*/
    char difficulty;         /* Difficulty of casting/learning */
    SMAUG_AFF* affects;      /* Spell affects, if any	*/
    char* components;        /* Spell components, if any	*/
    char* teachers;          /* Skill requires a special teacher */
    char participants;       /* # of required participants	*/
    TIMERSET userec;         /* Usage record			*/
    int alignment;           /* for jedi powers */
};

struct AUCTION_DATA
{
    OBJ_DATA* item;    /* a pointer to the item */
    CHAR_DATA* seller; /* a pointer to the seller - which may NOT quit */
    CHAR_DATA* buyer;  /* a pointer to the buyer - which may NOT quit */
    int bet;           /* last bet - or 0 if noone has bet anything */
    sh_int going;      /* 1,2, sold */
    sh_int pulse;      /* how many pulses (.25 sec) until another call-out ? */
    int starting;
};

/*
 * These are skill_lookup return values for basic skills and spells.
 */
extern sh_int gsn_smallspace;
extern sh_int gsn_mediumspace;
extern sh_int gsn_largespace;
extern sh_int gsn_weaponsystems;
extern sh_int gsn_navigation;
extern sh_int gsn_shipsystems;
extern sh_int gsn_tractorbeams;
extern sh_int gsn_spacecombat;
extern sh_int gsn_spacecombat2;
extern sh_int gsn_spacecombat3;
extern sh_int gsn_bomb;
extern sh_int gsn_split_s;
extern sh_int gsn_shipdesign;
extern sh_int gsn_chandelle;

/* Technician skills */
extern sh_int gsn_makemodule;
extern sh_int gsn_installmodule;
extern sh_int gsn_showmodules;
extern sh_int gsn_shipmaintenance;
extern sh_int gsn_scanbugs;
extern sh_int gsn_removebug;
extern sh_int gsn_removemodule;
extern sh_int gsn_makejetpack;

/* These are bh skills */
extern sh_int gsn_ambush;
extern sh_int gsn_bind;
extern sh_int gsn_gag;

extern sh_int gsn_battle_command;
extern sh_int gsn_reinforcements;
extern sh_int gsn_postguard;

extern sh_int gsn_addpatrol;
extern sh_int gsn_eliteguard;
extern sh_int gsn_specialforces;
extern sh_int gsn_jail;
extern sh_int gsn_smalltalk;
extern sh_int gsn_propeganda;
extern sh_int gsn_bribe;
extern sh_int gsn_seduce;
extern sh_int gsn_masspropeganda;
extern sh_int gsn_gather_intelligence;

/* hunter assassin gsn ints */
extern sh_int gsn_plantbug;
extern sh_int gsn_showbugs;
extern sh_int gsn_silent;
extern sh_int gsn_retreat;

/* The gsn ints for the slicers */
extern sh_int gsn_spy;
extern sh_int gsn_makecommsystem;
extern sh_int gsn_sabotage;
extern sh_int gsn_commsystem;
extern sh_int gsn_codecrack;
extern sh_int gsn_slicebank;
extern sh_int gsn_inquire;
extern sh_int gsn_makedatapad;
extern sh_int gsn_disable;
extern sh_int gsn_assignpilot;
extern sh_int gsn_checkprints;

extern sh_int gsn_torture;
extern sh_int gsn_snipe;
extern sh_int gsn_throw;
extern sh_int gsn_deception;
extern sh_int gsn_disguise;
extern sh_int gsn_mine;
extern sh_int gsn_first_aid;

extern sh_int gsn_beg;
extern sh_int gsn_makeblade;
extern sh_int gsn_makebug;
extern sh_int gsn_makebeacon;
extern sh_int gsn_makepike;
extern sh_int gsn_makejewelry;
extern sh_int gsn_makeblaster;
extern sh_int gsn_makelight;
extern sh_int gsn_makecomlink;
extern sh_int gsn_makegrenade;
extern sh_int gsn_makeshipbomb;
extern sh_int gsn_makelandmine;
extern sh_int gsn_makearmor;
extern sh_int gsn_makeshield;
extern sh_int gsn_makecontainer;
extern sh_int gsn_gemcutting;
extern sh_int gsn_makelightsaber;
extern sh_int gsn_makeduallightsaber;
extern sh_int gsn_repair;
extern sh_int gsn_shiprepair;
extern sh_int gsn_spice_refining;

extern sh_int gsn_detrap;
extern sh_int gsn_backstab;
extern sh_int gsn_dualstab;
extern sh_int gsn_bargain;
extern sh_int gsn_circle;
extern sh_int gsn_dodge;
extern sh_int gsn_hide;
extern sh_int gsn_concealment;
extern sh_int gsn_peek;
extern sh_int gsn_pick_lock;
extern sh_int gsn_scan;
extern sh_int gsn_sneak;
extern sh_int gsn_steal;
extern sh_int gsn_gouge;
extern sh_int gsn_track;
extern sh_int gsn_search;
extern sh_int gsn_dig;
extern sh_int gsn_mount;
extern sh_int gsn_bashdoor;
extern sh_int gsn_berserk;
extern sh_int gsn_hitall;
extern sh_int gsn_pickshiplock;
extern sh_int gsn_hijack;

extern sh_int gsn_disarm;
extern sh_int gsn_enhanced_damage;
extern sh_int gsn_kick;
extern sh_int gsn_parry;
extern sh_int gsn_rescue;
extern sh_int gsn_second_attack;
extern sh_int gsn_third_attack;
extern sh_int gsn_dual_wield;
extern sh_int gsn_reflect;

extern sh_int gsn_aid;
extern sh_int gsn_plantbeacon;
extern sh_int gsn_showbeacons;
extern sh_int gsn_checkbeacons;
extern sh_int gsn_nullifybeacons;
extern sh_int gsn_makebinders;
extern sh_int gsn_launchers;
extern sh_int gsn_makemissile;
extern sh_int gsn_makeempgrenade;
extern sh_int gsn_makegoggles;
extern sh_int gsn_truesight;
extern sh_int gsn_barrelroll;
extern sh_int gsn_juke;

/* used to do specific lookups */
extern sh_int gsn_first_spell;
extern sh_int gsn_first_skill;
extern sh_int gsn_first_weapon;
extern sh_int gsn_first_tongue;
extern sh_int gsn_top_sn;

/* spells */
extern sh_int gsn_blindness;
extern sh_int gsn_charm_person;
extern sh_int gsn_aqua_breath;
extern sh_int gsn_invis;
extern sh_int gsn_mass_invis;
extern sh_int gsn_poison;
extern sh_int gsn_sleep;
extern sh_int gsn_possess;
extern sh_int gsn_fireball;       /* for fireshield  */
extern sh_int gsn_lightning_bolt; /* for shockshield */

/* newer attack skills */
extern sh_int gsn_punch;
extern sh_int gsn_bash;
extern sh_int gsn_stun;

extern sh_int gsn_poison_weapon;
extern sh_int gsn_climb;

extern sh_int gsn_blasters;
extern sh_int gsn_force_pikes;
extern sh_int gsn_bowcasters;
extern sh_int gsn_lightsabers;
extern sh_int gsn_vibro_blades;
extern sh_int gsn_flexible_arms;
extern sh_int gsn_talonous_arms;
extern sh_int gsn_bludgeons;

extern sh_int gsn_grip;

/* languages */
extern sh_int gsn_basic;
extern sh_int gsn_wookiee;
extern sh_int gsn_twilek;
extern sh_int gsn_rodian;
extern sh_int gsn_hutt;
extern sh_int gsn_mon_calamari;
extern sh_int gsn_noghri;
extern sh_int gsn_ewok;
extern sh_int gsn_ithorian;
extern sh_int gsn_gotal;
extern sh_int gsn_devaronian;
extern sh_int gsn_binary;
extern sh_int gsn_firrerreo;
extern sh_int gsn_gamorrean;
extern sh_int gsn_togorian;
extern sh_int gsn_shistavanen;
extern sh_int gsn_jawa;
extern sh_int gsn_kubaz;
extern sh_int gsn_adarian;
extern sh_int gsn_verpine;
extern sh_int gsn_defel;
extern sh_int gsn_trandoshan;
extern sh_int gsn_hapan;
extern sh_int gsn_quarren;
extern sh_int gsn_sullustan;
extern sh_int gsn_falleen;
extern sh_int gsn_barabel;
extern sh_int gsn_yevethan;
extern sh_int gsn_gand;
extern sh_int gsn_coynite;
extern sh_int gsn_duinuogwuin;
extern sh_int gsn_droid;

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

#ifdef HASHSTR
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
#else
#define STRALLOC(point) str_dup((point))
#define QUICKLINK(point) str_dup((point))
#define QUICKMATCH(p1, p2) strcmp((p1), (p2)) == 0
#define STRFREE(point) DISPOSE((point))
#endif

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

/*
 * Structure for a command in the command lookup table.
 */
struct CMDTYPE
{
    CMDTYPE* next;
    char* name;
    DO_FUN* do_fun;
    char* fun_name;
    sh_int position;
    sh_int level;
    sh_int log;
    sh_int ooc;
    TIMERSET userec;
};

/*
 * Structure for a social in the socials table.
 */
struct SOCIALTYPE
{
    SOCIALTYPE* next;
    char* name;
    char* char_no_arg;
    char* others_no_arg;
    char* char_found;
    char* others_found;
    char* vict_found;
    char* char_auto;
    char* others_auto;
};

/*
 * Global constants.
 */
extern time_t last_restore_all_time;
extern time_t boot_time; /* this should be moved down */
extern HOUR_MIN_SEC* set_boot_time;
extern tm* new_boot_time;
extern time_t new_boot_time_t;

extern const STR_APP_TYPE str_app[26];
extern const INT_APP_TYPE int_app[26];
extern const WIS_APP_TYPE wis_app[26];
extern const DEX_APP_TYPE dex_app[30];
extern const CON_APP_TYPE con_app[26];
extern const CHA_APP_TYPE cha_app[26];
extern const LCK_APP_TYPE lck_app[26];
extern const FRC_APP_TYPE frc_app[26];
extern const RACE_TYPE race_table[MAX_RACE];
extern const LIQ_TYPE liq_table[LIQ_MAX];
extern const char* attack_table[13];
extern const char* ability_name[MAX_ABILITY];
extern const char* height_name[4];
extern const char* build_name[6];
extern const char* droid_name[8];

extern const char* skill_tname[];
extern sh_int const movement_loss[SECT_MAX];
extern const char* dir_name[];
extern const char* where_name[];
extern const sh_int rev_dir[];
extern const int trap_door[];
extern const char* r_flags[];
extern const char* r_flags2[];
extern const char* w_flags[];
extern const char* o_flags[];
extern const char* a_flags[];
extern const char* o_types[];
extern const char* a_types[];
extern const char* act_flags[];
extern const char* planet_flags[];
extern const char* mprog_flags[];
extern const char* weapon_table[13];
extern const char* spice_table[];
extern const char* plr_flags[];
extern const char* pc_flags[];
extern const char* trap_flags[];
extern const char* ris_flags[];
extern const char* trig_flags[];
extern const char* part_flags[];
extern const char* npc_race[];
extern const char* defense_flags[];
extern const char* attack_flags[];
extern const char* area_flags[];

extern int const lang_array[];
extern const char* lang_names[];

extern bool bootup;
extern char namefreq[MAX_STRING_LENGTH];
extern char bname[MAX_STRING_LENGTH];

/*
 * Global variables.
 */

extern MPSLEEP_DATA* first_mpwait;   /* Storing sleeping mud progs */
extern MPSLEEP_DATA* last_mpwait;    /* - */
extern MPSLEEP_DATA* current_mpwait; /* - */
extern int numobjsloaded;
extern int nummobsloaded;
extern int physicalobjects;
extern SYSTEM_DATA sysdata;
extern int top_sn;
extern int top_herb;

extern CMDTYPE* command_hash[126];

extern SKILL_TYPE* skill_table[MAX_SKILL];
extern SOCIALTYPE* social_index[27];
extern CHAR_DATA* cur_char;
extern ROOM_INDEX_DATA* cur_room;
extern bool cur_char_died;
extern ch_ret global_retcode;
extern SKILL_TYPE* herb_table[MAX_HERB];

extern int cur_obj;
extern int cur_obj_serial;
extern bool cur_obj_extracted;
extern obj_ret global_objcode;

extern HELP_DATA* first_help;
extern HELP_DATA* last_help;
extern SHOP_DATA* first_shop;
extern SHOP_DATA* last_shop;
extern REPAIR_DATA* first_repair;
extern REPAIR_DATA* last_repair;

extern BAN_DATA* first_ban;
extern BAN_DATA* last_ban;
extern CHAR_DATA* first_char;
extern CHAR_DATA* last_char;
extern std::vector<std::shared_ptr<DESCRIPTOR_DATA>> g_descriptors;
extern BOARD_DATA* first_board;
extern BOARD_DATA* last_board;
extern OBJ_DATA* first_object;
extern OBJ_DATA* last_object;
extern CLAN_DATA* first_clan;
extern CLAN_DATA* last_clan;
extern GUARD_DATA* first_guard;
extern GUARD_DATA* last_guard;
extern SHIP_DATA* first_ship;
extern SHIP_DATA* last_ship;
extern SPACE_DATA* first_starsystem;
extern SPACE_DATA* last_starsystem;
extern PLANET_DATA* first_planet;
extern PLANET_DATA* last_planet;
extern SENATE_DATA* first_senator;
extern SENATE_DATA* last_senator;
extern BOUNTY_DATA* first_bounty;
extern BOUNTY_DATA* last_bounty;
extern BOUNTY_DATA* first_disintegration;
extern BOUNTY_DATA* last_disintegration;
extern AREA_DATA* first_area;
extern AREA_DATA* last_area;
extern AREA_DATA* first_build;
extern AREA_DATA* last_build;
extern AREA_DATA* first_asort;
extern AREA_DATA* last_asort;
extern AREA_DATA* first_bsort;
extern AREA_DATA* last_bsort;

/*
extern		GOD_DATA	  *	first_imm;
extern		GOD_DATA	  *	last_imm;
*/
extern TELEPORT_DATA* first_teleport;
extern TELEPORT_DATA* last_teleport;
extern OBJ_DATA* extracted_obj_queue;
extern EXTRACT_CHAR_DATA* extracted_char_queue;
extern OBJ_DATA* save_equipment[MAX_WEAR][MAX_LAYERS];
extern CHAR_DATA* quitting_char;
extern CHAR_DATA* loading_char;
extern CHAR_DATA* saving_char;
extern OBJ_DATA* all_obj;

extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
extern bool fLogPC;
extern char log_buf[2 * MAX_INPUT_LENGTH];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;
extern AUCTION_DATA* auction;
extern ACT_PROG_DATA* mob_act_list;

extern BMARKET_DATA* first_market_ship;
extern BMARKET_DATA* last_market_ship;

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#define AREA_DIR "area/"
#define PLAYER_DIR "player/" /* Player files			*/
#define BACKUP_DIR "backup/" /* Backup Player files		*/
#define GOD_DIR "gods/"      /* God Info Dir			*/
#define BOARD_DIR "boards/"  /* Board data dir		*/
#define KEYS_DIR "keys/"
#define CLAN_DIR "clans/" /* Clan data dir		*/
#define SHIP_DIR "space/" // TODO duplicate?
#define SPACE_DIR "space/"
#define SHIP_PROTOTYPE_DIR "ships/"
#define FORCE_DIR "force/"
#define FORCE_HELP_DIR "force/help/"
#define PLANET_DIR "planets/"
#define GUARD_DIR "planets/"                           // TODO duplicate?
#define GUILD_DIR "guilds/"                            /* Guild data dir               */
#define HELP_FILE SYSTEM_DIR "help.txt"                /*For undefined helps*/
#define SLAY_FILE SYSTEM_DIR "slay.dat"                /* Slay data file for online editing - Samson 8-3-98 */
#define LAST_LIST SYSTEM_DIR "last.lst"                // last list
#define LAST_TEMP_LIST SYSTEM_DIR "ltemp.lst"          // temp file for the last list so the data can be copyover over
#define BUILD_DIR "building/"                          /* Online building save dir     */
#define SYSTEM_DIR "system/"                           /* Main system files		*/
#define PROG_DIR "mudprogs/" /* MUDProg files		*/ // TODO unused?
#define CORPSE_DIR "corpses/"                          /* Corpses			*/
#define AREA_LIST "area.lst"                           /* List of areas		*/
#define BAN_LIST "ban.lst"                             /* List of bans                 */
#define CLAN_LIST "clan.lst"                           /* List of clans		*/
#define SHIP_LIST "ship.lst"
#define PROTOTYPE_LIST "prototype.lst"
#define PLANET_LIST "planet.lst"
#define SPACE_LIST "space.lst"
#define BOUNTY_LIST "bounty.lst"
#define disintegration_LIST "disintegration.lst"
#define SENATE_LIST "senate.lst" /* List of senators		*/
#define GUILD_LIST "guild.lst"   /* List of guilds               */
#define GOD_LIST "gods.lst"      /* List of gods			*/
#define GUARD_LIST "guard.lst"

#define BOARD_FILE "boards.txt"      /* For bulletin boards	 */
#define SHUTDOWN_FILE "shutdown.txt" /* For 'shutdown'	 */

#define RIPSCREEN_FILE SYSTEM_DIR "mudrip.rip"
#define RIPTITLE_FILE SYSTEM_DIR "mudtitle.rip"
#define ANSITITLE_FILE SYSTEM_DIR "mudtitle.ans"
#define ASCTITLE_FILE SYSTEM_DIR "mudtitle.asc"
#define BOOTLOG_FILE SYSTEM_DIR "boot.txt"            /* Boot up error file	 */
#define BUG_FILE SYSTEM_DIR "sysbugs.txt"             /* For 'bug' and bug( )*/
#define PLRBUG_FILE SYSTEM_DIR "plrbugs.txt"          /* Used for player bugs */
#define IDEA_FILE SYSTEM_DIR "ideas.txt"              /* For 'idea'		 */
#define CHANGE_FILE SYSTEM_DIR "changes.txt"          /* Changes file - txt  */
#define DEBUG_FILE SYSTEM_DIR "debug.txt"             /* Catch-all for debug */
#define TYPO_FILE SYSTEM_DIR "typos.txt"              /* For 'typo'		 */
#define LOG_FILE SYSTEM_DIR "log.txt"                 /* For talking in logged rooms */
#define WIZLIST_FILE SYSTEM_DIR "WIZLIST"             /* Wizlist		 */
#define REQUEST_PIPE SYSTEM_DIR "REQUESTS"     /* Request FIFO	 */
#define SKILL_FILE SYSTEM_DIR "skills.dat"     /* Skill table	 */
#define HERB_FILE SYSTEM_DIR "herbs.dat"       /* Herb table		 */
#define SOCIAL_FILE SYSTEM_DIR "socials.dat"   /* Socials		 */
#define COMMAND_FILE SYSTEM_DIR "commands.dat" /* Commands		 */
#define NAMEBAN_FILE SYSTEM_DIR "nameban.dat"  /* Nameban		 */
#define USAGE_FILE                                                                                                     \
    SYSTEM_DIR "usage.txt"                      /* How many people are on                                              \
                               every half hour - trying to                                                             \
                               determine best reboot time */
#define TEMP_FILE SYSTEM_DIR "charsave.tmp"     /* More char save protect */
#define COPYOVER_FILE SYSTEM_DIR "copyover.dat" /* for warm reboots	 */
#define EXE_FILE "../bin/swr"                   /* executable path	 */
#define SLOG_FILE "../.slog/slog.txt"           /* Secret Log		 */

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD CHAR_DATA
#define MID MOB_INDEX_DATA
#define OD OBJ_DATA
#define OID OBJ_INDEX_DATA
#define RID ROOM_INDEX_DATA
#define SF SPEC_FUN
#define BD BOARD_DATA
#define EDD EXTRA_DESCR_DATA
#define RD RESET_DATA
#define ED EXIT_DATA
#define ST SOCIALTYPE
#define CO COUNCIL_DATA
#define DE DEITY_DATA
#define SK SKILL_TYPE
#define SH SHIP_DATA

/* editor.c cronel new editor */
#define start_editing(ch, data) start_editing_nolimit(ch, data, MAX_STRING_LENGTH)
void start_editing_nolimit(CHAR_DATA* ch, char* data, sh_int max_size);
void stop_editing(CHAR_DATA* ch);
void edit_buffer(CHAR_DATA* ch, char* argument);
char* copy_buffer(CHAR_DATA* ch);
void set_editor_desc(CHAR_DATA* ch, const char* desc);
void editor_desc_printf(CHAR_DATA* ch, const char* desc_fmt, ...);

/* pfiles.c */
void remove_member(char* name, char* shortname);
void add_member(char* name, char* shortname);

/* act_comm.c */
bool check_parse_name(const char* name);
void sound_to_room(ROOM_INDEX_DATA* room, const char* argument);
bool circle_follow(CHAR_DATA* ch, CHAR_DATA* victim);
char* smaug_crypt(const char* pwd);
void add_follower(CHAR_DATA* ch, CHAR_DATA* master);
void stop_follower(CHAR_DATA* ch);
void die_follower(CHAR_DATA* ch);
bool is_same_group(CHAR_DATA* ach, CHAR_DATA* bch);
void send_rip_screen(CHAR_DATA* ch);
void send_rip_title(CHAR_DATA* ch);
void send_ansi_title(CHAR_DATA* ch);
void send_ascii_title(CHAR_DATA* ch);
void to_channel(const char* argument, int channel, const char* verb, sh_int level);
void talk_auction(char* argument);
bool knows_language(CHAR_DATA* ch, int language, CHAR_DATA* cch);
bool can_learn_lang(CHAR_DATA* ch, int language);
int countlangs(int languages);
std::string obj_short(OBJ_DATA* obj);

/* act_info.c */
int get_door(char* arg);
char* format_obj_to_char(OBJ_DATA* obj, CHAR_DATA* ch, bool fShort);
void show_list_to_char(OBJ_DATA* list, CHAR_DATA* ch, bool fShort, bool fShowNothing);

/* act_move.c */
void clear_vrooms(void);
ED* find_door(CHAR_DATA* ch, const char* arg, bool quiet);
ED* get_exit(ROOM_INDEX_DATA* room, sh_int dir);
ED* get_exit_to(ROOM_INDEX_DATA* room, sh_int dir, int vnum);
ED* get_exit_num(ROOM_INDEX_DATA* room, sh_int count);
ch_ret move_char(CHAR_DATA* ch, EXIT_DATA* pexit, int fall);
void teleport(CHAR_DATA* ch, int room, int flags);
sh_int encumbrance(CHAR_DATA* ch, sh_int move);
bool will_fall(CHAR_DATA* ch, int fall);
int wherehome(CHAR_DATA* ch);

/* act_obj.c */

obj_ret damage_obj(OBJ_DATA* obj);
sh_int get_obj_resistance(OBJ_DATA* obj);
bool remove_obj(CHAR_DATA* ch, int iWear, bool fReplace);
void save_clan_storeroom(CHAR_DATA* ch, CLAN_DATA* clan);
void obj_fall(OBJ_DATA* obj, bool through);

/* act_wiz.c */
void close_area(AREA_DATA* pArea);
AREA_DATA* get_area(char* argument);
RID* find_location(CHAR_DATA* ch, char* arg);
void echo_to_room(sh_int AT_COLOR, ROOM_INDEX_DATA* room, const char* argument);
void echo_to_all(sh_int AT_COLOR, const char* argument, sh_int tar);
void get_reboot_string(void);
tm* update_time(struct tm* old_time);
void free_social(SOCIALTYPE* social);
void add_social(SOCIALTYPE* social);
void free_command(CMDTYPE* command);
void unlink_command(CMDTYPE* command);
void add_command(CMDTYPE* command);

/* boards.c */
void load_boards(void);
BD* get_board(OBJ_DATA* obj);
void free_note(NOTE_DATA* pnote);

/* build.c */
char* flag_string(int bitvector, const char* flagarray[]);
int get_mpflag(const char* flag);
int get_dir(char* txt);
char* strip_cr(char* str);
int get_vip_flag(const char* flag);
int get_wanted_flag(const char* flag);

/* changes.c */
void load_changes(void);
void save_changes(void);
void delete_change(int num);

/* clans.c */
CLAN_DATA* get_clan(const char* name);
void load_clans(void);
void save_clan(CLAN_DATA* clan);
void load_senate(void);
void save_senate(void);
PLANET_DATA* get_planet(char* name);
void load_planets(void);
void save_planet(PLANET_DATA* planet);
long get_taxes(PLANET_DATA* planet);

/* bounty.c */
BOUNTY_DATA* get_disintegration(char* target);
void load_bounties(void);
void save_bounties(void);
void save_disintegrations(void);
void remove_disintegration(BOUNTY_DATA* bounty);
void claim_disintegration(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_disintegration(CHAR_DATA* victim);

/* force.c */

bool check_reflect(CHAR_DATA* ch, CHAR_DATA* victim, int dam);
void write_all_forceskills();
void save_forceskill(FORCE_SKILL* fskill);
void write_forceskill_list();
bool load_forceskill(const char* forceskillfile);
void fread_forceskill(FORCE_SKILL* fskill, FILE* fp);
void write_all_forcehelps();
void save_forcehelp(FORCE_HELP* fhelp);
void write_forcehelp_list();
bool load_forcehelp(char const* forcehelpfile);
void fread_forcehelp(FORCE_HELP* fhelp, FILE* fp);
int check_force_skill(CHAR_DATA* ch, const char* command, char* argument);
void load_force_skills(void);
void load_force_help(void);
DO_FUN* get_force_skill_function(char* name);
FORCE_SKILL* get_force_skill(const char* argument);
FORCE_HELP* get_force_help(const char* fname, char* type);
void force_send_to_room(CHAR_DATA* ch, CHAR_DATA* victim, const char* msg);
CHAR_DATA* force_get_victim(CHAR_DATA* ch, char* argument, int loc);
const char* force_get_possessive(CHAR_DATA* ch);
const char* force_get_objective(CHAR_DATA* ch);
const char* force_get_pronoun(CHAR_DATA* ch);
const char* force_parse_string(CHAR_DATA* ch, CHAR_DATA* victim, const char* msg);
void force_learn_from_failure(CHAR_DATA* ch, FORCE_SKILL* fskill);
void force_learn_from_success(CHAR_DATA* ch, FORCE_SKILL* fskill);
FORCE_SKILL* force_test_skill_use(const char* skill_name, CHAR_DATA* ch, int skill_type);
const char* force_get_level(CHAR_DATA* ch);
int force_promote_ready(CHAR_DATA* ch);
void draw_force_line(CHAR_DATA* ch, int length);
void draw_force_line_rev(CHAR_DATA* ch, int length);
void update_force(void);

/* space.c */
SH* get_ship(char* name);
void load_ships(void);
void placeships(void);
void save_ship(SHIP_DATA* ship);
void load_space(void);
void save_starsystem(SPACE_DATA* starsystem);
SPACE_DATA* starsystem_from_name(const char* name);
SPACE_DATA* starsystem_from_room(ROOM_INDEX_DATA* room);
SHIP_DATA* ship_from_entrance(int vnum);
SHIP_DATA* ship_from_room(int vnum);
SHIP_DATA* ship_from_hanger(int vnum);
SHIP_DATA* ship_from_pilotseat(int vnum);
SHIP_DATA* ship_from_cockpit(int vnum);
SHIP_DATA* ship_from_turret(int vnum);
SHIP_DATA* ship_from_engine(int vnum);
SHIP_DATA* ship_from_pilot(char* name);
SHIP_DATA* get_ship_here(char* name, SPACE_DATA* starsystem);
void showstarsystem(CHAR_DATA* ch, SPACE_DATA* starsystem);
void update_space(void);
void recharge_ships(void);
void move_ships(void);
void update_bus(void);
void update_traffic(void);
bool check_pilot(CHAR_DATA* ch, SHIP_DATA* ship);
bool is_rental(CHAR_DATA* ch, SHIP_DATA* ship);
void echo_to_ship(int color, SHIP_DATA* ship, const char* argument);
void echo_to_cockpit(int color, SHIP_DATA* ship, const char* argument);
void echo_to_system(int color, SHIP_DATA* ship, const char* argument, SHIP_DATA* ignore);
bool extract_ship(SHIP_DATA* ship);
bool ship_to_room(SHIP_DATA* ship, int vnum);
bool ship_to_room2(SHIP_DATA* ship, ROOM_INDEX_DATA* shipto);
long get_ship_value(SHIP_DATA* ship);
bool rent_ship(CHAR_DATA* ch, SHIP_DATA* ship);
void damage_ship(SHIP_DATA* ship, int min, int max);
void damage_ship_ch(SHIP_DATA* ship, int min, int max, CHAR_DATA* ch);
void destroy_ship(SHIP_DATA* ship, CHAR_DATA* ch, const char* reason);
void ship_to_starsystem(SHIP_DATA* ship, SPACE_DATA* starsystem);
void ship_from_starsystem(SHIP_DATA* ship, SPACE_DATA* starsystem);
void new_missile(SHIP_DATA* ship, SHIP_DATA* target, CHAR_DATA* ch, int missiletype);
void extract_missile(MISSILE_DATA* missile);
SHIP_DATA* ship_in_room(ROOM_INDEX_DATA* room, char* name);

/* morespace.c */
SHIP_PROTOTYPE* get_ship_prototype(char* name);
void load_prototypes(void);
void save_ship_protoype(SHIP_PROTOTYPE* prototype);
long int get_prototype_value(SHIP_PROTOTYPE* prototype);
void create_ship_rooms(SHIP_DATA* ship);

/* comm.c */
const char* PERS(CHAR_DATA* ch, CHAR_DATA* looker);
FELLOW_DATA* knowsof(CHAR_DATA* ch, CHAR_DATA* victim);
void close_socket(DESCRIPTOR_DATA* dclose, bool force);
void close_socket(DESCRIPTOR_DATA* dclose, bool force);
void write_to_buffer(DESCRIPTOR_DATA* d, std::string_view string);
void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, std::string_view string);
void write_to_buffer(DESCRIPTOR_DATA* d, const char* txt, size_t length);
void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, const char* txt, size_t length);
void write_to_pager(DESCRIPTOR_DATA* d, const char* txt, size_t length);
void send_to_char(const char* txt, CHAR_DATA* ch);
void send_to_char_color(const char* txt, CHAR_DATA* ch);
void send_to_desc_color(const char* txt, DESCRIPTOR_DATA* d);
void send_to_desc_color2(const char* txt, DESCRIPTOR_DATA* d);
void send_to_desc_color2(const char* txt, std::shared_ptr<DESCRIPTOR_DATA> d);
void send_to_char_noand(const char* txt, CHAR_DATA* ch);
void send_to_pager(const char* txt, CHAR_DATA* ch);
void send_to_pager_color(const char* txt, CHAR_DATA* ch);
void set_char_color(sh_int AType, CHAR_DATA* ch);
void set_pager_color(sh_int AType, CHAR_DATA* ch);
void ch_printf(CHAR_DATA* ch, const char* fmt, ...);
char* chrmax(char* src, int length);
int strlen_color(char* argument);
char* format_str(char* str, int len);
void pager_printf(CHAR_DATA* ch, const char* fmt, ...);
void log_printf(char* fmt, ...);
void copyover_recover(void);

void act(sh_int AType, const char* format, CHAR_DATA* ch, const void* arg1, const void* arg2, int type);

/* reset.c */
void wipe_resets(ROOM_INDEX_DATA* room);
RD* make_reset(char letter, int extra, int arg1, int arg2, int arg3);
RD* add_reset(ROOM_INDEX_DATA* room, char letter, int extra, int arg1, int arg2, int arg3);
void reset_area(AREA_DATA* pArea);

/* db.c */
void show_file(CHAR_DATA* ch, char const* filename);
char* str_dup(const char* str);
std::string centertext(const std::string_view& text, int size);
void boot_db(bool fCopyOver);
void area_update(void);
void add_char(CHAR_DATA* ch);
CD* create_mobile(MOB_INDEX_DATA* pMobIndex);
OD* create_object(OBJ_INDEX_DATA* pObjIndex, int level);
void clear_char(CHAR_DATA* ch);
void free_char(CHAR_DATA* ch);
char* get_extra_descr(const char* name, EXTRA_DESCR_DATA* ed);
MID* get_mob_index(int vnum);
OID* get_obj_index(int vnum);
RID* get_room_index(int vnum);
char fread_letter(FILE* fp);
int fread_number(FILE* fp);
char* fread_string(FILE* fp);
char* fread_string_nohash(FILE* fp);
void fread_to_eol(FILE* fp);
char* fread_word(FILE* fp);
char* fread_line(FILE* fp);
int number_fuzzy(int number);
int number_range(int from, int to);
int number_percent(void);
int number_door(void);
int number_bits(int width);
int number_mm(void);
int dice(int number, int size);
int interpolate(int level, int value_00, int value_32);
void smash_tilde(char* str);
void hide_tilde(char* str);
char* show_tilde(const char* str);
bool str_cmp(const char* astr, const char* bstr);
bool str_prefix(const char* astr, const char* bstr);
bool str_infix(const char* astr, const char* bstr);
bool str_suffix(const char* astr, const char* bstr);
std::string capitalize(const std::string_view& str);
std::string strlower(const std::string_view& str);
std::string strupper(const std::string_view& str);
std::string aoran(const std::string_view& str);
void append_file(CHAR_DATA* ch, const char* file, char* str);
void append_to_file(const char* file, char* str);
void prepend_to_file(const char* file, char* str);
void bug(const char* str, ...);
void log_string_plus(const char* str, sh_int log_type, sh_int level);
void log(const char* str, sh_int log_type, sh_int level, ...);
RID* make_room(int vnum, AREA_DATA* area);
RID* make_ship_room(SHIP_DATA* ship, int vnum);
OID* make_object(int vnum, int cvnum, char* name);
MID* make_mobile(int vnum, int cvnum, char* name);
ED* make_exit(ROOM_INDEX_DATA* pRoomIndex, ROOM_INDEX_DATA* to_room, sh_int door);
void add_help(HELP_DATA* pHelp);
void fix_area_exits(AREA_DATA* tarea);
void load_area_file(AREA_DATA* tarea, char* filename);
void randomize_exits(ROOM_INDEX_DATA* room, sh_int maxdir);
void make_wizlist(void);
void delete_room(ROOM_INDEX_DATA* room);
void delete_obj(OBJ_INDEX_DATA* obj);
void delete_mob(MOB_INDEX_DATA* mob);
/* Functions to add to sorting lists. -- Altrag */
/*void	mob_sort	 ( MOB_INDEX_DATA *pMob ) ;
void	obj_sort	 ( OBJ_INDEX_DATA *pObj ) ;
void	room_sort	 ( ROOM_INDEX_DATA *pRoom ) ;*/
void sort_area(AREA_DATA* pArea, bool proto);
float fread_float(FILE* fp);

/* build.c */
bool can_rmodify(CHAR_DATA* ch, ROOM_INDEX_DATA* room);
bool can_omodify(CHAR_DATA* ch, OBJ_DATA* obj);
bool can_mmodify(CHAR_DATA* ch, CHAR_DATA* mob);
bool can_medit(CHAR_DATA* ch, MOB_INDEX_DATA* mob);
void free_reset(AREA_DATA* are, RESET_DATA* res);
void free_area(AREA_DATA* are);
void assign_area(CHAR_DATA* ch);
EDD* SetRExtra(ROOM_INDEX_DATA* room, const char* keywords);
bool DelRExtra(ROOM_INDEX_DATA* room, const char* keywords);
EDD* SetOExtra(OBJ_DATA* obj, const char* keywords);
bool DelOExtra(OBJ_DATA* obj, const char* keywords);
EDD* SetOExtraProto(OBJ_INDEX_DATA* obj, const char* keywords);
bool DelOExtraProto(OBJ_INDEX_DATA* obj, const char* keywords);
void fold_area(AREA_DATA* tarea, char* filename, bool install);
int get_otype(const char* type);
int get_atype(const char* type);
int get_aflag(const char* flag);
int get_oflag(const char* flag);
int get_wflag(const char* flag);

/* fight.c */
int max_fight(CHAR_DATA* ch);
void violence_update(void);
ch_ret multi_hit(CHAR_DATA* ch, CHAR_DATA* victim, int dt);
sh_int ris_damage(CHAR_DATA* ch, sh_int dam, int ris);
ch_ret damage(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt);
void update_pos(CHAR_DATA* victim);
void set_fighting(CHAR_DATA* ch, CHAR_DATA* victim);
void stop_fighting(CHAR_DATA* ch, bool fBoth);
void free_fight(CHAR_DATA* ch);
CD* who_fighting(CHAR_DATA* ch);
void check_killer(CHAR_DATA* ch, CHAR_DATA* victim);
void check_attacker(CHAR_DATA* ch, CHAR_DATA* victim);
void death_cry(CHAR_DATA* ch);
void stop_hunting(CHAR_DATA* ch);
void stop_hating(CHAR_DATA* ch);
void stop_fearing(CHAR_DATA* ch);
void start_hunting(CHAR_DATA* ch, CHAR_DATA* victim);
void start_hating(CHAR_DATA* ch, CHAR_DATA* victim);
void start_fearing(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_hunting(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_hating(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_fearing(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_safe(CHAR_DATA* ch, CHAR_DATA* victim);
bool is_safe_nm(CHAR_DATA* ch, CHAR_DATA* victim);
bool legal_loot(CHAR_DATA* ch, CHAR_DATA* victim);
bool check_illegal_pk(CHAR_DATA* ch, CHAR_DATA* victim);
void raw_kill(CHAR_DATA* ch, CHAR_DATA* victim);
bool in_arena(CHAR_DATA* ch);

/* makeobjs.c */
void make_corpse(CHAR_DATA* ch, char* killer);
void make_blood(CHAR_DATA* ch);
void make_bloodstain(CHAR_DATA* ch);
void make_scraps(OBJ_DATA* obj);
void make_fire(ROOM_INDEX_DATA* in_room, sh_int timer);
OD* make_trap(int v0, int v1, int v2, int v3);
OD* create_money(int amount);

/* misc.c */
void actiondesc(CHAR_DATA* ch, OBJ_DATA* obj, void* vo);
void jedi_checks(CHAR_DATA* ch);
void jedi_bonus(CHAR_DATA* ch);
void sith_penalty(CHAR_DATA* ch);

/* mud_comm.c */
const char* mprog_type_to_name(int type);

/* mud_prog.c */
#ifdef DUNNO_STRSTR
char* strstr(const char* s1, const char* s2);
#endif

void mprog_wordlist_check(char* arg, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type);
void mprog_percent_check(CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type);
void mprog_act_trigger(char* buf, CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj, void* vo);
void mprog_bribe_trigger(CHAR_DATA* mob, CHAR_DATA* ch, int amount);
void mprog_entry_trigger(CHAR_DATA* mob);
void mprog_give_trigger(CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj);
void mprog_greet_trigger(CHAR_DATA* mob);
void mprog_fight_trigger(CHAR_DATA* mob, CHAR_DATA* ch);
void mprog_hitprcnt_trigger(CHAR_DATA* mob, CHAR_DATA* ch);
void mprog_death_trigger(CHAR_DATA* killer, CHAR_DATA* mob);
void mprog_random_trigger(CHAR_DATA* mob);
void mprog_speech_trigger(char* txt, CHAR_DATA* mob);
void mprog_script_trigger(CHAR_DATA* mob);
void mprog_hour_trigger(CHAR_DATA* mob);
void mprog_time_trigger(CHAR_DATA* mob);
void progbug(const char* str, CHAR_DATA* mob);
void rset_supermob(ROOM_INDEX_DATA* room);
void release_supermob();
void mpsleep_update();

/* player.c */
void set_title(CHAR_DATA* ch, char* title);

/* skills.c */
bool check_skill(CHAR_DATA* ch, const char* command, char* argument);
void learn_from_success(CHAR_DATA* ch, int sn);
void learn_from_failure(CHAR_DATA* ch, int sn);
bool check_parry(CHAR_DATA* ch, CHAR_DATA* victim);
bool check_dodge(CHAR_DATA* ch, CHAR_DATA* victim);
bool check_grip(CHAR_DATA* ch, CHAR_DATA* victim);
void disarm(CHAR_DATA* ch, CHAR_DATA* victim);
void trip(CHAR_DATA* ch, CHAR_DATA* victim);

/* handler.c */
void explode(OBJ_DATA* obj);
int get_exp(CHAR_DATA* ch, int ability);
int get_exp_worth(CHAR_DATA* ch);
int exp_level(sh_int level);
sh_int get_trust(CHAR_DATA* ch);
sh_int get_age(CHAR_DATA* ch);
sh_int get_curr_str(CHAR_DATA* ch);
sh_int get_curr_int(CHAR_DATA* ch);
sh_int get_curr_wis(CHAR_DATA* ch);
sh_int get_curr_dex(CHAR_DATA* ch);
int count_users(OBJ_DATA* obj);
int max_weight(OBJ_DATA* obj);

sh_int get_curr_con(CHAR_DATA* ch);
sh_int get_curr_cha(CHAR_DATA* ch);
sh_int get_curr_lck(CHAR_DATA* ch);
sh_int get_curr_frc(CHAR_DATA* ch);
bool can_take_proto(CHAR_DATA* ch);
int can_carry_n(CHAR_DATA* ch);
int can_carry_w(CHAR_DATA* ch);
bool is_name(const char* str, const char* namelist);
bool is_name_prefix(const char* str, const char* namelist);
bool nifty_is_name(const char* str, const char* namelist);
bool nifty_is_name_prefix(const char* str, const char* namelist);
void affect_modify(CHAR_DATA* ch, AFFECT_DATA* paf, bool fAdd);
void affect_to_char(CHAR_DATA* ch, AFFECT_DATA* paf);
void affect_remove(CHAR_DATA* ch, AFFECT_DATA* paf);
void affect_strip(CHAR_DATA* ch, int sn);
bool is_affected(CHAR_DATA* ch, int sn);
void affect_join(CHAR_DATA* ch, AFFECT_DATA* paf);
void char_from_room(CHAR_DATA* ch);
void char_to_room(CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex);
OD* obj_to_char(OBJ_DATA* obj, CHAR_DATA* ch);
void obj_from_char(OBJ_DATA* obj);
int apply_ac(OBJ_DATA* obj, int iWear);
OD* get_eq_char(CHAR_DATA* ch, int iWear);
void equip_char(CHAR_DATA* ch, OBJ_DATA* obj, int iWear);
void unequip_char(CHAR_DATA* ch, OBJ_DATA* obj);
int count_obj_list(RESET_DATA* pReset, OBJ_INDEX_DATA* pObjIndex, OBJ_DATA* list);
int count_mob_in_room(MOB_INDEX_DATA* mob, ROOM_INDEX_DATA* list);
void obj_from_room(OBJ_DATA* obj);
OD* obj_to_room(OBJ_DATA* obj, ROOM_INDEX_DATA* pRoomIndex);
OD* obj_to_obj(OBJ_DATA* obj, OBJ_DATA* obj_to);
void obj_from_obj(OBJ_DATA* obj);
void extract_obj(OBJ_DATA* obj);
void extract_exit(ROOM_INDEX_DATA* room, EXIT_DATA* pexit);
void extract_room(ROOM_INDEX_DATA* room);
void clean_room(ROOM_INDEX_DATA* room);
void clean_obj(OBJ_INDEX_DATA* obj);
void clean_mob(MOB_INDEX_DATA* mob);
void clean_resets(ROOM_INDEX_DATA* room);
void extract_char(CHAR_DATA* ch, bool fPull);
CD* get_char_room(CHAR_DATA* ch, const char* argument);
CD* get_char_world(CHAR_DATA* ch, const char* argument);
CD* get_char_world_ooc(CHAR_DATA* ch, const char* argument);
CD* get_char_from_comfreq(CHAR_DATA* ch, const char* argument);
OD* get_obj_type(OBJ_INDEX_DATA* pObjIndexData);
OD* get_obj_list(CHAR_DATA* ch, const char* argument, OBJ_DATA* list);
OD* get_obj_list_rev(CHAR_DATA* ch, const char* argument, OBJ_DATA* list);
OD* get_obj_carry(CHAR_DATA* ch, const char* argument);
OD* get_obj_wear(CHAR_DATA* ch, const char* argument);
OD* get_obj_here(CHAR_DATA* ch, const char* argument);
OD* get_obj_world(CHAR_DATA* ch, const char* argument);
int get_obj_number(OBJ_DATA* obj);
int get_obj_weight(OBJ_DATA* obj);
bool room_is_dark(ROOM_INDEX_DATA* pRoomIndex);
bool room_is_private(CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex);
bool can_see(CHAR_DATA* ch, CHAR_DATA* victim);
bool can_see_obj(CHAR_DATA* ch, OBJ_DATA* obj);
bool can_drop_obj(CHAR_DATA* ch, OBJ_DATA* obj);
std::string item_type_name(OBJ_DATA* obj);
std::string affect_loc_name(int location);
std::string affect_bit_name(int vector);
std::string extra_bit_name(int extra_flags);
std::string magic_bit_name(int magic_flags);
ch_ret check_for_trap(CHAR_DATA* ch, OBJ_DATA* obj, int flag);
ch_ret check_room_for_traps(CHAR_DATA* ch, int flag);
bool is_trapped(OBJ_DATA* obj);
OD* get_trap(OBJ_DATA* obj);
ch_ret spring_trap(CHAR_DATA* ch, OBJ_DATA* obj);
void name_stamp_stats(CHAR_DATA* ch);
void fix_char(CHAR_DATA* ch);
void showaffect(CHAR_DATA* ch, AFFECT_DATA* paf);
void set_cur_obj(OBJ_DATA* obj);
bool obj_extracted(OBJ_DATA* obj);
void queue_extracted_obj(OBJ_DATA* obj);
void clean_obj_queue(void);
void set_cur_char(CHAR_DATA* ch);
bool char_died(CHAR_DATA* ch);
void queue_extracted_char(CHAR_DATA* ch, bool extract);
void clean_char_queue(void);
void add_timer(CHAR_DATA* ch, sh_int type, sh_int count, DO_FUN* fun, int value);
TIMER* get_timerptr(CHAR_DATA* ch, sh_int type);
sh_int get_timer(CHAR_DATA* ch, sh_int type);
void extract_timer(CHAR_DATA* ch, TIMER* timer);
void remove_timer(CHAR_DATA* ch, sh_int type);
bool in_soft_range(CHAR_DATA* ch, AREA_DATA* tarea);
bool in_hard_range(CHAR_DATA* ch, AREA_DATA* tarea);
bool chance(CHAR_DATA* ch, sh_int percent);
bool chance_attrib(CHAR_DATA* ch, sh_int percent, sh_int attrib);
OD* clone_object(OBJ_DATA* obj);
void split_obj(OBJ_DATA* obj, int num);
void separate_obj(OBJ_DATA* obj);
bool empty_obj(OBJ_DATA* obj, OBJ_DATA* destobj, ROOM_INDEX_DATA* destroom);
OD* find_obj(CHAR_DATA* ch, char* argument, bool carryonly);
bool ms_find_obj(CHAR_DATA* ch);
void worsen_mental_state(CHAR_DATA* ch, int mod);
void better_mental_state(CHAR_DATA* ch, int mod);
void boost_economy(AREA_DATA* tarea, int gold);
void lower_economy(AREA_DATA* tarea, int gold);
void economize_mobgold(CHAR_DATA* mob);
bool economy_has(AREA_DATA* tarea, int gold);
void add_kill(CHAR_DATA* ch, CHAR_DATA* mob);
int times_killed(CHAR_DATA* ch, CHAR_DATA* mob);

/* interp.c */
bool check_pos(CHAR_DATA* ch, sh_int position);
void interpret(CHAR_DATA* ch, char* argument);
bool is_number(const char* arg);
int number_argument(const char* argument, char* arg);
const char* one_argument(const char* argument, char* arg_first);
char* one_argument(char* argument, char* arg_first);
const char* one_argument2(const char* argument, char* arg_first);
char* one_argument2(char* argument, char* arg_first);
ST* find_social(const char* command);
CMDTYPE* find_command(const char* command);
void hash_commands();
void send_timer(TIMERSET* vtime, CHAR_DATA* ch);
void update_userec(std::chrono::steady_clock::duration time_used, TIMERSET* userec);

/* magic.c */
bool process_spell_components(CHAR_DATA* ch, int sn);
int ch_slookup(CHAR_DATA* ch, const char* name);
int find_spell(CHAR_DATA* ch, const char* name, bool know);
int find_skill(CHAR_DATA* ch, const char* name, bool know);
int find_weapon(CHAR_DATA* ch, const char* name, bool know);
int find_tongue(CHAR_DATA* ch, const char* name, bool know);
int skill_lookup(const char* name);
int herb_lookup(const char* name);
int personal_lookup(CHAR_DATA* ch, const char* name);
int slot_lookup(int slot);
int bsearch_skill(const char* name, int first, int top);
int bsearch_skill_exact(const char* name, int first, int top);
bool saves_poison_death(int level, CHAR_DATA* victim);
bool saves_wand(int level, CHAR_DATA* victim);
bool saves_para_petri(int level, CHAR_DATA* victim);
bool saves_breath(int level, CHAR_DATA* victim);
bool saves_spell_staff(int level, CHAR_DATA* victim);
ch_ret obj_cast_spell(int sn, int level, CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj);
int dice_parse(CHAR_DATA* ch, int level, char* exp);
SK* get_skilltype(int sn);

/* request.c */
void init_request_pipe(void);
void check_requests(void);

/* save.c */
/* object saving defines for fread/write_obj. -- Altrag */
#define OS_CARRY 0
#define OS_CORPSE 1
void save_char_obj(CHAR_DATA* ch);
void save_clone(CHAR_DATA* ch);
bool load_char_obj(DESCRIPTOR_DATA& d, const char* name, bool preload);
void set_alarm(long seconds);
void requip_char(CHAR_DATA* ch);
void fwrite_obj(CHAR_DATA* ch, OBJ_DATA* obj, FILE* fp, int iNest, sh_int os_type);
void fread_obj(CHAR_DATA* ch, FILE* fp, sh_int os_type);
void de_equip_char(CHAR_DATA* ch);
void re_equip_char(CHAR_DATA* ch);
void save_home(CHAR_DATA* ch);

/* shops.c */

/* special.c */
SF* spec_lookup(const char* name);
const char* lookup_spec(SPEC_FUN* special);

/* tables.c */
void read_last_file(CHAR_DATA* ch, int count, char* name);
void write_last_file(char* entry);
int get_skill(char* skilltype);
char* spell_name(SPELL_FUN* spell);
char* skill_name(DO_FUN* skill);
void load_skill_table(void);
void save_skill_table(int delnum);
void sort_skill_table(void);
void load_socials(void);
void save_socials(void);
void load_commands(void);
void save_commands(void);
SPELL_FUN* spell_function(char* name);
DO_FUN* skill_function(char* name);
void load_herb_table(void);
void save_herb_table(void);

/* track.c */
void found_prey(CHAR_DATA* ch, CHAR_DATA* victim);
void hunt_victim(CHAR_DATA* ch);

/* update.c */
void advance_level(CHAR_DATA* ch, int ability);
void gain_exp(CHAR_DATA* ch, int gain, int ability);
void gain_exp2(CHAR_DATA* ch, int gain, int ability);
void gain_condition(CHAR_DATA* ch, int iCond, int value);
void update_handler(void);
void reboot_check(time_t reset);
#if 0
void    reboot_check    (char* arg);
#endif
void auction_update(void);
void remove_portal(OBJ_DATA* portal);
int max_level(CHAR_DATA* ch, int ability);

/* hashstr.c */
char* str_alloc(char const* str);
char* quick_link(char* str);
int str_free(char* str);
void show_hash(int count);
char* hash_stats(void);
char* check_hash(char* str);
void hash_dump(int hash);
void show_high_hash(int top);
bool in_hash_table(char* str);
/* ships.c */
void load_ship_prototypes();
int load_prototype(const char* prototypefile, int prototype);
bool load_prototype_rooms(FILE* fp, int prototype);
bool fread_prototype_room(FILE* fp, int prototype);
bool load_prototype_header(FILE* fp, const std::string& filename, int prototype);
void write_all_prototypes();
void write_prototype_list();
void save_prototype(int prototype);
void write_all_prototypes();
void write_prototype_list();
void save_prototype(int prototype);
int find_vnum_block(int num_needed);
int make_prototype_rooms(int ship_type, int vnum, AREA_DATA* tarea, char* Sname);
int get_sp_rflag(char* flag);
SHIP_DATA* make_prototype_ship(int ship_type, int vnum, CHAR_DATA* ch, char* ship_name);
void write_ship_list(void);
void resetship(SHIP_DATA* ship);
char* parse_prog_string(char* inp, int ship_type, int vnum);
void make_rprogs(int ship_type, int vnum);
void load_ship_prototypes(void);

/* functions.c */
char* strrep(const char* src, const char* sch, const char* rep);
char* strlinwrp(char* src, int length);
char* line(int num, char inp);
std::string remand(const std::string_view& arg);
char* rembg(const char* arg);
char* format_str(char* src, int len);
int strlen_color(char* argument);
char* chrmax(char* src, int length);

/* newscore.c */
const char* get_race(CHAR_DATA* ch);

#undef SK
#undef CO
#undef ST
#undef CD
#undef MID
#undef OD
#undef OID
#undef RID
#undef SF
#undef BD
#undef EDD
#undef RD
#undef ED

/* ships.c */

void load_market_list(void);
void save_market_list(void);
void add_market_ship(SHIP_DATA* ship);
void remove_market_ship(BMARKET_DATA* marketship);
void make_random_marketlist(void);
void talk_channel(CHAR_DATA* ch, const char* argument, int channel, const char* verb);

/*
 *
 *  New Build Interface Stuff Follows
 *
 */

/*
 *  Data for a menu page
 */
struct MENU_DATA
{
    char* sectionNum;
    char* charChoice;
    int x;
    int y;
    char* outFormat;
    void* data;
    int ptrType;
    int cmdArgs;
    char* cmdString;
};

DECLARE_DO_FUN(do_redraw_page);
DECLARE_DO_FUN(do_refresh_page);
DECLARE_DO_FUN(do_pagelen);
DECLARE_DO_FUN(do_omenu);
DECLARE_DO_FUN(do_rmenu);
DECLARE_DO_FUN(do_mmenu);
DECLARE_DO_FUN(do_clear);

extern MENU_DATA room_page_a_data[];
extern MENU_DATA room_page_b_data[];
extern MENU_DATA room_page_c_data[];
extern MENU_DATA room_help_page_data[];

extern MENU_DATA mob_page_a_data[];
extern MENU_DATA mob_page_b_data[];
extern MENU_DATA mob_page_c_data[];
extern MENU_DATA mob_page_d_data[];
extern MENU_DATA mob_page_e_data[];
extern MENU_DATA mob_page_f_data[];
extern MENU_DATA mob_help_page_data[];

extern MENU_DATA obj_page_a_data[];
extern MENU_DATA obj_page_b_data[];
extern MENU_DATA obj_page_c_data[];
extern MENU_DATA obj_page_d_data[];
extern MENU_DATA obj_page_e_data[];
extern MENU_DATA obj_help_page_data[];

extern MENU_DATA control_page_a_data[];
extern MENU_DATA control_help_page_data[];

extern const char room_page_a[];
extern const char room_page_b[];
extern const char room_page_c[];
extern const char room_help_page[];

extern const char obj_page_a[];
extern const char obj_page_b[];
extern const char obj_page_c[];
extern const char obj_page_d[];
extern const char obj_page_e[];
extern const char obj_help_page[];

extern const char mob_page_a[];
extern const char mob_page_b[];
extern const char mob_page_c[];
extern const char mob_page_d[];
extern const char mob_page_e[];
extern const char mob_page_f[];
extern const char mob_help_page[];
extern const char* npc_sex[3];
extern const char* ris_strings[];

extern const char control_page_a[];
extern const char control_help_page[];

#define SH_INT 1
#define INT 2
#define CHAR 3
#define STRING 4
#define SPECIAL 5

#define NO_PAGE 0
#define MOB_PAGE_A 1
#define MOB_PAGE_B 2
#define MOB_PAGE_C 3
#define MOB_PAGE_D 4
#define MOB_PAGE_E 5
#define MOB_PAGE_F 17
#define MOB_HELP_PAGE 14
#define ROOM_PAGE_A 6
#define ROOM_PAGE_B 7
#define ROOM_PAGE_C 8
#define ROOM_HELP_PAGE 15
#define OBJ_PAGE_A 9
#define OBJ_PAGE_B 10
#define OBJ_PAGE_C 11
#define OBJ_PAGE_D 12
#define OBJ_PAGE_E 13
#define OBJ_HELP_PAGE 16
#define CONTROL_PAGE_A 18
#define CONTROL_HELP_PAGE 19

#define NO_TYPE 0
#define MOB_TYPE 1
#define OBJ_TYPE 2
#define ROOM_TYPE 3
#define CONTROL_TYPE 4

#define SUB_NORTH DIR_NORTH
#define SUB_EAST DIR_EAST
#define SUB_SOUTH DIR_SOUTH
#define SUB_WEST DIR_WEST
#define SUB_UP DIR_UP
#define SUB_DOWN DIR_DOWN
#define SUB_NE DIR_NORTHEAST
#define SUB_NW DIR_NORTHWEST
#define SUB_SE DIR_SOUTHEAST
#define SUB_SW DIR_SOUTHWEST

/*
 * defines for use with this get_affect function
 */

#define RIS_000 BV00
#define RIS_R00 BV01
#define RIS_0I0 BV02
#define RIS_RI0 BV03
#define RIS_00S BV04
#define RIS_R0S BV05
#define RIS_0IS BV06
#define RIS_RIS BV07

#define GA_AFFECTED BV09
#define GA_RESISTANT BV10
#define GA_IMMUNE BV11
#define GA_SUSCEPTIBLE BV12
#define GA_RIS BV30

/*
 *   Map Structures
 */

DECLARE_DO_FUN(do_mapout);
DECLARE_DO_FUN(do_lookmap);

struct MAP_DATA /* contains per-room data */
{
    int vnum;   /* which map this room belongs to */
    int x;      /* horizontal coordinate */
    int y;      /* vertical coordinate */
    char entry; /* code that shows up on map */
};

struct MAP_INDEX_DATA
{
    MAP_INDEX_DATA* next;
    int vnum;                 /* vnum of the map */
    int map_of_vnums[49][81]; /* room vnums aranged as a map */
};

MAP_INDEX_DATA* get_map_index(int vnum);
void init_maps();

/*
 * mudprograms stuff
 */
extern CHAR_DATA* supermob;

void oprog_speech_trigger(char* txt, CHAR_DATA* ch);
void oprog_random_trigger(OBJ_DATA* obj);
void oprog_wear_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
bool oprog_use_trigger(CHAR_DATA* ch, OBJ_DATA* obj, CHAR_DATA* vict, OBJ_DATA* targ, void* vo);
void oprog_remove_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_sac_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_damage_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_repair_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_drop_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
// void oprog_zap_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
char* oprog_type_to_name(int type);

/*
 * MUD_PROGS START HERE
 * (object stuff)
 */
int rprog_custom_trigger(const char* command, const char* argument, CHAR_DATA* ch);
int mprog_custom_trigger(const char* command, const char* argument, CHAR_DATA* ch);
int oprog_custom_trigger(const char* command, const char* argument, CHAR_DATA* ch);
void oprog_greet_trigger(CHAR_DATA* ch);
void oprog_speech_trigger(char* txt, CHAR_DATA* ch);
void oprog_random_trigger(OBJ_DATA* obj);
void oprog_random_trigger(OBJ_DATA* obj);
void oprog_remove_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_sac_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_get_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_damage_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_repair_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_drop_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_examine_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
// void oprog_zap_trigger( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_pull_trigger(CHAR_DATA* ch, OBJ_DATA* obj);
void oprog_push_trigger(CHAR_DATA* ch, OBJ_DATA* obj);

/* mud prog defines */

#define ERROR_PROG -1
#define IN_FILE_PROG 0
#define ACT_PROG BV00
#define SPEECH_PROG BV01
#define RAND_PROG BV02
#define FIGHT_PROG BV03
#define RFIGHT_PROG BV03
#define DEATH_PROG BV04
#define RDEATH_PROG BV04
#define HITPRCNT_PROG BV05
#define ENTRY_PROG BV06
#define ENTER_PROG BV06
#define GREET_PROG BV07
#define RGREET_PROG BV07
#define OGREET_PROG BV07
#define ALL_GREET_PROG BV08
#define GIVE_PROG BV09
#define BRIBE_PROG BV10
#define HOUR_PROG BV11
#define TIME_PROG BV12
#define WEAR_PROG BV13
#define REMOVE_PROG BV14
#define SAC_PROG BV15
#define LOOK_PROG BV16
#define EXA_PROG BV17
#define CUSTOM_PROG BV18
#define GET_PROG BV19
#define DROP_PROG BV20
#define DAMAGE_PROG BV21
#define REPAIR_PROG BV22
#define RANDIW_PROG BV23
#define SPEECHIW_PROG BV24
#define PULL_PROG BV25
#define PUSH_PROG BV26
#define SLEEP_PROG BV27
#define REST_PROG BV28
#define LEAVE_PROG BV29
#define SCRIPT_PROG BV30
#define USE_PROG BV31

void rprog_leave_trigger(CHAR_DATA* ch);
void rprog_enter_trigger(CHAR_DATA* ch);
void rprog_sleep_trigger(CHAR_DATA* ch);
void rprog_rest_trigger(CHAR_DATA* ch);
void rprog_rfight_trigger(CHAR_DATA* ch);
void rprog_death_trigger(CHAR_DATA* killer, CHAR_DATA* ch);
void rprog_speech_trigger(char* txt, CHAR_DATA* ch);
void rprog_random_trigger(CHAR_DATA* ch);
void rprog_time_trigger(CHAR_DATA* ch);
void rprog_hour_trigger(CHAR_DATA* ch);
char* rprog_type_to_name(int type);

#define OPROG_ACT_TRIGGER
#ifdef OPROG_ACT_TRIGGER
void oprog_act_trigger(char* buf, OBJ_DATA* mobj, CHAR_DATA* ch, OBJ_DATA* obj, void* vo);
#endif
#define RPROG_ACT_TRIGGER
#ifdef RPROG_ACT_TRIGGER
void rprog_act_trigger(char* buf, ROOM_INDEX_DATA* room, CHAR_DATA* ch, OBJ_DATA* obj, void* vo);
#endif

#define GET_BETTED_ON(ch) ((ch)->betted_on)
#define GET_BET_AMT(ch) ((ch)->bet_amt)
#define IN_ARENA(ch) (IS_SET((ch)->in_room->room_flags2, ROOM_ARENA))
