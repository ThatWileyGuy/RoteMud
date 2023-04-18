module;

#include <limits>
#include <chrono>

#include "mud.hxx"

export module mud;

#define GLOBAL DEDUPE

export
{
    typedef int ch_ret;
    typedef int obj_ret;

#ifndef WIN32
    // Linux doesn't define the safe CRT functions that use template deduction, but it's easy enough
    template <size_t dest_size> int sprintf_s(char(&dest)[dest_size], const char* format, ...)
    {
        va_list args;

        va_start(args, format);
        int ret = vsnprintf(&dest[0], dest_size, format, args);
        va_end(args);

        return ret;
    }

    int sprintf_s(char* dest, size_t dest_size, const char* format, ...);

    template <size_t dest_size> char* strcpy_s(char(&dest)[dest_size], const char* source)
    {
        if (strlen(source) >= dest_size)
        {
            assert(0);
            return nullptr;
        }

        return strcpy(&dest[0], source);
    }

    char* strcpy_s(char* dest, size_t dest_size, const char* source);

    template <size_t dest_size> char* strncpy_s(char(&dest)[dest_size], const char* source, size_t num)
    {
        if (num >= dest_size)
        {
            assert(0);
            return nullptr;
        }

        return strncpy(&dest[0], source, num);
    }

    template <size_t dest_size> char* strcat_s(char(&dest)[dest_size], const char* source)
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

    /*
     * Short scalar types.
     * Diavolo reports AIX compiler has bugs with short types.
     */

    // TODO this needs to go away very rapidly
    constexpr int BERR = 255;

    typedef short int sh_int;

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
    typedef void DO_FUN(CHAR_DATA * ch, char* argument);
    typedef bool SPEC_FUN(CHAR_DATA * ch);
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


    constexpr double DUR_CONV = 23.333333333333333333333333;
    constexpr char HIDDEN_TILDE = '*';

    enum BitValue
    {
        BV00 = (1 << 0),
        BV01 = (1 << 1),
        BV02 = (1 << 2),
        BV03 = (1 << 3),
        BV04 = (1 << 4),
        BV05 = (1 << 5),
        BV06 = (1 << 6),
        BV07 = (1 << 7),
        BV08 = (1 << 8),
        BV09 = (1 << 9),
        BV10 = (1 << 10),
        BV11 = (1 << 11),
        BV12 = (1 << 12),
        BV13 = (1 << 13),
        BV14 = (1 << 14),
        BV15 = (1 << 15),
        BV16 = (1 << 16),
        BV17 = (1 << 17),
        BV18 = (1 << 18),
        BV19 = (1 << 19),
        BV20 = (1 << 20),
        BV21 = (1 << 21),
        BV22 = (1 << 22),
        BV23 = (1 << 23),
        BV24 = (1 << 24),
        BV25 = (1 << 25),
        BV26 = (1 << 26),
        BV27 = (1 << 27),
        BV28 = (1 << 28),
        BV29 = (1 << 29),
        BV30 = (1 << 30),
        BV31 = (1 << 31),
    };
    /* 32 USED! DO NOT ADD MORE! SB */

    /*
     * String and memory management parameters.
     */
    constexpr size_t MAX_STRING_LENGTH = 4096; /* buf */ // TODO this needs to go away
    constexpr size_t MAX_INPUT_LENGTH = 1024;            /* arg */
    constexpr size_t MAX_INBUF_SIZE = 1024;
    constexpr size_t MAX_OUTPUT_SIZE = 4096;
    constexpr size_t LAST_FILE_SIZE = 500; // max entries in the last file
    constexpr size_t MSL = MAX_STRING_LENGTH;
    constexpr size_t MIL = MAX_INPUT_LENGTH;
    constexpr int MAX_MOB_COUNT = 10;

    constexpr int MAX_LAYERS = 8; /* maximum clothing layers */
    constexpr int MAX_NEST = 100; /* maximum container nesting */

    constexpr int MAX_KILLTRACK = 20; /* track mob vnums killed */

    /*
     * Game parameters.
     * Increase the max'es if you add more of something.
     * Adjust the pulse numbers to suit yourself.
     */
    constexpr int MAX_EXP_WORTH = 500000;
    constexpr int MIN_EXP_WORTH = 25;

    constexpr size_t MAX_REXITS = 20; /* Maximum exits allowed in 1 room */
    constexpr size_t MAX_SKILL = 282;
    constexpr size_t MAX_ABILITY = 10;
    constexpr size_t MAX_RL_ABILITY = 8;
    constexpr size_t MAX_RACE = 41;
    constexpr size_t MAX_NPC_RACE = 91;
    constexpr size_t MAX_VNUMS = 500000;
    constexpr int MAX_LEVEL = 36;
    constexpr size_t MAX_CLAN = 50;
    constexpr size_t MAX_PLANET = 100;
    constexpr size_t MAX_SHIP = 1000;
    constexpr size_t MAX_SHIP_ROOMS = 25;
    constexpr size_t MAX_BOUNTY = 255;
    constexpr size_t MAX_GOV = 255;

    constexpr size_t MAX_HERB = 20;

    constexpr int LEVEL_HERO = (MAX_LEVEL - 4);
    constexpr int LEVEL_IMMORTAL = (MAX_LEVEL - 4);
    constexpr int LEVEL_SUPREME = MAX_LEVEL;
    constexpr int LEVEL_INFINITE = (MAX_LEVEL - 4);
    constexpr int LEVEL_ETERNAL = (MAX_LEVEL - 4);
    constexpr int LEVEL_IMPLEMENTOR = (MAX_LEVEL);
    constexpr int LEVEL_SUB_IMPLEM = (MAX_LEVEL - 1);
    constexpr int LEVEL_ASCENDANT = (MAX_LEVEL - 2);
    constexpr int LEVEL_GREATER = (MAX_LEVEL - 3);
    constexpr int LEVEL_LESSER = (MAX_LEVEL - 4);
    constexpr int LEVEL_RETIRED = (MAX_LEVEL - 5);
    constexpr int LEVEL_GOD = (MAX_LEVEL - 4);
    constexpr int LEVEL_TRUEIMM = (MAX_LEVEL - 4);
    constexpr int LEVEL_DEMI = (MAX_LEVEL - 4);
    constexpr int LEVEL_SAVIOR = (MAX_LEVEL - 4);
    constexpr int LEVEL_CREATOR = (MAX_LEVEL - 4);
    constexpr int LEVEL_ACOLYTE = (MAX_LEVEL - 4);
    constexpr int LEVEL_NEOPHYTE = (MAX_LEVEL - 4);
    constexpr int LEVEL_AVATAR = (MAX_LEVEL - 5);
    constexpr int LEVEL_NOHUNGER = 30;

    // begin do_fun

    // Seperated Do Functions in hopes to clean things up -KeB
    // No more need for tables.c since Trax's and Samson's Dynamic command support

    // Order
    // Do functions
    // Force functions
    // Spell functions

    /*Do Functions List*/
    DECLARE_DO_FUN(do_aaccept);
    DECLARE_DO_FUN(do_aassign);
    DECLARE_DO_FUN(do_accelerate);
    DECLARE_DO_FUN(do_accept);
    DECLARE_DO_FUN(do_add_patrol);
    DECLARE_DO_FUN(do_addbounty);
    DECLARE_DO_FUN(do_addchange);
    DECLARE_DO_FUN(do_addpilot);
    DECLARE_DO_FUN(do_addsenator);
    DECLARE_DO_FUN(do_adecline);
    DECLARE_DO_FUN(do_advance);
    DECLARE_DO_FUN(do_affected);
    DECLARE_DO_FUN(do_afk);
    DECLARE_DO_FUN(do_ahall);
    DECLARE_DO_FUN(do_ahelp);
    DECLARE_DO_FUN(do_aid);
    DECLARE_DO_FUN(do_allow);
    DECLARE_DO_FUN(do_allships);
    DECLARE_DO_FUN(do_allspeeders);
    DECLARE_DO_FUN(do_ambush);
    DECLARE_DO_FUN(do_ammo);
    DECLARE_DO_FUN(do_ansi);
    DECLARE_DO_FUN(do_answer);
    DECLARE_DO_FUN(do_apply);
    DECLARE_DO_FUN(do_appoint);
    DECLARE_DO_FUN(do_appraise);
    DECLARE_DO_FUN(do_areas);
    DECLARE_DO_FUN(do_arena);
    DECLARE_DO_FUN(do_arm);
    DECLARE_DO_FUN(do_aset);
    DECLARE_DO_FUN(do_ask);
    DECLARE_DO_FUN(do_assignpilot);
    DECLARE_DO_FUN(do_astat);
    DECLARE_DO_FUN(do_at);
    DECLARE_DO_FUN(do_auction);
    DECLARE_DO_FUN(do_authorize);
    DECLARE_DO_FUN(do_autopilot);
    DECLARE_DO_FUN(do_autorecharge);
    DECLARE_DO_FUN(do_autotrack);
    DECLARE_DO_FUN(do_avtalk);
    DECLARE_DO_FUN(do_awho);
    DECLARE_DO_FUN(do_backstab);
    DECLARE_DO_FUN(do_backup);
    DECLARE_DO_FUN(do_balzhur);
    DECLARE_DO_FUN(do_bamfin);
    DECLARE_DO_FUN(do_bamfout);
    DECLARE_DO_FUN(do_ban);
    DECLARE_DO_FUN(do_bank);
    DECLARE_DO_FUN(do_barrel_roll);
    DECLARE_DO_FUN(do_bash);
    DECLARE_DO_FUN(do_bashdoor);
    DECLARE_DO_FUN(do_battle_command);
    DECLARE_DO_FUN(do_beep);
    DECLARE_DO_FUN(do_beg);
    DECLARE_DO_FUN(do_berserk);
    DECLARE_DO_FUN(do_bestow);
    DECLARE_DO_FUN(do_bestowarea);
    DECLARE_DO_FUN(do_bet);
    DECLARE_DO_FUN(do_bind);
    DECLARE_DO_FUN(do_bio);
    DECLARE_DO_FUN(do_bite);
    DECLARE_DO_FUN(do_board);
    DECLARE_DO_FUN(do_boards);
    DECLARE_DO_FUN(do_bodybag);
    DECLARE_DO_FUN(do_bomb);
    DECLARE_DO_FUN(do_bomb);
    DECLARE_DO_FUN(do_bounties);
    DECLARE_DO_FUN(do_brandish);
    DECLARE_DO_FUN(do_brew);
    DECLARE_DO_FUN(do_bribe);
    DECLARE_DO_FUN(do_bset);
    DECLARE_DO_FUN(do_bstat);
    DECLARE_DO_FUN(do_bug);
    DECLARE_DO_FUN(do_bury);
    DECLARE_DO_FUN(do_buy);
    DECLARE_DO_FUN(do_buyhome);
    DECLARE_DO_FUN(do_buymobship);
    DECLARE_DO_FUN(do_buyship);
    DECLARE_DO_FUN(do_buytroops);
    DECLARE_DO_FUN(do_buzz);
    DECLARE_DO_FUN(do_calculate);
    DECLARE_DO_FUN(do_capture);
    DECLARE_DO_FUN(do_cast);
    DECLARE_DO_FUN(do_cedit);
    DECLARE_DO_FUN(do_chaff);
    DECLARE_DO_FUN(do_challenge);
    DECLARE_DO_FUN(do_chandelle);
    DECLARE_DO_FUN(do_changes);
    DECLARE_DO_FUN(do_channels);
    DECLARE_DO_FUN(do_chaos);
    DECLARE_DO_FUN(do_chat);
    DECLARE_DO_FUN(do_check_vnums);
    DECLARE_DO_FUN(do_checkbeacons);
    DECLARE_DO_FUN(do_checkprints);
    DECLARE_DO_FUN(do_checkwar);
    DECLARE_DO_FUN(do_chedit);
    DECLARE_DO_FUN(do_circle);
    DECLARE_DO_FUN(do_clan_donate);
    DECLARE_DO_FUN(do_clan_withdraw);
    DECLARE_DO_FUN(do_clanbuyship);
    DECLARE_DO_FUN(do_clanbuytroops);
    DECLARE_DO_FUN(do_clangiveship);
    DECLARE_DO_FUN(do_clans);
    DECLARE_DO_FUN(do_clansalvage);
    DECLARE_DO_FUN(do_clanstat);
    DECLARE_DO_FUN(do_clantalk);
    DECLARE_DO_FUN(do_claw);
    DECLARE_DO_FUN(do_cleanroom);
    DECLARE_DO_FUN(do_climb);
    DECLARE_DO_FUN(do_clone);
    DECLARE_DO_FUN(do_close);
    DECLARE_DO_FUN(do_closebay);
    DECLARE_DO_FUN(do_closehatch);
    DECLARE_DO_FUN(do_cmdtable);
    DECLARE_DO_FUN(do_cmenu);
    DECLARE_DO_FUN(do_codecrack);
    DECLARE_DO_FUN(do_color);
    DECLARE_DO_FUN(do_commands);
    DECLARE_DO_FUN(do_comment);
    DECLARE_DO_FUN(do_compare);
    DECLARE_DO_FUN(do_concealment);
    DECLARE_DO_FUN(do_config);
    DECLARE_DO_FUN(do_consider);
    DECLARE_DO_FUN(do_contract);
    DECLARE_DO_FUN(do_copyover);
    DECLARE_DO_FUN(do_copyship);
    DECLARE_DO_FUN(do_credits);
    DECLARE_DO_FUN(do_cset);
    DECLARE_DO_FUN(do_cut);
    DECLARE_DO_FUN(do_debitorder);
    DECLARE_DO_FUN(do_deception);
    DECLARE_DO_FUN(do_decline);
    DECLARE_DO_FUN(do_deities);
    DECLARE_DO_FUN(do_demote);
    DECLARE_DO_FUN(do_deny);
    DECLARE_DO_FUN(do_describe);
    DECLARE_DO_FUN(do_description);
    DECLARE_DO_FUN(do_designship);
    DECLARE_DO_FUN(do_destro);
    DECLARE_DO_FUN(do_destroy);
    DECLARE_DO_FUN(do_detrap);
    DECLARE_DO_FUN(do_devote);
    DECLARE_DO_FUN(do_diagnose);
    DECLARE_DO_FUN(do_dig);
    DECLARE_DO_FUN(do_disableship);
    DECLARE_DO_FUN(do_disarm);
    DECLARE_DO_FUN(do_disarmgrenade);
    DECLARE_DO_FUN(do_disconnect);
    DECLARE_DO_FUN(do_disguise);
    DECLARE_DO_FUN(do_dismount);
    DECLARE_DO_FUN(do_divorce);
    DECLARE_DO_FUN(do_dmesg);
    DECLARE_DO_FUN(do_down);
    DECLARE_DO_FUN(do_drag);
    DECLARE_DO_FUN(do_drink);
    DECLARE_DO_FUN(do_drive);
    DECLARE_DO_FUN(do_drop);
    DECLARE_DO_FUN(do_droptroops);
    DECLARE_DO_FUN(do_dualstab);
    DECLARE_DO_FUN(do_east);
    DECLARE_DO_FUN(do_eat);
    DECLARE_DO_FUN(do_echo);
    DECLARE_DO_FUN(do_elite_guard);
    DECLARE_DO_FUN(do_email);
    DECLARE_DO_FUN(do_emote);
    DECLARE_DO_FUN(do_empower);
    DECLARE_DO_FUN(do_empty);
    DECLARE_DO_FUN(do_endsimulator);
    DECLARE_DO_FUN(do_enlist);
    DECLARE_DO_FUN(do_enter);
    DECLARE_DO_FUN(do_equipment);
    DECLARE_DO_FUN(do_examine);
    DECLARE_DO_FUN(do_exempt);
    DECLARE_DO_FUN(do_exits);
    DECLARE_DO_FUN(do_feed);
    DECLARE_DO_FUN(do_fill);
    DECLARE_DO_FUN(do_fire);
    DECLARE_DO_FUN(do_first_aid);
    DECLARE_DO_FUN(do_fixchar);
    DECLARE_DO_FUN(do_flee);
    DECLARE_DO_FUN(do_fly);
    DECLARE_DO_FUN(do_foldarea);
    DECLARE_DO_FUN(do_follow);
    DECLARE_DO_FUN(do_for);
    DECLARE_DO_FUN(do_force);
    DECLARE_DO_FUN(do_forceclose);
    DECLARE_DO_FUN(do_fquit); /* Gorog */
    DECLARE_DO_FUN(do_freeship);
    DECLARE_DO_FUN(do_freeze);
    DECLARE_DO_FUN(do_fset);
    DECLARE_DO_FUN(do_fslay);
    DECLARE_DO_FUN(do_gag);
    DECLARE_DO_FUN(do_gain);
    DECLARE_DO_FUN(do_gather_intelligence);
    DECLARE_DO_FUN(do_gatherclans);
    DECLARE_DO_FUN(do_gemcutting);
    DECLARE_DO_FUN(do_generate_market);
    DECLARE_DO_FUN(do_get);
    DECLARE_DO_FUN(do_getship);
    DECLARE_DO_FUN(do_give);
    DECLARE_DO_FUN(do_giveship);
    DECLARE_DO_FUN(do_giveslug);
    DECLARE_DO_FUN(do_glance);
    DECLARE_DO_FUN(do_gold);
    DECLARE_DO_FUN(do_goto);
    DECLARE_DO_FUN(do_gouge);
    DECLARE_DO_FUN(do_group);
    DECLARE_DO_FUN(do_grub);
    DECLARE_DO_FUN(do_gtell);
    DECLARE_DO_FUN(do_guilds);
    DECLARE_DO_FUN(do_guildtalk);
    DECLARE_DO_FUN(do_hail);
    DECLARE_DO_FUN(do_hale);
    DECLARE_DO_FUN(do_hedit);
    DECLARE_DO_FUN(do_hell);
    DECLARE_DO_FUN(do_help);
    DECLARE_DO_FUN(do_hide);
    DECLARE_DO_FUN(do_hijack);
    DECLARE_DO_FUN(do_hitall);
    DECLARE_DO_FUN(do_hlist);
    DECLARE_DO_FUN(do_holonet);
    DECLARE_DO_FUN(do_holylight);
    DECLARE_DO_FUN(do_homepage);
    DECLARE_DO_FUN(do_hset);
    DECLARE_DO_FUN(do_hyperspace);
    DECLARE_DO_FUN(do_i103);
    DECLARE_DO_FUN(do_i104);
    DECLARE_DO_FUN(do_i105);
    DECLARE_DO_FUN(do_ide);
    DECLARE_DO_FUN(do_idea);
    DECLARE_DO_FUN(do_idealog);
    DECLARE_DO_FUN(do_immortalize);
    DECLARE_DO_FUN(do_immtalk);
    DECLARE_DO_FUN(do_imports);
    DECLARE_DO_FUN(do_induct);
    DECLARE_DO_FUN(do_info);
    DECLARE_DO_FUN(do_inquire);
    DECLARE_DO_FUN(do_installarea);
    DECLARE_DO_FUN(do_installmodule);
    DECLARE_DO_FUN(do_instaroom);
    DECLARE_DO_FUN(do_instazone);
    DECLARE_DO_FUN(do_introduce);
    DECLARE_DO_FUN(do_inventory);
    DECLARE_DO_FUN(do_invis);
    DECLARE_DO_FUN(do_invite);
    DECLARE_DO_FUN(do_jail);
    DECLARE_DO_FUN(do_juke);
    DECLARE_DO_FUN(do_jumpvector);
    DECLARE_DO_FUN(do_keypad);
    DECLARE_DO_FUN(do_kick);
    DECLARE_DO_FUN(do_kill);
    DECLARE_DO_FUN(do_land);
    DECLARE_DO_FUN(do_languages);
    DECLARE_DO_FUN(do_last);
    DECLARE_DO_FUN(do_launch);
    DECLARE_DO_FUN(do_launch2);
    DECLARE_DO_FUN(do_leave);
    DECLARE_DO_FUN(do_leaveship);
    DECLARE_DO_FUN(do_level);
    DECLARE_DO_FUN(do_light);
    DECLARE_DO_FUN(do_link);
    DECLARE_DO_FUN(do_list);
    DECLARE_DO_FUN(do_litterbug);
    DECLARE_DO_FUN(do_load);
    DECLARE_DO_FUN(do_loadarea);
    DECLARE_DO_FUN(do_load_cargo);
    DECLARE_DO_FUN(do_loadup);
    DECLARE_DO_FUN(do_lock);
    DECLARE_DO_FUN(do_log);
    DECLARE_DO_FUN(do_look);
    DECLARE_DO_FUN(do_low_purge);
    DECLARE_DO_FUN(do_mailroom);
    DECLARE_DO_FUN(do_make);
    DECLARE_DO_FUN(do_make_master);
    DECLARE_DO_FUN(do_makearmor);
    DECLARE_DO_FUN(do_makebeacon);
    DECLARE_DO_FUN(do_makebinders);
    DECLARE_DO_FUN(do_makeblade);
    DECLARE_DO_FUN(do_makeblaster);
    DECLARE_DO_FUN(do_makeboard);
    DECLARE_DO_FUN(do_makebug);
    DECLARE_DO_FUN(do_makeclan);
    DECLARE_DO_FUN(do_makecomlink);
    DECLARE_DO_FUN(do_makecommsystem);
    DECLARE_DO_FUN(do_makecontainer);
    DECLARE_DO_FUN(do_makedatapad);
    DECLARE_DO_FUN(do_makeduallightsaber);
    DECLARE_DO_FUN(do_makefree);
    DECLARE_DO_FUN(do_makegoggles);
    DECLARE_DO_FUN(do_makegrenade);
    DECLARE_DO_FUN(do_makeguild);
    DECLARE_DO_FUN(do_makejetpack);
    DECLARE_DO_FUN(do_makejewelry);
    DECLARE_DO_FUN(do_makelandmine);
    DECLARE_DO_FUN(do_makelight);
    DECLARE_DO_FUN(do_makelightsaber);
    DECLARE_DO_FUN(do_makemissile);
    DECLARE_DO_FUN(do_makemobship);
    DECLARE_DO_FUN(do_makemodule);
    DECLARE_DO_FUN(do_makepike);
    DECLARE_DO_FUN(do_makeplanet);
    DECLARE_DO_FUN(do_makeprototypeship);
    DECLARE_DO_FUN(do_makerepair);
    DECLARE_DO_FUN(do_makeshield);
    DECLARE_DO_FUN(do_makeship);
    DECLARE_DO_FUN(do_makeship2);
    DECLARE_DO_FUN(do_makeshipbomb);
    DECLARE_DO_FUN(do_makeshop);
    DECLARE_DO_FUN(do_makesimulator);
    DECLARE_DO_FUN(do_makeslay);
    DECLARE_DO_FUN(do_makespice);
    DECLARE_DO_FUN(do_makestarsystem);
    DECLARE_DO_FUN(do_makewizlist);
    DECLARE_DO_FUN(do_marry);
    DECLARE_DO_FUN(do_mass_propeganda);
    DECLARE_DO_FUN(do_massign);
    DECLARE_DO_FUN(do_mcreate);
    DECLARE_DO_FUN(do_mdelete);
    DECLARE_DO_FUN(do_members);
    DECLARE_DO_FUN(do_memory);
    DECLARE_DO_FUN(do_mfind);
    DECLARE_DO_FUN(do_mine);
    DECLARE_DO_FUN(do_minvoke);
    DECLARE_DO_FUN(do_mlist);
    DECLARE_DO_FUN(do_mobrepair);
    DECLARE_DO_FUN(do_mount);
    DECLARE_DO_FUN(do_mp_close_passage);
    DECLARE_DO_FUN(do_mp_damage);
    DECLARE_DO_FUN(do_mp_deposit);
    DECLARE_DO_FUN(do_mp_open_passage);
    DECLARE_DO_FUN(do_mp_practice);
    DECLARE_DO_FUN(do_mp_restore);
    DECLARE_DO_FUN(do_mp_slay);
    DECLARE_DO_FUN(do_mp_withdraw);
    DECLARE_DO_FUN(do_mpadvance);
    DECLARE_DO_FUN(do_mpapply);
    DECLARE_DO_FUN(do_mpapplyb);
    DECLARE_DO_FUN(do_mpasound);
    DECLARE_DO_FUN(do_mpat);
    DECLARE_DO_FUN(do_mpdream);
    DECLARE_DO_FUN(do_mpecho);
    DECLARE_DO_FUN(do_mpechoaround);
    DECLARE_DO_FUN(do_mpechoat);
    DECLARE_DO_FUN(do_mpedit);
    DECLARE_DO_FUN(do_mpforce);
    DECLARE_DO_FUN(do_mpgain);
    DECLARE_DO_FUN(do_mpgoto);
    DECLARE_DO_FUN(do_mpinvis);
    DECLARE_DO_FUN(do_mpjunk);
    DECLARE_DO_FUN(do_mpkill);
    DECLARE_DO_FUN(do_mpmload);
    DECLARE_DO_FUN(do_mpmset);
    DECLARE_DO_FUN(do_mpnothing);
    DECLARE_DO_FUN(do_mpoload);
    DECLARE_DO_FUN(do_mposet);
    DECLARE_DO_FUN(do_mppkset);
    DECLARE_DO_FUN(do_mppurge);
    DECLARE_DO_FUN(do_mpstat);
    DECLARE_DO_FUN(do_mptransfer);
    DECLARE_DO_FUN(do_mrange);
    DECLARE_DO_FUN(do_mset);
    DECLARE_DO_FUN(do_mstat);
    DECLARE_DO_FUN(do_murde);
    DECLARE_DO_FUN(do_murder);
    DECLARE_DO_FUN(do_music);
    DECLARE_DO_FUN(do_mwhere);
    DECLARE_DO_FUN(do_name);
    DECLARE_DO_FUN(do_newbiechat);
    DECLARE_DO_FUN(do_newbieset);
    DECLARE_DO_FUN(do_newclan);
    DECLARE_DO_FUN(do_newexits);
    DECLARE_DO_FUN(do_newpassword);
    DECLARE_DO_FUN(do_newzones);
    DECLARE_DO_FUN(do_noemote);
    DECLARE_DO_FUN(do_noresolve);
    DECLARE_DO_FUN(do_north);
    DECLARE_DO_FUN(do_northeast);
    DECLARE_DO_FUN(do_northwest);
    DECLARE_DO_FUN(do_notell);
    DECLARE_DO_FUN(do_noteroom);
    DECLARE_DO_FUN(do_notitle);
    DECLARE_DO_FUN(do_nullifybeacons);
    DECLARE_DO_FUN(do_oassign);
    DECLARE_DO_FUN(do_ocreate);
    DECLARE_DO_FUN(do_odelete);
    DECLARE_DO_FUN(do_ofind);
    DECLARE_DO_FUN(do_ogrub);
    DECLARE_DO_FUN(do_oinvoke);
    DECLARE_DO_FUN(do_oldmstat);
    DECLARE_DO_FUN(do_oldscore);
    DECLARE_DO_FUN(do_olist);
    DECLARE_DO_FUN(do_ooc);
    DECLARE_DO_FUN(do_opedit);
    DECLARE_DO_FUN(do_open);
    DECLARE_DO_FUN(do_openbay);
    DECLARE_DO_FUN(do_openhatch);
    DECLARE_DO_FUN(do_opentourney);
    DECLARE_DO_FUN(do_opstat);
    DECLARE_DO_FUN(do_orange);
    DECLARE_DO_FUN(do_order);
    DECLARE_DO_FUN(do_orderclanship);
    DECLARE_DO_FUN(do_orders);
    DECLARE_DO_FUN(do_ordership);
    DECLARE_DO_FUN(do_ordertalk);
    DECLARE_DO_FUN(do_oset);
    DECLARE_DO_FUN(do_ostat);
    DECLARE_DO_FUN(do_ot);
    DECLARE_DO_FUN(do_outcast);
    DECLARE_DO_FUN(do_outlaw);
    DECLARE_DO_FUN(do_owhere);
    DECLARE_DO_FUN(do_pager);
    DECLARE_DO_FUN(do_pardon);
    DECLARE_DO_FUN(do_password);
    DECLARE_DO_FUN(do_peace);
    DECLARE_DO_FUN(do_pcrename);
    DECLARE_DO_FUN(do_pick);
    DECLARE_DO_FUN(do_pickshiplock);
    DECLARE_DO_FUN(do_planets);
    DECLARE_DO_FUN(do_plantbeacon);
    DECLARE_DO_FUN(do_plantbug);
    DECLARE_DO_FUN(do_playslots);
    DECLARE_DO_FUN(do_plrbuglist);
    DECLARE_DO_FUN(do_pluogus);
    DECLARE_DO_FUN(do_poison_weapon);
    DECLARE_DO_FUN(do_pose);
    DECLARE_DO_FUN(do_postguard);
    DECLARE_DO_FUN(do_practice);
    DECLARE_DO_FUN(do_prompt);
    DECLARE_DO_FUN(do_propeganda);
    DECLARE_DO_FUN(do_propose);
    DECLARE_DO_FUN(do_prototypes);
    DECLARE_DO_FUN(do_pull);
    DECLARE_DO_FUN(do_punch);
    DECLARE_DO_FUN(do_purge);
    DECLARE_DO_FUN(do_push);
    DECLARE_DO_FUN(do_put);
    DECLARE_DO_FUN(do_qpset);
    DECLARE_DO_FUN(do_quaff);
    DECLARE_DO_FUN(do_quest);
    DECLARE_DO_FUN(do_qui);
    DECLARE_DO_FUN(do_quit);
    DECLARE_DO_FUN(do_radar);
    DECLARE_DO_FUN(do_rank);
    DECLARE_DO_FUN(do_rassign);
    DECLARE_DO_FUN(do_rat);
    DECLARE_DO_FUN(do_rdelete);
    DECLARE_DO_FUN(do_reboo);
    DECLARE_DO_FUN(do_reboot);
    DECLARE_DO_FUN(do_recall);
    DECLARE_DO_FUN(do_recall);
    DECLARE_DO_FUN(do_recharge);
    DECLARE_DO_FUN(do_recho);
    DECLARE_DO_FUN(do_recite);
    DECLARE_DO_FUN(do_redit);
    DECLARE_DO_FUN(do_refuel);
    DECLARE_DO_FUN(do_regoto);
    DECLARE_DO_FUN(do_reinforcements);
    DECLARE_DO_FUN(do_reload);
    DECLARE_DO_FUN(do_rembounty);
    DECLARE_DO_FUN(do_remclan);
    DECLARE_DO_FUN(do_remcontract);
    DECLARE_DO_FUN(do_remember);
    DECLARE_DO_FUN(do_remove);
    DECLARE_DO_FUN(do_removebug);
    DECLARE_DO_FUN(do_removemodule);
    DECLARE_DO_FUN(do_removeship);
    DECLARE_DO_FUN(do_rempilot);
    DECLARE_DO_FUN(do_remsenator);
    DECLARE_DO_FUN(do_remslay);
    DECLARE_DO_FUN(do_rent);
    DECLARE_DO_FUN(do_renumber);
    DECLARE_DO_FUN(do_repair);
    DECLARE_DO_FUN(do_repairset);
    DECLARE_DO_FUN(do_repairshops);
    DECLARE_DO_FUN(do_repairstat);
    DECLARE_DO_FUN(do_reply);
    DECLARE_DO_FUN(do_report);
    DECLARE_DO_FUN(do_request);
    DECLARE_DO_FUN(do_rescue);
    DECLARE_DO_FUN(do_reset);
    DECLARE_DO_FUN(do_resetship);
    DECLARE_DO_FUN(do_resign);
    DECLARE_DO_FUN(do_rest);
    DECLARE_DO_FUN(do_restore);
    DECLARE_DO_FUN(do_restorefile);
    DECLARE_DO_FUN(do_restoretime);
    DECLARE_DO_FUN(do_restrict);
    DECLARE_DO_FUN(do_retire);
    DECLARE_DO_FUN(do_retran);
    DECLARE_DO_FUN(do_retreat);
    DECLARE_DO_FUN(do_retune);
    DECLARE_DO_FUN(do_return);
    DECLARE_DO_FUN(do_revert);
    DECLARE_DO_FUN(do_reward);
    DECLARE_DO_FUN(do_rip);
    DECLARE_DO_FUN(do_rlist);
    DECLARE_DO_FUN(do_rpconvert);
    DECLARE_DO_FUN(do_rpedit);
    DECLARE_DO_FUN(do_rpstat);
    DECLARE_DO_FUN(do_rset);
    DECLARE_DO_FUN(do_rstat);
    DECLARE_DO_FUN(do_sabotage);
    DECLARE_DO_FUN(do_sacrifice);
    DECLARE_DO_FUN(do_salvage);
    DECLARE_DO_FUN(do_save);
    DECLARE_DO_FUN(do_savearea);
    DECLARE_DO_FUN(do_say);
    DECLARE_DO_FUN(do_scan);
    DECLARE_DO_FUN(do_scanbugs);
    DECLARE_DO_FUN(do_score);
    DECLARE_DO_FUN(do_screenname);
    DECLARE_DO_FUN(do_scribe);
    DECLARE_DO_FUN(do_search);
    DECLARE_DO_FUN(do_sedit);
    DECLARE_DO_FUN(do_seduce);
    DECLARE_DO_FUN(do_sell);
    DECLARE_DO_FUN(do_sellhome);
    DECLARE_DO_FUN(do_sellship);
    DECLARE_DO_FUN(do_senate);
    DECLARE_DO_FUN(do_set_boot_time);
    DECLARE_DO_FUN(do_setblaster);
    DECLARE_DO_FUN(do_setclan);
    DECLARE_DO_FUN(do_setinfrared);
    DECLARE_DO_FUN(do_setplanet);
    DECLARE_DO_FUN(do_setprototype);
    DECLARE_DO_FUN(do_setrank);
    DECLARE_DO_FUN(do_setship);
    DECLARE_DO_FUN(do_setslay);
    DECLARE_DO_FUN(do_setstarsystem);
    DECLARE_DO_FUN(do_setwage);
    DECLARE_DO_FUN(do_shiftvnums);
    DECLARE_DO_FUN(do_shiplist);
    DECLARE_DO_FUN(do_shiplock);
    DECLARE_DO_FUN(do_shipmaintenance);
    DECLARE_DO_FUN(do_shiprepair);
    DECLARE_DO_FUN(do_ships);
    DECLARE_DO_FUN(do_shipstat);
    DECLARE_DO_FUN(do_shiptalk);
    DECLARE_DO_FUN(do_shops);
    DECLARE_DO_FUN(do_shopset);
    DECLARE_DO_FUN(do_shopstat);
    DECLARE_DO_FUN(do_shout);
    DECLARE_DO_FUN(do_shove);
    DECLARE_DO_FUN(do_showbeacons);
    DECLARE_DO_FUN(do_showbugs);
    DECLARE_DO_FUN(do_showclan);
    DECLARE_DO_FUN(do_showcontracts);
    DECLARE_DO_FUN(do_showslay);
    DECLARE_DO_FUN(do_showmodules);
    DECLARE_DO_FUN(do_showplanet);
    DECLARE_DO_FUN(do_showprototype);
    DECLARE_DO_FUN(do_showship);
    DECLARE_DO_FUN(do_showsocial);
    DECLARE_DO_FUN(do_showstarsystem);
    DECLARE_DO_FUN(do_shutdow);
    DECLARE_DO_FUN(do_shutdown);
    DECLARE_DO_FUN(do_silence);
    DECLARE_DO_FUN(do_sit);
    DECLARE_DO_FUN(do_sla);
    DECLARE_DO_FUN(do_slay);
    DECLARE_DO_FUN(do_sleep);
    DECLARE_DO_FUN(do_slice);
    DECLARE_DO_FUN(do_slicebank);
    DECLARE_DO_FUN(do_slist);
    DECLARE_DO_FUN(do_slog);
    DECLARE_DO_FUN(do_slookup);
    DECLARE_DO_FUN(do_smalltalk);
    DECLARE_DO_FUN(do_smoke);
    DECLARE_DO_FUN(do_sneak);
    DECLARE_DO_FUN(do_snipe);
    DECLARE_DO_FUN(do_snoop);
    DECLARE_DO_FUN(do_sober);
    DECLARE_DO_FUN(do_socials);
    DECLARE_DO_FUN(do_sound);
    DECLARE_DO_FUN(do_south);
    DECLARE_DO_FUN(do_southeast);
    DECLARE_DO_FUN(do_southwest);
    DECLARE_DO_FUN(do_spacetalk);
    DECLARE_DO_FUN(do_speak);
    DECLARE_DO_FUN(do_special_forces);
    DECLARE_DO_FUN(do_speeders);
    DECLARE_DO_FUN(do_split);
    DECLARE_DO_FUN(do_split_s);
    DECLARE_DO_FUN(do_spousetalk);
    DECLARE_DO_FUN(do_sset);
    DECLARE_DO_FUN(do_stand);
    DECLARE_DO_FUN(do_starsystems);
    DECLARE_DO_FUN(do_starttourney);
    DECLARE_DO_FUN(do_status);
    DECLARE_DO_FUN(do_std);
    DECLARE_DO_FUN(do_steal);
    DECLARE_DO_FUN(do_sting);
    DECLARE_DO_FUN(do_stun);
    DECLARE_DO_FUN(do_suicide);
    DECLARE_DO_FUN(do_supplicate);
    DECLARE_DO_FUN(do_switch);
    DECLARE_DO_FUN(do_sysbuglist);
    DECLARE_DO_FUN(do_systemtalk);
    DECLARE_DO_FUN(do_tail);
    DECLARE_DO_FUN(do_takedrug);
    DECLARE_DO_FUN(do_tamp);
    DECLARE_DO_FUN(do_target);
    DECLARE_DO_FUN(do_teach);
    DECLARE_DO_FUN(do_tell);
    DECLARE_DO_FUN(do_tellsnoop);
    DECLARE_DO_FUN(do_throw);
    DECLARE_DO_FUN(do_time);
    DECLARE_DO_FUN(do_timecmd);
    DECLARE_DO_FUN(do_title);
    DECLARE_DO_FUN(do_torture);
    DECLARE_DO_FUN(do_track);
    DECLARE_DO_FUN(do_tractorbeam);
    DECLARE_DO_FUN(do_train);
    DECLARE_DO_FUN(do_trajectory);
    DECLARE_DO_FUN(do_transfer);
    DECLARE_DO_FUN(do_transmit_call);
    DECLARE_DO_FUN(do_transmit_pass);
    DECLARE_DO_FUN(do_transmit_status);
    DECLARE_DO_FUN(do_transmit_broadcast);
    DECLARE_DO_FUN(do_transship);
    DECLARE_DO_FUN(do_transshipss);
    DECLARE_DO_FUN(do_trust);
    DECLARE_DO_FUN(do_tune);
    DECLARE_DO_FUN(do_typo);
    DECLARE_DO_FUN(do_typoslist);
    DECLARE_DO_FUN(do_unbind);
    DECLARE_DO_FUN(do_ungag);
    DECLARE_DO_FUN(do_unhell);
    DECLARE_DO_FUN(do_unlink);
    DECLARE_DO_FUN(do_unload);
    DECLARE_DO_FUN(do_unload_cargo);
    DECLARE_DO_FUN(do_unlock);
    DECLARE_DO_FUN(do_unoutlaw);
    DECLARE_DO_FUN(do_unsilence);
    DECLARE_DO_FUN(do_up);
    DECLARE_DO_FUN(do_upgradeship);
    DECLARE_DO_FUN(do_use);
    DECLARE_DO_FUN(do_users);
    DECLARE_DO_FUN(do_value);
    DECLARE_DO_FUN(do_vassign);
    DECLARE_DO_FUN(do_viewskills);
    DECLARE_DO_FUN(do_visible);
    DECLARE_DO_FUN(do_vnums);
    DECLARE_DO_FUN(do_vsearch);
    DECLARE_DO_FUN(do_wake);
    DECLARE_DO_FUN(do_war);
    DECLARE_DO_FUN(do_wartalk);
    DECLARE_DO_FUN(do_wear);
    DECLARE_DO_FUN(do_weather);
    DECLARE_DO_FUN(do_west);
    DECLARE_DO_FUN(do_where);
    DECLARE_DO_FUN(do_whisper);
    DECLARE_DO_FUN(do_who);
    DECLARE_DO_FUN(do_whoinvis);
    DECLARE_DO_FUN(do_whois);
    DECLARE_DO_FUN(do_wimpy);
    DECLARE_DO_FUN(do_wizhelp);
    DECLARE_DO_FUN(do_wizlist);
    DECLARE_DO_FUN(do_wizlock);
    DECLARE_DO_FUN(do_wwwimage);
    DECLARE_DO_FUN(do_yell);
    DECLARE_DO_FUN(do_zap);
    DECLARE_DO_FUN(do_zecho);
    DECLARE_DO_FUN(do_zones);

    // Force skills
    DECLARE_DO_FUN(fskill_awareness);
    DECLARE_DO_FUN(fskill_convert);
    DECLARE_DO_FUN(fskill_fdisguise);
    DECLARE_DO_FUN(fskill_fhelp);
    DECLARE_DO_FUN(fskill_finfo);
    DECLARE_DO_FUN(fskill_finish);
    DECLARE_DO_FUN(fskill_force_lightning);
    DECLARE_DO_FUN(fskill_fshield);
    DECLARE_DO_FUN(fskill_heal);
    DECLARE_DO_FUN(fskill_identify);
    DECLARE_DO_FUN(fskill_instruct);
    DECLARE_DO_FUN(fskill_makedualsaber);
    DECLARE_DO_FUN(fskill_makelightsaber);
    DECLARE_DO_FUN(fskill_master);
    DECLARE_DO_FUN(fskill_promote);
    DECLARE_DO_FUN(fskill_protect);
    DECLARE_DO_FUN(fskill_refresh);
    DECLARE_DO_FUN(fskill_slash);
    DECLARE_DO_FUN(fskill_squeeze);
    DECLARE_DO_FUN(fskill_student);
    DECLARE_DO_FUN(fskill_whirlwind);
    DECLARE_DO_FUN(skill_notfound);

    /* Spells and such*/
    DECLARE_SPELL_FUN(spell_acetum_primus);
    DECLARE_SPELL_FUN(spell_acid_blast);
    DECLARE_SPELL_FUN(spell_acid_breath);
    DECLARE_SPELL_FUN(spell_animate_dead);
    DECLARE_SPELL_FUN(spell_astral_walk);
    DECLARE_SPELL_FUN(spell_black_fist);
    DECLARE_SPELL_FUN(spell_black_hand);
    DECLARE_SPELL_FUN(spell_black_lightning);
    DECLARE_SPELL_FUN(spell_blindness);
    DECLARE_SPELL_FUN(spell_burning_hands);
    DECLARE_SPELL_FUN(spell_call_lightning);
    DECLARE_SPELL_FUN(spell_calm);
    DECLARE_SPELL_FUN(spell_cause_critical);
    DECLARE_SPELL_FUN(spell_cause_light);
    DECLARE_SPELL_FUN(spell_cause_serious);
    DECLARE_SPELL_FUN(spell_caustic_fount);
    DECLARE_SPELL_FUN(spell_change_sex);
    DECLARE_SPELL_FUN(spell_charm_person);
    DECLARE_SPELL_FUN(spell_chill_touch);
    DECLARE_SPELL_FUN(spell_colour_spray);
    DECLARE_SPELL_FUN(spell_control_weather);
    DECLARE_SPELL_FUN(spell_create_food);
    DECLARE_SPELL_FUN(spell_create_water);
    DECLARE_SPELL_FUN(spell_cure_addiction);
    DECLARE_SPELL_FUN(spell_cure_blindness);
    DECLARE_SPELL_FUN(spell_cure_poison);
    DECLARE_SPELL_FUN(spell_curse);
    DECLARE_SPELL_FUN(spell_detect_poison);
    DECLARE_SPELL_FUN(spell_dispel_evil);
    DECLARE_SPELL_FUN(spell_dispel_magic);
    DECLARE_SPELL_FUN(spell_disruption);
    DECLARE_SPELL_FUN(spell_dream);
    DECLARE_SPELL_FUN(spell_earthquake);
    DECLARE_SPELL_FUN(spell_enchant_weapon);
    DECLARE_SPELL_FUN(spell_energy_drain);
    DECLARE_SPELL_FUN(spell_ethereal_fist);
    DECLARE_SPELL_FUN(spell_faerie_fire);
    DECLARE_SPELL_FUN(spell_faerie_fog);
    DECLARE_SPELL_FUN(spell_farsight);
    DECLARE_SPELL_FUN(spell_fire_breath);
    DECLARE_SPELL_FUN(spell_fireball);
    DECLARE_SPELL_FUN(spell_flamestrike);
    DECLARE_SPELL_FUN(spell_force_disarm);
    DECLARE_SPELL_FUN(spell_forcepush);
    DECLARE_SPELL_FUN(spell_frost_breath);
    DECLARE_SPELL_FUN(spell_galvanic_whip);
    DECLARE_SPELL_FUN(spell_gas_breath);
    DECLARE_SPELL_FUN(spell_gate);
    DECLARE_SPELL_FUN(spell_hand_of_chaos);
    DECLARE_SPELL_FUN(spell_helical_flow);
    DECLARE_SPELL_FUN(spell_identify);
    DECLARE_SPELL_FUN(spell_injure);
    DECLARE_SPELL_FUN(spell_invis);
    DECLARE_SPELL_FUN(spell_knock);
    DECLARE_SPELL_FUN(spell_know_alignment);
    DECLARE_SPELL_FUN(spell_lightning_bolt);
    DECLARE_SPELL_FUN(spell_lightning_breath);
    DECLARE_SPELL_FUN(spell_locate_object);
    DECLARE_SPELL_FUN(spell_magic_missile);
    DECLARE_SPELL_FUN(spell_magnetic_thrust);
    DECLARE_SPELL_FUN(spell_midas_touch);
    DECLARE_SPELL_FUN(spell_mind_wrack);
    DECLARE_SPELL_FUN(spell_mind_wrench);
    DECLARE_SPELL_FUN(spell_mist_walk);
    DECLARE_SPELL_FUN(spell_notfound);
    DECLARE_SPELL_FUN(spell_null);
    DECLARE_SPELL_FUN(spell_pass_door);
    DECLARE_SPELL_FUN(spell_plant_pass);
    DECLARE_SPELL_FUN(spell_poison);
    DECLARE_SPELL_FUN(spell_polymorph);
    DECLARE_SPELL_FUN(spell_portal);
    DECLARE_SPELL_FUN(spell_possess);
    DECLARE_SPELL_FUN(spell_quantum_spike);
    DECLARE_SPELL_FUN(spell_recharge);
    DECLARE_SPELL_FUN(spell_remove_curse);
    DECLARE_SPELL_FUN(spell_remove_invis);
    DECLARE_SPELL_FUN(spell_remove_trap);
    DECLARE_SPELL_FUN(spell_revive);
    DECLARE_SPELL_FUN(spell_scorching_surge);
    DECLARE_SPELL_FUN(spell_shocking_grasp);
    DECLARE_SPELL_FUN(spell_sleep);
    DECLARE_SPELL_FUN(spell_smaug);
    DECLARE_SPELL_FUN(spell_solar_flight);
    DECLARE_SPELL_FUN(spell_sonic_resonance);
    DECLARE_SPELL_FUN(spell_spectral_furor);
    DECLARE_SPELL_FUN(spell_spiral_blast);
    DECLARE_SPELL_FUN(spell_steal_life);
    DECLARE_SPELL_FUN(spell_suggest);
    DECLARE_SPELL_FUN(spell_sulfurous_spray);
    DECLARE_SPELL_FUN(spell_summon);
    DECLARE_SPELL_FUN(spell_teleport);
    DECLARE_SPELL_FUN(spell_transport);
    DECLARE_SPELL_FUN(spell_ventriloquate);
    DECLARE_SPELL_FUN(spell_weaken);
    DECLARE_SPELL_FUN(spell_word_of_recall);

    // end do_fun

    /****************************************************************************
     *               Color Module -- Allow user customizable Colors.            *
     *                                   --Matthew                              *
     *                      Enhanced ANSI parser by Samson                      *
     ****************************************************************************/

    DECLARE_DO_FUN(do_color);

    void reset_colors(CHAR_DATA * ch);
    void set_char_color(short AType, CHAR_DATA* ch);
    void set_pager_color(short AType, CHAR_DATA* ch);
    const char* color_str(short AType, CHAR_DATA* ch);
    const char* const_color_align(const char* argument, int size, int align);

    /*
     * Color Alignment Parameters
     */

    enum TextAlignment
    {
        ALIGN_LEFT = 1,
        ALIGN_CENTER = 2,
        ALIGN_RIGHT = 3,
    };

#define DEDUPE __declspec(selectany)

    /* These are the ANSI codes for foreground text colors */
    DEDUPE const char* ANSI_BLACK = "\x1b[0;30m";
    DEDUPE const char* ANSI_DRED = "\x1b[0;31m";
    DEDUPE const char* ANSI_DGREEN = "\x1b[0;32m";
    DEDUPE const char* ANSI_ORANGE = "\x1b[0;33m";
    DEDUPE const char* ANSI_DBLUE = "\x1b[0;34m";
    DEDUPE const char* ANSI_PURPLE = "\x1b[0;35m";
    DEDUPE const char* ANSI_CYAN = "\x1b[0;36m";
    DEDUPE const char* ANSI_GREY = "\x1b[1;37m";
    DEDUPE const char* ANSI_DGREY = "\x1b[1;30m";
    DEDUPE const char* ANSI_RED = "\x1b[1;31m";
    DEDUPE const char* ANSI_GREEN = "\x1b[1;32m";
    DEDUPE const char* ANSI_YELLOW = "\x1b[1;33m";
    DEDUPE const char* ANSI_BLUE = "\x1b[1;34m";
    DEDUPE const char* ANSI_PINK = "\x1b[1;35m";
    DEDUPE const char* ANSI_LBLUE = "\x1b[1;36m";
    DEDUPE const char* ANSI_WHITE = "\x1b[0;37m";
    DEDUPE const char* ANSI_RESET = "\x1b[0m";

    /* These are the ANSI codes for blinking foreground text colors */
    DEDUPE const char* BLINK_BLACK = "\x1b[0;5;30m";
    DEDUPE const char* BLINK_DRED = "\x1b[0;5;31m";
    DEDUPE const char* BLINK_DGREEN = "\x1b[0;5;32m";
    DEDUPE const char* BLINK_ORANGE = "\x1b[0;5;33m";
    DEDUPE const char* BLINK_DBLUE = "\x1b[0;5;34m";
    DEDUPE const char* BLINK_PURPLE = "\x1b[0;5;35m";
    DEDUPE const char* BLINK_CYAN = "\x1b[0;5;36m";
    DEDUPE const char* BLINK_GREY = "\x1b[0;5;37m";
    DEDUPE const char* BLINK_DGREY = "\x1b[1;5;30m";
    DEDUPE const char* BLINK_RED = "\x1b[1;5;31m";
    DEDUPE const char* BLINK_GREEN = "\x1b[1;5;32m";
    DEDUPE const char* BLINK_YELLOW = "\x1b[1;5;33m";
    DEDUPE const char* BLINK_BLUE = "\x1b[1;5;34m";
    DEDUPE const char* BLINK_PINK = "\x1b[1;5;35m";
    DEDUPE const char* BLINK_LBLUE = "\x1b[1;5;36m";
    DEDUPE const char* BLINK_WHITE = "\x1b[1;5;37m";

    /* These are the ANSI codes for background colors */
    DEDUPE const char* BACK_BLACK = "\x1b[40m";
    DEDUPE const char* BACK_DRED = "\x1b[41m";
    DEDUPE const char* BACK_DGREEN = "\x1b[42m";
    DEDUPE const char* BACK_ORANGE = "\x1b[43m";
    DEDUPE const char* BACK_DBLUE = "\x1b[44m";
    DEDUPE const char* BACK_PURPLE = "\x1b[45m";
    DEDUPE const char* BACK_CYAN = "\x1b[46m";
    DEDUPE const char* BACK_GREY = "\x1b[47m";

    /* Other miscelaneous ANSI tags that can be used */
    DEDUPE const char* ANSI_BOLD = "\x1b[1m";      /* For bright color stuff */
    DEDUPE const char* ANSI_ITALIC = "\x1b[3m";    /* Italic text */
    DEDUPE const char* ANSI_UNDERLINE = "\x1b[4m"; /* Underline text */
    DEDUPE const char* ANSI_BLINK = "\x1b[5m";     /* Blinking text */
    DEDUPE const char* ANSI_REVERSE = "\x1b[7m";   /* Reverse colors */
    DEDUPE const char* ANSI_STRIKEOUT = "\x1b[9m"; /* Overstrike line */

    enum AsciiTextColor
    {
        AT_BLACK = 0,
        AT_BLOOD = 1,
        AT_DGREEN = 2,
        AT_ORANGE = 3,
        AT_DBLUE = 4,
        AT_PURPLE = 5,
        AT_CYAN = 6,
        AT_GREY = 7,
        AT_DGREY = 8,
        AT_RED = 9,
        AT_GREEN = 10,
        AT_YELLOW = 11,
        AT_BLUE = 12,
        AT_PINK = 13,
        AT_LBLUE = 14,
        AT_WHITE = 15,

        /*People, Objects or Room Related*/
        AT_IMMORT = 16,
        AT_NOTE = 17,
        AT_OBJECT = 18,
        AT_PERSON = 19,
        AT_RMDESC = 20,
        AT_RMNAME = 21,
        AT_SHIP = 22,

        /*Actions or Commands*/
        AT_ACTION = 23,
        AT_BLINK = 24,
        AT_CONSIDER = 25,
        AT_EXITS = 26,
        AT_GOLD = 27,
        AT_HELP = 28, /* Added by Samson 1-15-01 for helpfiles */
        AT_LIST = 29,
        AT_OLDSCORE = 30,
        AT_PLAIN = 31,
        AT_QUIT = 32,
        AT_REPORT = 33,
        AT_SKILL = 34,
        AT_SLIST = 35,

        /*Fighting Stuffs*/
        AT_DAMAGE = 36,
        AT_FLEE = 37,
        AT_HIT = 38,
        AT_HITME = 39,
        AT_HURT = 40,

        /*Continual Messages*/
        AT_DEAD = 41,
        AT_DYING = 42,
        AT_FALLING = 43,
        AT_HUNGRY = 44,
        AT_POISON = 45,
        AT_RESET = 46,
        AT_SOBER = 47,
        AT_THIRSTY = 48,
        AT_WEAROFF = 49,

        /*Mortal Channels*/
        AT_ARENA = 50,
        AT_AUCTION = 51, /* Added by Samson 12-25-98 for auction channel */
        AT_CHAT = 52,
        AT_CLAN = 53,
        AT_GOSSIP = 54,
        AT_GTELL = 55,
        AT_HOLONET = 56,
        AT_OOC = 57,
        AT_MUSIC = 58,
        AT_SAY = 59,
        AT_SHIPTALK = 60,
        AT_SHOUT = 61, /* Added by Samson 9-29-98 for shout channel */
        AT_SOCIAL = 62,
        AT_TELL = 63,
        AT_WARTALK = 64,
        AT_WHISPER = 65, /* Added by Samson 9-29-98 for version 1.4 code */
        AT_YELL = 66,

        /*Imm Only Colors*/
        AT_AVATAR = 67,
        AT_BUILD = 68,
        AT_COMM = 69,
        AT_IMMTALK = 70,
        AT_LOG = 71,
        AT_RFLAGS1 = 72,
        AT_RFLAGS2 = 73,
        AT_RVNUM = 74,

        /* Should ALWAYS be one more than the last numerical value in the list */
        MAX_COLORS = 75,

        AT_MAGIC = AT_WHITE,
        AT_FIRE = AT_RED,
        AT_DIEMSG = AT_BLOOD,
        AT_DANGER = AT_RED,
    };

    extern const short default_set[MAX_COLORS];

    // end color

    constexpr int LEVEL_LOG = LEVEL_LESSER;
    constexpr int LEVEL_HIGOD = LEVEL_GOD;

    constexpr int OBJ_VNUM_DEED = 67; /* vnum of deed */
    constexpr int VNUM_DEBIT_CARD = 87;
    constexpr int PULSE_PER_SECOND = 4;
    constexpr int PULSE_MINUTE = (60 * PULSE_PER_SECOND);
    constexpr int PULSE_VIOLENCE = (3 * PULSE_PER_SECOND);
    constexpr int PULSE_MOBILE = (4 * PULSE_PER_SECOND);
    constexpr int PULSE_TICK = (70 * PULSE_PER_SECOND);
    constexpr int PULSE_AREA = (60 * PULSE_PER_SECOND);
    constexpr int PULSE_AUCTION = (10 * PULSE_PER_SECOND);
    constexpr int PULSE_SPACE = (10 * PULSE_PER_SECOND);
    constexpr int PULSE_TAXES = (60 * PULSE_MINUTE);
    constexpr int PULSE_ARENA = (30 * PULSE_PER_SECOND);
    constexpr int PULSE_FORCE = PULSE_MINUTE;

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

    constexpr int MAX_FORCE_SKILL = 24;

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

    constexpr int MAX_FORCE_ALIGN = 100;
    constexpr int MIN_FORCE_ALIGN = -100;

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
    enum EchoType
    {
        ECHOTAR_ALL = 0,
        ECHOTAR_PC = 1,
        ECHOTAR_IMM = 2,
    };

    /* defines for new do_who */
    enum WhoType
    {
        WT_MORTAL = 0,
        WT_IMM = 2,
        WT_AVATAR = 1,
        WT_NEWBIE = 3,
    };

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
    enum AbilityClass
    {
        ABILITY_NONE = -1,
        COMBAT_ABILITY = 0,
        PILOTING_ABILITY = 1,
        ENGINEERING_ABILITY = 2,
        HUNTING_ABILITY = 3,
        SMUGGLING_ABILITY = 4,
        /*#define DIPLOMACY_ABILITY	5
        #define LEADERSHIP_ABILITY	6*/  /* Gonna replace the diplomacy and leadership abilities and make them POLITICIANs */
        POLITICIAN_ABILITY = 5,
        FORCE_ABILITY = 6,
        SLICER_ABILITY = 7,
        ASSASSIN_ABILITY = 8,
        TECHNICIAN_ABILITY = 9,
    };

    /* the races */
    enum Race
    {
        RACE_HUMAN = 0,
        RACE_WOOKIEE = 1,
        RACE_TWI_LEK = 2,
        RACE_RODIAN = 3,
        RACE_HUTT = 4,
        RACE_MON_CALAMARI = 5,
        RACE_NOGHRI = 6,
        RACE_GAMORREAN = 7,
        RACE_JAWA = 8,
        RACE_ADARIAN = 9,
        RACE_EWOK = 10,
        RACE_VERPINE = 11,
        RACE_DEFEL = 12,
        RACE_TRANDOSHAN = 13,
        RACE_HAPAN = 14,
        RACE_QUARREN = 15,
        RACE_SHISTAVANEN = 16,
        RACE_FALLEEN = 17,
        RACE_ITHORIAN = 18,
        RACE_DEVARONIAN = 19,
        RACE_GOTAL = 20,
        RACE_DROID = 21,
        RACE_FIRRERREO = 22,
        RACE_BARABEL = 23,
        RACE_BOTHAN = 24,
        RACE_TOGORIAN = 25,
        RACE_DUG = 26,
        RACE_KUBAZ = 27,
        RACE_SELONIAN = 28,
        RACE_GRAN = 29,
        RACE_YEVETHA = 30,
        RACE_GAND = 31,
        RACE_DUROS = 32,
        RACE_COYNITE = 33,
        RACE_SULLUSTAN = 34,
        RACE_PROTOCAL_DROID = 35,
        RACE_ASSASSIN_DROID = 36,
        RACE_GLADIATOR_DROID = 37,
        RACE_ASTROMECH_DROID = 38,
        RACE_INTERROGATION_DROID = 39,
    };
    /*
     * Languages -- Altrag
     */
    enum LanguageBit
    {
        LANG_BASIC = BV00, /* Human base language */
        LANG_WOOKIEE = BV01,
        LANG_TWI_LEK = BV02,
        LANG_RODIAN = BV03,
        LANG_HUTT = BV04,
        LANG_MON_CALAMARI = BV05,
        LANG_NOGHRI = BV06,
        LANG_EWOK = BV07,
        LANG_ITHORIAN = BV08,
        LANG_GOTAL = BV09,
        LANG_DEVARONIAN = BV10,
        LANG_BINARY = BV11,
        LANG_FIRRERREO = BV12,
        LANG_CLAN = BV13,
        LANG_GAMORREAN = BV14,
        LANG_TOGORIAN = BV15,
        LANG_SHISTAVANEN = BV16,
        LANG_JAWA = BV17,
        LANG_KUBAZ = BV18,
        LANG_ADARIAN = BV19,
        LANG_VERPINE = BV20,
        LANG_DEFEL = BV21,
        LANG_TRANDOSHAN = BV22,
        LANG_HAPAN = BV23,
        LANG_QUARREN = BV24,
        LANG_SULLUSTAN = BV25,
        LANG_FALLEEN = BV26,
        LANG_BARABEL = BV27,
        LANG_YEVETHAN = BV28,
        LANG_GAND = BV29,
        LANG_DUROS = BV30,
        LANG_COYNITE = BV31,
        LANG_UNKNOWN = 0, /* Anything that doesnt fit a category */
    };

    constexpr int VALID_LANGS =
        (LANG_BASIC | LANG_WOOKIEE | LANG_TWI_LEK | LANG_RODIAN | LANG_HUTT | LANG_MON_CALAMARI | LANG_NOGHRI |
         LANG_GAMORREAN | LANG_JAWA | LANG_ADARIAN | LANG_EWOK | LANG_VERPINE | LANG_DEFEL | LANG_TRANDOSHAN |
         LANG_HAPAN | LANG_QUARREN | LANG_SULLUSTAN | LANG_BINARY | LANG_FIRRERREO | LANG_CLAN | LANG_TOGORIAN |
         LANG_SHISTAVANEN | LANG_KUBAZ | LANG_YEVETHAN | LANG_GAND | LANG_DUROS | LANG_COYNITE | LANG_GOTAL |
         LANG_DEVARONIAN | LANG_FALLEEN | LANG_ITHORIAN | LANG_BARABEL);
    /*  32 Languages */

    /*
     * TO types for act.
     */
    enum ActTarget
    {
        TO_ROOM = 0,
        TO_NOTVICT = 1,
        TO_VICT = 2,
        TO_CHAR = 3,
        TO_MUD = 4,
    };

    constexpr int INIT_WEAPON_CONDITION = 12;
    constexpr int MAX_ITEM_IMPACT = 30;

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
    constexpr int MAX_TRADE = 5;

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

    constexpr int MAX_FIX = 3;
    constexpr int SHOP_FIX = 1;
    constexpr int SHOP_RECHARGE = 2;

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
    constexpr int MAX_IFS = 20; /* should always be generous */
    constexpr int IN_IF = 0;
    constexpr int IN_ELSE = 1;
    constexpr int DO_IF = 2;
    constexpr int DO_ELSE = 3;

    constexpr int MAX_PROG_NEST = 20;

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

    GLOBAL bool MOBtrigger;

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

    constexpr int LASER_DAMAGED = -1;
    constexpr int LASER_READY = 0;

    enum CargoType
    {
        CARGO_NONE = 0,
        CARGO_FOOD = 1,
        CARGO_WATER = 2,
        CARGO_MEDICAL = 3,
        CARGO_METALS = 4,
        CARGO_RARE_METALS = 5,
        CARGO_ELECTRONICS = 6,
        CARGO_PRODUCTS = 7,
        CARGO_MAX = 8,
    };

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

    constexpr int PLANET_NOCAPTURE = BV00;

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
        char* adjective; // the adjective to show on 'shiplist' ex. "Rebel" Starfighter, "Imperial" Death Cruiser, that
                         // sort of thing
        char* leader;     /* Head clan leader			*/
        char* number1;    /* First officer			*/
        char* number2;    /* Second officer			*/
        int pkills;       /* Number of pkills on behalf of clan	*/
        int pdeaths;      /* Number of pkills against clan	*/
        int mkills;       /* Number of mkills on behalf of clan	*/
        int mdeaths;      /* Number of clan deaths due to mobs	*/
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
    constexpr int MOB_VNUM_ANIMATED_CORPSE = 5;
    constexpr int MOB_VNUM_POLY_WOLF = 10;

    constexpr int MOB_VNUM_STORMTROOPER = 20;
    constexpr int MOB_VNUM_IMP_GUARD = 21;
    constexpr int MOB_VNUM_NR_GUARD = 22;
    constexpr int MOB_VNUM_NR_TROOPER = 23;
    constexpr int MOB_VNUM_MERCINARY = 24;
    constexpr int MOB_VNUM_BOUNCER = 25;
    constexpr int MOB_VNUM_IMP_ELITE = 26;
    constexpr int MOB_VNUM_IMP_PATROL = 27;
    constexpr int MOB_VNUM_IMP_FORCES = 28;
    constexpr int MOB_VNUM_NR_ELITE = 29;
    constexpr int MOB_VNUM_NR_PATROL = 30;
    constexpr int MOB_VNUM_NR_FORCES = 31;
    constexpr int MOB_VNUM_MERC_ELITE = 32;
    constexpr int MOB_VNUM_MERC_PATROL = 33;
    constexpr int MOB_VNUM_MERC_FORCES = 34;
    constexpr int MOB_VNUM_SHIP_GUARD = 35;

    /* Ship Flags */
    enum ShipFlag
    {
        SHIP_NOHIJACK = BV00,
        SHIP_SHIELD_BOOST = BV01,
        SHIP_TORP_BOOST = BV02,
        SHIP_CHAFF_BOOST = BV03,
        SHIP_HULL_BOOST = BV04,
        SHIP_LASER_BOOST = BV05,
        SHIP_MISSILE_BOOST = BV06,
        SHIP_ROCKET_BOOST = BV07,
        SHIP_SIMULATOR = BV08,
        SHIP_NODESTROY = BV09,
        SHIP_NOSLICER = BV10,
        XSHIP_ION_LASERS = BV11,
        XSHIP_ION_DRIVE = BV12,
        XSHIP_ION_ION = BV13,
        XSHIP_ION_TURRET1 = BV14,
        XSHIP_ION_TURRET2 = BV15,
        XSHIP_ION_TURRET3 = BV16,
        XSHIP_ION_TURRET4 = BV17,
        XSHIP_ION_TURRET5 = BV18,
        XSHIP_ION_TURRET6 = BV19,
        XSHIP_ION_TURRET7 = BV20,
        XSHIP_ION_TURRET8 = BV21,
        XSHIP_ION_TURRET9 = BV22,
        XSHIP_ION_TURRET10 = BV23,
        SHIP_RESPAWN = BV24,
        XSHIP_ION_HYPER = BV25,
        XSHIP_ION_MISSILES = BV26,
        SHIP_CLOAK = BV27,
    };

    enum ShipDamageType
    {
        SHIP_DAMAGE_DRIVE = BV00,
        SHIP_DAMAGE_HYPERDRIVE = BV01,
        SHIP_DAMAGE_LASER = BV02,
        SHIP_DAMAGE_ION = BV03,
        SHIP_DAMAGE_TURRET1 = BV04,
        SHIP_DAMAGE_TURRET2 = BV05,
        SHIP_DAMAGE_TURRET3 = BV06,
        SHIP_DAMAGE_TURRET4 = BV07,
        SHIP_DAMAGE_TURRET5 = BV08,
        SHIP_DAMAGE_TURRET6 = BV09,
        SHIP_DAMAGE_TURRET7 = BV10,
        SHIP_DAMAGE_TURRET8 = BV11,
        SHIP_DAMAGE_TURRET9 = BV12,
        SHIP_DAMAGE_TURRET10 = BV13,
        SHIP_DAMAGE_SHIELD = BV14,
        SHIP_DAMAGE_PLASMASHIELD = BV15,
        SHIP_DAMAGE_LIFESUPPORT = BV16,
        SHIP_DAMAGE_MISSILE = BV17,
    };

    /*
     * ACT bits for mobs.
     * Used in #MOBILES.
     */
    enum ActFlag
    {
        ACT_IS_NPC = BV00,     /* Auto set for mobs	*/
        ACT_SENTINEL = BV01,   /* Stays in one room	*/
        ACT_SCAVENGER = BV02,  /* Picks up objects	*/
        ACT_NOFLEE = BV03,     /* Mobs don't flee. -T  */
        ACT_AGGRESSIVE = BV05, /* Attacks PC's		*/
        ACT_STAY_AREA = BV06,  /* Won't leave area	*/
        ACT_WIMPY = BV07,      /* Flees when hurt	*/
        ACT_PET = BV08,        /* Auto set for pets	*/
        ACT_TRAIN = BV09,      /* Can train PC's	*/
        ACT_PRACTICE = BV10,   /* Can practice PC's	*/
        ACT_IMMORTAL = BV11,   /* Cannot be killed	*/
        ACT_DEADLY = BV12,     /* Has a deadly poison  */
        ACT_POLYSELF = BV13,
        ACT_META_AGGR = BV14,   /* Extremely aggressive */
        ACT_GUARDIAN = BV15,    /* Protects master	*/
        ACT_RUNNING = BV16,     /* Hunts quickly	*/
        ACT_NOWANDER = BV17,    /* Doesn't wander	*/
        ACT_MOUNTABLE = BV18,   /* Can be mounted	*/
        ACT_MOUNTED = BV19,     /* Is mounted		*/
        ACT_SCHOLAR = BV20,     /* Can teach languages  */
        ACT_SECRETIVE = BV21,   /* actions aren't seen	*/
        ACT_POLYMORPHED = BV22, /* Mob is a ch		*/
        ACT_MOBINVIS = BV23,    /* Like wizinvis	*/
        ACT_NOASSIST = BV24,    /* Doesn't assist mobs	*/
        ACT_NOKILL = BV25,      /* Mob can't die */
        ACT_DROID = BV26,       /* mob is a droid */
        ACT_NOCORPSE = BV27,
        ACT_PUEBLO = BV28,    /* This is the pueblo flag */
        ACT_PROTOTYPE = BV30, /* A prototype mob	*/
    };

    /* Act2 Flags */
    enum Act2Flag
    {
        ACT_BOUND = BV00,  /* This is the bind flag */
        ACT_EXEMPT = BV01, /* Makes a player exampt from pfile deletion */
        ACT_JEDI = BV02,   /* This is a light jedi */
        ACT_SITH = BV03,   /* This is a dark jedi */
        ACT_GAGGED = BV04, /* This is a gagged flag */
    };
    /* 21 acts */

    /* bits for vip flags */
    enum VipFlag
    {
        VIP_CORUSCANT = BV00,
        VIP_YAVIN_IV = BV01,
        VIP_TATOOINE = BV02,
        VIP_KASHYYYK = BV03,
        VIP_MON_CALAMARI = BV04,
        VIP_ENDOR = BV05,
        VIP_ORD_MANTELL = BV06,
        VIP_NAL_HUTTA = BV07,
        VIP_CORELLIA = BV08,
        VIP_BAKURA = BV09,
    };

    /* player wanted bits */
    enum WantedFlag
    {
        WANTED_CORUSCANT = VIP_CORUSCANT,
        WANTED_YAVIN_IV = VIP_YAVIN_IV,
        WANTED_TATOOINE = VIP_TATOOINE,
        WANTED_KASHYYYK = VIP_KASHYYYK,
        WANTED_MON_CALAMARI = VIP_MON_CALAMARI,
        WANTED_ENDOR = VIP_ENDOR,
        WANTED_ORD_MANTELL = VIP_ORD_MANTELL,
        WANTED_NAL_HUTTA = VIP_NAL_HUTTA,
        WANTED_CORELLIA = VIP_CORELLIA,
        WANTED_BAKURA = VIP_BAKURA,
    };

    /*
     * Bits for 'affected_by'.
     * Used in #MOBILES.
     */
    enum AffectedFlags
    {
        AFF_NONE = 0,

        AFF_BLIND = BV00,
        AFF_INVISIBLE = BV01,
        AFF_DETECT_EVIL = BV02,
        AFF_DETECT_INVIS = BV03,
        AFF_DETECT_MAGIC = BV04,
        AFF_DETECT_HIDDEN = BV05,
        AFF_WEAKEN = BV06,
        AFF_SANCTUARY = BV07,
        AFF_FAERIE_FIRE = BV08,
        AFF_INFRARED = BV09,
        AFF_CURSE = BV10,
        AFF_COVER_TRAIL = BV11,
        AFF_POISON = BV12,
        AFF_PROTECT = BV13,
        AFF_PARALYSIS = BV14,
        AFF_SNEAK = BV15,
        AFF_HIDE = BV16,
        AFF_SLEEP = BV17,
        AFF_CHARM = BV18,
        AFF_FLYING = BV19,
        AFF_PASS_DOOR = BV20,
        AFF_FLOATING = BV21,
        AFF_TRUESIGHT = BV22,
        AFF_DETECTTRAPS = BV23,
        AFF_SCRYING = BV24,
        AFF_FIRESHIELD = BV25,
        AFF_SHOCKSHIELD = BV26,
        AFF_FASTHEAL = BV27,
        AFF_ICESHIELD = BV28,
        AFF_POSSESS = BV29,
        AFF_BERSERK = BV30,
        AFF_AQUA_BREATH = BV31,
    };

    /* 31 aff's (1 left.. :P) */
    /* make that none - ugh - time for another field? :P */
    /*
     * Resistant Immune Susceptible flags
     */
    enum ResistantImmuneSusceptibleFlag
    {
        RIS_FIRE = BV00,
        RIS_COLD = BV01,
        RIS_ELECTRICITY = BV02,
        RIS_ENERGY = BV03,
        RIS_BLUNT = BV04,
        RIS_PIERCE = BV05,
        RIS_SLASH = BV06,
        RIS_ACID = BV07,
        RIS_POISON = BV08,
        RIS_DRAIN = BV09,
        RIS_SLEEP = BV10,
        RIS_CHARM = BV11,
        RIS_HOLD = BV12,
        RIS_NONMAGIC = BV13,
        RIS_PLUS1 = BV14,
        RIS_PLUS2 = BV15,
        RIS_PLUS3 = BV16,
        RIS_PLUS4 = BV17,
        RIS_PLUS5 = BV18,
        RIS_PLUS6 = BV19,
        RIS_MAGIC = BV20,
        RIS_PARALYSIS = BV21,
    };
    /* 21 RIS's*/

    /*
     * Attack types
     */
    enum AttackType
    {
        ATCK_BITE = BV00,
        ATCK_CLAWS = BV01,
        ATCK_TAIL = BV02,
        ATCK_STING = BV03,
        ATCK_PUNCH = BV04,
        ATCK_KICK = BV05,
        ATCK_TRIP = BV06,
        ATCK_BACKSTAB = BV10,
    };

    /*
     * Defense types
     */
    enum DefenseType
    {
        DFND_PARRY = BV00,
        DFND_DODGE = BV01,
        DFND_DISARM = BV19,
        DFND_GRIP = BV21,
    };

    /*
     * Body parts
     */
    enum BodyPart
    {
        PART_HEAD = BV00,
        PART_ARMS = BV01,
        PART_LEGS = BV02,
        PART_HEART = BV03,
        PART_BRAINS = BV04,
        PART_GUTS = BV05,
        PART_HANDS = BV06,
        PART_FEET = BV07,
        PART_FINGERS = BV08,
        PART_EAR = BV09,
        PART_EYE = BV10,
        PART_LONG_TONGUE = BV11,
        PART_EYESTALKS = BV12,
        PART_TENTACLES = BV13,
        PART_FINS = BV14,
        PART_WINGS = BV15,
        PART_TAIL = BV16,
        PART_SCALES = BV17,
        /* for combat */
        PART_CLAWS = BV18,
        PART_FANGS = BV19,
        PART_HORNS = BV20,
        PART_TUSKS = BV21,
        PART_TAILATTACK = BV22,
        PART_SHARPSCALES = BV23,
        PART_BEAK = BV24,

        PART_HAUNCH = BV25,
        PART_HOOVES = BV26,
        PART_PAWS = BV27,
        PART_FORELEGS = BV28,
        PART_FEATHERS = BV29,
    };

    /*
     * Autosave flags
     */
    enum AutosaveFlag
    {
        SV_DEATH = BV00,
        SV_KILL = BV01,
        SV_PASSCHG = BV02,
        SV_DROP = BV03,
        SV_PUT = BV04,
        SV_GIVE = BV05,
        SV_AUTO = BV06,
        SV_ZAPDROP = BV07,
        SV_AUCTION = BV08,
        SV_GET = BV09,
        SV_RECEIVE = BV10,
        SV_IDLE = BV11,
        SV_BACKUP = BV12,
    };

    /*
     * Pipe flags
     */
    enum PipeFlag
    {
        PIPE_TAMPED = BV01,
        PIPE_LIT = BV02,
        PIPE_HOT = BV03,
        PIPE_DIRTY = BV04,
        PIPE_FILTHY = BV05,
        PIPE_GOINGOUT = BV06,
        PIPE_BURNT = BV07,
        PIPE_FULLOFASH = BV08,
    };

    /*
     * Skill/Spell flags	The minimum BV *MUST* be 11!
     */
    enum SkillFlag
    {
        SF_WATER = BV11,
        SF_EARTH = BV12,
        SF_AIR = BV13,
        SF_ASTRAL = BV14,
        SF_AREA = BV15,    /* is an area spell		*/
        SF_DISTANT = BV16, /* affects something far away	*/
        SF_REVERSE = BV17,
        SF_SAVE_HALF_DAMAGE = BV18, /* save for half damage		*/
        SF_SAVE_NEGATES = BV19,     /* save negates affect		*/
        SF_ACCUMULATIVE = BV20,     /* is accumulative		*/
        SF_RECASTABLE = BV21,       /* can be refreshed		*/
        SF_NOSCRIBE = BV22,         /* cannot be scribed		*/
        SF_NOBREW = BV23,           /* cannot be brewed		*/
        SF_GROUPSPELL = BV24,       /* only affects group members	*/
        SF_OBJECT = BV25,           /* directed at an object	*/
        SF_CHARACTER = BV26,        /* directed at a character	*/
        SF_SECRETSKILL = BV27,      /* hidden unless learned	*/
        SF_PKSENSITIVE = BV28,      /* much harder for plr vs. plr	*/
        SF_STOPONFAIL = BV29,       /* stops spell on first failure */
    };

    typedef enum
    {
        SS_NONE,
        SS_POISON_DEATH,
        SS_ROD_WANDS,
        SS_PARA_PETRI,
        SS_BREATH,
        SS_SPELL_STAFF
    } save_types;

    constexpr int ALL_BITS = INT_MAX;
    constexpr int SDAM_MASK = ALL_BITS & ~(BV00 | BV01 | BV02);
    constexpr int SACT_MASK = ALL_BITS & ~(BV03 | BV04 | BV05);
    constexpr int SCLA_MASK = ALL_BITS & ~(BV06 | BV07 | BV08);
    constexpr int SPOW_MASK = ALL_BITS & ~(BV09 | BV10);

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

    constexpr int MAX_TRAPTYPE = TRAP_TYPE_SEX_CHANGE;

    enum TrapFlag
    {
        TRAP_ROOM = BV00,
        TRAP_OBJ = BV01,
        TRAP_ENTER_ROOM = BV02,
        TRAP_LEAVE_ROOM = BV03,
        TRAP_OPEN = BV04,
        TRAP_CLOSE = BV05,
        TRAP_GET = BV06,
        TRAP_PUT = BV07,
        TRAP_PICK = BV08,
        TRAP_UNLOCK = BV09,
        TRAP_N = BV10,
        TRAP_S = BV11,
        TRAP_E = BV12,
        TRAP_W = BV13,
        TRAP_U = BV14,
        TRAP_D = BV15,
        TRAP_EXAMINE = BV16,
        TRAP_NE = BV17,
        TRAP_NW = BV18,
        TRAP_SE = BV19,
        TRAP_SW = BV20,
    };

    /*
     * Well known object virtual numbers.
     * Defined in #OBJECTS.
     */
    constexpr int OBJ_VNUM_MONEY_ONE = 2;
    constexpr int OBJ_VNUM_MONEY_SOME = 3;

    constexpr int COMMSYS_VNUM = 62;
    constexpr int DATAPAD_VNUM = 63;

    constexpr int MODULE_VNUM = 73;
    constexpr int SABER_VNUM = 72;

    constexpr int OBJ_VNUM_DROID_CORPSE = 9;
    constexpr int OBJ_VNUM_CORPSE_NPC = 10;
    constexpr int OBJ_VNUM_CORPSE_PC = 11;
    constexpr int OBJ_VNUM_SEVERED_HEAD = 12;
    constexpr int OBJ_VNUM_TORN_HEART = 13;
    constexpr int OBJ_VNUM_SLICED_ARM = 14;
    constexpr int OBJ_VNUM_SLICED_LEG = 15;
    constexpr int OBJ_VNUM_SPILLED_GUTS = 16;
    constexpr int OBJ_VNUM_BLOOD = 17;
    constexpr int OBJ_VNUM_BLOODSTAIN = 18;
    constexpr int OBJ_VNUM_SCRAPS = 19;

    constexpr int OBJ_VNUM_MUSHROOM = 20;
    constexpr int OBJ_VNUM_LIGHT_BALL = 21;
    constexpr int OBJ_VNUM_SPRING = 22;

    constexpr int OBJ_VNUM_SLICE = 24;
    constexpr int OBJ_VNUM_SHOPPING_BAG = 25;

    constexpr int OBJ_VNUM_FIRE = 30;
    constexpr int OBJ_VNUM_TRAP = 31;
    constexpr int OBJ_VNUM_PORTAL = 32;

    constexpr int OBJ_VNUM_BLACK_POWDER = 33;
    constexpr int OBJ_VNUM_SCROLL_SCRIBING = 34;
    constexpr int OBJ_VNUM_FLASK_BREWING = 35;
    constexpr int OBJ_VNUM_NOTE = 36;

    /* Academy eq */
    constexpr int OBJ_VNUM_SCHOOL_MACE = 10315;
    constexpr int OBJ_VNUM_SCHOOL_DAGGER = 10312;
    constexpr int OBJ_VNUM_SCHOOL_SWORD = 10313;
    constexpr int OBJ_VNUM_SCHOOL_VEST = 10308;
    constexpr int OBJ_VNUM_SCHOOL_SHIELD = 10310;
    constexpr int OBJ_VNUM_SCHOOL_BANNER = 10311;
    constexpr int OBJ_VNUM_SCHOOL_DIPLOMA = 10321;

    constexpr int OBJ_VNUM_BLASTECH_E11 = 50;
    constexpr int OBJ_VNUM_SHIPBOMB = 68;

    /* These are some defines for modules */
    enum ShipModuleAffect
    {
        AFFECT_PRIMARY = 1,
        AFFECT_SECONDARY = 2,
        AFFECT_MISSILE = 3,
        AFFECT_ROCKET = 4,
        AFFECT_TORPEDO = 5,
        AFFECT_HULL = 6,
        AFFECT_SHIELD = 7,
        AFFECT_SPEED = 8,
        AFFECT_HYPER = 9,
        AFFECT_ENERGY = 10,
        AFFECT_MANUEVER = 11,
        AFFECT_CHAFF = 12,
        AFFECT_ALARM = 13,
        AFFECT_SLAVE = 14,
        AFFECT_TRACTOR = 15,
    };

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

    constexpr int MAX_ITEM_TYPE = ITEM_EMP_GRENADE;
    /*
     * Extra flags.
     * Used in #OBJECTS.
     */
    enum ItemFlag
    {
        ITEM_GLOW = BV00,
        ITEM_HUM = BV01,
        ITEM_DARK = BV02,
        ITEM_HUTT_SIZE = BV03,
        ITEM_CONTRABAND = BV04,
        ITEM_INVIS = BV05,
        ITEM_MAGIC = BV06,
        ITEM_NODROP = BV07,
        ITEM_BLESS = BV08,
        ITEM_ANTI_GOOD = BV09,
        ITEM_ANTI_EVIL = BV10,
        ITEM_ANTI_NEUTRAL = BV11,
        ITEM_NOREMOVE = BV12,
        ITEM_INVENTORY = BV13,
        ITEM_ANTI_SOLDIER = BV14,
        ITEM_ANTI_THIEF = BV15,
        ITEM_ANTI_HUNTER = BV16,
        ITEM_ANTI_JEDI = BV17,
        ITEM_SMALL_SIZE = BV18,
        ITEM_LARGE_SIZE = BV19,
        ITEM_DONATION = BV20,
        ITEM_CLANOBJECT = BV21,
        ITEM_ANTI_CITIZEN = BV22,
        ITEM_ANTI_SITH = BV23,
        ITEM_ANTI_PILOT = BV24,
        ITEM_HIDDEN = BV25,
        ITEM_POISONED = BV26,
        ITEM_COVERING = BV27,
        ITEM_DEATHROT = BV28,
        ITEM_BURRIED = BV29, /* item is underground */
        ITEM_PROTOTYPE = BV30,
        ITEM_HUMAN_SIZE = BV31,
    };

    /* Magic flags - extra extra_flags for objects that are used in spells */
    enum ItemMagicFlag
    {
        ITEM_RETURNING = BV00,
        ITEM_BACKSTABBER = BV01,
        ITEM_BANE = BV02,
        ITEM_LOYAL = BV03,
        ITEM_HASTE = BV04,
        ITEM_DRAIN = BV05,
        ITEM_LIGHTNING_BLADE = BV06,
    };

    /* Blaster settings - only saves on characters */
    enum BlasterSetting
    {
        BLASTER_NORMAL = 0,
        BLASTER_HALF = 2,
        BLASTER_FULL = 5,
        BLASTER_LOW = 1,
        BLASTER_STUN = 3,
        BLASTER_HIGH = 4,
    };

    /* Weapon Types */
    enum WeaponType
    {
        WEAPON_NONE = 0,
        WEAPON_VIBRO_AXE = 1,
        WEAPON_VIBRO_BLADE = 2,
        WEAPON_LIGHTSABER = 3,
        WEAPON_WHIP = 4,
        WEAPON_CLAW = 5,
        WEAPON_BLASTER = 6,
        WEAPON_BLUDGEON = 8,
        WEAPON_BOWCASTER = 9,
        WEAPON_FORCE_PIKE = 11,
        WEAPON_DUAL_LIGHTSABER = 12,
    };

    /* Lever/dial/switch/button/pullchain flags */
    enum ControlFlag
    {
        TRIG_UP = BV00,
        TRIG_UNLOCK = BV01,
        TRIG_LOCK = BV02,
        TRIG_D_NORTH = BV03,
        TRIG_D_SOUTH = BV04,
        TRIG_D_EAST = BV05,
        TRIG_D_WEST = BV06,
        TRIG_D_UP = BV07,
        TRIG_D_DOWN = BV08,
        TRIG_DOOR = BV09,
        TRIG_CONTAINER = BV10,
        TRIG_OPEN = BV11,
        TRIG_CLOSE = BV12,
        TRIG_PASSAGE = BV13,
        TRIG_OLOAD = BV14,
        TRIG_MLOAD = BV15,
        TRIG_TELEPORT = BV16,
        TRIG_TELEPORTALL = BV17,
        TRIG_TELEPORTPLUS = BV18,
        TRIG_DEATH = BV19,
        TRIG_CAST = BV20,
        TRIG_FAKEBLADE = BV21,
        TRIG_RAND4 = BV22,
        TRIG_RAND6 = BV23,
        TRIG_TRAPDOOR = BV24,
        TRIG_ANOTHEROOM = BV25,
        TRIG_USEDIAL = BV26,
        TRIG_ABSOLUTEVNUM = BV27,
        TRIG_SHOWROOMDESC = BV28,
        TRIG_AUTORETURN = BV29,
    };

    enum TeleFlag
    {
        TELE_SHOWDESC = BV00,
        TELE_TRANSALL = BV01,
        TELE_TRANSALLPLUS = BV02,
    };

    /* drug types */
    enum DrugType
    {
        SPICE_GLITTERSTIM = 0,
        SPICE_CARSANUM = 1,
        SPICE_RYLL = 2,
        SPICE_ANDRIS = 3,
    };

    /* crystal types */
    enum GemType
    {
        GEM_NON_ADEGEN = 0,
        GEM_KATHRACITE = 1,
        GEM_RELACITE = 2,
        GEM_DANITE = 3,
        GEM_MEPHITE = 4,
        GEM_PONITE = 5,
        GEM_ILLUM = 6,
        GEM_CORUSCA = 7,
    };

    /*
     * Wear flags.
     * Used in #OBJECTS.
     */
    enum WearFlag
    {
        ITEM_TAKE = BV00,
        ITEM_WEAR_FINGER = BV01,
        ITEM_WEAR_NECK = BV02,
        ITEM_WEAR_BODY = BV03,
        ITEM_WEAR_HEAD = BV04,
        ITEM_WEAR_LEGS = BV05,
        ITEM_WEAR_FEET = BV06,
        ITEM_WEAR_HANDS = BV07,
        ITEM_WEAR_ARMS = BV08,
        ITEM_WEAR_SHIELD = BV09,
        ITEM_WEAR_ABOUT = BV10,
        ITEM_WEAR_WAIST = BV11,
        ITEM_WEAR_WRIST = BV12,
        ITEM_WIELD = BV13,
        ITEM_HOLD = BV14,
        ITEM_DUAL_WIELD = BV15,
        ITEM_WEAR_EARS = BV16,
        ITEM_WEAR_EYES = BV17,
        ITEM_MISSILE_WIELD = BV18,
        ITEM_WEAR_BACK = BV19,
        ITEM_WEAR_HOLSTER1 = BV20,
        ITEM_WEAR_HOLSTER2 = BV21,
        ITEM_WEAR_BOTHWRISTS = BV22,
    };

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

    constexpr int REVERSE_APPLY = 1000;

    /*
     * Values for containers (value[1]).
     * Used in #OBJECTS.
     */
    enum ContainerFlag
    {
        CONT_CLOSEABLE = 1,
        CONT_PICKPROOF = 2,
        CONT_CLOSED = 4,
        CONT_LOCKED = 8,
    };

    /*
     * Sitting/Standing/Sleeping/Sitting on/in/at Objects - Xerves
     * Used for furniture (value[2]) in the #OBJECTS Section
     */
    enum SitStandFlag
    {
        SIT_ON = BV00,
        SIT_IN = BV01,
        SIT_AT = BV02,

        STAND_ON = BV03,
        STAND_IN = BV04,
        STAND_AT = BV05,

        SLEEP_ON = BV06,
        SLEEP_IN = BV07,
        SLEEP_AT = BV08,

        REST_ON = BV09,
        REST_IN = BV10,
        REST_AT = BV11,
    };

    /*
     * Well known room virtual numbers.
     * Defined in #ROOMS.
     */
    constexpr int ROOM_VNUM_LIMBO = 2;
    constexpr int ROOM_VNUM_POLY = 3;
    constexpr int ROOM_VNUM_CHAT = 32144;
    constexpr int ROOM_VNUM_TEMPLE = 32144;
    constexpr int ROOM_VNUM_ALTAR = 32144;
    constexpr int ROOM_VNUM_SCHOOL = 115;
    constexpr int ROOM_AUTH_START = 10300;
    constexpr int ROOM_START_HUMAN = 211;
    constexpr int ROOM_START_WOOKIEE = 28600;
    constexpr int ROOM_START_TWILEK = 32148;
    constexpr int ROOM_START_RODIAN = 32148;
    constexpr int ROOM_START_HUTT = 32148;
    constexpr int ROOM_START_MON_CALAMARIAN = 21069;
    constexpr int ROOM_START_NOGHRI = 1015;
    constexpr int ROOM_START_GAMORREAN = 28100;
    constexpr int ROOM_START_JAWA = 31819;
    constexpr int ROOM_START_ADARIAN = 29000;
    constexpr int ROOM_START_EWOK = 32148;
    constexpr int ROOM_START_VERPINE = 32148;
    constexpr int ROOM_START_DEFEL = 32148;
    constexpr int ROOM_START_TRANDOSHAN = 32148;
    constexpr int ROOM_START_HAPAN = 32148;
    constexpr int ROOM_START_DUINUOGWUIN = 32148;
    constexpr int ROOM_START_QUARREN = 21069;
    constexpr int ROOM_START_IMMORTAL = 100;
    constexpr int ROOM_LIMBO_SHIPYARD = 45;
    constexpr int ROOM_DEFAULT_CRASH = 28025;

    constexpr int ROOM_PLUOGUS_QUIT = 905;

    constexpr int ROOM_SHUTTLE_BUS = 907;   /* Sol */
    constexpr int ROOM_SHUTTLE_BUS_2 = 914; /* Monir*/
    constexpr int ROOM_SHUTTLE_BUS_3 = 921; /* Fau */
    constexpr int ROOM_SHUTTLE_BUS_4 = 928; /* Taw */
    constexpr int ROOM_CORUSCANT_SHUTTLE = 199;
    constexpr int ROOM_SENATE_SHUTTLE = 10197;
    constexpr int ROOM_CORUSCANT_TURBOCAR = 226;

    DEDUPE const char* SHIP_AREA = "shipvnum.are";

    /*
     * Room flags.           Holy cow!  Talked about stripped away..
     * Used in #ROOMS.       Those merc guys know how to strip code down.
     *			 Lets put it all back... ;)
     */

    enum RoomFlag
    {
        ROOM_DARK = BV00,
        /* BV01 now reserved for track  BV01  and hunt */
        ROOM_NO_MOB = BV02,
        ROOM_INDOORS = BV03,
        ROOM_CAN_LAND = BV04,
        ROOM_CAN_FLY = BV05,
        ROOM_NO_DRIVING = BV06,
        ROOM_NO_MAGIC = BV07,
        ROOM_BANK = BV08,
        ROOM_PRIVATE = BV09,
        ROOM_SAFE = BV10,
        ROOM_SOLITARY = BV11,
        ROOM_PET_SHOP = BV12,
        ROOM_NO_RECALL = BV13,
        ROOM_DONATION = BV14,
        ROOM_NODROPALL = BV15,
        ROOM_SILENCE = BV16,
        ROOM_LOGSPEECH = BV17,
        ROOM_NODROP = BV18,
        ROOM_CLANSTOREROOM = BV19,
        ROOM_PLR_HOME = BV20,
        ROOM_EMPTY_HOME = BV21,
        ROOM_TELEPORT = BV22,
        ROOM_HOTEL = BV23,
        ROOM_NOFLOOR = BV24,
        ROOM_REFINERY = BV25,
        ROOM_FACTORY = BV26,
        ROOM_R_RECRUIT = BV27,
        ROOM_E_RECRUIT = BV28,
        ROOM_SPACECRAFT = BV29,
        ROOM_PROTOTYPE = BV30,
        ROOM_AUCTION = BV31,
    };

    /* Second Set of Room Flags */
    enum RoomFlag2
    {
        ROOM_EMPTY_SHOP = BV00,
        ROOM_PLR_SHOP = BV01,
        ROOM_SHIPYARD = BV02,
        ROOM_GARAGE = BV03,
        ROOM_BARRACKS = BV04,
        ROOM_CONTROL = BV05,
        ROOM_CLANLAND = BV06,
        ROOM_ARENA = BV07,
        ROOM_CLANJAIL = BV08,
        ROOM_BLACKMARKET = BV09,
        ROOM_HIDDENPAD = BV10,
        ROOM_SLOTS = BV11,
        ROOM_IMPORT = BV12,
        ROOM_STORAGEDOCK = BV13,
    };

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

    constexpr int MAX_DIR = DIR_SOUTHWEST;    /* max for normal walking */
    constexpr int DIR_PORTAL = DIR_SOMEWHERE; /* portal direction	  */

    /*
     * Exit flags.
     * Used in #ROOMS.
     */
    enum ExitFlag
    {
        EX_ISDOOR = BV00,
        EX_CLOSED = BV01,
        EX_LOCKED = BV02,
        EX_SECRET = BV03,
        EX_SWIM = BV04,
        EX_PICKPROOF = BV05,
        EX_FLY = BV06,
        EX_CLIMB = BV07,
        EX_DIG = BV08,
        EX_RES1 = BV09, /* are these res[1-4] important? */
        EX_NOPASSDOOR = BV10,
        EX_HIDDEN = BV11,
        EX_PASSAGE = BV12,
        EX_PORTAL = BV13,
        EX_RES2 = BV14,
        EX_RES3 = BV15,
        EX_xCLIMB = BV16,
        EX_xENTER = BV17,
        EX_xLEAVE = BV18,
        EX_xAUTO = BV19,
        EX_RES4 = BV20,
        EX_xSEARCHABLE = BV21,
        EX_BASHED = BV22,
        EX_BASHPROOF = BV23,
        EX_NOMOB = BV24,
        EX_WINDOW = BV25,
        EX_xLOOK = BV26,
        MAX_EXFLAG = 26,
    };

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
    enum PlayerActFlag
    {
        PLR_IS_NPC = BV00, /* Don't EVER set.	*/
        PLR_BOUGHT_PET = BV01,
        PLR_SHOVEDRAG = BV02,
        PLR_AUTOEXIT = BV03,
        PLR_AUTOLOOT = BV04,
        PLR_AUTOSAC = BV05,
        PLR_BLANK = BV06,
        PLR_OUTCAST = BV07,
        PLR_BRIEF = BV08,
        PLR_COMBINE = BV09,
        PLR_PROMPT = BV10,
        PLR_TELNET_GA = BV11,

        PLR_HOLYLIGHT = BV12,
        PLR_WIZINVIS = BV13,
        PLR_ROOMVNUM = BV14,

        PLR_SILENCE = BV15,
        PLR_NO_EMOTE = BV16,
        PLR_ATTACKER = BV17,
        PLR_NO_TELL = BV18,
        PLR_LOG = BV19,
        PLR_DENY = BV20,
        PLR_FREEZE = BV21,
        PLR_KILLER = BV22,
        PLR_WHOINVIS = BV23,
        PLR_LITTERBUG = BV24,
        PLR_ANSI = BV25,
        PLR_SOUND = BV26,
        PLR_NICE = BV27,
        PLR_FLEE = BV28,
        PLR_AUTOGOLD = BV29,
        PLR_SLOG = BV30,
        PLR_AFK = BV31,
    };

    enum PCFlag
    {
        /* Bits for pc_data->flags. */
        PCFLAG_R1 = BV00,
        /*
        #define PCFLAG_                    BV01     extra flag
        */
        PCFLAG_UNAUTHED = BV02,
        PCFLAG_NORECALL = BV03,
        PCFLAG_NOINTRO = BV04,
        PCFLAG_GAG = BV05,
        PCFLAG_RETIRED = BV06,
        PCFLAG_GUEST = BV07,
        PCFLAG_HASSLUG = BV08,
        PCFLAG_PAGERON = BV09,
        PCFLAG_NOTITLE = BV10,
        PCFLAG_ROOM = BV11,
        PCFLAG_MAP = BV12,
    };

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
    enum ChannelFlag
    {
        CHANNEL_AUCTION = BV00,
        CHANNEL_CHAT = BV01,
        CHANNEL_QUEST = BV02,
        CHANNEL_IMMTALK = BV03,
        CHANNEL_MUSIC = BV04,
        CHANNEL_ASK = BV05,
        CHANNEL_SHOUT = BV06,
        CHANNEL_YELL = BV07,
        CHANNEL_MONITOR = BV08,
        CHANNEL_LOG = BV09,
        CHANNEL_104 = BV10,
        CHANNEL_CLAN = BV11,
        CHANNEL_BUILD = BV12,
        CHANNEL_105 = BV13,
        CHANNEL_AVTALK = BV14,
        CHANNEL_PRAY = BV15,
        CHANNEL_COUNCIL = BV16,
        CHANNEL_GUILD = BV17,
        CHANNEL_COMM = BV18,
        CHANNEL_TELLS = BV19,
        CHANNEL_ORDER = BV20,
        CHANNEL_NEWBIE = BV21,
        CHANNEL_WARTALK = BV22,
        CHANNEL_OOC = BV23,
        CHANNEL_SHIP = BV24,
        CHANNEL_SYSTEM = BV25,
        CHANNEL_SPACE = BV26,
        CHANNEL_103 = BV27,
        CHANNEL_SPORTS = BV27,
        CHANNEL_HOLONET = BV31,

        CHANNEL_CLANTALK = CHANNEL_CLAN,
    };

    /* Area defines - Scryn 8/11
     *
     */
    enum AreaStateFlags
    {
        AREA_DELETED = BV00,
        AREA_LOADED = BV01,
    };

    /* Area flags - Narn Mar/96 */
    enum AreaFlags
    {
        AFLAG_NOPKILL = BV00,
    };

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
    constexpr int LIQ_WATER = 0;
    constexpr int LIQ_MAX = 19;

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
    enum ResetBits
    {
        BIT_RESET_DOOR = 0,
        BIT_RESET_OBJECT = 1,
        BIT_RESET_MOBILE = 2,
        BIT_RESET_ROOM = 3,
        BIT_RESET_TYPE_MASK = 0xFF, /* 256 should be enough */
        BIT_RESET_DOOR_THRESHOLD = 8,
        BIT_RESET_DOOR_MASK = 0xFF00, /* 256 should be enough */
        BIT_RESET_SET = BV30,
        BIT_RESET_TOGGLE = BV31,
        BIT_RESET_FREEBITS = 0x3FFF0000, /* For reference */
    };

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
    constexpr int TYPE_UNDEFINED = -1;
    constexpr int TYPE_MISSILE = 111;
    constexpr int TYPE_HIT = 1000;      /* allows for 1000 skills/spells */
    constexpr int TYPE_HERB = 2000;     /* allows for 1000 attack types  */
    constexpr int TYPE_PERSONAL = 3000; /* allows for 1000 herb types    */

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
    GLOBAL sh_int gsn_smallspace;
    GLOBAL sh_int gsn_mediumspace;
    GLOBAL sh_int gsn_largespace;
    GLOBAL sh_int gsn_weaponsystems;
    GLOBAL sh_int gsn_navigation;
    GLOBAL sh_int gsn_shipsystems;
    GLOBAL sh_int gsn_tractorbeams;
    GLOBAL sh_int gsn_spacecombat;
    GLOBAL sh_int gsn_spacecombat2;
    GLOBAL sh_int gsn_spacecombat3;
    GLOBAL sh_int gsn_bomb;
    GLOBAL sh_int gsn_split_s;
    GLOBAL sh_int gsn_shipdesign;
    GLOBAL sh_int gsn_chandelle;

    /* Technician skills */
    GLOBAL sh_int gsn_makemodule;
    GLOBAL sh_int gsn_installmodule;
    GLOBAL sh_int gsn_showmodules;
    GLOBAL sh_int gsn_shipmaintenance;
    GLOBAL sh_int gsn_scanbugs;
    GLOBAL sh_int gsn_removebug;
    GLOBAL sh_int gsn_removemodule;
    GLOBAL sh_int gsn_makejetpack;

    /* These are bh skills */
    GLOBAL sh_int gsn_ambush;
    GLOBAL sh_int gsn_bind;
    GLOBAL sh_int gsn_gag;

    GLOBAL sh_int gsn_battle_command;
    GLOBAL sh_int gsn_reinforcements;
    GLOBAL sh_int gsn_postguard;

    GLOBAL sh_int gsn_addpatrol;
    GLOBAL sh_int gsn_eliteguard;
    GLOBAL sh_int gsn_specialforces;
    GLOBAL sh_int gsn_jail;
    GLOBAL sh_int gsn_smalltalk;
    GLOBAL sh_int gsn_propeganda;
    GLOBAL sh_int gsn_bribe;
    GLOBAL sh_int gsn_seduce;
    GLOBAL sh_int gsn_masspropeganda;
    GLOBAL sh_int gsn_gather_intelligence;

    /* hunter assassin gsn ints */
    GLOBAL sh_int gsn_plantbug;
    GLOBAL sh_int gsn_showbugs;
    GLOBAL sh_int gsn_silent;
    GLOBAL sh_int gsn_retreat;

    /* The gsn ints for the slicers */
    GLOBAL sh_int gsn_spy;
    GLOBAL sh_int gsn_makecommsystem;
    GLOBAL sh_int gsn_sabotage;
    GLOBAL sh_int gsn_commsystem;
    GLOBAL sh_int gsn_codecrack;
    GLOBAL sh_int gsn_slicebank;
    GLOBAL sh_int gsn_inquire;
    GLOBAL sh_int gsn_makedatapad;
    GLOBAL sh_int gsn_disable;
    GLOBAL sh_int gsn_assignpilot;
    GLOBAL sh_int gsn_checkprints;

    GLOBAL sh_int gsn_torture;
    GLOBAL sh_int gsn_snipe;
    GLOBAL sh_int gsn_throw;
    GLOBAL sh_int gsn_deception;
    GLOBAL sh_int gsn_disguise;
    GLOBAL sh_int gsn_mine;
    GLOBAL sh_int gsn_first_aid;

    GLOBAL sh_int gsn_beg;
    GLOBAL sh_int gsn_makeblade;
    GLOBAL sh_int gsn_makebug;
    GLOBAL sh_int gsn_makebeacon;
    GLOBAL sh_int gsn_makepike;
    GLOBAL sh_int gsn_makejewelry;
    GLOBAL sh_int gsn_makeblaster;
    GLOBAL sh_int gsn_makelight;
    GLOBAL sh_int gsn_makecomlink;
    GLOBAL sh_int gsn_makegrenade;
    GLOBAL sh_int gsn_makeshipbomb;
    GLOBAL sh_int gsn_makelandmine;
    GLOBAL sh_int gsn_makearmor;
    GLOBAL sh_int gsn_makeshield;
    GLOBAL sh_int gsn_makecontainer;
    GLOBAL sh_int gsn_gemcutting;
    GLOBAL sh_int gsn_makelightsaber;
    GLOBAL sh_int gsn_makeduallightsaber;
    GLOBAL sh_int gsn_repair;
    GLOBAL sh_int gsn_shiprepair;
    GLOBAL sh_int gsn_spice_refining;

    GLOBAL sh_int gsn_detrap;
    GLOBAL sh_int gsn_backstab;
    GLOBAL sh_int gsn_dualstab;
    GLOBAL sh_int gsn_bargain;
    GLOBAL sh_int gsn_circle;
    GLOBAL sh_int gsn_dodge;
    GLOBAL sh_int gsn_hide;
    GLOBAL sh_int gsn_concealment;
    GLOBAL sh_int gsn_peek;
    GLOBAL sh_int gsn_pick_lock;
    GLOBAL sh_int gsn_scan;
    GLOBAL sh_int gsn_sneak;
    GLOBAL sh_int gsn_steal;
    GLOBAL sh_int gsn_gouge;
    GLOBAL sh_int gsn_track;
    GLOBAL sh_int gsn_search;
    GLOBAL sh_int gsn_dig;
    GLOBAL sh_int gsn_mount;
    GLOBAL sh_int gsn_bashdoor;
    GLOBAL sh_int gsn_berserk;
    GLOBAL sh_int gsn_hitall;
    GLOBAL sh_int gsn_pickshiplock;
    GLOBAL sh_int gsn_hijack;

    GLOBAL sh_int gsn_disarm;
    GLOBAL sh_int gsn_enhanced_damage;
    GLOBAL sh_int gsn_kick;
    GLOBAL sh_int gsn_parry;
    GLOBAL sh_int gsn_rescue;
    GLOBAL sh_int gsn_second_attack;
    GLOBAL sh_int gsn_third_attack;
    GLOBAL sh_int gsn_dual_wield;
    GLOBAL sh_int gsn_reflect;

    GLOBAL sh_int gsn_aid;
    GLOBAL sh_int gsn_plantbeacon;
    GLOBAL sh_int gsn_showbeacons;
    GLOBAL sh_int gsn_checkbeacons;
    GLOBAL sh_int gsn_nullifybeacons;
    GLOBAL sh_int gsn_makebinders;
    GLOBAL sh_int gsn_launchers;
    GLOBAL sh_int gsn_makemissile;
    GLOBAL sh_int gsn_makeempgrenade;
    GLOBAL sh_int gsn_makegoggles;
    GLOBAL sh_int gsn_truesight;
    GLOBAL sh_int gsn_barrelroll;
    GLOBAL sh_int gsn_juke;

    /* used to do specific lookups */
    GLOBAL sh_int gsn_first_spell;
    GLOBAL sh_int gsn_first_skill;
    GLOBAL sh_int gsn_first_weapon;
    GLOBAL sh_int gsn_first_tongue;
    GLOBAL sh_int gsn_top_sn;

    /* spells */
    GLOBAL sh_int gsn_blindness;
    GLOBAL sh_int gsn_charm_person;
    GLOBAL sh_int gsn_aqua_breath;
    GLOBAL sh_int gsn_invis;
    GLOBAL sh_int gsn_mass_invis;
    GLOBAL sh_int gsn_poison;
    GLOBAL sh_int gsn_sleep;
    GLOBAL sh_int gsn_possess;
    GLOBAL sh_int gsn_fireball;       /* for fireshield  */
    GLOBAL sh_int gsn_lightning_bolt; /* for shockshield */

    /* newer attack skills */
    GLOBAL sh_int gsn_punch;
    GLOBAL sh_int gsn_bash;
    GLOBAL sh_int gsn_stun;

    GLOBAL sh_int gsn_poison_weapon;
    GLOBAL sh_int gsn_climb;

    GLOBAL sh_int gsn_blasters;
    GLOBAL sh_int gsn_force_pikes;
    GLOBAL sh_int gsn_bowcasters;
    GLOBAL sh_int gsn_lightsabers;
    GLOBAL sh_int gsn_vibro_blades;
    GLOBAL sh_int gsn_flexible_arms;
    GLOBAL sh_int gsn_talonous_arms;
    GLOBAL sh_int gsn_bludgeons;

    GLOBAL sh_int gsn_grip;

    /* languages */
    GLOBAL sh_int gsn_basic;
    GLOBAL sh_int gsn_wookiee;
    GLOBAL sh_int gsn_twilek;
    GLOBAL sh_int gsn_rodian;
    GLOBAL sh_int gsn_hutt;
    GLOBAL sh_int gsn_mon_calamari;
    GLOBAL sh_int gsn_noghri;
    GLOBAL sh_int gsn_ewok;
    GLOBAL sh_int gsn_ithorian;
    GLOBAL sh_int gsn_gotal;
    GLOBAL sh_int gsn_devaronian;
    GLOBAL sh_int gsn_binary;
    GLOBAL sh_int gsn_firrerreo;
    GLOBAL sh_int gsn_gamorrean;
    GLOBAL sh_int gsn_togorian;
    GLOBAL sh_int gsn_shistavanen;
    GLOBAL sh_int gsn_jawa;
    GLOBAL sh_int gsn_kubaz;
    GLOBAL sh_int gsn_adarian;
    GLOBAL sh_int gsn_verpine;
    GLOBAL sh_int gsn_defel;
    GLOBAL sh_int gsn_trandoshan;
    GLOBAL sh_int gsn_hapan;
    GLOBAL sh_int gsn_quarren;
    GLOBAL sh_int gsn_sullustan;
    GLOBAL sh_int gsn_falleen;
    GLOBAL sh_int gsn_barabel;
    GLOBAL sh_int gsn_yevethan;
    GLOBAL sh_int gsn_gand;
    GLOBAL sh_int gsn_coynite;
    GLOBAL sh_int gsn_duinuogwuin;
    GLOBAL sh_int gsn_droid;

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
    GLOBAL time_t last_restore_all_time;
    GLOBAL time_t boot_time; /* this should be moved down */
    GLOBAL HOUR_MIN_SEC* set_boot_time;
    GLOBAL tm* new_boot_time;
    GLOBAL time_t new_boot_time_t;

    GLOBAL const STR_APP_TYPE str_app[26];
    GLOBAL const INT_APP_TYPE int_app[26];
    GLOBAL const WIS_APP_TYPE wis_app[26];
    GLOBAL const DEX_APP_TYPE dex_app[30];
    GLOBAL const CON_APP_TYPE con_app[26];
    GLOBAL const CHA_APP_TYPE cha_app[26];
    GLOBAL const LCK_APP_TYPE lck_app[26];
    GLOBAL const FRC_APP_TYPE frc_app[26];
    GLOBAL const RACE_TYPE race_table[MAX_RACE];
    GLOBAL const LIQ_TYPE liq_table[LIQ_MAX];
    GLOBAL const char* attack_table[13];
    GLOBAL const char* ability_name[MAX_ABILITY];
    GLOBAL const char* height_name[4];
    GLOBAL const char* build_name[6];
    GLOBAL const char* droid_name[8];

    GLOBAL const char* skill_tname[];
    GLOBAL sh_int const movement_loss[SECT_MAX];
    GLOBAL const char* dir_name[];
    GLOBAL const char* where_name[];
    GLOBAL const sh_int rev_dir[];
    GLOBAL const int trap_door[];
    GLOBAL const char* r_flags[];
    GLOBAL const char* r_flags2[];
    GLOBAL const char* w_flags[];
    GLOBAL const char* o_flags[];
    GLOBAL const char* a_flags[];
    GLOBAL const char* o_types[];
    GLOBAL const char* a_types[];
    GLOBAL const char* act_flags[];
    GLOBAL const char* planet_flags[];
    GLOBAL const char* mprog_flags[];
    GLOBAL const char* weapon_table[13];
    GLOBAL const char* spice_table[];
    GLOBAL const char* plr_flags[];
    GLOBAL const char* pc_flags[];
    GLOBAL const char* trap_flags[];
    GLOBAL const char* ris_flags[];
    GLOBAL const char* trig_flags[];
    GLOBAL const char* part_flags[];
    GLOBAL const char* npc_race[];
    GLOBAL const char* defense_flags[];
    GLOBAL const char* attack_flags[];
    GLOBAL const char* area_flags[];

    GLOBAL int const lang_array[];
    GLOBAL const char* lang_names[];

    GLOBAL bool bootup;
    GLOBAL char namefreq[MAX_STRING_LENGTH];
    GLOBAL char bname[MAX_STRING_LENGTH];

    /*
     * Global variables.
     */

    GLOBAL MPSLEEP_DATA* first_mpwait;   /* Storing sleeping mud progs */
    GLOBAL MPSLEEP_DATA* last_mpwait;    /* - */
    GLOBAL MPSLEEP_DATA* current_mpwait; /* - */
    GLOBAL int numobjsloaded;
    GLOBAL int nummobsloaded;
    GLOBAL int physicalobjects;
    GLOBAL SYSTEM_DATA sysdata;
    GLOBAL int top_sn;
    GLOBAL int top_herb;

    GLOBAL CMDTYPE* command_hash[126];

    GLOBAL SKILL_TYPE* skill_table[MAX_SKILL];
    GLOBAL SOCIALTYPE* social_index[27];
    GLOBAL CHAR_DATA* cur_char;
    GLOBAL ROOM_INDEX_DATA* cur_room;
    GLOBAL bool cur_char_died;
    GLOBAL ch_ret global_retcode;
    GLOBAL SKILL_TYPE* herb_table[MAX_HERB];

    GLOBAL int cur_obj;
    GLOBAL int cur_obj_serial;
    GLOBAL bool cur_obj_extracted;
    GLOBAL obj_ret global_objcode;

    GLOBAL HELP_DATA* first_help;
    GLOBAL HELP_DATA* last_help;
    GLOBAL SHOP_DATA* first_shop;
    GLOBAL SHOP_DATA* last_shop;
    GLOBAL REPAIR_DATA* first_repair;
    GLOBAL REPAIR_DATA* last_repair;

    GLOBAL BAN_DATA* first_ban;
    GLOBAL BAN_DATA* last_ban;
    GLOBAL CHAR_DATA* first_char;
    GLOBAL CHAR_DATA* last_char;
    GLOBAL std::vector<std::shared_ptr<DESCRIPTOR_DATA>> g_descriptors;
    GLOBAL BOARD_DATA* first_board;
    GLOBAL BOARD_DATA* last_board;
    GLOBAL OBJ_DATA* first_object;
    GLOBAL OBJ_DATA* last_object;
    GLOBAL CLAN_DATA* first_clan;
    GLOBAL CLAN_DATA* last_clan;
    GLOBAL GUARD_DATA* first_guard;
    GLOBAL GUARD_DATA* last_guard;
    GLOBAL SHIP_DATA* first_ship;
    GLOBAL SHIP_DATA* last_ship;
    GLOBAL SPACE_DATA* first_starsystem;
    GLOBAL SPACE_DATA* last_starsystem;
    GLOBAL PLANET_DATA* first_planet;
    GLOBAL PLANET_DATA* last_planet;
    GLOBAL SENATE_DATA* first_senator;
    GLOBAL SENATE_DATA* last_senator;
    GLOBAL BOUNTY_DATA* first_bounty;
    GLOBAL BOUNTY_DATA* last_bounty;
    GLOBAL BOUNTY_DATA* first_disintegration;
    GLOBAL BOUNTY_DATA* last_disintegration;
    GLOBAL AREA_DATA* first_area;
    GLOBAL AREA_DATA* last_area;
    GLOBAL AREA_DATA* first_build;
    GLOBAL AREA_DATA* last_build;
    GLOBAL AREA_DATA* first_asort;
    GLOBAL AREA_DATA* last_asort;
    GLOBAL AREA_DATA* first_bsort;
    GLOBAL AREA_DATA* last_bsort;

    /*
    GLOBAL		GOD_DATA	  *	first_imm;
    GLOBAL		GOD_DATA	  *	last_imm;
    */
    GLOBAL TELEPORT_DATA* first_teleport;
    GLOBAL TELEPORT_DATA* last_teleport;
    GLOBAL OBJ_DATA* extracted_obj_queue;
    GLOBAL EXTRACT_CHAR_DATA* extracted_char_queue;
    GLOBAL OBJ_DATA* save_equipment[MAX_WEAR][MAX_LAYERS];
    GLOBAL CHAR_DATA* quitting_char;
    GLOBAL CHAR_DATA* loading_char;
    GLOBAL CHAR_DATA* saving_char;
    GLOBAL OBJ_DATA* all_obj;

    GLOBAL char bug_buf[];
    GLOBAL time_t current_time;
    GLOBAL bool fLogAll;
    GLOBAL bool fLogPC;
    GLOBAL char log_buf[2 * MAX_INPUT_LENGTH];
    GLOBAL TIME_INFO_DATA time_info;
    GLOBAL WEATHER_DATA weather_info;
    GLOBAL AUCTION_DATA* auction;
    GLOBAL ACT_PROG_DATA* mob_act_list;

    GLOBAL BMARKET_DATA* first_market_ship;
    GLOBAL BMARKET_DATA* last_market_ship;

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
    DEDUPE const char* AREA_DIR = "area/";
    DEDUPE const char* PLAYER_DIR = "player/"; /* Player files			*/
    DEDUPE const char* BACKUP_DIR = "backup/"; /* Backup Player files		*/
    DEDUPE const char* GOD_DIR = "gods/";      /* God Info Dir			*/
    DEDUPE const char* BOARD_DIR = "boards/";  /* Board data dir		*/
    DEDUPE const char* KEYS_DIR = "keys/";
    DEDUPE const char* CLAN_DIR = "clans/"; /* Clan data dir		*/
    DEDUPE const char* SHIP_DIR = "space/"; // TODO duplicate?
    DEDUPE const char* SPACE_DIR = "space/";
    DEDUPE const char* SHIP_PROTOTYPE_DIR = "ships/";
    DEDUPE const char* FORCE_DIR = "force/";
    DEDUPE const char* FORCE_HELP_DIR = "force/help/";
    DEDUPE const char* PLANET_DIR = "planets/";
    DEDUPE const char* GUARD_DIR = "planets/";        // TODO duplicate?
    DEDUPE const char* GUILD_DIR = "guilds/";         /* Guild data dir               */
    DEDUPE const char* HELP_FILE = "system/help.txt"; /*For undefined helps*/
    DEDUPE const char* SLAY_FILE = "system/slay.dat"; /* Slay data file for online editing - Samson 8-3-98 */
    DEDUPE const char* LAST_LIST = "system/last.lst"; // last list
    DEDUPE const char* LAST_TEMP_LIST =
        "system/ltemp.lst";                     // temp file for the last list so the data can be copyover over
    DEDUPE const char* BUILD_DIR = "building/"; /* Online building save dir     */
    DEDUPE const char* SYSTEM_DIR = "system/";  /* Main system files		*/
    DEDUPE const char* PROG_DIR = "mudprogs/"; /* MUDProg files		*/ // TODO unused?
    DEDUPE const char* CORPSE_DIR = "corpses/";                        /* Corpses			*/
    DEDUPE const char* AREA_LIST = "area/area.lst";                    /* List of areas		*/
    DEDUPE const char* BAN_LIST = "system/ban.lst";                    /* List of bans                 */
    DEDUPE const char* CLAN_LIST = "clan.lst";                         /* List of clans		*/
    DEDUPE const char* SHIP_LIST = "ship.lst";
    DEDUPE const char* PROTOTYPE_LIST = "prototype.lst";
    DEDUPE const char* PLANET_LIST = "planet.lst";
    DEDUPE const char* SPACE_LIST = "space.lst";
    DEDUPE const char* BOUNTY_LIST = "bounty.lst";
    DEDUPE const char* disintegration_LIST = "disintegration.lst";
    DEDUPE const char* SENATE_LIST = "senate.lst"; /* List of senators		*/
    DEDUPE const char* GUILD_LIST = "guild.lst";   /* List of guilds               */
    DEDUPE const char* GOD_LIST = "gods.lst";      /* List of gods			*/
    DEDUPE const char* GUARD_LIST = "guard.lst";

    DEDUPE const char* BOARD_FILE = "boards.txt";      /* For bulletin boards	 */
    DEDUPE const char* SHUTDOWN_FILE = "shutdown.txt"; /* For 'shutdown'	 */

    DEDUPE const char* RIPSCREEN_FILE = "system/mudrip.rip";
    DEDUPE const char* RIPTITLE_FILE = "system/mudtitle.rip";
    DEDUPE const char* ANSITITLE_FILE = "system/mudtitle.ans";
    DEDUPE const char* ASCTITLE_FILE = "system/mudtitle.asc";
    DEDUPE const char* BOOTLOG_FILE = "system/boot.txt";      /* Boot up error file	 */
    DEDUPE const char* BUG_FILE = "system/sysbugs.txt";       /* For 'bug' and bug( )*/
    DEDUPE const char* PLRBUG_FILE = "system/plrbugs.txt";    /* Used for player bugs */
    DEDUPE const char* IDEA_FILE = "system/ideas.txt";        /* For 'idea'		 */
    DEDUPE const char* CHANGE_FILE = "system/changes.txt";    /* Changes file - txt  */
    DEDUPE const char* DEBUG_FILE = "system/debug.txt";       /* Catch-all for debug */
    DEDUPE const char* TYPO_FILE = "system/typos.txt";        /* For 'typo'		 */
    DEDUPE const char* LOG_FILE = "system/log.txt";           /* For talking in logged rooms */
    DEDUPE const char* WIZLIST_FILE = "system/WIZLIST";       /* Wizlist		 */
    DEDUPE const char* REQUEST_PIPE = "system/REQUESTS";      /* Request FIFO	 */
    DEDUPE const char* SKILL_FILE = "system/skills.dat";      /* Skill table	 */
    DEDUPE const char* HERB_FILE = "system/herbs.dat";        /* Herb table		 */
    DEDUPE const char* SOCIAL_FILE = "system/socials.dat";    /* Socials		 */
    DEDUPE const char* COMMAND_FILE = "system/commands.dat";  /* Commands		 */
    DEDUPE const char* NAMEBAN_FILE = "system/nameban.dat";   /* Nameban		 */
    DEDUPE const char* USAGE_FILE = "system/usage.txt";       /* How many people are on           \
                               every half hour - trying to                                                             \
                               determine best reboot time */
    DEDUPE const char* TEMP_FILE = "system/charsave.tmp";     /* More char save protect */
    DEDUPE const char* COPYOVER_FILE = "system/copyover.dat"; /* for warm reboots	 */
    DEDUPE const char* EXE_FILE = "../bin/swr";               /* executable path	 */
    DEDUPE const char* SLOG_FILE = "../.slog/slog.txt";       /* Secret Log		 */

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */

/* editor.c cronel new editor */
    void start_editing_nolimit(CHAR_DATA * ch, char* data, sh_int max_size);
    void start_editing(CHAR_DATA * ch, char* data)
    {
        // TODO move to editor
        start_editing_nolimit(ch, data, MAX_STRING_LENGTH);
    }
    void stop_editing(CHAR_DATA * ch);
    void edit_buffer(CHAR_DATA * ch, char* argument);
    char* copy_buffer(CHAR_DATA * ch);
    void set_editor_desc(CHAR_DATA * ch, const char* desc);
    void editor_desc_printf(CHAR_DATA * ch, const char* desc_fmt, ...);

    /* pfiles.c */
    void remove_member(char* name, char* shortname);
    void add_member(char* name, char* shortname);

    /* act_comm.c */
    bool check_parse_name(const char* name);
    void sound_to_room(ROOM_INDEX_DATA * room, const char* argument);
    bool circle_follow(CHAR_DATA * ch, CHAR_DATA * victim);
    char* smaug_crypt(const char* pwd);
    void add_follower(CHAR_DATA * ch, CHAR_DATA * master);
    void stop_follower(CHAR_DATA * ch);
    void die_follower(CHAR_DATA * ch);
    bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch);
    void send_rip_screen(CHAR_DATA * ch);
    void send_rip_title(CHAR_DATA * ch);
    void send_ansi_title(CHAR_DATA * ch);
    void send_ascii_title(CHAR_DATA * ch);
    void to_channel(const char* argument, int channel, const char* verb, sh_int level);
    void talk_auction(char* argument);
    bool knows_language(CHAR_DATA * ch, int language, CHAR_DATA* cch);
    bool can_learn_lang(CHAR_DATA * ch, int language);
    int countlangs(int languages);
    std::string obj_short(OBJ_DATA * obj);

    /* act_info.c */
    int get_door(char* arg);
    char* format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort);
    void show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing);
    void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch);

    /* act_move.c */
    void clear_vrooms(void);
    EXIT_DATA* find_door(CHAR_DATA * ch, const char* arg, bool quiet);
    EXIT_DATA* get_exit(ROOM_INDEX_DATA * room, sh_int dir);
    EXIT_DATA* get_exit_to(ROOM_INDEX_DATA * room, sh_int dir, int vnum);
    EXIT_DATA* get_exit_num(ROOM_INDEX_DATA * room, sh_int count);
    ch_ret move_char(CHAR_DATA * ch, EXIT_DATA * pexit, int fall);
    void teleport(CHAR_DATA * ch, int room, int flags);
    sh_int encumbrance(CHAR_DATA * ch, sh_int move);
    bool will_fall(CHAR_DATA * ch, int fall);
    int wherehome(CHAR_DATA * ch);
    ROOM_INDEX_DATA* generate_exit(ROOM_INDEX_DATA * in_room, EXIT_DATA * *pexit);

    /* act_obj.c */

    obj_ret damage_obj(OBJ_DATA * obj);
    sh_int get_obj_resistance(OBJ_DATA * obj);
    bool remove_obj(CHAR_DATA * ch, int iWear, bool fReplace);
    void save_clan_storeroom(CHAR_DATA * ch, CLAN_DATA * clan);
    void obj_fall(OBJ_DATA * obj, bool through);

    /* act_wiz.c */
    void close_area(AREA_DATA * pArea);
    AREA_DATA* get_area(char* argument);
    ROOM_INDEX_DATA* find_location(CHAR_DATA * ch, char* arg);
    void echo_to_room(sh_int AT_COLOR, ROOM_INDEX_DATA * room, const char* argument);
    void echo_to_all(sh_int AT_COLOR, const char* argument, sh_int tar);
    void get_reboot_string(void);
    tm* update_time(struct tm * old_time);
    void free_social(SOCIALTYPE * social);
    void add_social(SOCIALTYPE * social);
    void free_command(CMDTYPE * command);
    void unlink_command(CMDTYPE * command);
    void add_command(CMDTYPE * command);

    /* boards.c */
    void load_boards(void);
    BOARD_DATA* get_board(OBJ_DATA * obj);
    void free_note(NOTE_DATA * pnote);

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
    void save_clan(CLAN_DATA * clan);
    void load_senate(void);
    void save_senate(void);
    PLANET_DATA* get_planet(char* name);
    void load_planets(void);
    void save_planet(PLANET_DATA * planet);
    long get_taxes(PLANET_DATA * planet);

    /* bounty.c */
    BOUNTY_DATA* get_disintegration(char* target);
    void load_bounties(void);
    void save_bounties(void);
    void save_disintegrations(void);
    void remove_disintegration(BOUNTY_DATA * bounty);
    void claim_disintegration(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_disintegration(CHAR_DATA * victim);

    /* force.c */

    bool check_reflect(CHAR_DATA * ch, CHAR_DATA * victim, int dam);
    void write_all_forceskills();
    void save_forceskill(FORCE_SKILL * fskill);
    void write_forceskill_list();
    bool load_forceskill(const char* forceskillfile);
    void fread_forceskill(FORCE_SKILL * fskill, FILE * fp);
    void write_all_forcehelps();
    void save_forcehelp(FORCE_HELP * fhelp);
    void write_forcehelp_list();
    bool load_forcehelp(char const* forcehelpfile);
    void fread_forcehelp(FORCE_HELP * fhelp, FILE * fp);
    int check_force_skill(CHAR_DATA * ch, const char* command, char* argument);
    void load_force_skills(void);
    void load_force_help(void);
    DO_FUN* get_force_skill_function(char* name);
    FORCE_SKILL* get_force_skill(const char* argument);
    FORCE_HELP* get_force_help(const char* fname, char* type);
    void force_send_to_room(CHAR_DATA * ch, CHAR_DATA * victim, const char* msg);
    CHAR_DATA* force_get_victim(CHAR_DATA * ch, char* argument, int loc);
    const char* force_get_possessive(CHAR_DATA * ch);
    const char* force_get_objective(CHAR_DATA * ch);
    const char* force_get_pronoun(CHAR_DATA * ch);
    const char* force_parse_string(CHAR_DATA * ch, CHAR_DATA * victim, const char* msg);
    void force_learn_from_failure(CHAR_DATA * ch, FORCE_SKILL * fskill);
    void force_learn_from_success(CHAR_DATA * ch, FORCE_SKILL * fskill);
    FORCE_SKILL* force_test_skill_use(const char* skill_name, CHAR_DATA* ch, int skill_type);
    const char* force_get_level(CHAR_DATA * ch);
    int force_promote_ready(CHAR_DATA * ch);
    void draw_force_line(CHAR_DATA * ch, int length);
    void draw_force_line_rev(CHAR_DATA * ch, int length);
    void update_force(void);

    /* space.c */
    SHIP_DATA* get_ship(char* name);
    void load_ships(void);
    void placeships(void);
    void save_ship(SHIP_DATA * ship);
    void load_space(void);
    void save_starsystem(SPACE_DATA * starsystem);
    SPACE_DATA* starsystem_from_name(const char* name);
    SPACE_DATA* starsystem_from_room(ROOM_INDEX_DATA * room);
    SHIP_DATA* ship_from_entrance(int vnum);
    SHIP_DATA* ship_from_room(int vnum);
    SHIP_DATA* ship_from_hanger(int vnum);
    SHIP_DATA* ship_from_pilotseat(int vnum);
    SHIP_DATA* ship_from_cockpit(int vnum);
    SHIP_DATA* ship_from_turret(int vnum);
    SHIP_DATA* ship_from_engine(int vnum);
    SHIP_DATA* ship_from_pilot(char* name);
    SHIP_DATA* get_ship_here(char* name, SPACE_DATA* starsystem);
    void showstarsystem(CHAR_DATA * ch, SPACE_DATA * starsystem);
    void update_space(void);
    void recharge_ships(void);
    void move_ships(void);
    void update_bus(void);
    void update_traffic(void);
    bool check_pilot(CHAR_DATA * ch, SHIP_DATA * ship);
    bool is_rental(CHAR_DATA * ch, SHIP_DATA * ship);
    void echo_to_ship(int color, SHIP_DATA* ship, const char* argument);
    void echo_to_cockpit(int color, SHIP_DATA* ship, const char* argument);
    void echo_to_system(int color, SHIP_DATA* ship, const char* argument, SHIP_DATA* ignore);
    bool extract_ship(SHIP_DATA * ship);
    bool ship_to_room(SHIP_DATA * ship, int vnum);
    bool ship_to_room2(SHIP_DATA * ship, ROOM_INDEX_DATA * shipto);
    long get_ship_value(SHIP_DATA * ship);
    bool rent_ship(CHAR_DATA * ch, SHIP_DATA * ship);
    void damage_ship(SHIP_DATA * ship, int min, int max);
    void damage_ship_ch(SHIP_DATA * ship, int min, int max, CHAR_DATA* ch);
    void destroy_ship(SHIP_DATA * ship, CHAR_DATA * ch, const char* reason);
    void ship_to_starsystem(SHIP_DATA * ship, SPACE_DATA * starsystem);
    void ship_from_starsystem(SHIP_DATA * ship, SPACE_DATA * starsystem);
    void new_missile(SHIP_DATA * ship, SHIP_DATA * target, CHAR_DATA * ch, int missiletype);
    void extract_missile(MISSILE_DATA * missile);
    SHIP_DATA* ship_in_room(ROOM_INDEX_DATA * room, char* name);
    void write_ship_list(void);
    void resetship(SHIP_DATA * ship);

    /* morespace.c */
    SHIP_PROTOTYPE* get_ship_prototype(char* name);
    void load_prototypes(void);
    void save_ship_protoype(SHIP_PROTOTYPE * prototype);
    long int get_prototype_value(SHIP_PROTOTYPE * prototype);
    void create_ship_rooms(SHIP_DATA * ship);

    /* comm.c */
    const char* PERS(CHAR_DATA * ch, CHAR_DATA * looker);
    FELLOW_DATA* knowsof(CHAR_DATA * ch, CHAR_DATA * victim);
    void close_socket(DESCRIPTOR_DATA * dclose, bool force);
    void close_socket(DESCRIPTOR_DATA * dclose, bool force);
    void write_to_buffer(DESCRIPTOR_DATA * d, std::string_view string);
    void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, std::string_view string);
    void write_to_buffer(DESCRIPTOR_DATA * d, const char* txt, size_t length);
    void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, const char* txt, size_t length);
    void write_to_pager(DESCRIPTOR_DATA * d, const char* txt, size_t length);
    void send_to_char(const char* txt, CHAR_DATA* ch);
    void send_to_char_color(const char* txt, CHAR_DATA* ch);
    void send_to_desc_color(const char* txt, DESCRIPTOR_DATA* d);
    void send_to_desc_color2(const char* txt, DESCRIPTOR_DATA* d);
    void send_to_desc_color2(const char* txt, std::shared_ptr<DESCRIPTOR_DATA> d);
    void send_to_char_noand(const char* txt, CHAR_DATA* ch);
    void send_to_pager(const char* txt, CHAR_DATA* ch);
    void send_to_pager_color(const char* txt, CHAR_DATA* ch);
    void set_char_color(sh_int AType, CHAR_DATA * ch);
    void set_pager_color(sh_int AType, CHAR_DATA * ch);
    void ch_printf(CHAR_DATA * ch, const char* fmt, ...);
    char* chrmax(char* src, int length);
    int strlen_color(char* argument);
    char* format_str(char* str, int len);
    void pager_printf(CHAR_DATA * ch, const char* fmt, ...);
    void log_printf(char* fmt, ...);
    void copyover_recover(void);

    void act(sh_int AType, const char* format, CHAR_DATA* ch, const void* arg1, const void* arg2, int type);

    /* reset.c */
    void wipe_resets(ROOM_INDEX_DATA * room);
    RESET_DATA* make_reset(char letter, int extra, int arg1, int arg2, int arg3);
    RESET_DATA* add_reset(ROOM_INDEX_DATA * room, char letter, int extra, int arg1, int arg2, int arg3);
    void reset_area(AREA_DATA * pArea);
    void instaroom(ROOM_INDEX_DATA * pRoom, bool dodoors);

    // swskills.c
    void add_reinforcements(CHAR_DATA * ch);

    /* db.c */
    void show_file(CHAR_DATA * ch, char const* filename);
    char* str_dup(const char* str);
    std::string centertext(const std::string_view& text, int size);
    void boot_db(bool fCopyOver);
    void area_update(void);
    void add_char(CHAR_DATA * ch);
    CHAR_DATA* create_mobile(MOB_INDEX_DATA * pMobIndex);
    OBJ_DATA* create_object(OBJ_INDEX_DATA * pObjIndex, int level);
    void clear_char(CHAR_DATA * ch);
    void free_char(CHAR_DATA * ch);
    char* get_extra_descr(const char* name, EXTRA_DESCR_DATA* ed);
    MOB_INDEX_DATA* get_mob_index(int vnum);
    OBJ_INDEX_DATA* get_obj_index(int vnum);
    ROOM_INDEX_DATA* get_room_index(int vnum);
    char fread_letter(FILE * fp);
    int fread_number(FILE * fp);
    char* fread_string(FILE * fp);
    char* fread_string_nohash(FILE * fp);
    void fread_to_eol(FILE * fp);
    char* fread_word(FILE * fp);
    char* fread_line(FILE * fp);
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
    void append_file(CHAR_DATA * ch, const char* file, char* str);
    void append_to_file(const char* file, char* str);
    void prepend_to_file(const char* file, char* str);
    void bug(const char* str, ...);
    void log_string_plus(const char* str, sh_int log_type, sh_int level);
    void log(const char* str, sh_int log_type, sh_int level, ...);
    ROOM_INDEX_DATA* make_room(int vnum, AREA_DATA* area);
    ROOM_INDEX_DATA* make_ship_room(SHIP_DATA * ship, int vnum);
    OBJ_INDEX_DATA* make_object(int vnum, int cvnum, char* name);
    MOB_INDEX_DATA* make_mobile(int vnum, int cvnum, char* name);
    EXIT_DATA* make_exit(ROOM_INDEX_DATA * pRoomIndex, ROOM_INDEX_DATA * to_room, sh_int door);
    void add_help(HELP_DATA * pHelp);
    void fix_area_exits(AREA_DATA * tarea);
    void load_area_file(AREA_DATA * tarea, char* filename);
    void randomize_exits(ROOM_INDEX_DATA * room, sh_int maxdir);
    void make_wizlist(void);
    void delete_room(ROOM_INDEX_DATA * room);
    void delete_obj(OBJ_INDEX_DATA * obj);
    void delete_mob(MOB_INDEX_DATA * mob);
    /* Functions to add to sorting lists. -- Altrag */
    /*void	mob_sort	 ( MOB_INDEX_DATA *pMob ) ;
    void	obj_sort	 ( OBJ_INDEX_DATA *pObj ) ;
    void	room_sort	 ( ROOM_INDEX_DATA *pRoom ) ;*/
    void sort_area(AREA_DATA * pArea, bool proto);
    float fread_float(FILE * fp);

    /* build.c */
    bool can_rmodify(CHAR_DATA * ch, ROOM_INDEX_DATA * room);
    bool can_omodify(CHAR_DATA * ch, OBJ_DATA * obj);
    bool can_mmodify(CHAR_DATA * ch, CHAR_DATA * mob);
    bool can_medit(CHAR_DATA * ch, MOB_INDEX_DATA * mob);
    void free_reset(AREA_DATA * are, RESET_DATA * res);
    void free_area(AREA_DATA * are);
    void assign_area(CHAR_DATA * ch);
    EXTRA_DESCR_DATA* SetRExtra(ROOM_INDEX_DATA * room, const char* keywords);
    bool DelRExtra(ROOM_INDEX_DATA * room, const char* keywords);
    EXTRA_DESCR_DATA* SetOExtra(OBJ_DATA * obj, const char* keywords);
    bool DelOExtra(OBJ_DATA * obj, const char* keywords);
    EXTRA_DESCR_DATA* SetOExtraProto(OBJ_INDEX_DATA * obj, const char* keywords);
    bool DelOExtraProto(OBJ_INDEX_DATA * obj, const char* keywords);
    void fold_area(AREA_DATA * tarea, char* filename, bool install);
    int get_otype(const char* type);
    int get_atype(const char* type);
    int get_aflag(const char* flag);
    int get_oflag(const char* flag);
    int get_wflag(const char* flag);

    /* fight.c */
    int max_fight(CHAR_DATA * ch);
    void violence_update(void);
    ch_ret multi_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt);
    sh_int ris_damage(CHAR_DATA * ch, sh_int dam, int ris);
    ch_ret damage(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt);
    void update_pos(CHAR_DATA * victim);
    void set_fighting(CHAR_DATA * ch, CHAR_DATA * victim);
    void stop_fighting(CHAR_DATA * ch, bool fBoth);
    void free_fight(CHAR_DATA * ch);
    CHAR_DATA* who_fighting(CHAR_DATA * ch);
    void check_killer(CHAR_DATA * ch, CHAR_DATA * victim);
    void check_attacker(CHAR_DATA * ch, CHAR_DATA * victim);
    void death_cry(CHAR_DATA * ch);
    void stop_hunting(CHAR_DATA * ch);
    void stop_hating(CHAR_DATA * ch);
    void stop_fearing(CHAR_DATA * ch);
    void start_hunting(CHAR_DATA * ch, CHAR_DATA * victim);
    void start_hating(CHAR_DATA * ch, CHAR_DATA * victim);
    void start_fearing(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_hunting(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_hating(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_fearing(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_safe(CHAR_DATA * ch, CHAR_DATA * victim);
    bool is_safe_nm(CHAR_DATA * ch, CHAR_DATA * victim);
    bool legal_loot(CHAR_DATA * ch, CHAR_DATA * victim);
    bool check_illegal_pk(CHAR_DATA * ch, CHAR_DATA * victim);
    void raw_kill(CHAR_DATA * ch, CHAR_DATA * victim);
    bool in_arena(CHAR_DATA * ch);

    /* makeobjs.c */
    void make_corpse(CHAR_DATA * ch, char* killer);
    void make_blood(CHAR_DATA * ch);
    void make_bloodstain(CHAR_DATA * ch);
    void make_scraps(OBJ_DATA * obj);
    void make_fire(ROOM_INDEX_DATA * in_room, sh_int timer);
    OBJ_DATA* make_trap(int v0, int v1, int v2, int v3);
    OBJ_DATA* create_money(int amount);

    /* misc.c */
    void actiondesc(CHAR_DATA * ch, OBJ_DATA * obj, void* vo);
    void jedi_checks(CHAR_DATA * ch);
    void jedi_bonus(CHAR_DATA * ch);
    void sith_penalty(CHAR_DATA * ch);

    /* mud_comm.c */
    const char* mprog_type_to_name(int type);

/* mud_prog.c */
#ifdef DUNNO_STRSTR
    char* strstr(const char* s1, const char* s2);
#endif

    void mprog_wordlist_check(char* arg, CHAR_DATA* mob, CHAR_DATA* actor, OBJ_DATA* object, void* vo, int type);
    void mprog_percent_check(CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void* vo, int type);
    void mprog_act_trigger(char* buf, CHAR_DATA* mob, CHAR_DATA* ch, OBJ_DATA* obj, void* vo);
    void mprog_bribe_trigger(CHAR_DATA * mob, CHAR_DATA * ch, int amount);
    void mprog_entry_trigger(CHAR_DATA * mob);
    void mprog_give_trigger(CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj);
    void mprog_greet_trigger(CHAR_DATA * mob);
    void mprog_fight_trigger(CHAR_DATA * mob, CHAR_DATA * ch);
    void mprog_hitprcnt_trigger(CHAR_DATA * mob, CHAR_DATA * ch);
    void mprog_death_trigger(CHAR_DATA * killer, CHAR_DATA * mob);
    void mprog_random_trigger(CHAR_DATA * mob);
    void mprog_speech_trigger(char* txt, CHAR_DATA* mob);
    void mprog_script_trigger(CHAR_DATA * mob);
    void mprog_hour_trigger(CHAR_DATA * mob);
    void mprog_time_trigger(CHAR_DATA * mob);
    void progbug(const char* str, CHAR_DATA* mob);
    void rset_supermob(ROOM_INDEX_DATA * room);
    void release_supermob();
    void mpsleep_update();

    /* player.c */
    void set_title(CHAR_DATA * ch, char* title);

    /* skills.c */
    bool check_skill(CHAR_DATA * ch, const char* command, char* argument);
    void learn_from_success(CHAR_DATA * ch, int sn);
    void learn_from_failure(CHAR_DATA * ch, int sn);
    bool check_parry(CHAR_DATA * ch, CHAR_DATA * victim);
    bool check_dodge(CHAR_DATA * ch, CHAR_DATA * victim);
    bool check_grip(CHAR_DATA * ch, CHAR_DATA * victim);
    void disarm(CHAR_DATA * ch, CHAR_DATA * victim);
    void trip(CHAR_DATA * ch, CHAR_DATA * victim);

    /* handler.c */
    void explode(OBJ_DATA * obj);
    int get_exp(CHAR_DATA * ch, int ability);
    int get_exp_worth(CHAR_DATA * ch);
    int exp_level(sh_int level);
    sh_int get_trust(CHAR_DATA * ch);
    sh_int get_age(CHAR_DATA * ch);
    sh_int get_curr_str(CHAR_DATA * ch);
    sh_int get_curr_int(CHAR_DATA * ch);
    sh_int get_curr_wis(CHAR_DATA * ch);
    sh_int get_curr_dex(CHAR_DATA * ch);
    int count_users(OBJ_DATA * obj);
    int max_weight(OBJ_DATA * obj);

    sh_int get_curr_con(CHAR_DATA * ch);
    sh_int get_curr_cha(CHAR_DATA * ch);
    sh_int get_curr_lck(CHAR_DATA * ch);
    sh_int get_curr_frc(CHAR_DATA * ch);
    bool can_take_proto(CHAR_DATA * ch);
    int can_carry_n(CHAR_DATA * ch);
    int can_carry_w(CHAR_DATA * ch);
    bool is_name(const char* str, const char* namelist);
    bool is_name_prefix(const char* str, const char* namelist);
    bool nifty_is_name(const char* str, const char* namelist);
    bool nifty_is_name_prefix(const char* str, const char* namelist);
    void affect_modify(CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd);
    void affect_to_char(CHAR_DATA * ch, AFFECT_DATA * paf);
    void affect_remove(CHAR_DATA * ch, AFFECT_DATA * paf);
    void affect_strip(CHAR_DATA * ch, int sn);
    bool is_affected(CHAR_DATA * ch, int sn);
    void affect_join(CHAR_DATA * ch, AFFECT_DATA * paf);
    void char_from_room(CHAR_DATA * ch);
    void char_to_room(CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex);
    OBJ_DATA* obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch);
    void obj_from_char(OBJ_DATA * obj);
    int apply_ac(OBJ_DATA * obj, int iWear);
    OBJ_DATA* get_eq_char(CHAR_DATA * ch, int iWear);
    void equip_char(CHAR_DATA * ch, OBJ_DATA * obj, int iWear);
    void unequip_char(CHAR_DATA * ch, OBJ_DATA * obj);
    int count_obj_list(RESET_DATA * pReset, OBJ_INDEX_DATA * pObjIndex, OBJ_DATA * list);
    int count_mob_in_room(MOB_INDEX_DATA * mob, ROOM_INDEX_DATA * list);
    void obj_from_room(OBJ_DATA * obj);
    OBJ_DATA* obj_to_room(OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex);
    OBJ_DATA* obj_to_obj(OBJ_DATA * obj, OBJ_DATA * obj_to);
    void obj_from_obj(OBJ_DATA * obj);
    void extract_obj(OBJ_DATA * obj);
    void extract_exit(ROOM_INDEX_DATA * room, EXIT_DATA * pexit);
    void extract_room(ROOM_INDEX_DATA * room);
    void clean_room(ROOM_INDEX_DATA * room);
    void clean_obj(OBJ_INDEX_DATA * obj);
    void clean_mob(MOB_INDEX_DATA * mob);
    void clean_resets(ROOM_INDEX_DATA * room);
    void extract_char(CHAR_DATA * ch, bool fPull);
    CHAR_DATA* get_char_room(CHAR_DATA * ch, const char* argument);
    CHAR_DATA* get_char_world(CHAR_DATA * ch, const char* argument);
    CHAR_DATA* get_char_world_ooc(CHAR_DATA * ch, const char* argument);
    CHAR_DATA* get_char_from_comfreq(CHAR_DATA * ch, const char* argument);
    OBJ_DATA* get_obj_type(OBJ_INDEX_DATA * pObjIndexData);
    OBJ_DATA* get_obj_list(CHAR_DATA * ch, const char* argument, OBJ_DATA* list);
    OBJ_DATA* get_obj_list_rev(CHAR_DATA * ch, const char* argument, OBJ_DATA* list);
    OBJ_DATA* get_obj_carry(CHAR_DATA * ch, const char* argument);
    OBJ_DATA* get_obj_wear(CHAR_DATA * ch, const char* argument);
    OBJ_DATA* get_obj_here(CHAR_DATA * ch, const char* argument);
    OBJ_DATA* get_obj_world(CHAR_DATA * ch, const char* argument);
    int get_obj_number(OBJ_DATA * obj);
    int get_obj_weight(OBJ_DATA * obj);
    bool room_is_dark(ROOM_INDEX_DATA * pRoomIndex);
    bool room_is_private(CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex);
    bool can_see(CHAR_DATA * ch, CHAR_DATA * victim);
    bool can_see_obj(CHAR_DATA * ch, OBJ_DATA * obj);
    bool can_drop_obj(CHAR_DATA * ch, OBJ_DATA * obj);
    std::string item_type_name(OBJ_DATA * obj);
    std::string affect_loc_name(int location);
    std::string affect_bit_name(int vector);
    std::string extra_bit_name(int extra_flags);
    std::string magic_bit_name(int magic_flags);
    ch_ret check_for_trap(CHAR_DATA * ch, OBJ_DATA * obj, int flag);
    ch_ret check_room_for_traps(CHAR_DATA * ch, int flag);
    bool is_trapped(OBJ_DATA * obj);
    OBJ_DATA* get_trap(OBJ_DATA * obj);
    ch_ret spring_trap(CHAR_DATA * ch, OBJ_DATA * obj);
    void name_stamp_stats(CHAR_DATA * ch);
    void fix_char(CHAR_DATA * ch);
    void showaffect(CHAR_DATA * ch, AFFECT_DATA * paf);
    void set_cur_obj(OBJ_DATA * obj);
    bool obj_extracted(OBJ_DATA * obj);
    void queue_extracted_obj(OBJ_DATA * obj);
    void clean_obj_queue(void);
    void set_cur_char(CHAR_DATA * ch);
    bool char_died(CHAR_DATA * ch);
    void queue_extracted_char(CHAR_DATA * ch, bool extract);
    void clean_char_queue(void);
    void add_timer(CHAR_DATA * ch, sh_int type, sh_int count, DO_FUN * fun, int value);
    TIMER* get_timerptr(CHAR_DATA * ch, sh_int type);
    sh_int get_timer(CHAR_DATA * ch, sh_int type);
    void extract_timer(CHAR_DATA * ch, TIMER * timer);
    void remove_timer(CHAR_DATA * ch, sh_int type);
    bool in_soft_range(CHAR_DATA * ch, AREA_DATA * tarea);
    bool in_hard_range(CHAR_DATA * ch, AREA_DATA * tarea);
    bool chance(CHAR_DATA * ch, sh_int percent);
    bool chance_attrib(CHAR_DATA * ch, sh_int percent, sh_int attrib);
    OBJ_DATA* clone_object(OBJ_DATA * obj);
    void split_obj(OBJ_DATA * obj, int num);
    void separate_obj(OBJ_DATA * obj);
    bool empty_obj(OBJ_DATA * obj, OBJ_DATA * destobj, ROOM_INDEX_DATA * destroom);
    OBJ_DATA* find_obj(CHAR_DATA * ch, char* argument, bool carryonly);
    bool ms_find_obj(CHAR_DATA * ch);
    void worsen_mental_state(CHAR_DATA * ch, int mod);
    void better_mental_state(CHAR_DATA * ch, int mod);
    void boost_economy(AREA_DATA * tarea, int gold);
    void lower_economy(AREA_DATA * tarea, int gold);
    void economize_mobgold(CHAR_DATA * mob);
    bool economy_has(AREA_DATA * tarea, int gold);
    void add_kill(CHAR_DATA * ch, CHAR_DATA * mob);
    int times_killed(CHAR_DATA * ch, CHAR_DATA * mob);

    /* interp.c */
    bool check_pos(CHAR_DATA * ch, sh_int position);
    void interpret(CHAR_DATA * ch, char* argument);
    bool is_number(const char* arg);
    int number_argument(const char* argument, char* arg);
    const char* one_argument(const char* argument, char* arg_first);
    char* one_argument(char* argument, char* arg_first);
    const char* one_argument2(const char* argument, char* arg_first);
    char* one_argument2(char* argument, char* arg_first);
    SOCIALTYPE* find_social(const char* command);
    CMDTYPE* find_command(const char* command);
    void hash_commands();
    void send_timer(TIMERSET * vtime, CHAR_DATA * ch);
    void update_userec(std::chrono::steady_clock::duration time_used, TIMERSET * userec);
    bool check_social(CHAR_DATA * ch, const char* command, const char* argument);

    /* magic.c */
    bool process_spell_components(CHAR_DATA * ch, int sn);
    int ch_slookup(CHAR_DATA * ch, const char* name);
    int find_spell(CHAR_DATA * ch, const char* name, bool know);
    int find_skill(CHAR_DATA * ch, const char* name, bool know);
    int find_weapon(CHAR_DATA * ch, const char* name, bool know);
    int find_tongue(CHAR_DATA * ch, const char* name, bool know);
    int skill_lookup(const char* name);
    int herb_lookup(const char* name);
    int personal_lookup(CHAR_DATA * ch, const char* name);
    int slot_lookup(int slot);
    int bsearch_skill(const char* name, int first, int top);
    int bsearch_skill_exact(const char* name, int first, int top);
    bool saves_poison_death(int level, CHAR_DATA* victim);
    bool saves_wand(int level, CHAR_DATA* victim);
    bool saves_para_petri(int level, CHAR_DATA* victim);
    bool saves_breath(int level, CHAR_DATA* victim);
    bool saves_spell_staff(int level, CHAR_DATA* victim);
    ch_ret obj_cast_spell(int sn, int level, CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj);
    int dice_parse(CHAR_DATA * ch, int level, char* exp);
    SKILL_TYPE* get_skilltype(int sn);

    /* request.c */
    void init_request_pipe(void);
    void check_requests(void);

    /* save.c */
    /* object saving defines for fread/write_obj. -- Altrag */
    constexpr int OS_CARRY = 0;
    constexpr int OS_CORPSE = 1;
    void save_char_obj(CHAR_DATA * ch);
    void save_clone(CHAR_DATA * ch);
    bool load_char_obj(DESCRIPTOR_DATA & d, const char* name, bool preload);
    void set_alarm(long seconds);
    void requip_char(CHAR_DATA * ch);
    void fwrite_obj(CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest, sh_int os_type);
    void fread_obj(CHAR_DATA * ch, FILE * fp, sh_int os_type);
    void de_equip_char(CHAR_DATA * ch);
    void re_equip_char(CHAR_DATA * ch);
    void save_home(CHAR_DATA * ch);
    void write_corpses(CHAR_DATA * ch, char* name);

    /* shops.c */
    CHAR_DATA* find_keeper(CHAR_DATA * ch);

    /* special.c */
    SPEC_FUN* spec_lookup(const char* name);
    const char* lookup_spec(SPEC_FUN * special);

    /* tables.c */
    void read_last_file(CHAR_DATA * ch, int count, char* name);
    void write_last_file(char* entry);
    int get_skill(char* skilltype);
    char* spell_name(SPELL_FUN * spell);
    char* skill_name(DO_FUN * skill);
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
    void found_prey(CHAR_DATA * ch, CHAR_DATA * victim);
    void hunt_victim(CHAR_DATA * ch);

    /* update.c */
    void advance_level(CHAR_DATA * ch, int ability);
    void gain_exp(CHAR_DATA * ch, int gain, int ability);
    void gain_exp2(CHAR_DATA * ch, int gain, int ability);
    void gain_condition(CHAR_DATA * ch, int iCond, int value);
    void update_handler(void);
    void reboot_check(time_t reset);
#if 0
void    reboot_check    (char* arg);
#endif
    void auction_update(void);
    void remove_portal(OBJ_DATA * portal);
    int max_level(CHAR_DATA * ch, int ability);

    /* 11.c */
    std::string beam_name(sh_int type, bool plural);
    std::string primary_beam_name(SHIP_DATA * ship);
    std::string secondary_beam_name(SHIP_DATA * ship);
}
