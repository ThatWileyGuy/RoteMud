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

extern int _filbuf(FILE*);

#if defined(KEY)
#undef KEY
#endif

void init_supermob();

#define KEY(literal, field, value)                                                                                     \
    if (!str_cmp(word, literal))                                                                                       \
    {                                                                                                                  \
        field = value;                                                                                                 \
        fMatch = true;                                                                                                 \
        break;                                                                                                         \
    }

/*
 * Globals.
 */

WIZENT* first_wiz;
WIZENT* last_wiz;

time_t last_restore_all_time = 0;

HELP_DATA* first_help;
HELP_DATA* last_help;

SHOP_DATA* first_shop;
SHOP_DATA* last_shop;

REPAIR_DATA* first_repair;
REPAIR_DATA* last_repair;

TELEPORT_DATA* first_teleport;
TELEPORT_DATA* last_teleport;

BMARKET_DATA* first_market_ship;
BMARKET_DATA* last_market_ship;

OBJ_DATA* extracted_obj_queue;
EXTRACT_CHAR_DATA* extracted_char_queue;

char bug_buf[2 * MAX_INPUT_LENGTH];
CHAR_DATA* first_char;
CHAR_DATA* last_char;
char* help_greeting;
char log_buf[2 * MAX_INPUT_LENGTH];

OBJ_DATA* first_object;
OBJ_DATA* last_object;
TIME_INFO_DATA time_info;
WEATHER_DATA weather_info;

int cur_qobjs;
int cur_qchars;
int nummobsloaded;
int numobjsloaded;
int physicalobjects;

MAP_INDEX_DATA* first_map; /* maps */

AUCTION_DATA* auction; /* auctions */

/* criminals */
sh_int gsn_torture;
sh_int gsn_deception;
sh_int gsn_disguise;
sh_int gsn_beg;
sh_int gsn_pickshiplock;
sh_int gsn_hijack;

sh_int gsn_plantbug;
sh_int gsn_showbugs;
sh_int gsn_silent;
sh_int gsn_retreat;

/* Technicians */
sh_int gsn_makemodule;
sh_int gsn_installmodule;
sh_int gsn_showmodules;
sh_int gsn_removemodule;
sh_int gsn_removebug;
sh_int gsn_shipmaintenance;
sh_int gsn_scanbugs;
sh_int gsn_makejetpack;

/* slicers */
sh_int gsn_spy;
sh_int gsn_makecommsystem;
sh_int gsn_commsystem;
sh_int gsn_codecrack;
sh_int gsn_slicebank;
sh_int gsn_inquire;
sh_int gsn_checkprints;
sh_int gsn_makedatapad;
sh_int gsn_disable;
sh_int gsn_assignpilot;

/* soldiers and officers */
sh_int gsn_battle_command;
sh_int gsn_reinforcements;
sh_int gsn_postguard;
sh_int gsn_mine;
sh_int gsn_first_aid;
sh_int gsn_snipe;
sh_int gsn_throw;

sh_int gsn_addpatrol;
sh_int gsn_eliteguard;
sh_int gsn_specialforces;
sh_int gsn_jail;
sh_int gsn_smalltalk;
sh_int gsn_propeganda;
sh_int gsn_bribe;
sh_int gsn_seduce;
sh_int gsn_masspropeganda;
sh_int gsn_gather_intelligence;

/* pilots and smugglers */
sh_int gsn_smallspace;
sh_int gsn_mediumspace;
sh_int gsn_largespace;
sh_int gsn_chandelle;
sh_int gsn_weaponsystems;
sh_int gsn_navigation;
sh_int gsn_shipsystems;
sh_int gsn_tractorbeams;
sh_int gsn_shipdesign;
sh_int gsn_spacecombat;
sh_int gsn_spacecombat2;
sh_int gsn_spacecombat3;
sh_int gsn_bomb;
sh_int gsn_split_s;
sh_int gsn_truesight;
/* player building skills */
sh_int gsn_makelightsaber;
sh_int gsn_makeduallightsaber;
sh_int gsn_spice_refining;
sh_int gsn_makeblade;
sh_int gsn_makepike;
sh_int gsn_sabotage;
sh_int gsn_makeblaster;
sh_int gsn_makelight;
sh_int gsn_makecomlink;
sh_int gsn_makegrenade;
sh_int gsn_makeshipbomb;
sh_int gsn_makelandmine;
sh_int gsn_makearmor;
sh_int gsn_makeshield;
sh_int gsn_makecontainer;
sh_int gsn_makemissile;
sh_int gsn_gemcutting;
sh_int gsn_makejewelry;
sh_int gsn_repair;
sh_int gsn_shiprepair;
sh_int gsn_makebeacon;
sh_int gsn_makebug;
sh_int gsn_plantbeacon;
sh_int gsn_showbeacons;
sh_int gsn_checkbeacons;
sh_int gsn_nullifybeacons;
sh_int gsn_makebinders;
sh_int gsn_makeempgrenade;
sh_int gsn_makegoggles;
sh_int gsn_barrelroll;
sh_int gsn_juke;

/* weaponry */
sh_int gsn_blasters;
sh_int gsn_bowcasters;
sh_int gsn_force_pikes;
sh_int gsn_lightsabers;
sh_int gsn_vibro_blades;
sh_int gsn_flexible_arms;
sh_int gsn_talonous_arms;
sh_int gsn_bludgeons;
sh_int gsn_shieldwork;
sh_int gsn_launchers;

/* Hunter */
sh_int gsn_ambush;
sh_int gsn_bind;
sh_int gsn_gag;

/* thief */
sh_int gsn_detrap;
sh_int gsn_bargain;
sh_int gsn_backstab;
sh_int gsn_dualstab;
sh_int gsn_circle;
sh_int gsn_dodge;
sh_int gsn_concealment;
sh_int gsn_hide;
sh_int gsn_peek;
sh_int gsn_pick_lock;
sh_int gsn_sneak;
sh_int gsn_steal;
sh_int gsn_gouge;
sh_int gsn_poison_weapon;

/* thief & warrior */
sh_int gsn_disarm;
sh_int gsn_enhanced_damage;
sh_int gsn_kick;
sh_int gsn_parry;
sh_int gsn_reflect;
sh_int gsn_rescue;
sh_int gsn_second_attack;
sh_int gsn_third_attack;
sh_int gsn_fourth_attack;
sh_int gsn_fifth_attack;
sh_int gsn_dual_wield;
sh_int gsn_punch;
sh_int gsn_bash;
sh_int gsn_stun;
sh_int gsn_bashdoor;
sh_int gsn_grip;
sh_int gsn_berserk;
sh_int gsn_hitall;

/* other   */
sh_int gsn_aid;
sh_int gsn_track;
sh_int gsn_search;
sh_int gsn_dig;
sh_int gsn_mount;
sh_int gsn_bite;
sh_int gsn_claw;
sh_int gsn_sting;
sh_int gsn_tail;
sh_int gsn_scribe;
sh_int gsn_brew;
sh_int gsn_climb;
sh_int gsn_scan;
sh_int gsn_slice;

/* spells */
sh_int gsn_aqua_breath;
sh_int gsn_blindness;
sh_int gsn_charm_person;
sh_int gsn_curse;
sh_int gsn_invis;
sh_int gsn_mass_invis;
sh_int gsn_poison;
sh_int gsn_sleep;
sh_int gsn_possess;
sh_int gsn_fireball;
sh_int gsn_chill_touch;
sh_int gsn_lightning_bolt;

/* languages */
sh_int gsn_basic;
sh_int gsn_wookiee;
sh_int gsn_twilek;
sh_int gsn_rodian;
sh_int gsn_hutt;
sh_int gsn_mon_calamari;
sh_int gsn_noghri;
sh_int gsn_ewok;
sh_int gsn_ithorian;
sh_int gsn_gotal;
sh_int gsn_devaronian;
sh_int gsn_binary;
sh_int gsn_firrerreo;
sh_int gsn_gamorrean;
sh_int gsn_togorian;
sh_int gsn_shistavanen;
sh_int gsn_jawa;
sh_int gsn_kubaz;
sh_int gsn_adarian;
sh_int gsn_verpine;
sh_int gsn_defel;
sh_int gsn_trandoshan;
sh_int gsn_hapan;
sh_int gsn_quarren;
sh_int gsn_sullustan;
sh_int gsn_falleen;
sh_int gsn_barabel;
sh_int gsn_yevethan;
sh_int gsn_gand;
sh_int gsn_coynite;
sh_int gsn_duinuogwuin;
sh_int gsn_droid;

/* for searching */
sh_int gsn_first_spell;
sh_int gsn_first_skill;
sh_int gsn_first_weapon;
sh_int gsn_first_tongue;
sh_int gsn_top_sn;

bool MOBtrigger;

/*
 * Locals.
 */
std::unordered_map<int, MOB_INDEX_DATA*> g_mobIndex;
std::unordered_map<int, OBJ_INDEX_DATA*> g_objectIndex;
std::unordered_map<int, ROOM_INDEX_DATA*> g_roomIndex;

AREA_DATA* first_area;
AREA_DATA* last_area;
AREA_DATA* first_build;
AREA_DATA* last_build;
AREA_DATA* first_asort;
AREA_DATA* last_asort;
AREA_DATA* first_bsort;
AREA_DATA* last_bsort;

SYSTEM_DATA sysdata;

int top_affect;
int top_area;
int top_ed;
int top_exit;
int top_help;
int top_reset;
int top_shop;
int top_repair;

/*
 * Semi-locals.
 */
bool fBootDb;
FILE* fpArea;
char strArea[MAX_INPUT_LENGTH];
char bname[MAX_STRING_LENGTH];

/*
 * Local booting procedures.
 */
void init_mm(void);

void boot_log(const char* str, ...);
void load_area(FILE* fp);
void load_author(AREA_DATA* tarea, FILE* fp);
void load_economy(AREA_DATA* tarea, FILE* fp);
void load_resetmsg(AREA_DATA* tarea, FILE* fp); /* Rennard */
void load_flags(AREA_DATA* tarea, FILE* fp);
void load_helps(AREA_DATA* tarea, FILE* fp);
void load_mobiles(AREA_DATA* tarea, FILE* fp);
void load_objects(AREA_DATA* tarea, FILE* fp);
void load_resets(AREA_DATA* tarea, FILE* fp);
void load_rooms(AREA_DATA* tarea, FILE* fp);
void load_shops(AREA_DATA* tarea, FILE* fp);
void load_repairs(AREA_DATA* tarea, FILE* fp);
void load_specials(AREA_DATA* tarea, FILE* fp);
void load_ranges(AREA_DATA* tarea, FILE* fp);
void load_buildlist(void);
bool load_systemdata(SYSTEM_DATA* sys);
void load_banlist(void);
void initialize_economy(void);

void fix_exits(void);

/*
 * External booting function
 */
void load_corpses(void);
void renumber_put_resets(ROOM_INDEX_DATA* room);
void wipe_resets(ROOM_INDEX_DATA* room);

/*
 * MUDprogram locals
 */

int mprog_name_to_type(char* name);
MPROG_DATA* mprog_file_read(char* f, MPROG_DATA* mprg, MOB_INDEX_DATA* pMobIndex);
/* int 		oprog_name_to_type	( char* name ); */
MPROG_DATA* oprog_file_read(char* f, MPROG_DATA* mprg, OBJ_INDEX_DATA* pObjIndex);
/* int 		rprog_name_to_type	( char* name ); */
MPROG_DATA* rprog_file_read(char* f, MPROG_DATA* mprg, ROOM_INDEX_DATA* pRoomIndex);
void load_mudprogs(AREA_DATA* tarea, FILE* fp);
void load_objprogs(AREA_DATA* tarea, FILE* fp);
void load_roomprogs(AREA_DATA* tarea, FILE* fp);
void mprog_read_programs(FILE* fp, MOB_INDEX_DATA* pMobIndex);
void oprog_read_programs(FILE* fp, OBJ_INDEX_DATA* pObjIndex);
void rprog_read_programs(FILE* fp, ROOM_INDEX_DATA* pRoomIndex);

void shutdown_mud(char const* reason)
{
    FILE* fp;

    if ((fp = fopen(SHUTDOWN_FILE, "a")) != nullptr)
    {
        fprintf(fp, "%s\n", reason);
        fclose(fp);
    }
}

/*
 * Big mama top level function.
 */
void boot_db(bool fCopyOver)
{
    sh_int wear, x;

    show_hash(32);
    unlink(BOOTLOG_FILE);
    boot_log("---------------------[ Boot Log ]--------------------");

    log_string("Loading commands");
    load_commands();

    log_string("Loading sysdata configuration...");

    /* default values */
    sysdata.read_all_mail = LEVEL_DEMI;
    sysdata.read_mail_free = LEVEL_IMMORTAL;
    sysdata.write_mail_free = LEVEL_IMMORTAL;
    sysdata.take_others_mail = LEVEL_DEMI;
    sysdata.muse_level = LEVEL_DEMI;
    sysdata.think_level = LEVEL_HIGOD;
    sysdata.build_level = LEVEL_DEMI;
    sysdata.log_level = LEVEL_LOG;
    sysdata.level_modify_proto = LEVEL_LESSER;
    sysdata.level_override_private = LEVEL_GREATER;
    sysdata.level_mset_player = LEVEL_LESSER;
    sysdata.stun_plr_vs_plr = 15;
    sysdata.stun_regular = 15;
    sysdata.dam_plr_vs_plr = 100;
    sysdata.dam_plr_vs_mob = 100;
    sysdata.dam_mob_vs_plr = 100;
    sysdata.dam_mob_vs_mob = 100;
    sysdata.level_getobjnotake = LEVEL_GREATER;
    sysdata.save_frequency = 20; /* minutes */
    sysdata.save_flags =
        SV_DEATH | SV_PASSCHG | SV_AUTO | SV_PUT | SV_DROP | SV_GIVE | SV_AUCTION | SV_ZAPDROP | SV_IDLE;
    if (!load_systemdata(&sysdata))
    {
        log_string("Not found.  Creating new configuration.");
        sysdata.alltimemax = 0;
    }

    log_string("Loading socials");
    load_socials();

    load_skill_table();
    sort_skill_table();

    gsn_first_spell = 0;
    gsn_first_skill = 0;
    gsn_first_weapon = 0;
    gsn_first_tongue = 0;
    gsn_top_sn = top_sn;

    for (x = 0; x < top_sn; x++)
        if (!gsn_first_spell && skill_table[x]->type == SKILL_SPELL)
            gsn_first_spell = x;
        else if (!gsn_first_skill && skill_table[x]->type == SKILL_SKILL)
            gsn_first_skill = x;
        else if (!gsn_first_weapon && skill_table[x]->type == SKILL_WEAPON)
            gsn_first_weapon = x;
        else if (!gsn_first_tongue && skill_table[x]->type == SKILL_TONGUE)
            gsn_first_tongue = x;

    log_string("Loading herb table");
    load_herb_table();

    log_string("Making wizlist");
    make_wizlist();

    log_string("Loading Changes List");
    load_changes();

    load_force_skills();
    load_force_help();
    load_ship_prototypes();

    fBootDb = true;

    nummobsloaded = 0;
    numobjsloaded = 0;
    physicalobjects = 0;
    sysdata.maxplayers = 0;
    first_object = nullptr;
    last_object = nullptr;
    first_char = nullptr;
    last_char = nullptr;
    first_area = nullptr;
    last_area = nullptr;
    first_build = nullptr;
    last_area = nullptr;
    first_shop = nullptr;
    last_shop = nullptr;
    first_repair = nullptr;
    last_repair = nullptr;
    first_teleport = nullptr;
    last_teleport = nullptr;
    first_asort = nullptr;
    last_asort = nullptr;
    extracted_obj_queue = nullptr;
    extracted_char_queue = nullptr;
    cur_qobjs = 0;
    cur_qchars = 0;
    cur_char = nullptr;
    cur_obj = 0;
    cur_obj_serial = 0;
    cur_char_died = false;
    cur_obj_extracted = false;
    cur_room = nullptr;
    quitting_char = nullptr;
    loading_char = nullptr;
    saving_char = nullptr;
    CREATE(auction, AUCTION_DATA, 1);
    auction->item = nullptr;
    for (wear = 0; wear < MAX_WEAR; wear++)
        for (x = 0; x < MAX_LAYERS; x++)
            save_equipment[wear][x] = nullptr;

    /*
     * Init random number generator.
     */
    log_string("Initializing random number generator");
    init_mm();

    /*
     * Set time and weather.
     */
    {
        long lhour, lday, lmonth;

        log_string("Setting time and weather");

        lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
        time_info.hour = lhour % 24;
        lday = lhour / 24;
        time_info.day = lday % 35;
        lmonth = lday / 35;
        time_info.month = lmonth % 17;
        time_info.year = lmonth / 17;

        if (time_info.hour < 5)
            weather_info.sunlight = SUN_DARK;
        else if (time_info.hour < 6)
            weather_info.sunlight = SUN_RISE;
        else if (time_info.hour < 19)
            weather_info.sunlight = SUN_LIGHT;
        else if (time_info.hour < 20)
            weather_info.sunlight = SUN_SET;
        else
            weather_info.sunlight = SUN_DARK;

        weather_info.change = 0;
        weather_info.mmhg = 960;
        if (time_info.month >= 7 && time_info.month <= 12)
            weather_info.mmhg += number_range(1, 50);
        else
            weather_info.mmhg += number_range(1, 80);

        if (weather_info.mmhg <= 980)
            weather_info.sky = SKY_LIGHTNING;
        else if (weather_info.mmhg <= 1000)
            weather_info.sky = SKY_RAINING;
        else if (weather_info.mmhg <= 1020)
            weather_info.sky = SKY_CLOUDY;
        else
            weather_info.sky = SKY_CLOUDLESS;
    }

    /*
     * Assign gsn's for skills which need them.
     */
    {
        log_string("Assigning gsn's");
        ASSIGN_GSN(gsn_split_s, "split_s");
        ASSIGN_GSN(gsn_eliteguard, "elite_guard");
        ASSIGN_GSN(gsn_addpatrol, "add_patrol");
        ASSIGN_GSN(gsn_gather_intelligence, "gather_intelligence");
        ASSIGN_GSN(gsn_specialforces, "special_forces");
        ASSIGN_GSN(gsn_jail, "jail");
        ASSIGN_GSN(gsn_smalltalk, "smalltalk");
        ASSIGN_GSN(gsn_chandelle, "chandelle");
        ASSIGN_GSN(gsn_propeganda, "propeganda");
        ASSIGN_GSN(gsn_bribe, "bribe");
        ASSIGN_GSN(gsn_seduce, "seduce");
        ASSIGN_GSN(gsn_masspropeganda, "mass_propeganda");
        ASSIGN_GSN(gsn_beg, "beg");
        ASSIGN_GSN(gsn_hijack, "hijack");
        ASSIGN_GSN(gsn_makejewelry, "makejewelry");
        ASSIGN_GSN(gsn_makeblade, "makeblade");
        ASSIGN_GSN(gsn_sabotage, "sabotage");
        ASSIGN_GSN(gsn_makeblaster, "makeblaster");
        ASSIGN_GSN(gsn_makelight, "makeflashlight");
        ASSIGN_GSN(gsn_makecomlink, "makecomlink");
        ASSIGN_GSN(gsn_makegrenade, "makegrenade");
        ASSIGN_GSN(gsn_makeshipbomb, "makeshipbomb");
        ASSIGN_GSN(gsn_makelandmine, "makelandmine");
        ASSIGN_GSN(gsn_makearmor, "makearmor");
        ASSIGN_GSN(gsn_makepike, "makepike");
        ASSIGN_GSN(gsn_makeshield, "makeshield");
        ASSIGN_GSN(gsn_makecontainer, "makecontainer");
        ASSIGN_GSN(gsn_makemissile, "makemissile");
        ASSIGN_GSN(gsn_gemcutting, "gemcutting");
        ASSIGN_GSN(gsn_battle_command, "battle_command");
        ASSIGN_GSN(gsn_reinforcements, "reinforcements");
        ASSIGN_GSN(gsn_postguard, "post guard");
        ASSIGN_GSN(gsn_smallspace, "small spacecraft");
        ASSIGN_GSN(gsn_mediumspace, "medium spacecraft");
        ASSIGN_GSN(gsn_largespace, "large spacecraft");
        ASSIGN_GSN(gsn_torture, "torture");
        ASSIGN_GSN(gsn_throw, "throw");
        ASSIGN_GSN(gsn_snipe, "snipe");
        ASSIGN_GSN(gsn_deception, "deception");
        ASSIGN_GSN(gsn_disguise, "disguise");
        ASSIGN_GSN(gsn_mine, "mine");
        ASSIGN_GSN(gsn_plantbug, "plantbug");
        ASSIGN_GSN(gsn_showbugs, "showbug");
        ASSIGN_GSN(gsn_truesight, "true sight");
        ASSIGN_GSN(gsn_makemodule, "makemodule");
        ASSIGN_GSN(gsn_installmodule, "installmodule");
        ASSIGN_GSN(gsn_showmodules, "showmodules");
        ASSIGN_GSN(gsn_removemodule, "removemodule");
        ASSIGN_GSN(gsn_makejetpack, "makejetpack");
        ASSIGN_GSN(gsn_removebug, "removebug");
        ASSIGN_GSN(gsn_spy, "spy");
        ASSIGN_GSN(gsn_makecommsystem, "makecommsystem");
        ASSIGN_GSN(gsn_commsystem, "commsystem");
        ASSIGN_GSN(gsn_codecrack, "codecrack");
        ASSIGN_GSN(gsn_slicebank, "slicebank");
        ASSIGN_GSN(gsn_inquire, "inquire");
        ASSIGN_GSN(gsn_checkprints, "checkprints");
        ASSIGN_GSN(gsn_makedatapad, "makedatapad");
        ASSIGN_GSN(gsn_disable, "disable");
        ASSIGN_GSN(gsn_assignpilot, "assignpilot");
        ASSIGN_GSN(gsn_first_aid, "first aid");
        ASSIGN_GSN(gsn_makelightsaber, "makelightsaber");
        ASSIGN_GSN(gsn_makeduallightsaber, "makeduallightsaber");
        ASSIGN_GSN(gsn_spice_refining, "spice refining");
        ASSIGN_GSN(gsn_spacecombat, "space combat 1");
        ASSIGN_GSN(gsn_spacecombat2, "space combat 2");
        ASSIGN_GSN(gsn_spacecombat3, "space combat 3");
        ASSIGN_GSN(gsn_weaponsystems, "weapon systems");
        ASSIGN_GSN(gsn_bomb, "bomb");
        ASSIGN_GSN(gsn_navigation, "navigation");
        ASSIGN_GSN(gsn_shipsystems, "ship systems");
        ASSIGN_GSN(gsn_tractorbeams, "tractor beams");
        ASSIGN_GSN(gsn_shipmaintenance, "shipmaintenance");
        ASSIGN_GSN(gsn_scanbugs, "checkbugs");
        ASSIGN_GSN(gsn_shipdesign, "ship design");
        ASSIGN_GSN(gsn_blasters, "blasters");
        ASSIGN_GSN(gsn_bowcasters, "bowcasters");
        ASSIGN_GSN(gsn_force_pikes, "force pikes");
        ASSIGN_GSN(gsn_lightsabers, "lightsabers");
        ASSIGN_GSN(gsn_vibro_blades, "vibro-blades");
        ASSIGN_GSN(gsn_flexible_arms, "flexible arms");
        ASSIGN_GSN(gsn_talonous_arms, "talonous arms");
        ASSIGN_GSN(gsn_bludgeons, "bludgeons");
        ASSIGN_GSN(gsn_shieldwork, "shieldwork");
        ASSIGN_GSN(gsn_detrap, "detrap");
        ASSIGN_GSN(gsn_bargain, "bargain");
        ASSIGN_GSN(gsn_backstab, "backstab");
        ASSIGN_GSN(gsn_dualstab, "dualstab");
        ASSIGN_GSN(gsn_ambush, "ambush");
        ASSIGN_GSN(gsn_bind, "bind");
        ASSIGN_GSN(gsn_gag, "gag");
        ASSIGN_GSN(gsn_circle, "circle");
        ASSIGN_GSN(gsn_dodge, "dodge");
        ASSIGN_GSN(gsn_concealment, "concealment");
        ASSIGN_GSN(gsn_hide, "hide");
        ASSIGN_GSN(gsn_peek, "peek");
        ASSIGN_GSN(gsn_pick_lock, "pick lock");
        ASSIGN_GSN(gsn_pickshiplock, "pick ship lock");
        ASSIGN_GSN(gsn_retreat, "retreat");
        ASSIGN_GSN(gsn_silent, "silent");
        ASSIGN_GSN(gsn_sneak, "sneak");
        ASSIGN_GSN(gsn_steal, "steal");
        ASSIGN_GSN(gsn_gouge, "gouge");
        ASSIGN_GSN(gsn_poison_weapon, "poison weapon");
        ASSIGN_GSN(gsn_disarm, "disarm");
        ASSIGN_GSN(gsn_enhanced_damage, "enhanced damage");
        ASSIGN_GSN(gsn_kick, "kick");
        ASSIGN_GSN(gsn_parry, "parry");
        ASSIGN_GSN(gsn_reflect, "reflect");
        ASSIGN_GSN(gsn_rescue, "rescue");
        ASSIGN_GSN(gsn_repair, "repair");
        ASSIGN_GSN(gsn_shiprepair, "shiprepair");
        ASSIGN_GSN(gsn_second_attack, "second attack");
        ASSIGN_GSN(gsn_third_attack, "third attack");
        ASSIGN_GSN(gsn_fourth_attack, "fourth attack");
        ASSIGN_GSN(gsn_fifth_attack, "fifth attack");
        ASSIGN_GSN(gsn_dual_wield, "dual wield");
        ASSIGN_GSN(gsn_punch, "punch");
        ASSIGN_GSN(gsn_bash, "bash");
        ASSIGN_GSN(gsn_stun, "stun");
        ASSIGN_GSN(gsn_bashdoor, "doorbash");
        ASSIGN_GSN(gsn_grip, "grip");
        ASSIGN_GSN(gsn_berserk, "berserk");
        ASSIGN_GSN(gsn_hitall, "hitall");
        ASSIGN_GSN(gsn_aid, "aid");
        ASSIGN_GSN(gsn_track, "track");
        ASSIGN_GSN(gsn_search, "search");
        ASSIGN_GSN(gsn_dig, "dig");
        ASSIGN_GSN(gsn_mount, "mount");
        ASSIGN_GSN(gsn_bite, "bite");
        ASSIGN_GSN(gsn_claw, "claw");
        ASSIGN_GSN(gsn_sting, "sting");
        ASSIGN_GSN(gsn_tail, "tail");
        ASSIGN_GSN(gsn_scribe, "scribe");
        ASSIGN_GSN(gsn_brew, "brew");
        ASSIGN_GSN(gsn_climb, "climb");
        ASSIGN_GSN(gsn_scan, "scan");
        ASSIGN_GSN(gsn_slice, "slice");
        ASSIGN_GSN(gsn_fireball, "fireball");
        ASSIGN_GSN(gsn_chill_touch, "chill touch");
        ASSIGN_GSN(gsn_lightning_bolt, "force bolt");
        ASSIGN_GSN(gsn_aqua_breath, "aqua breath");
        ASSIGN_GSN(gsn_blindness, "blindness");
        ASSIGN_GSN(gsn_charm_person, "affect mind");
        ASSIGN_GSN(gsn_curse, "curse");
        ASSIGN_GSN(gsn_invis, "mask");
        ASSIGN_GSN(gsn_mass_invis, "group masking");
        ASSIGN_GSN(gsn_poison, "poison");
        ASSIGN_GSN(gsn_sleep, "sleep");
        ASSIGN_GSN(gsn_possess, "possess");
        ASSIGN_GSN(gsn_basic, "basic");
        ASSIGN_GSN(gsn_wookiee, "wookiee");
        ASSIGN_GSN(gsn_twilek, "twilek");
        ASSIGN_GSN(gsn_rodian, "rodian");
        ASSIGN_GSN(gsn_hutt, "hutt");
        ASSIGN_GSN(gsn_mon_calamari, "mon calamari");
        ASSIGN_GSN(gsn_noghri, "noghri");
        ASSIGN_GSN(gsn_ewok, "ewok");
        ASSIGN_GSN(gsn_ithorian, "ithorian");
        ASSIGN_GSN(gsn_gotal, "gotal");
        ASSIGN_GSN(gsn_devaronian, "devaronian");
        ASSIGN_GSN(gsn_binary, "binary");
        ASSIGN_GSN(gsn_firrerreo, "firrerreo");
        ASSIGN_GSN(gsn_gamorrean, "gamorrean");
        ASSIGN_GSN(gsn_togorian, "togorian");
        ASSIGN_GSN(gsn_shistavanen, "shistavanen");
        ASSIGN_GSN(gsn_jawa, "jawa");
        ASSIGN_GSN(gsn_kubaz, "kubaz");
        ASSIGN_GSN(gsn_adarian, "asdarian");
        ASSIGN_GSN(gsn_verpine, "verpine");
        ASSIGN_GSN(gsn_defel, "defel");
        ASSIGN_GSN(gsn_trandoshan, "trandoshan");
        ASSIGN_GSN(gsn_hapan, "hapan");
        ASSIGN_GSN(gsn_quarren, "quarren");
        ASSIGN_GSN(gsn_sullustan, "sullustan");
        ASSIGN_GSN(gsn_falleen, "falleen");
        ASSIGN_GSN(gsn_barabel, "barabel");
        ASSIGN_GSN(gsn_yevethan, "yevethan");
        ASSIGN_GSN(gsn_gand, "gand");
        ASSIGN_GSN(gsn_coynite, "coynite");
        ASSIGN_GSN(gsn_duinuogwuin, "duinuogwuin");
        ASSIGN_GSN(gsn_droid, "droid");
        ASSIGN_GSN(gsn_makebeacon, "makebeacon");
        ASSIGN_GSN(gsn_makebug, "makebug");
        ASSIGN_GSN(gsn_plantbeacon, "plantbeacon");
        ASSIGN_GSN(gsn_showbeacons, "showbeacons");
        ASSIGN_GSN(gsn_checkbeacons, "checkbeacons");
        ASSIGN_GSN(gsn_nullifybeacons, "nullifybeacons");
        ASSIGN_GSN(gsn_makebinders, "makebinders");
        ASSIGN_GSN(gsn_launchers, "launchers");
        ASSIGN_GSN(gsn_makemissile, "makemissile");
        ASSIGN_GSN(gsn_makeempgrenade, "makeempgrenade");
        ASSIGN_GSN(gsn_makegoggles, "makegoggles");
        ASSIGN_GSN(gsn_barrelroll, "barrel roll");
        ASSIGN_GSN(gsn_juke, "juke");
    }

    /*
     * Read in all the area files.
     */
    {
        FILE* fpList;

        log_string("Reading in area files...");
        if ((fpList = fopen(AREA_DIR AREA_LIST, "r")) == nullptr)
        {
            shutdown_mud("Unable to open area list");
            exit(1);
        }

        for (;;)
        {
            strcpy_s(strArea, fread_word(fpList));
            if (strArea[0] == '$')
                break;

            load_area_file(last_area, strArea);
        }
        fclose(fpList);
    }

    /*
     *   initialize supermob.
     *    must be done before reset_area!
     *
     */
    init_supermob();

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     */
    {
        log_string("Fixing exits");
        fix_exits();
        fBootDb = false;
        log_string("Initializing economy");
        initialize_economy();
        log_string("Loading buildlist");
        load_buildlist();
        log_string("Loading boards");
        load_boards();

        load_clans();
        log_string("Loading senate");
        load_senate();
        log_string("Loading bans");
        load_banlist();
        log_string("Loading slay table"); /* Online slay table - Samson 8-3-98 */
        load_slays();
        log_string("Loading corpses");
        load_corpses();
        log_string("Loading space");
        load_space();
        //        log_string( "Loading ship prototypes" );
        //        load_prototypes( );
        log_string("Loading ships");
        load_ships();
        //        log_string( "Place ships" );
        //        placeships( );
        log_string("Loading bounties");
        load_bounties();
        log_string("Loading governments");
        load_planets();
        log_string("Resetting areas");
        area_update();

        // log_string( "Loading black market ships");
        // load_market_list();

        log_string("Making new blackmarket list.");
        make_random_marketlist();

        MOBtrigger = true;
        // TODO copyover is dead
        /*
        if (fCopyOver)
        {
            log_string("Running copyover_recover.");
            copyover_recover();
        }
        */
    }

    /* init_maps ( ); */

    return;
}

/*
 * Load an 'area' header line.
 */
void load_area(FILE* fp)
{
    AREA_DATA* pArea;

    CREATE(pArea, AREA_DATA, 1);
    pArea->first_room = pArea->last_room = nullptr;
    pArea->next_on_planet = nullptr;
    pArea->prev_on_planet = nullptr;
    pArea->planet = nullptr;
    pArea->name = fread_string_nohash(fp);
    pArea->author = STRALLOC("unknown");
    pArea->filename = str_dup(strArea);
    pArea->age = 15;
    pArea->nplayer = 0;
    pArea->low_r_vnum = 0;
    pArea->low_o_vnum = 0;
    pArea->low_m_vnum = 0;
    pArea->hi_r_vnum = 0;
    pArea->hi_o_vnum = 0;
    pArea->hi_m_vnum = 0;
    pArea->low_soft_range = 0;
    pArea->hi_soft_range = MAX_LEVEL;
    pArea->low_hard_range = 0;
    pArea->hi_hard_range = MAX_LEVEL;

    LINK(pArea, first_area, last_area, next, prev);
    top_area++;
    return;
}

/*
 * Load an author section. Scryn 2/1/96
 */
void load_author(AREA_DATA* tarea, FILE* fp)
{
    if (!tarea)
    {
        bug("Load_author: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    if (tarea->author)
        STRFREE(tarea->author);
    tarea->author = fread_string(fp);
    return;
}

/*
 * Load an economy section. Thoric
 */
void load_economy(AREA_DATA* tarea, FILE* fp)
{
    if (!tarea)
    {
        bug("Load_economy: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    tarea->high_economy = fread_number(fp);
    tarea->low_economy = fread_number(fp);
    return;
}

/* Reset Message Load, Rennard */
void load_resetmsg(AREA_DATA* tarea, FILE* fp)
{
    if (!tarea)
    {
        bug("Load_resetmsg: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    if (tarea->resetmsg)
        DISPOSE(tarea->resetmsg);
    tarea->resetmsg = fread_string_nohash(fp);
    return;
}

/*
 * Load area flags. Narn, Mar/96
 */
void load_flags(AREA_DATA* tarea, FILE* fp)
{
    char* ln;
    int x1, x2;

    if (!tarea)
    {
        bug("Load_flags: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }
    ln = fread_line(fp);
    x1 = x2 = 0;
    sscanf(ln, "%d %d", &x1, &x2);
    tarea->flags = x1;
    tarea->reset_frequency = x2;
    if (x2)
        tarea->age = x2;
    return;
}

void free_help(HELP_DATA* pHelp)
{
    if (!fBootDb)
        UNLINK(pHelp, first_help, last_help, next, prev);
    DISPOSE(pHelp->text);
    DISPOSE(pHelp->keyword);
    DISPOSE(pHelp);
    return;
}

/*
 * Adds a help page to the list if it is not a duplicate of an existing page.
 * Page is insert-sorted by keyword.                    -Thoric
 * (The reason for sorting is to keep do_hlist looking nice)
 */
void add_help(HELP_DATA* pHelp)
{
    HELP_DATA* tHelp;
    int match;

    for (tHelp = first_help; tHelp; tHelp = tHelp->next)
        if (pHelp->level == tHelp->level && !str_cmp(pHelp->keyword, tHelp->keyword))
        {
            bug("add_help: duplicate: %s.  Deleting.", pHelp->keyword);
            free_help(pHelp);
            return;
        }
        else if ((match = strcmp(pHelp->keyword[0] == '\'' ? pHelp->keyword + 1 : pHelp->keyword,
                                 tHelp->keyword[0] == '\'' ? tHelp->keyword + 1 : tHelp->keyword)) < 0 ||
                 (match == 0 && pHelp->level > tHelp->level))
        {
            if (!tHelp->prev)
                first_help = pHelp;
            else
                tHelp->prev->next = pHelp;
            pHelp->prev = tHelp->prev;
            pHelp->next = tHelp;
            tHelp->prev = pHelp;
            break;
        }

    if (!tHelp)
        LINK(pHelp, first_help, last_help, next, prev);

    top_help++;
}

/*
 * Load a help section.
 */
void load_helps(AREA_DATA* tarea, FILE* fp)
{
    HELP_DATA* pHelp;

    for (;;)
    {
        CREATE(pHelp, HELP_DATA, 1);
        pHelp->level = fread_number(fp);
        pHelp->keyword = fread_string(fp);
        if (pHelp->keyword[0] == '$')
            break;
        pHelp->text = fread_string(fp);
        if (pHelp->keyword[0] == '\0')
        {
            STRFREE(pHelp->text);
            STRFREE(pHelp->keyword);
            DISPOSE(pHelp);
            continue;
        }

        if (!str_cmp(pHelp->keyword, "greeting"))
            help_greeting = pHelp->text;
        add_help(pHelp);
    }
    return;
}

/*
 * Add a character to the list of all characters		-Thoric
 */
void add_char(CHAR_DATA* ch)
{
    LINK(ch, first_char, last_char, next, prev);
}

/*
 * Load a mob section.
 */
void load_mobiles(AREA_DATA* tarea, FILE* fp)
{
    MOB_INDEX_DATA* pMobIndex;
    char* ln;
    int x1, x2, x3, x4, x5, x6, x7, x8;

    if (!tarea)
    {
        bug("Load_mobiles: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    for (;;)
    {
        char buf[MAX_STRING_LENGTH];
        int vnum;
        char letter;
        int iHash;
        bool oldmob;
        bool tmpBootDb;

        letter = fread_letter(fp);
        if (letter != '#')
        {
            bug("Load_mobiles: # not found.");
            if (fBootDb)
            {
                shutdown_mud("# not found");
                exit(1);
            }
            else
                return;
        }

        vnum = fread_number(fp);
        if (vnum == 0)
            break;

        tmpBootDb = fBootDb;
        fBootDb = false;
        if (get_mob_index(vnum))
        {
            if (tmpBootDb)
            {
                bug("Load_mobiles: vnum %d duplicated.", vnum);
                shutdown_mud("duplicate vnum");
                exit(1);
            }
            else
            {
                pMobIndex = get_mob_index(vnum);
                sprintf_s(buf, "Cleaning mobile: %d", vnum);
                log_string_plus(buf, LOG_BUILD, sysdata.log_level);
                clean_mob(pMobIndex);
                oldmob = true;
            }
        }
        else
        {
            oldmob = false;
            CREATE(pMobIndex, MOB_INDEX_DATA, 1);
        }
        fBootDb = tmpBootDb;

        pMobIndex->vnum = vnum;
        if (fBootDb)
        {
            if (!tarea->low_m_vnum)
                tarea->low_m_vnum = vnum;
            if (vnum > tarea->hi_m_vnum)
                tarea->hi_m_vnum = vnum;
        }
        pMobIndex->player_name = fread_string(fp);
        pMobIndex->short_descr = fread_string(fp);
        pMobIndex->long_descr = fread_string(fp);
        pMobIndex->description = fread_string(fp);

        pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
        pMobIndex->description[0] = UPPER(pMobIndex->description[0]);

        pMobIndex->act = fread_number(fp) | ACT_IS_NPC;
        pMobIndex->affected_by = fread_number(fp);
        pMobIndex->pShop = nullptr;
        pMobIndex->rShop = nullptr;
        pMobIndex->alignment = fread_number(fp);
        letter = fread_letter(fp);
        pMobIndex->level = fread_number(fp);

        pMobIndex->mobthac0 = fread_number(fp);
        pMobIndex->ac = fread_number(fp);
        pMobIndex->hitnodice = fread_number(fp);
        /* 'd'		*/ fread_letter(fp);
        pMobIndex->hitsizedice = fread_number(fp);
        /* '+'		*/ fread_letter(fp);
        pMobIndex->hitplus = fread_number(fp);
        pMobIndex->damnodice = fread_number(fp);
        /* 'd'		*/ fread_letter(fp);
        pMobIndex->damsizedice = fread_number(fp);
        /* '+'		*/ fread_letter(fp);
        pMobIndex->damplus = fread_number(fp);
        pMobIndex->gold = fread_number(fp);
        pMobIndex->exp = fread_number(fp);
        pMobIndex->position = fread_number(fp);
        pMobIndex->defposition = fread_number(fp);

        /*
         * Back to meaningful values.
         */
        pMobIndex->sex = fread_number(fp);

        if (letter != 'S' && letter != 'C' && letter != 'Z')
        {
            bug("Load_mobiles: vnum %d: letter '%c' not Z, S or C.", vnum, letter);
            shutdown_mud("bad mob data");
            exit(1);
        }
        if (letter == 'C' || letter == 'Z') /* Realms complex mob 	-Thoric  */
        {
            pMobIndex->perm_str = fread_number(fp);
            pMobIndex->perm_int = fread_number(fp);
            pMobIndex->perm_wis = fread_number(fp);
            pMobIndex->perm_dex = fread_number(fp);
            pMobIndex->perm_con = fread_number(fp);
            pMobIndex->perm_cha = fread_number(fp);
            pMobIndex->perm_lck = fread_number(fp);
            pMobIndex->saving_poison_death = fread_number(fp);
            pMobIndex->saving_wand = fread_number(fp);
            pMobIndex->saving_para_petri = fread_number(fp);
            pMobIndex->saving_breath = fread_number(fp);
            pMobIndex->saving_spell_staff = fread_number(fp);
            ln = fread_line(fp);
            x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
            sscanf(ln, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);
            pMobIndex->race = x1;
            pMobIndex->height = x3;
            pMobIndex->weight = x4;
            pMobIndex->speaks = x5;
            pMobIndex->speaking = x6;
            pMobIndex->numattacks = x7;
            if (!pMobIndex->speaks)
                pMobIndex->speaks = race_table[pMobIndex->race].language | LANG_BASIC;
            if (!pMobIndex->speaking)
                pMobIndex->speaking = race_table[pMobIndex->race].language;

            ln = fread_line(fp);
            x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = 0;
            sscanf(ln, "%d %d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);
            pMobIndex->hitroll = x1;
            pMobIndex->damroll = x2;
            pMobIndex->xflags = x3;
            pMobIndex->resistant = x4;
            pMobIndex->immune = x5;
            pMobIndex->susceptible = x6;
            pMobIndex->attacks = x7;
            pMobIndex->defenses = x8;
        }
        else
        {
            pMobIndex->perm_str = 10;
            pMobIndex->perm_dex = 10;
            pMobIndex->perm_int = 10;
            pMobIndex->perm_wis = 10;
            pMobIndex->perm_cha = 10;
            pMobIndex->perm_con = 10;
            pMobIndex->perm_lck = 10;
            pMobIndex->race = 0;
            pMobIndex->xflags = 0;
            pMobIndex->resistant = 0;
            pMobIndex->immune = 0;
            pMobIndex->susceptible = 0;
            pMobIndex->numattacks = 0;
            pMobIndex->attacks = 0;
            pMobIndex->defenses = 0;
        }
        if (letter == 'Z') /*  STar Wars Reality Complex Mob  */
        {
            ln = fread_line(fp);
            x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = 0;
            sscanf(ln, "%d %d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);
            pMobIndex->vip_flags = x1;
        }

        letter = fread_letter(fp);
        if (letter == '>')
        {
            ungetc(letter, fp);
            mprog_read_programs(fp, pMobIndex);
        }
        else
            ungetc(letter, fp);

        if (!oldmob)
        {
            g_mobIndex.insert({vnum, pMobIndex});
        }
    }

    return;
}

/*
 * Load an obj section.
 */
void load_objects(AREA_DATA* tarea, FILE* fp)
{
    OBJ_INDEX_DATA* pObjIndex;
    char letter;
    char* ln;
    int x1, x2, x3, x4, x5, x6;

    if (!tarea)
    {
        bug("Load_objects: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    for (;;)
    {
        char buf[MAX_STRING_LENGTH];
        int vnum;
        int iHash;
        bool tmpBootDb;
        bool oldobj;

        letter = fread_letter(fp);
        if (letter != '#')
        {
            bug("Load_objects: # not found.");
            if (fBootDb)
            {
                shutdown_mud("# not found");
                exit(1);
            }
            else
                return;
        }

        vnum = fread_number(fp);
        if (vnum == 0)
            break;

        tmpBootDb = fBootDb;
        fBootDb = false;
        if (get_obj_index(vnum))
        {
            if (tmpBootDb)
            {
                bug("Load_objects: vnum %d duplicated.", vnum);
                shutdown_mud("duplicate vnum");
                exit(1);
            }
            else
            {
                pObjIndex = get_obj_index(vnum);
                sprintf_s(buf, "Cleaning object: %d", vnum);
                log_string_plus(buf, LOG_BUILD, sysdata.log_level);
                clean_obj(pObjIndex);
                oldobj = true;
            }
        }
        else
        {
            oldobj = false;
            CREATE(pObjIndex, OBJ_INDEX_DATA, 1);
        }
        fBootDb = tmpBootDb;

        pObjIndex->vnum = vnum;
        if (fBootDb)
        {
            if (!tarea->low_o_vnum)
                tarea->low_o_vnum = vnum;
            if (vnum > tarea->hi_o_vnum)
                tarea->hi_o_vnum = vnum;
        }
        pObjIndex->name = fread_string(fp);
        pObjIndex->short_descr = fread_string(fp);
        pObjIndex->description = fread_string(fp);
        pObjIndex->action_desc = fread_string(fp);

        /* Commented out by Narn, Apr/96 to allow item short descs like
           Bonecrusher and Oblivion */
        /*pObjIndex->short_descr[0]	= LOWER(pObjIndex->short_descr[0]);*/
        pObjIndex->description[0] = UPPER(pObjIndex->description[0]);

        ln = fread_line(fp);
        x1 = x2 = x3 = x4 = 0;
        sscanf(ln, "%d %d %d %d", &x1, &x2, &x3, &x4);
        pObjIndex->item_type = x1;
        pObjIndex->extra_flags = x2;
        pObjIndex->wear_flags = x3;
        pObjIndex->layers = x4;

        ln = fread_line(fp);
        x1 = x2 = x3 = x4 = x5 = x6 = 0;
        sscanf(ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);
        pObjIndex->value[0] = x1;
        pObjIndex->value[1] = x2;
        pObjIndex->value[2] = x3;
        pObjIndex->value[3] = x4;
        pObjIndex->value[4] = x5;
        pObjIndex->value[5] = x6;
        pObjIndex->weight = fread_number(fp);
        pObjIndex->weight = UMAX(1, pObjIndex->weight);
        pObjIndex->cost = fread_number(fp);
        pObjIndex->rent = fread_number(fp); /* unused */

        for (;;)
        {
            letter = fread_letter(fp);

            if (letter == 'A')
            {
                AFFECT_DATA* paf;

                CREATE(paf, AFFECT_DATA, 1);
                paf->type = -1;
                paf->duration = -1;
                paf->location = fread_number(fp);
                if (paf->location == APPLY_WEAPONSPELL || paf->location == APPLY_WEARSPELL ||
                    paf->location == APPLY_REMOVESPELL || paf->location == APPLY_STRIPSN)
                    paf->modifier = slot_lookup(fread_number(fp));
                else
                    paf->modifier = fread_number(fp);
                paf->bitvector = 0;
                LINK(paf, pObjIndex->first_affect, pObjIndex->last_affect, next, prev);
                top_affect++;
            }

            else if (letter == 'E')
            {
                EXTRA_DESCR_DATA* ed;

                CREATE(ed, EXTRA_DESCR_DATA, 1);
                ed->keyword = fread_string(fp);
                ed->description = fread_string(fp);
                LINK(ed, pObjIndex->first_extradesc, pObjIndex->last_extradesc, next, prev);
                top_ed++;
            }

            else if (letter == '>')
            {
                ungetc(letter, fp);
                oprog_read_programs(fp, pObjIndex);
            }

            else
            {
                ungetc(letter, fp);
                break;
            }
        }

        /*
         * Translate spell "slot numbers" to internal "skill numbers."
         */
        switch (pObjIndex->item_type)
        {
        case ITEM_PILL:
        case ITEM_POTION:
            pObjIndex->value[1] = slot_lookup(pObjIndex->value[1]);
            pObjIndex->value[2] = slot_lookup(pObjIndex->value[2]);
            pObjIndex->value[3] = slot_lookup(pObjIndex->value[3]);
            break;

        case ITEM_DEVICE:
            pObjIndex->value[3] = slot_lookup(pObjIndex->value[3]);
            break;
        case ITEM_SALVE:
            pObjIndex->value[4] = slot_lookup(pObjIndex->value[4]);
            pObjIndex->value[5] = slot_lookup(pObjIndex->value[5]);
            break;
        }

        if (!oldobj)
        {
            assert(!g_objectIndex.contains(vnum));
            g_objectIndex.insert({vnum, pObjIndex});
        }
    }

    return;
}

/*
 * Load a reset section.
 */
void load_resets(AREA_DATA* tarea, FILE* fp)
{
    ROOM_INDEX_DATA* pRoomIndex = nullptr;
    ROOM_INDEX_DATA* roomlist;
    bool not01 = false;
    int count = 0;

    if (!tarea)
    {
        bug("%s", "Load_resets: no #AREA seen yet.");
        if (fBootDb)
        {
            shutdown_mud("No #AREA");
            exit(1);
        }
        else
            return;
    }

    if (!tarea->first_room)
    {
        bug("%s: No #ROOMS section found. Cannot load resets.", __FUNCTION__);
        if (fBootDb)
        {
            shutdown_mud("No #ROOMS");
            exit(1);
        }
        else
            return;
    }

    for (;;)
    {
        EXIT_DATA* pexit;
        char letter;
        int extra, arg1, arg2, arg3;

        if ((letter = fread_letter(fp)) == 'S')
            break;

        if (letter == '*')
        {
            fread_to_eol(fp);
            continue;
        }

        extra = fread_number(fp);
        if (letter == 'M' || letter == 'O')
            extra = 0;
        arg1 = fread_number(fp);
        arg2 = fread_number(fp);
        arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number(fp);
        fread_to_eol(fp);
        ++count;

        /*
         * Validate parameters.
         * We're calling the index functions for the side effect.
         */
        switch (letter)
        {
        default:
            bug("%s: bad command '%c'.", __FUNCTION__, letter);
            if (fBootDb)
                boot_log("%s: %s (%d) bad command '%c'.", __FUNCTION__, tarea->filename, count, letter);
            return;

        case 'M':
            if (get_mob_index(arg1) == nullptr && fBootDb)
                boot_log("%s: %s (%d) 'M': mobile %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1);

            if ((pRoomIndex = get_room_index(arg3)) == nullptr && fBootDb)
                boot_log("%s: %s (%d) 'M': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg3);
            else
                add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            break;

        case 'O':
            if (get_obj_index(arg1) == nullptr && fBootDb)
                boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter,
                         arg1);

            if ((pRoomIndex = get_room_index(arg3)) == nullptr && fBootDb)
                boot_log("%s: %s (%d) '%c': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter,
                         arg3);
            else
            {
                if (!pRoomIndex)
                    bug("%s: Unable to add room reset - room not found.", __FUNCTION__);
                else
                    add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            }
            break;

        case 'P':
            if (get_obj_index(arg1) == nullptr && fBootDb)
                boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter,
                         arg1);
            if (arg3 > 0)
            {
                if (get_obj_index(arg3) == nullptr && fBootDb)
                    boot_log("%s: %s (%d) 'P': destination object %d doesn't exist.", __FUNCTION__, tarea->filename,
                             count, arg3);
                if (extra > 1)
                    not01 = true;
            }
            if (!pRoomIndex)
                bug("%s: Unable to add room reset - room not found.", __FUNCTION__);
            else
            {
                if (arg3 == 0)
                    arg3 = OBJ_VNUM_MONEY_ONE; // This may look stupid, but for some reason it works.
                add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            }
            break;

        case 'G':
        case 'E':
            if (get_obj_index(arg1) == nullptr && fBootDb)
                boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, tarea->filename, count, letter,
                         arg1);
            if (!pRoomIndex)
                bug("%s: Unable to add room reset - room not found.", __FUNCTION__);
            else
                add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            break;

        case 'T':
            if (IS_SET(extra, TRAP_OBJ))
                bug("%s: Unable to add legacy object trap reset. Must be converted manually.", __FUNCTION__);
            else
            {
                if (!(pRoomIndex = get_room_index(arg3)))
                    bug("%s: Unable to add trap reset - room not found.", __FUNCTION__);
                else
                    add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            }
            break;

        case 'H':
            bug("%s: Unable to convert legacy hide reset. Must be converted manually.", __FUNCTION__);
            break;

        case 'D':
            if (!(pRoomIndex = get_room_index(arg1)))
            {
                bug("%s: 'D': room %d doesn't exist.", __FUNCTION__, arg1);
                bug("Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3);
                if (fBootDb)
                    boot_log("%s: %s (%d) 'D': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1);
                break;
            }

            if (arg2 < 0 || arg2 > MAX_DIR + 1 || !(pexit = get_exit(pRoomIndex, arg2)) ||
                !IS_SET(pexit->exit_info, EX_ISDOOR))
            {
                bug("%s: 'D': exit %d not door.", __FUNCTION__, arg2);
                bug("Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3);
                if (fBootDb)
                    boot_log("%s: %s (%d) 'D': exit %d not door.", __FUNCTION__, tarea->filename, count, arg2);
            }

            if (arg3 < 0 || arg3 > 2)
            {
                bug("%s: 'D': bad 'locks': %d.", __FUNCTION__, arg3);
                if (fBootDb)
                    boot_log("%s: %s (%d) 'D': bad 'locks': %d.", __FUNCTION__, tarea->filename, count, arg3);
            }
            add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            break;

        case 'R':
            if (!(pRoomIndex = get_room_index(arg1)) && fBootDb)
                boot_log("%s: %s (%d) 'R': room %d doesn't exist.", __FUNCTION__, tarea->filename, count, arg1);
            else
                add_reset(pRoomIndex, letter, extra, arg1, arg2, arg3);
            if (arg2 < 0 || arg2 > 10)
            {
                bug("%s: 'R': bad exit %d.", __FUNCTION__, arg2);
                if (fBootDb)
                    boot_log("%s: %s (%d) 'R': bad exit %d.", __FUNCTION__, tarea->filename, count, arg2);
                break;
            }
            break;
        }
    }
    if (!not01)
    {
        for (roomlist = tarea->first_room; roomlist; roomlist = roomlist->next_aroom)
            renumber_put_resets(roomlist);
    }
    return;
}

void load_room_reset(ROOM_INDEX_DATA* room, FILE* fp)
{
    EXIT_DATA* pexit;
    char letter;
    int extra, arg1, arg2, arg3;
    bool not01 = false;
    int count = 0;

    letter = fread_letter(fp);
    extra = fread_number(fp);
    if (letter == 'M' || letter == 'O')
        extra = 0;
    arg1 = fread_number(fp);
    arg2 = fread_number(fp);
    arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number(fp);
    fread_to_eol(fp);
    ++count;

    /*
     * Validate parameters.
     * We're calling the index functions for the side effect.
     */
    switch (letter)
    {
    default:
        bug("%s: bad command '%c'.", __FUNCTION__, letter);
        if (fBootDb)
            boot_log("%s: %s (%d) bad command '%c'.", __FUNCTION__, room->area->filename, count, letter);
        return;

    case 'M':
        if (get_mob_index(arg1) == nullptr && fBootDb)
            boot_log("%s: %s (%d) 'M': mobile %d doesn't exist.", __FUNCTION__, room->area->filename, count, arg1);
        break;

    case 'O':
        if (get_obj_index(arg1) == nullptr && fBootDb)
            boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                     arg1);
        break;

    case 'P':
        if (get_obj_index(arg1) == nullptr && fBootDb)
            boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                     arg1);

        if (arg3 <= 0)
            arg3 = OBJ_VNUM_MONEY_ONE; // This may look stupid, but for some reason it works.
        if (get_obj_index(arg3) == nullptr && fBootDb)
            boot_log("%s: %s (%d) 'P': destination object %d doesn't exist.", __FUNCTION__, room->area->filename, count,
                     arg3);
        if (extra > 1)
            not01 = true;
        break;

    case 'G':
    case 'E':
        if (get_obj_index(arg1) == nullptr && fBootDb)
            boot_log("%s: %s (%d) '%c': object %d doesn't exist.", __FUNCTION__, room->area->filename, count, letter,
                     arg1);
        break;

    case 'T':
    case 'H':
        break;

    case 'D':
        if (arg2 < 0 || arg2 > MAX_DIR + 1 || !(pexit = get_exit(room, arg2)) || !IS_SET(pexit->exit_info, EX_ISDOOR))
        {
            bug("%s: 'D': exit %d not door.", __FUNCTION__, arg2);
            bug("Reset: %c %d %d %d %d", letter, extra, arg1, arg2, arg3);
            if (fBootDb)
                boot_log("%s: %s (%d) 'D': exit %d not door.", __FUNCTION__, room->area->filename, count, arg2);
        }

        if (arg3 < 0 || arg3 > 2)
        {
            bug("%s: 'D': bad 'locks': %d.", __FUNCTION__, arg3);
            if (fBootDb)
                boot_log("%s: %s (%d) 'D': bad 'locks': %d.", __FUNCTION__, room->area->filename, count, arg3);
        }
        break;

    case 'R':
        if (arg2 < 0 || arg2 > 10)
        {
            bug("%s: 'R': bad exit %d.", __FUNCTION__, arg2);
            if (fBootDb)
                boot_log("%s: %s (%d) 'R': bad exit %d.", __FUNCTION__, room->area->filename, count, arg2);
            break;
        }
        break;
    }
    add_reset(room, letter, extra, arg1, arg2, arg3);

    if (!not01)
        renumber_put_resets(room);
    return;
}

/*
 * Load a room section.
 */
void load_rooms(AREA_DATA* tarea, FILE* fp)
{
    ROOM_INDEX_DATA* pRoomIndex;
    char buf[MAX_STRING_LENGTH];
    char* ln;

    if (!tarea)
    {
        bug("Load_rooms: no #AREA seen yet.");
        shutdown_mud("No #AREA");
        exit(1);
    }

    tarea->first_room = tarea->last_room = nullptr;

    for (;;)
    {
        int vnum;
        char letter;
        int door;
        int iHash;
        bool tmpBootDb;
        bool oldroom;
        int x1, x2, x3, x4, x5, x6, x7;

        letter = fread_letter(fp);
        if (letter != '#')
        {
            bug("Load_rooms: # not found.");
            if (fBootDb)
            {
                shutdown_mud("# not found");
                exit(1);
            }
            else
                return;
        }

        vnum = fread_number(fp);
        if (vnum == 0)
            break;

        tmpBootDb = fBootDb;
        fBootDb = false;
        if (get_room_index(vnum) != nullptr)
        {
            if (tmpBootDb)
            {
                bug("Load_rooms: vnum %d duplicated.", vnum);
                shutdown_mud("duplicate vnum");
                exit(1);
            }
            else
            {
                pRoomIndex = get_room_index(vnum);
                sprintf_s(buf, "Cleaning room: %d", vnum);
                log_string_plus(buf, LOG_BUILD, sysdata.log_level);
                clean_room(pRoomIndex);
                oldroom = true;
            }
        }
        else
        {
            oldroom = false;
            CREATE(pRoomIndex, ROOM_INDEX_DATA, 1);
            pRoomIndex->first_person = nullptr;
            pRoomIndex->last_person = nullptr;
            pRoomIndex->first_content = nullptr;
            pRoomIndex->last_content = nullptr;
        }

        fBootDb = tmpBootDb;
        pRoomIndex->area = tarea;
        pRoomIndex->vnum = vnum;
        pRoomIndex->first_extradesc = nullptr;
        pRoomIndex->last_extradesc = nullptr;

        if (fBootDb)
        {
            if (!tarea->low_r_vnum)
                tarea->low_r_vnum = vnum;
            if (vnum > tarea->hi_r_vnum)
                tarea->hi_r_vnum = vnum;
        }
        pRoomIndex->name = fread_string(fp);
        pRoomIndex->description = fread_string(fp);

        /* Area number			  fread_number( fp ); */
        ln = fread_line(fp);
        x1 = x2 = x3 = x4 = x5 = x6 = 0;
        sscanf(ln, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6, &x7);

        pRoomIndex->room_flags = x2;
        pRoomIndex->sector_type = x3;
        pRoomIndex->room_flags2 = x4;
        pRoomIndex->tele_delay = x5;
        pRoomIndex->tele_vnum = x6;
        pRoomIndex->tunnel = x7;
        if (pRoomIndex->sector_type < 0 || pRoomIndex->sector_type >= SECT_MAX)
        {
            bug("Fread_rooms: vnum %d has bad sector_type %d.", vnum, pRoomIndex->sector_type);
            pRoomIndex->sector_type = 1;
        }
        pRoomIndex->light = 0;
        pRoomIndex->first_exit = nullptr;
        pRoomIndex->last_exit = nullptr;

        for (;;)
        {
            letter = fread_letter(fp);

            if (letter == 'S')
                break;

            if (letter == 'D')
            {
                EXIT_DATA* pexit;
                int locks;

                door = fread_number(fp);
                if (door < 0 || door > 10)
                {
                    bug("Fread_rooms: vnum %d has bad door number %d.", vnum, door);
                    if (fBootDb)
                        exit(1);
                }
                else
                {
                    pexit = make_exit(pRoomIndex, nullptr, door);
                    pexit->description = fread_string(fp);
                    pexit->keyword = fread_string(fp);
                    pexit->exit_info = 0;
                    ln = fread_line(fp);
                    x1 = x2 = x3 = x4 = 0;
                    sscanf(ln, "%d %d %d %d %d", &x1, &x2, &x3, &x4, &x5);

                    locks = x1;
                    pexit->key = x2;
                    pexit->vnum = x3;
                    pexit->vdir = door;
                    pexit->distance = x4;
                    pexit->keypad = x5;

                    switch (locks)
                    {
                    case 1:
                        pexit->exit_info = EX_ISDOOR;
                        break;
                    case 2:
                        pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;
                        break;
                    default:
                        pexit->exit_info = locks;
                    }
                }
            }
            else if (letter == 'E')
            {
                EXTRA_DESCR_DATA* ed;

                CREATE(ed, EXTRA_DESCR_DATA, 1);
                ed->keyword = fread_string(fp);
                ed->description = fread_string(fp);
                LINK(ed, pRoomIndex->first_extradesc, pRoomIndex->last_extradesc, next, prev);
                top_ed++;
            }

            else if (letter == 'R')
                load_room_reset(pRoomIndex, fp);

            else if (letter == '>')
            {
                ungetc(letter, fp);
                rprog_read_programs(fp, pRoomIndex);
            }
            else
            {
                bug("Load_rooms: vnum %d has flag '%c' not 'DES'.", vnum, letter);
                shutdown_mud("Room flag not DES");
                exit(1);
            }
        }

        if (!oldroom)
        {
            g_roomIndex.insert({vnum, pRoomIndex});
            LINK(pRoomIndex, tarea->first_room, tarea->last_room, next_aroom, prev_aroom);
        }
    }

    return;
}

/*
 * Load a shop section.
 */
void load_shops(AREA_DATA* tarea, FILE* fp)
{
    SHOP_DATA* pShop;

    for (;;)
    {
        MOB_INDEX_DATA* pMobIndex;
        int iTrade;

        CREATE(pShop, SHOP_DATA, 1);
        pShop->keeper = fread_number(fp);
        if (pShop->keeper == 0)
            break;
        for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
            pShop->buy_type[iTrade] = fread_number(fp);
        pShop->profit_buy = fread_number(fp);
        pShop->profit_sell = fread_number(fp);
        pShop->profit_buy = URANGE(pShop->profit_sell + 5, pShop->profit_buy, 1000);
        pShop->profit_sell = URANGE(0, pShop->profit_sell, pShop->profit_buy - 5);
        pShop->open_hour = fread_number(fp);
        pShop->close_hour = fread_number(fp);
        fread_to_eol(fp);
        pMobIndex = get_mob_index(pShop->keeper);
        pMobIndex->pShop = pShop;

        if (!first_shop)
            first_shop = pShop;
        else
            last_shop->next = pShop;
        pShop->next = nullptr;
        pShop->prev = last_shop;
        last_shop = pShop;
        top_shop++;
    }
    return;
}

/*
 * Load a repair shop section.					-Thoric
 */
void load_repairs(AREA_DATA* tarea, FILE* fp)
{
    REPAIR_DATA* rShop;

    for (;;)
    {
        MOB_INDEX_DATA* pMobIndex;
        int iFix;

        CREATE(rShop, REPAIR_DATA, 1);
        rShop->keeper = fread_number(fp);
        if (rShop->keeper == 0)
            break;
        for (iFix = 0; iFix < MAX_FIX; iFix++)
            rShop->fix_type[iFix] = fread_number(fp);
        rShop->profit_fix = fread_number(fp);
        rShop->shop_type = fread_number(fp);
        rShop->open_hour = fread_number(fp);
        rShop->close_hour = fread_number(fp);
        fread_to_eol(fp);
        pMobIndex = get_mob_index(rShop->keeper);
        pMobIndex->rShop = rShop;

        if (!first_repair)
            first_repair = rShop;
        else
            last_repair->next = rShop;
        rShop->next = nullptr;
        rShop->prev = last_repair;
        last_repair = rShop;
        top_repair++;
    }
    return;
}

/*
 * Load spec proc declarations.
 */
void load_specials(AREA_DATA* tarea, FILE* fp)
{
    for (;;)
    {
        MOB_INDEX_DATA* pMobIndex;
        char letter;

        switch (letter = fread_letter(fp))
        {
        default:
            bug("Load_specials: letter '%c' not *MS.", letter);
            exit(1);

        case 'S':
            return;

        case '*':
            break;

        case 'M':
            pMobIndex = get_mob_index(fread_number(fp));
            if (!pMobIndex->spec_fun)
            {
                pMobIndex->spec_fun = spec_lookup(fread_word(fp));

                if (pMobIndex->spec_fun == 0)
                {
                    bug("Load_specials: 'M': vnum %d.", pMobIndex->vnum);
                    exit(1);
                }
            }
            else if (!pMobIndex->spec_2)
            {
                pMobIndex->spec_2 = spec_lookup(fread_word(fp));

                if (pMobIndex->spec_2 == 0)
                {
                    bug("Load_specials: 'M': vnum %d.", pMobIndex->vnum);
                    exit(1);
                }
            }

            break;
        }

        fread_to_eol(fp);
    }
}

/*
 * Load soft / hard area ranges.
 */
void load_ranges(AREA_DATA* tarea, FILE* fp)
{
    int x1, x2, x3, x4;
    char* ln;

    if (!tarea)
    {
        bug("Load_ranges: no #AREA seen yet.");
        shutdown_mud("No #AREA");
        exit(1);
    }

    for (;;)
    {
        ln = fread_line(fp);

        if (ln[0] == '$')
            break;

        x1 = x2 = x3 = x4 = 0;
        sscanf(ln, "%d %d %d %d", &x1, &x2, &x3, &x4);

        tarea->low_soft_range = x1;
        tarea->hi_soft_range = x2;
        tarea->low_hard_range = x3;
        tarea->hi_hard_range = x4;
    }
    return;
}

/*
 * Go through all areas, and set up initial economy based on mob
 * levels and gold
 */
void initialize_economy(void)
{
    AREA_DATA* tarea;
    MOB_INDEX_DATA* mob;
    int idx, gold, rng;

    for (tarea = first_area; tarea; tarea = tarea->next)
    {
        /* skip area if they already got some gold */
        if (tarea->high_economy > 0 || tarea->low_economy > 10000)
            continue;
        rng = tarea->hi_soft_range - tarea->low_soft_range;
        if (rng)
            rng /= 2;
        else
            rng = 25;
        gold = rng * rng * 10000;
        boost_economy(tarea, gold);
        for (idx = tarea->low_m_vnum; idx < tarea->hi_m_vnum; idx++)
            if ((mob = get_mob_index(idx)) != nullptr)
                boost_economy(tarea, mob->gold * 10);
    }
}

/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits(void)
{
    EXIT_DATA *pexit = nullptr, *pexit_next = nullptr, *rev_exit = nullptr;

    for (auto pair : g_roomIndex)
    {
        auto pRoomIndex = pair.second;

        bool fexit;

        fexit = false;
        for (pexit = pRoomIndex->first_exit; pexit; pexit = pexit_next)
        {
            pexit_next = pexit->next;
            pexit->rvnum = pRoomIndex->vnum;
            if (pexit->vnum <= 0 || (pexit->to_room = get_room_index(pexit->vnum)) == nullptr)
            {
                if (fBootDb)
                    boot_log("Fix_exits: room %d, exit %s leads to bad vnum (%d)", pRoomIndex->vnum,
                             dir_name[pexit->vdir], pexit->vnum);

                bug("Deleting %s exit in room %d", dir_name[pexit->vdir], pRoomIndex->vnum);
                extract_exit(pRoomIndex, pexit);
            }
            else
                fexit = true;
        }
        if (!fexit)
            SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
    }

    /* Set all the rexit pointers 	-Thoric */
    for (auto pair : g_roomIndex)
    {
        auto pRoomIndex = pair.second;

        for (pexit = pRoomIndex->first_exit; pexit; pexit = pexit->next)
        {
            if (pexit->to_room && !pexit->rexit)
            {
                rev_exit = get_exit_to(pexit->to_room, rev_dir[pexit->vdir], pRoomIndex->vnum);
                if (rev_exit)
                {
                    pexit->rexit = rev_exit;
                    rev_exit->rexit = pexit;
                }
            }
        }
    }

    return;
}

/*
 * Get diku-compatable exit by number				-Thoric
 */
EXIT_DATA* get_exit_number(ROOM_INDEX_DATA* room, int xit)
{
    EXIT_DATA* pexit;
    int count;

    count = 0;
    for (pexit = room->first_exit; pexit; pexit = pexit->next)
        if (++count == xit)
            return pexit;
    return nullptr;
}

/*
 * (prelude...) This is going to be fun... NOT!
 * (conclusion) QSort is f*cked!
 */
int exit_comp(EXIT_DATA** xit1, EXIT_DATA** xit2)
{
    int d1, d2;

    d1 = (*xit1)->vdir;
    d2 = (*xit2)->vdir;

    if (d1 < d2)
        return -1;
    if (d1 > d2)
        return 1;
    return 0;
}

void sort_exits(ROOM_INDEX_DATA* room)
{
    EXIT_DATA* pexit; /* *texit */ /* Unused */
    EXIT_DATA* exits[MAX_REXITS];
    int x, nexits;

    nexits = 0;
    for (pexit = room->first_exit; pexit; pexit = pexit->next)
    {
        exits[nexits++] = pexit;
        if (nexits > MAX_REXITS)
        {
            bug("sort_exits: more than %d exits in room... fatal", nexits);
            return;
        }
    }
    qsort(&exits[0], nexits, sizeof(EXIT_DATA*), (int (*)(const void*, const void*))exit_comp);
    for (x = 0; x < nexits; x++)
    {
        if (x > 0)
            exits[x]->prev = exits[x - 1];
        else
        {
            exits[x]->prev = nullptr;
            room->first_exit = exits[x];
        }
        if (x >= (nexits - 1))
        {
            exits[x]->next = nullptr;
            room->last_exit = exits[x];
        }
        else
            exits[x]->next = exits[x + 1];
    }
}

void randomize_exits(ROOM_INDEX_DATA* room, sh_int maxdir)
{
    EXIT_DATA* pexit;
    int nexits, /* maxd, */ d0, d1, count, door; /* Maxd unused */
    int vdirs[MAX_REXITS];

    nexits = 0;
    for (pexit = room->first_exit; pexit; pexit = pexit->next)
        vdirs[nexits++] = pexit->vdir;

    for (d0 = 0; d0 < nexits; d0++)
    {
        if (vdirs[d0] > maxdir)
            continue;
        count = 0;
        while (vdirs[(d1 = number_range(d0, nexits - 1))] > maxdir || ++count > 5)
            ;
        if (vdirs[d1] > maxdir)
            continue;
        door = vdirs[d0];
        vdirs[d0] = vdirs[d1];
        vdirs[d1] = door;
    }
    count = 0;
    for (pexit = room->first_exit; pexit; pexit = pexit->next)
        pexit->vdir = vdirs[count++];

    sort_exits(room);
}

/*
 * Repopulate areas periodically.
 */
void area_update(void)
{
    AREA_DATA* pArea;

    for (pArea = first_area; pArea; pArea = pArea->next)
    {
        CHAR_DATA* pch;
        int reset_age = pArea->reset_frequency ? pArea->reset_frequency : 15;

        if ((reset_age == -1 && pArea->age == -1) || ++pArea->age < (reset_age - 1))
            continue;

        /*
         * Check for PC's.
         */
        if (pArea->nplayer > 0 && pArea->age >= reset_age)
        {
            char buf[MAX_STRING_LENGTH];

            /* Rennard */
            if (pArea->resetmsg)
                sprintf_s(buf, "%s\n\r", pArea->resetmsg);
            else
                strcpy_s(buf, "You hear some squeaking sounds...\n\r");
            for (pch = first_char; pch; pch = pch->next)
            {
                if (!IS_NPC(pch) && IS_AWAKE(pch) && pch->in_room && pch->in_room->area == pArea)
                {
                    set_char_color(AT_RESET, pch);
                    send_to_char(buf, pch);
                }
            }
        }

        /*
         * Check age and reset.
         * Note: Mud Academy resets every 3 minutes (not 15).
         */
        if (pArea->nplayer == 0 || pArea->age >= reset_age)
        {
            ROOM_INDEX_DATA* pRoomIndex;

            fprintf(stderr, "Resetting: %s\n", pArea->filename);
            reset_area(pArea);
            if (reset_age == -1)
                pArea->age = -1;
            else
                pArea->age = number_range(0, reset_age / 5);
            pRoomIndex = get_room_index(ROOM_VNUM_SCHOOL);
            if (pRoomIndex != nullptr && pArea == pRoomIndex->area && pArea->reset_frequency == 0)
                pArea->age = 15 - 3;
        }
    }
    return;
}

/*
 * Create an instance of a mobile.
 */
CHAR_DATA* create_mobile(MOB_INDEX_DATA* pMobIndex)
{
    CHAR_DATA* mob;

    if (!pMobIndex)
    {
        bug("Create_mobile: nullptr pMobIndex.");
        exit(1);
    }

    CREATE(mob, CHAR_DATA, 1);
    clear_char(mob);
    mob->pIndexData = pMobIndex;

    mob->editor = nullptr;
    mob->name = QUICKLINK(pMobIndex->player_name);
    mob->short_descr = QUICKLINK(pMobIndex->short_descr);
    mob->long_descr = QUICKLINK(pMobIndex->long_descr);
    mob->description = QUICKLINK(pMobIndex->description);
    mob->spec_fun = pMobIndex->spec_fun;
    mob->spec_2 = pMobIndex->spec_2;
    mob->mpscriptpos = 0;
    mob->top_level = number_fuzzy(pMobIndex->level);
    {
        int ability;
        for (ability = 0; ability < MAX_ABILITY; ability++)
            mob->skill_level[ability] = mob->top_level;
    }
    mob->act = pMobIndex->act;
    mob->affected_by = pMobIndex->affected_by;
    mob->alignment = pMobIndex->alignment;
    mob->sex = pMobIndex->sex;
    mob->main_ability = 0;
    mob->mob_clan = STRALLOC("");
    mob->was_sentinel = nullptr;
    mob->plr_home = nullptr;
    mob->guard_data = nullptr;

    if (pMobIndex->ac)
        mob->armor = pMobIndex->ac;
    else
        mob->armor = 100 - mob->top_level * 2.5;

    if (!pMobIndex->hitnodice)
        mob->max_hit = mob->top_level * 10 + number_range(mob->top_level, mob->top_level * 10);
    else
        mob->max_hit = pMobIndex->hitnodice * number_range(1, pMobIndex->hitsizedice) + pMobIndex->hitplus;
    mob->hit = mob->max_hit;
    /* lets put things back the way they used to be! -Thoric */
    mob->gold = pMobIndex->gold;
    mob->position = pMobIndex->position;
    mob->defposition = pMobIndex->defposition;
    mob->barenumdie = pMobIndex->damnodice;
    mob->baresizedie = pMobIndex->damsizedice;
    mob->mobthac0 = pMobIndex->mobthac0;
    mob->hitplus = pMobIndex->hitplus;
    mob->damplus = pMobIndex->damplus;
    mob->mana = 0;
    mob->max_mana = 0;
    mob->perm_str = pMobIndex->perm_str;
    mob->perm_dex = pMobIndex->perm_dex;
    mob->perm_wis = pMobIndex->perm_wis;
    mob->perm_int = pMobIndex->perm_int;
    mob->perm_con = pMobIndex->perm_con;
    mob->perm_cha = pMobIndex->perm_cha;
    mob->perm_lck = pMobIndex->perm_lck;
    mob->hitroll = pMobIndex->hitroll;
    mob->damroll = pMobIndex->damroll;
    mob->race = pMobIndex->race;
    mob->xflags = pMobIndex->xflags;
    mob->saving_poison_death = pMobIndex->saving_poison_death;
    mob->saving_wand = pMobIndex->saving_wand;
    mob->saving_para_petri = pMobIndex->saving_para_petri;
    mob->saving_breath = pMobIndex->saving_breath;
    mob->saving_spell_staff = pMobIndex->saving_spell_staff;
    mob->height = pMobIndex->height;
    mob->weight = pMobIndex->weight;
    mob->resistant = pMobIndex->resistant;
    mob->immune = pMobIndex->immune;
    mob->susceptible = pMobIndex->susceptible;
    mob->attacks = pMobIndex->attacks;
    mob->defenses = pMobIndex->defenses;
    mob->numattacks = pMobIndex->numattacks;
    mob->speaks = pMobIndex->speaks;
    mob->speaking = pMobIndex->speaking;
    mob->vip_flags = pMobIndex->vip_flags;

    /*
     * Insert in list.
     */
    add_char(mob);
    pMobIndex->count++;
    nummobsloaded++;
    return mob;
}

/*
 * Create an instance of an object.
 */
OBJ_DATA* create_object(OBJ_INDEX_DATA* pObjIndex, int level)
{
    OBJ_DATA* obj;

    if (!pObjIndex)
    {
        bug("Create_object: nullptr pObjIndex.");
        exit(1);
    }

    CREATE(obj, OBJ_DATA, 1);

    obj->pIndexData = pObjIndex;
    obj->in_room = nullptr;
    obj->level = level;
    obj->wear_loc = -1;
    obj->count = 1;
    cur_obj_serial = UMAX((cur_obj_serial + 1) & (BV30 - 1), 1);
    obj->serial = obj->pIndexData->serial = cur_obj_serial;

    obj->armed_by = STRALLOC("");
    obj->name = QUICKLINK(pObjIndex->name);
    obj->short_descr = QUICKLINK(pObjIndex->short_descr);
    obj->description = QUICKLINK(pObjIndex->description);
    obj->action_desc = QUICKLINK(pObjIndex->action_desc);
    obj->item_type = pObjIndex->item_type;
    obj->extra_flags = pObjIndex->extra_flags;
    obj->wear_flags = pObjIndex->wear_flags;
    obj->value[0] = pObjIndex->value[0];
    obj->value[1] = pObjIndex->value[1];
    obj->value[2] = pObjIndex->value[2];
    obj->value[3] = pObjIndex->value[3];
    obj->value[4] = pObjIndex->value[4];
    obj->value[5] = pObjIndex->value[5];
    obj->weight = pObjIndex->weight;
    obj->cost = pObjIndex->cost;
    /*
    obj->cost		= number_fuzzy( 10 )
            * number_fuzzy( level ) * number_fuzzy( level );
     */

    /*
     * Mess with object properties.
     */
    switch (obj->item_type)
    {
    default:
        bug("Read_object: vnum %d bad type.", pObjIndex->vnum);
        bug("------------------------>     ", obj->item_type);
        break;

    case ITEM_GOVERNMENT:
    case ITEM_SPACECRAFT:
    case ITEM_RAWSPICE:
    case ITEM_LENS:
    case ITEM_CRYSTAL:
    case ITEM_DURAPLAST:
    case ITEM_DURASTEEL:
    case ITEM_SUPERCONDUCTOR:
    case ITEM_COMLINK:
    case ITEM_MEDPAC:
    case ITEM_FABRIC:
    case ITEM_RARE_METAL:
    case ITEM_MAGNET:
    case ITEM_THREAD:
    case ITEM_CHEMICAL:
    case ITEM_SPICE:
    case ITEM_DEBIT_CARD:
    case ITEM_SMUT:
    case ITEM_OVEN:
    case ITEM_MIRROR:
    case ITEM_CIRCUIT:
    case ITEM_TOOLKIT:
    case ITEM_LIGHT:
    case ITEM_TREASURE:
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_BINDERS:
    case ITEM_GOGGLES:
        break;
    case ITEM_FOOD:
        /*
         * optional food condition (rotting food)		-Thoric
         * value1 is the max condition of the food
         * value4 is the optional initial condition
         */
        if (obj->value[4])
            obj->timer = obj->value[4];
        else
            obj->timer = obj->value[1];
        break;

    case ITEM_DROID_CORPSE:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_SCRAPS:
    case ITEM_GRENADE:
    case ITEM_EMP_GRENADE:
    case ITEM_SHIPBOMB:
    case ITEM_LANDMINE:
    case ITEM_FIRE:
    case ITEM_BOOK:
    case ITEM_SWITCH:
    case ITEM_LEVER:
    case ITEM_BUTTON:
    case ITEM_DIAL:
    case ITEM_TRAP:
    case ITEM_MAP:
    case ITEM_PAPER:
    case ITEM_PEN:
    case ITEM_LOCKPICK:
    case ITEM_FUEL:
    case ITEM_MISSILE:
    case ITEM_COMMSYSTEM:
    case ITEM_MODULE:
    case ITEM_DATAPAD:
    case ITEM_BEACON:
    case ITEM_BUG:
    case ITEM_RLAUNCHER:
    case ITEM_GLAUNCHER:
    case ITEM_SHOVEL:
        break;

    case ITEM_SALVE:
        obj->value[3] = number_fuzzy(obj->value[3]);
        break;

    case ITEM_DEVICE:
        obj->value[0] = number_fuzzy(obj->value[0]);
        obj->value[1] = number_fuzzy(obj->value[1]);
        obj->value[2] = obj->value[1];
        break;

    case ITEM_BATTERY:
        if (obj->value[0] <= 0)
            obj->value[0] = number_fuzzy(95);
        break;

    case ITEM_BOLT:
        if (obj->value[0] <= 0)
            obj->value[0] = number_fuzzy(95);
        break;

    case ITEM_AMMO:
        if (obj->value[0] <= 0)
            obj->value[0] = number_fuzzy(495);
        break;

    case ITEM_WEAPON:
        /*
            if ( obj->value[1] && obj->value[2] )
               obj->value[2] *= obj->value[1];
            else
            {
               obj->value[1] = number_fuzzy( number_fuzzy( 1 + level/20 ) );
               obj->value[2] = number_fuzzy( number_fuzzy( 10 + level/10 ) );
            }
        */
        if (obj->value[1] > obj->value[2])
            obj->value[1] = obj->value[2] / 3;
        if (obj->value[0] == 0)
            obj->value[0] = INIT_WEAPON_CONDITION;
        switch (obj->value[3])
        {
        case WEAPON_BLASTER:
        case WEAPON_LIGHTSABER:
        case WEAPON_DUAL_LIGHTSABER:
        case WEAPON_VIBRO_BLADE:
        case WEAPON_FORCE_PIKE:
        case WEAPON_BOWCASTER:
            if (obj->value[5] <= 0)
                obj->value[5] = number_fuzzy(1000);
        }
        obj->value[4] = obj->value[5];
        break;

    case ITEM_ARMOR:
        if (obj->value[0] == 0)
            obj->value[0] = obj->value[1];
        obj->timer = obj->value[3];
        break;

    case ITEM_POTION:
    case ITEM_PILL:
        obj->value[0] = number_fuzzy(number_fuzzy(obj->value[0]));
        break;

    case ITEM_MONEY:
        obj->value[0] = obj->cost;
        break;
    }

    LINK(obj, first_object, last_object, next, prev);
    ++pObjIndex->count;
    ++numobjsloaded;
    ++physicalobjects;

    return obj;
}

/*
 * Clear a new character.
 */
void clear_char(CHAR_DATA* ch)
{
    ch->editor = nullptr;
    ch->hunting = nullptr;
    ch->fearing = nullptr;
    ch->hating = nullptr;
    ch->name = nullptr;
    ch->short_descr = nullptr;
    ch->long_descr = nullptr;
    ch->description = nullptr;
    ch->next = nullptr;
    ch->prev = nullptr;
    ch->first_carrying = nullptr;
    ch->last_carrying = nullptr;
    ch->next_in_room = nullptr;
    ch->prev_in_room = nullptr;
    ch->fighting = nullptr;
    ch->switched = nullptr;
    ch->first_affect = nullptr;
    ch->last_affect = nullptr;
    ch->prev_cmd = nullptr; /* maps */
    ch->last_cmd = nullptr;
    ch->dest_buf = nullptr;
    ch->dest_buf_2 = nullptr;
    ch->spare_ptr = nullptr;
    ch->mount = nullptr;
    ch->affected_by = 0;
    ch->logon = current_time;
    ch->armor = 100;
    ch->position = POS_STANDING;
    ch->hit = 500;
    ch->max_hit = 500;
    ch->mana = 1000;
    ch->max_mana = 0;
    ch->move = 1000;
    ch->max_move = 1000;
    ch->height = 72;
    ch->weight = 180;
    ch->xflags = 0;
    ch->race = 0;
    ch->speaking = LANG_BASIC;
    ch->speaks = LANG_BASIC;
    ch->barenumdie = 1;
    ch->baresizedie = 4;
    ch->substate = 0;
    ch->tempnum = 0;
    ch->perm_str = 10;
    ch->perm_dex = 10;
    ch->perm_int = 10;
    ch->perm_wis = 10;
    ch->perm_cha = 10;
    ch->perm_con = 10;
    ch->perm_lck = 10;
    ch->mod_str = 0;
    ch->mod_dex = 0;
    ch->mod_int = 0;
    ch->mod_wis = 0;
    ch->mod_cha = 0;
    ch->mod_con = 0;
    ch->mod_lck = 0;
    ch->pagelen = 24;               /* BUILD INTERFACE */
    ch->inter_page = NO_PAGE;       /* BUILD INTERFACE */
    ch->inter_type = NO_TYPE;       /* BUILD INTERFACE */
    ch->inter_editing = nullptr;       /* BUILD INTERFACE */
    ch->inter_editing_vnum = -1;    /* BUILD INTERFACE */
    ch->inter_substate = SUB_NORTH; /* BUILD INTERFACE */
    ch->plr_home = nullptr;
    return;
}

/*
 * Free a character.
 */
void free_char(CHAR_DATA* ch)
{
    OBJ_DATA* obj;
    BUG_DATA* bugs;
    CONTRACT_DATA* contract;
    FELLOW_DATA* fellow;
    AFFECT_DATA* paf;
    TIMER* timer;
    MPROG_ACT_LIST *mpact, *mpact_next;
    NOTE_DATA *comments, *comments_next;

    if (!ch)
    {
        bug("Free_char: null ch!");
        return;
    }

    if (ch->desc)
        bug("Free_char: char still has descriptor.");

    while ((obj = ch->last_carrying) != nullptr)
        extract_obj(obj);

    while ((paf = ch->last_affect) != nullptr)
        affect_remove(ch, paf);

    while ((timer = ch->first_timer) != nullptr)
        extract_timer(ch, timer);

    while ((bugs = ch->first_bug) != nullptr)
    {
        UNLINK(bugs, ch->first_bug, ch->last_bug, next_in_bug, prev_in_bug);
        DISPOSE(bugs);
    }

    while ((contract = ch->first_contract) != nullptr)
    {
        UNLINK(contract, ch->first_contract, ch->last_contract, next_in_contract, prev_in_contract);
        DISPOSE(contract);
    }

    while ((fellow = ch->first_fellow) != nullptr)
    {
        UNLINK(fellow, ch->first_fellow, ch->last_fellow, next, prev);
        DISPOSE(fellow);
    }

    STRFREE(ch->name);
    STRFREE(ch->short_descr);
    STRFREE(ch->long_descr);
    STRFREE(ch->description);
    if (ch->editor)
        stop_editing(ch);

    if (ch->inter_editing)
        DISPOSE(ch->inter_editing);

    stop_hunting(ch);
    stop_hating(ch);
    stop_fearing(ch);
    free_fight(ch);

    if (ch->pnote)
        free_note(ch->pnote);

    if (ch->pcdata)
    {
        STRFREE(ch->pcdata->clan_name);
        DISPOSE(ch->pcdata->pwd);     /* no hash */
        DISPOSE(ch->pcdata->email);   /* no hash */
        DISPOSE(ch->pcdata->bamfin);  /* no hash */
        DISPOSE(ch->pcdata->bamfout); /* no hash */
        STRFREE(ch->pcdata->title);
        STRFREE(ch->pcdata->disguise);
        STRFREE(ch->pcdata->bio);
        DISPOSE(ch->pcdata->bestowments); /* no hash */
        DISPOSE(ch->pcdata->homepage);    /* no hash */
        DISPOSE(ch->pcdata->screenname);  /* no hash */
        DISPOSE(ch->pcdata->image);       /* no hash */
        STRFREE(ch->pcdata->authed_by);
        STRFREE(ch->pcdata->prompt);
        if (ch->pcdata->subprompt)
            STRFREE(ch->pcdata->subprompt);

        /* if (string) STRFREE/DISPOSE(string), if exists free, if not, great. list anyway

            Currently unlisted
            Not going to fuck with it until I know what a hash is
            See non-pcdata unlisted below

            char *		store_title;
                char *		tell_snoop
                        char * 		last_name;
                    char *		fiance;
            char *		propose;
            char *		proposed;
            char *		spouse;
                char *		shipname;
                char *		helled_by;
        */
        DISPOSE(ch->pcdata);
    }
    /* Non-pcdata char_data unfreed chars

        char * 		owner;
        char                *force_master;
        char                *force_temp_master;
        char                *force_disguise;
        char * comfreq;
        char * rank;

    */
    for (mpact = ch->mpact; mpact; mpact = mpact_next)
    {
        mpact_next = mpact->next;
        DISPOSE(mpact->buf);
        DISPOSE(mpact);
    }

    for (comments = ch->comments; comments; comments = comments_next)
    {
        comments_next = comments->next;
        STRFREE(comments->text);
        STRFREE(comments->to_list);
        STRFREE(comments->subject);
        STRFREE(comments->sender);
        STRFREE(comments->date);
        DISPOSE(comments);
    }
    DISPOSE(ch);
    return;
}

/*
 * Get an extra description from a list.
 */
char* get_extra_descr(const char* name, EXTRA_DESCR_DATA* ed)
{
    for (; ed; ed = ed->next)
        if (is_name(name, ed->keyword))
            return ed->description;

    return nullptr;
}

/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA* get_mob_index(int vnum)
{
    MOB_INDEX_DATA* pMobIndex;

    if (vnum < 0)
        vnum = 0;

    auto iter = g_mobIndex.find(vnum);

    if (iter != g_mobIndex.end())
    {
        return iter->second;
    }

    if (fBootDb)
        bug("Get_mob_index: bad vnum %d.", vnum);

    return nullptr;
}

/*
 * Translates obj virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA* get_obj_index(int vnum)
{
    OBJ_INDEX_DATA* pObjIndex;

    if (vnum < 0)
        vnum = 0;

    auto iter = g_objectIndex.find(vnum);

    if (iter != g_objectIndex.end())
    {
        return iter->second;
    }

    if (fBootDb)
        bug("Get_obj_index: bad vnum %d.", vnum);

    return nullptr;
}

/*
 * Translates room virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA* get_room_index(int vnum)
{
    ROOM_INDEX_DATA* pRoomIndex;

    if (vnum < 0)
        vnum = 0;

    auto iter = g_roomIndex.find(vnum);

    if (iter != g_roomIndex.end())
    {
        return iter->second;
    }

    if (fBootDb)
        bug("Get_room_index: bad vnum %d.", vnum);

    return nullptr;
}

/*
 * Added lots of EOF checks, as most of the file crashes are based on them.
 * If an area file encounters EOF, the fread_* functions will shutdown the
 * MUD, as all area files should be read in in full or bad things will
 * happen during the game.  Any files loaded in without fBootDb which
 * encounter EOF will return what they have read so far.   These files
 * should include player files, and in-progress areas that are not loaded
 * upon bootup.
 * -- Altrag
 */

/*
 * Read a letter from a file.
 */
char fread_letter(FILE* fp)
{
    char c;

    do
    {
        if (feof(fp))
        {
            // bug("fread_letter: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return '\0';
        }
        c = getc(fp);
    } while (isspace(c));

    return c;
}

/*
 * Read a number from a file.
 */
int fread_number(FILE* fp)
{
    int number;
    bool sign;
    char c;

    do
    {
        if (feof(fp))
        {
            // bug("fread_number: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return 0;
        }
        c = getc(fp);
    } while (isspace(c));

    number = 0;

    sign = false;
    if (c == '+')
    {
        c = getc(fp);
    }
    else if (c == '-')
    {
        sign = true;
        c = getc(fp);
    }

    if (!isdigit(c))
    {
        // bug( "Fread_number: bad format. (%c)", c );
        if (fBootDb)
            exit(1);
        return 0;
    }

    while (isdigit(c))
    {
        if (feof(fp))
        {
            // bug("fread_number: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return number;
        }
        number = number * 10 + c - '0';
        c = getc(fp);
    }

    if (sign)
        number = 0 - number;

    if (c == '|')
        number += fread_number(fp);
    else if (c != ' ')
        ungetc(c, fp);

    return number;
}

/*
 * custom str_dup using create					-Thoric
 */
char* str_dup(char const* str)
{
    static char* ret;
    int len;

    if (!str)
        return nullptr;

    len = strlen(str) + 1;

    CREATE(ret, char, len);
    strcpy_s(ret, len, str);
    return ret;
}

/*
 * Read a string from file fp
 */
char* fread_string(FILE* fp)
{
    char buf[MAX_STRING_LENGTH];
    char* plast;
    char c;
    int ln;

    plast = buf;
    buf[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        if (feof(fp))
        {
            // bug("fread_string: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return STRALLOC("");
        }
        c = getc(fp);
    } while (isspace(c));

    if ((*plast++ = c) == '~')
        return STRALLOC("");

    for (;;)
    {
        if (ln >= (MAX_STRING_LENGTH - 1))
        {
            // bug( "fread_string: string too long" );
            *plast = '\0';
            return STRALLOC(buf);
        }
        switch (*plast = getc(fp))
        {
        default:
            plast++;
            ln++;
            break;

        case EOF:
            // bug( "Fread_string: EOF" );
            if (fBootDb)
                exit(1);
            *plast = '\0';
            return STRALLOC(buf);
            break;

        case '\n':
            plast++;
            ln++;
            *plast++ = '\r';
            ln++;
            break;

        case '\r':
            break;

        case '~':
            *plast = '\0';
            return STRALLOC(buf);
        }
    }
}

/*
 * Read a string from file fp using str_dup (ie: no string hashing)
 */
char* fread_string_nohash(FILE* fp)
{
    char buf[MAX_STRING_LENGTH];
    char* plast;
    char c;
    int ln;

    plast = buf;
    buf[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        if (feof(fp))
        {
            bug("fread_string_no_hash: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return str_dup("");
        }
        c = getc(fp);
    } while (isspace(c));

    if ((*plast++ = c) == '~')
        return str_dup("");

    for (;;)
    {
        if (ln >= (MAX_STRING_LENGTH - 1))
        {
            bug("fread_string_no_hash: string too long");
            *plast = '\0';
            return str_dup(buf);
        }
        switch (*plast = getc(fp))
        {
        default:
            plast++;
            ln++;
            break;

        case EOF:
            bug("Fread_string_no_hash: EOF");
            if (fBootDb)
                exit(1);
            *plast = '\0';
            return str_dup(buf);
            break;

        case '\n':
            plast++;
            ln++;
            *plast++ = '\r';
            ln++;
            break;

        case '\r':
            break;

        case '~':
            *plast = '\0';
            return str_dup(buf);
        }
    }
}

/*
 * Read to end of line (for comments).
 */
void fread_to_eol(FILE* fp)
{
    char c;

    do
    {
        if (feof(fp))
        {
            bug("fread_to_eol: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            return;
        }
        c = getc(fp);
    } while (c != '\n' && c != '\r');

    do
    {
        c = getc(fp);
    } while (c == '\n' || c == '\r');

    ungetc(c, fp);
    return;
}

/*
 * Read to end of line into static buffer			-Thoric
 */
char* fread_line(FILE* fp)
{
    static char line[MAX_STRING_LENGTH];
    char* pline;
    char c;
    int ln;

    pline = line;
    line[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        if (feof(fp))
        {
            bug("fread_line: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            strcpy_s(line, "");
            return line;
        }
        c = getc(fp);
    } while (isspace(c));

    ungetc(c, fp);
    do
    {
        if (feof(fp))
        {
            bug("fread_line: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            *pline = '\0';
            return line;
        }
        c = getc(fp);
        *pline++ = c;
        ln++;
        if (ln >= (MAX_STRING_LENGTH - 1))
        {
            bug("fread_line: line too long");
            break;
        }
    } while (c != '\n' && c != '\r');

    do
    {
        c = getc(fp);
    } while (c == '\n' || c == '\r');

    ungetc(c, fp);
    *pline = '\0';
    return line;
}

/*
 * Read one word (into static buffer).
 */
char* fread_word(FILE* fp)
{
    static char word[MAX_INPUT_LENGTH];
    char* pword;
    char cEnd;

    do
    {
        if (feof(fp))
        {
            bug("fread_word: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            word[0] = '\0';
            return word;
        }
        cEnd = getc(fp);
    } while (isspace(cEnd));

    if (cEnd == '\'' || cEnd == '"')
    {
        pword = word;
    }
    else
    {
        word[0] = cEnd;
        pword = word + 1;
        cEnd = ' ';
    }

    for (; pword < word + MAX_INPUT_LENGTH; pword++)
    {
        if (feof(fp))
        {
            bug("fread_word: EOF encountered on read.\n\r");
            if (fBootDb)
                exit(1);
            *pword = '\0';
            return word;
        }
        *pword = getc(fp);
        if (cEnd == ' ' ? isspace(*pword) : *pword == cEnd)
        {
            if (cEnd == ' ')
                ungetc(*pword, fp);
            *pword = '\0';
            return word;
        }
    }

    bug("Fread_word: word too long");
    *pword = '\0';
    return word;
}

void do_memory(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    int hash;

    argument = one_argument(argument, arg);
    ch_printf(ch, "Affects %5d    Areas   %5d\n\r", top_affect, top_area);
    ch_printf(ch, "ExtDes  %5d    Exits   %5d\n\r", top_ed, top_exit);
    ch_printf(ch, "Helps   %5d    Resets  %5d\n\r", top_help, top_reset);
    ch_printf(ch, "IdxMobs %5zu    Mobs    %5d\n\r", g_mobIndex.size(), nummobsloaded);
    ch_printf(ch, "IdxObjs %5zu    Objs    %5d (%d)\n\r", g_objectIndex.size(), numobjsloaded, physicalobjects);
    ch_printf(ch, "Rooms   %5zu    VRooms  %5zu\n\r", g_roomIndex.size(), g_vrooms.size());
    ch_printf(ch, "Shops   %5d    RepShps %5d\n\r", top_shop, top_repair);
    ch_printf(ch, "CurOq's %5d    CurCq's %5d\n\r", cur_qobjs, cur_qchars);
    ch_printf(ch, "Players %5d    Maxplrs %5d\n\r", -1, sysdata.maxplayers); // TODO num_descriptors ded :(
    ch_printf(ch, "MaxEver %5d    Topsn   %5d (%d)\n\r", sysdata.alltimemax, top_sn, MAX_SKILL);
    ch_printf(ch, "MaxEver time recorded at:   %s\n\r", sysdata.time_of_max);
    if (!str_cmp(arg, "check"))
    {
#ifdef HASHSTR
        send_to_char(check_hash(argument), ch);
#else
        send_to_char("Hash strings not enabled.\n\r", ch);
#endif
        return;
    }
    if (!str_cmp(arg, "showhigh"))
    {
#ifdef HASHSTR
        show_high_hash(atoi(argument));
#else
        send_to_char("Hash strings not enabled.\n\r", ch);
#endif
        return;
    }
    if (argument[0] != '\0')
        hash = atoi(argument);
    else
        hash = -1;
    if (!str_cmp(arg, "hash"))
    {
#ifdef HASHSTR
        ch_printf(ch, "Hash statistics:\n\r%s", hash_stats());
        if (hash != -1)
            hash_dump(hash);
#else
        send_to_char("Hash strings not enabled.\n\r", ch);
#endif
    }
    return;
}

/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy(int number)
{
    switch (number_bits(2))
    {
    case 0:
        number -= 1;
        break;
    case 3:
        number += 1;
        break;
    }

    return UMAX(1, number);
}

/*
 * Generate a random number.
 */

int number_range(int from, int to)
{
    if ((to - from) < 1)
        return from;
    return ((number_mm() % (to - from + 1)) + from);
}

/*
 * Generate a percentile roll.
 * number_mm() % 100 only does 0-99, changed to do 1-100 -Shaddai
 */
int number_percent(void)
{
    return (number_mm() % 100) + 1;
}

/*
 * Generate a random door.
 */
int number_door(void)
{
    int door;

    while ((door = number_mm() & (16 - 1)) > 9)
        ;

    return door;
    /*    return number_mm() & 10; */
}

int number_bits(int width)
{
    return number_mm() & ((1 << width) - 1);
}

/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static int rgiState[2 + 55];

void init_mm()
{
    int* piState;
    int iState;

    piState = &rgiState[2];

    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;

    piState[0] = ((int)current_time) & ((1 << 30) - 1);
    piState[1] = 1;
    for (iState = 2; iState < 55; iState++)
    {
        piState[iState] = (piState[iState - 1] + piState[iState - 2]) & ((1 << 30) - 1);
    }
    return;
}

int number_mm(void)
{
    int* piState;
    int iState1;
    int iState2;
    int iRand;

    piState = &rgiState[2];
    iState1 = piState[-2];
    iState2 = piState[-1];
    iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
    piState[iState1] = iRand;
    if (++iState1 == 55)
        iState1 = 0;
    if (++iState2 == 55)
        iState2 = 0;
    piState[-2] = iState1;
    piState[-1] = iState2;
    return iRand >> 6;
}

/*
 * Roll some dice.						-Thoric
 */
int dice(int number, int size)
{
    int idice;
    int sum;

    switch (size)
    {
    case 0:
        return 0;
    case 1:
        return number;
    }

    for (idice = 0, sum = 0; idice < number; idice++)
        sum += number_range(1, size);

    return sum;
}

/*
 * Simple linear interpolation.
 */
int interpolate(int level, int value_00, int value_32)
{
    return value_00 + level * (value_32 - value_00) / 32;
}

/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde(char* str)
{
    for (; *str != '\0'; str++)
        if (*str == '~')
            *str = '-';

    return;
}

/*
 * Encodes the tildes in a string.				-Thoric
 * Used for player-entered strings that go into disk files.
 */
void hide_tilde(char* str)
{
    for (; *str != '\0'; str++)
        if (*str == '~')
            *str = HIDDEN_TILDE;

    return;
}

char* show_tilde(const char* str)
{
    static char buf[MAX_STRING_LENGTH];
    char* bufptr;

    bufptr = buf;
    for (; *str != '\0'; str++, bufptr++)
    {
        if (*str == HIDDEN_TILDE)
            *bufptr = '~';
        else
            *bufptr = *str;
    }
    *bufptr = '\0';

    return buf;
}

/*
 * Compare strings, case insensitive.
 * Return true if different
 *   (compatibility with historical functions).
 */
bool str_cmp(const char* astr, const char* bstr)
{
    if (!astr)
    {
        // bug( "Str_cmp: null astr." );
        if (bstr)
            // fprintf( stderr, "str_cmp: astr: (null)  bstr: %s\n", bstr );
            return true;
    }

    if (!bstr)
    {
        // bug( "Str_cmp: null bstr." );
        if (astr)
            // fprintf( stderr, "str_cmp: astr: %s  bstr: (null)\n", astr );
            return true;
    }

    for (; *astr || *bstr; astr++, bstr++)
    {
        if (LOWER(*astr) != LOWER(*bstr))
            return true;
    }

    return false;
}

/*
 * Compare strings, case insensitive, for prefix matching.
 * Return true if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix(const char* astr, const char* bstr)
{
    if (!astr)
    {
        // bug( "Strn_cmp: null astr." );
        return true;
    }

    if (!bstr)
    {
        // bug( "Strn_cmp: null bstr." );
        return true;
    }

    for (; *astr; astr++, bstr++)
    {
        if (LOWER(*astr) != LOWER(*bstr))
            return true;
    }

    return false;
}

/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns true is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix(const char* astr, const char* bstr)
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ((c0 = LOWER(astr[0])) == '\0')
        return false;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for (ichar = 0; ichar <= sstr2 - sstr1; ichar++)
        if (c0 == LOWER(bstr[ichar]) && !str_prefix(astr, bstr + ichar))
            return false;

    return true;
}

/*
 * Compare strings, case insensitive, for suffix matching.
 * Return true if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix(const char* astr, const char* bstr)
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if (sstr1 <= sstr2 && !str_cmp(astr, bstr + sstr2 - sstr1))
        return false;
    else
        return true;
}

/*
 * Returns an initial-capped string.
 */
std::string capitalize(const std::string_view& str)
{
    std::string result = strlower(str);

    if (!result.empty())
        result[0] = UPPER(result[0]);

    return result;
}

/*
 * Returns a lowercase string.
 */
std::string strlower(const std::string_view& str)
{
    std::string result{str};

    for (char& c : result)
        c = LOWER(c);

    return result;
}

/*
 * Returns an uppercase string.
 */
std::string strupper(const std::string_view& str)
{
    std::string result{str};

    for (char& c : result)
        c = UPPER(c);

    return result;
}

/*
 * Returns true or false if a letter is a vowel			-Thoric
 */
bool isavowel(char letter)
{
    char c;

    c = tolower(letter);
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        return true;
    else
        return false;
}

/*
 * Shove either "a " or "an " onto the beginning of a string	-Thoric
 */
std::string aoran(const std::string_view& str)
{
    if (str.empty())
    {
        bug("Aoran(): empty str");
        return "";
    }

    if (isavowel(str[0]) || (str.size() > 1 && tolower(str[0]) == 'y' && !isavowel(str[1])))
        return std::string{"an "} + std::string{str};
    else
        return std::string{"a "} + std::string{str};
}

/*
 * Append a string to a file.
 */
void append_file(CHAR_DATA* ch, const char* file, char* str)
{
    FILE* fp;

    if (IS_NPC(ch) || str[0] == '\0')
        return;

    if ((fp = fopen(file, "a")) == nullptr)
    {
        send_to_char("Could not open the file!\n\r", ch);
    }
    else
    {
        fprintf(fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str);
        fclose(fp);
    }

    return;
}

/*
 * Append a string to a file.
 */
void append_to_file(char const* file, char* str)
{
    FILE* fp;

    if ((fp = fopen(file, "a")) == nullptr)
    {
    }
    else
    {
        fprintf(fp, "%s\n", str);
        fclose(fp);
    }

    return;
}

/*
 * Prepend a string to a file.
 */
void prepend_to_file(char* file, char* str)
{
    char temp[MAX_STRING_LENGTH];
    char tempname[MAX_STRING_LENGTH];

    sprintf_s(tempname, "%s.temp", file);
    rename(file, tempname);
    append_to_file(file, str);
    sprintf_s(temp, "cat %s >> %s", tempname, file);
    system(temp);
    remove(tempname);
    return;
}

/*
 * Reports a bug.
 */
void bug(const char* str, ...)
{
    char buf[MAX_STRING_LENGTH];
    FILE* fp;
    struct stat fst;

    if (fpArea != nullptr)
    {
        int iChar;

        if (fpArea == stdin)
        {
            // iLine = 0;
        }
        else
        {
            iChar = ftell(fpArea);
            /*
            fseek(fpArea, 0, 0);
            for (iLine = 0; ftell(fpArea) < iChar; iLine++)
            {
                while (getc(fpArea) != '\n')
                    ;
            }
            fseek(fpArea, iChar, 0);
            */
        }

        sprintf_s(buf, "[*****] FILE: %s POS: %d", strArea, iChar);
        log_string(buf);

        if (stat(SHUTDOWN_FILE, &fst) != -1) /* file exists */
        {
            if ((fp = fopen(SHUTDOWN_FILE, "a")) != nullptr)
            {
                fprintf(fp, "[*****] %s\n", buf);
                fclose(fp);
            }
        }
    }

    strcpy_s(buf, "[*****] BUG: ");
    {
        va_list param;

        va_start(param, str);
        vsprintf(buf + strlen(buf), str, param);
        va_end(param);
    }
    log_string(buf);

    if ((fp = fopen(BUG_FILE, "a")) != nullptr)
    {
        fprintf(fp, "%s\n", buf);
        fclose(fp);
    }

    return;
}

/*
 * Add a string to the boot-up log				-Thoric
 */
void boot_log(const char* str, ...)
{
    char buf[MAX_STRING_LENGTH];
    FILE* fp;
    va_list param;

    strcpy_s(buf, "[*****] BOOT: ");
    va_start(param, str);
    vsprintf(buf + strlen(buf), str, param);
    va_end(param);
    log_string(buf);

    if ((fp = fopen(BOOTLOG_FILE, "a")) != nullptr)
    {
        fprintf(fp, "%s\n", buf);
        fclose(fp);
    }

    return;
}

/*
 * Dump a text file to a player, a line at a time		-Thoric
 */
void show_file(CHAR_DATA* ch, char const* filename)
{
    FILE* fp;
    char buf[MAX_STRING_LENGTH];
    int c;
    int num = 0;

    if ((fp = fopen(filename, "r")) != nullptr)
    {
        while (!feof(fp))
        {
            while ((buf[num] = fgetc(fp)) != EOF && buf[num] != '\n' && buf[num] != '\r' &&
                   num < (MAX_STRING_LENGTH - 2))
                num++;
            c = fgetc(fp);
            if ((c != '\n' && c != '\r') || c == buf[num])
                ungetc(c, fp);
            buf[num++] = '\n';
            buf[num++] = '\r';
            buf[num] = '\0';
            send_to_pager(buf, ch);
            num = 0;
        }
        fclose(fp);
    }
}

/*
 * Show the boot log file					-Thoric
 */
void do_dmesg(CHAR_DATA* ch, char* argument)
{
    set_pager_color(AT_LOG, ch);
    show_file(ch, BOOTLOG_FILE);
}

/*
 * Writes a string to the log, extended version			-Thoric
 */
void log_string_plus(const char* str, sh_int log_type, sh_int level)
{
    char* strtime;
    int offset;

    strtime = ctime(&current_time);
    strtime[strlen(strtime) - 1] = '\0';
    fprintf(stderr, "%s :: %s\n", strtime, str);
    if (strncmp(str, "Log ", 4) == 0)
        offset = 4;
    else
        offset = 0;
    switch (log_type)
    {
    default:
        to_channel(str + offset, CHANNEL_LOG, "Log", level);
        break;
    case LOG_BUILD:
        to_channel(str + offset, CHANNEL_BUILD, "Build", level);
        break;
    case LOG_COMM:
        to_channel(str + offset, CHANNEL_COMM, "Comm", level);
        break;
    case LOG_ALL:
        break;
    }
    return;
}

void log(const char* str, sh_int log_type, sh_int level, ...)
{
    char buf[MAX_STRING_LENGTH];
    {
        va_list param;

        va_start(param, level);
        vsprintf(buf, str, param);
        va_end(param);
    }
    log_string_plus(buf, log_type, level);
}

/*
 * wizlist builder!						-Thoric
 */

void towizfile(const char* line, bool Border)
{
    int filler = 0;
    int xx = 0;
    int ofiller = 0;
    char outline[MAX_STRING_LENGTH] = {};
    char outline2[MAX_STRING_LENGTH] = {};
    FILE* wfp = nullptr;
    bool SNIP = false;
    outline[0] = '\0';
    outline2[0] = '\0';
    if (line && line[0] != '\0')
    {
        filler = (78 - strlen(line));
        if (filler < 1)
            filler = 1;
        ofiller = filler;
        if (Border && filler % 2 == 0)
            SNIP = true;
        filler /= 2;
        if (SNIP)
            filler--;
        if (Border)
            filler -= 10;
        for (xx = 0; xx < filler; xx++)
            strcat_s(outline, " ");
        strcat_s(outline, line);
    }
    if (Border)
    {
        if ((filler * 2) != ofiller)
            filler++;
        if (SNIP)
            filler++;
        sprintf_s(outline2, "         &C| |&R%s&C", outline);
        for (xx = 0; xx < filler; xx++)
            strcat_s(outline2, " ");
        strcat_s(outline2, "&C| |");
    }
    else
        strcpy_s(outline2, outline);
    strcat_s(outline2, "\n\r");
    wfp = fopen(WIZLIST_FILE, "a");
    if (wfp)
    {
        fputs(outline2, wfp);
        fclose(wfp);
    }
}

void add_to_wizlist(char const* name, int level)
{
    WIZENT *wiz, *tmp;

#ifdef DEBUG
    log_string("Adding to wizlist...");
#endif

    CREATE(wiz, WIZENT, 1);
    wiz->name = str_dup(name);
    wiz->level = level;

    if (!first_wiz)
    {
        wiz->last = nullptr;
        wiz->next = nullptr;
        first_wiz = wiz;
        last_wiz = wiz;
        return;
    }

    /* insert sort, of sorts */
    for (tmp = first_wiz; tmp; tmp = tmp->next)
        if (level > tmp->level)
        {
            if (!tmp->last)
                first_wiz = wiz;
            else
                tmp->last->next = wiz;
            wiz->last = tmp->last;
            wiz->next = tmp;
            tmp->last = wiz;
            return;
        }

    wiz->last = last_wiz;
    wiz->next = nullptr;
    last_wiz->next = wiz;
    last_wiz = wiz;
    return;
}

/*
 * Wizlist builder						-Thoric
 */
void make_wizlist()
{
    FILE* gfp;
    char const* word;
    int ilevel, iflags;
    WIZENT *wiz, *wiznext;
    char buf[MAX_STRING_LENGTH];
    char mudnamebuf[MSL];

    first_wiz = nullptr;
    last_wiz = nullptr;

    ilevel = 0;

    for (auto& file : std::filesystem::directory_iterator(GOD_DIR))
    {
        gfp = fopen(file.path().string().c_str(), "r");
        if (gfp)
        {
            word = feof(gfp) ? "End" : fread_word(gfp);
            ilevel = fread_number(gfp);
            fread_to_eol(gfp);
            word = feof(gfp) ? "End" : fread_word(gfp);
            if (!str_cmp(word, "Pcflags"))
                iflags = fread_number(gfp);
            else
                iflags = 0;
            fclose(gfp);
            if (IS_SET(iflags, PCFLAG_RETIRED))
                ilevel = MAX_LEVEL - 5;
            if (IS_SET(iflags, PCFLAG_GUEST))
                ilevel = MAX_LEVEL - 10;
            add_to_wizlist(file.path().string().c_str(), ilevel);
        }
    }

    buf[0] = '\0';
    unlink(WIZLIST_FILE);
    towizfile(" ", 0);
    towizfile("&B ________   ___   ____                  ", 0);
    towizfile("&B/ __   __| / _ \\ |  _ \\                ", 0);
    towizfile(" &C           _____ &B> \\ | |   |  _  ||    / &C____________________________ ", 0);
    towizfile(" &C          / ___ &B<__/ |_|   |_| |_||_|\\__> &C__________________________ \\ ", 0);
    towizfile(" &C       / /                                                        \\ \\ ", 0);

    sprintf_s(mudnamebuf, " &C        | |              &Y%-40.40s&C    | | ", sysdata.mudname);
    towizfile(mudnamebuf, 0);
    ilevel = MAX_LEVEL + 1;
    for (wiz = first_wiz; wiz; wiz = wiz->next)
    {
        if (wiz->level > MAX_LEVEL - 11)
        {
            if (wiz->level < ilevel)
            {
                if (buf[0])
                {
                    towizfile(buf, 1);
                    buf[0] = '\0';
                }
                towizfile(" &C        | |&B                                                          &C| &C|", 0);
                ilevel = wiz->level;
                switch (ilevel)
                {

                case LEVEL_IMPLEMENTOR:
                    towizfile(" &C        | &C|                        &W&GThe Admin &C                        | &C|&C",
                              0);
                    break;
                case LEVEL_SUB_IMPLEM:
                    towizfile(" &C        | &C|                        &W&GHead Imms    &C                     | &C| ",
                              0);
                    break;
                case LEVEL_ASCENDANT:
                    towizfile(" &C        | &C|                     &W&GChief Builders  &C                     | &C| ",
                              0);
                    break;
                case LEVEL_GREATER:
                    towizfile(" &C        | &C|                     &W&GThe Architects  &C                     | &C| ",
                              0);
                    break;
                case LEVEL_LESSER:
                    towizfile(" &C        | &C|                     &W&GThe Apprentices  &C                    | &C| ",
                              0);
                    break;
                case LEVEL_RETIRED:
                    towizfile(" &C        | &C|                       &W&GThe Retired   &C                     | &C| ",
                              0);
                    break;
                default:
                    towizfile(" &C        | &C|                      &W&GThe Guest Imms   &C                   | &C| ",
                              0);
                    break;
                }
            }
            if (strlen(buf) + strlen(wiz->name) > 76)
            {
                towizfile(buf, 1);
                buf[0] = '\0';
            }
            strcat_s(buf, " ");
            strcat_s(buf, wiz->name);
            if (strlen(buf) > 70)
            {
                towizfile(buf, 1);
                buf[0] = '\0';
            }
        }
    }

    if (buf[0])
    {
        towizfile(buf, 1);
    }
    for (wiz = first_wiz; wiz; wiz = wiznext)
    {
        wiznext = wiz->next;
        DISPOSE(wiz->name);
        DISPOSE(wiz);
    }
    first_wiz = nullptr;
    last_wiz = nullptr;
    towizfile("&C         | |                                                          | &C|", 0);
    towizfile("&C          \\ \\__________________________ &B _    _  ___   ____   ___&C ___/ /", 0);
    towizfile("&C           \\___________________________ &B| |  | |/ _ \\ |  _ \\ / __>&C ___/", 0);
    towizfile("                                      &B| |/\\| || _  ||    / > \\&W     LS", 0);
    towizfile("                              &B\\_/\\_/ |_||_||_|\\_\\|__/", 0);
}

void do_makewizlist(CHAR_DATA* ch, char* argument)
{
    make_wizlist();
}

/* mud prog functions */

/* This routine reads in scripts of MUDprograms from a file */

int mprog_name_to_type(char* name)
{
    if (!str_cmp(name, "in_file_prog"))
        return IN_FILE_PROG;
    if (!str_cmp(name, "act_prog"))
        return ACT_PROG;
    if (!str_cmp(name, "speech_prog"))
        return SPEECH_PROG;
    if (!str_cmp(name, "rand_prog"))
        return RAND_PROG;
    if (!str_cmp(name, "fight_prog"))
        return FIGHT_PROG;
    if (!str_cmp(name, "hitprcnt_prog"))
        return HITPRCNT_PROG;
    if (!str_cmp(name, "death_prog"))
        return DEATH_PROG;
    if (!str_cmp(name, "entry_prog"))
        return ENTRY_PROG;
    if (!str_cmp(name, "greet_prog"))
        return GREET_PROG;
    if (!str_cmp(name, "all_greet_prog"))
        return ALL_GREET_PROG;
    if (!str_cmp(name, "give_prog"))
        return GIVE_PROG;
    if (!str_cmp(name, "bribe_prog"))
        return BRIBE_PROG;
    if (!str_cmp(name, "time_prog"))
        return TIME_PROG;
    if (!str_cmp(name, "hour_prog"))
        return HOUR_PROG;
    if (!str_cmp(name, "wear_prog"))
        return WEAR_PROG;
    if (!str_cmp(name, "remove_prog"))
        return REMOVE_PROG;
    if (!str_cmp(name, "sac_prog"))
        return SAC_PROG;
    if (!str_cmp(name, "look_prog"))
        return LOOK_PROG;
    if (!str_cmp(name, "exa_prog"))
        return EXA_PROG;
    if (!str_cmp(name, "custom_prog"))
        return CUSTOM_PROG;
    if (!str_cmp(name, "get_prog"))
        return GET_PROG;
    if (!str_cmp(name, "drop_prog"))
        return DROP_PROG;
    if (!str_cmp(name, "damage_prog"))
        return DAMAGE_PROG;
    if (!str_cmp(name, "repair_prog"))
        return REPAIR_PROG;
    if (!str_cmp(name, "greet_prog"))
        return GREET_PROG;
    if (!str_cmp(name, "randiw_prog"))
        return RANDIW_PROG;
    if (!str_cmp(name, "speechiw_prog"))
        return SPEECHIW_PROG;
    if (!str_cmp(name, "pull_prog"))
        return PULL_PROG;
    if (!str_cmp(name, "push_prog"))
        return PUSH_PROG;
    if (!str_cmp(name, "sleep_prog"))
        return SLEEP_PROG;
    if (!str_cmp(name, "rest_prog"))
        return REST_PROG;
    if (!str_cmp(name, "rfight_prog"))
        return FIGHT_PROG;
    if (!str_cmp(name, "enter_prog"))
        return ENTRY_PROG;
    if (!str_cmp(name, "leave_prog"))
        return LEAVE_PROG;
    if (!str_cmp(name, "rdeath_prog"))
        return DEATH_PROG;
    if (!str_cmp(name, "script_prog"))
        return SCRIPT_PROG;
    if (!str_cmp(name, "use_prog"))
        return USE_PROG;
    return (ERROR_PROG);
}

MPROG_DATA* mprog_file_read(char* f, MPROG_DATA* mprg, MOB_INDEX_DATA* pMobIndex)
{

    char MUDProgfile[MAX_INPUT_LENGTH];
    FILE* progfile;
    char letter;
    MPROG_DATA *mprg_next, *mprg2;
    bool done = false;

    sprintf_s(MUDProgfile, "%s%s", PROG_DIR, f);

    progfile = fopen(MUDProgfile, "r");
    if (!progfile)
    {
        bug("Mob: %d couldn't open mudprog file", pMobIndex->vnum);
        exit(1);
    }

    mprg2 = mprg;
    switch (letter = fread_letter(progfile))
    {
    case '>':
        break;
    case '|':
        bug("empty mudprog file.");
        exit(1);
        break;
    default:
        bug("in mudprog file syntax error.");
        exit(1);
        break;
    }

    while (!done)
    {
        mprg2->type = mprog_name_to_type(fread_word(progfile));
        switch (mprg2->type)
        {
        case ERROR_PROG:
            bug("mudprog file type error");
            exit(1);
            break;
        case IN_FILE_PROG:
            bug("mprog file contains a call to file.");
            exit(1);
            break;
        default:
            pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
            mprg2->arglist = fread_string(progfile);
            mprg2->comlist = fread_string(progfile);
            switch (letter = fread_letter(progfile))
            {
            case '>':
                CREATE(mprg_next, MPROG_DATA, 1);
                mprg_next->next = mprg2;
                mprg2 = mprg_next;
                break;
            case '|':
                done = true;
                break;
            default:
                bug("in mudprog file syntax error.");
                exit(1);
                break;
            }
            break;
        }
    }
    fclose(progfile);
    return mprg2;
}

/* Load a MUDprogram section from the area file.
 */
void load_mudprogs(AREA_DATA* tarea, FILE* fp)
{
    MOB_INDEX_DATA* iMob;
    MPROG_DATA* original;
    MPROG_DATA* working;
    char letter;
    int value;

    for (;;)
        switch (letter = fread_letter(fp))
        {
        default:
            bug("Load_mudprogs: bad command '%c'.", letter);
            exit(1);
            break;
        case 'S':
        case 's':
            fread_to_eol(fp);
            return;
        case '*':
            fread_to_eol(fp);
            break;
        case 'M':
        case 'm':
            value = fread_number(fp);
            if ((iMob = get_mob_index(value)) == nullptr)
            {
                bug("Load_mudprogs: vnum %d doesnt exist", value);
                exit(1);
            }

            /* Go to the end of the prog command list if other commands
           exist */

            if ((original = iMob->mudprogs) != nullptr)
                for (; original->next; original = original->next)
                    ;

            CREATE(working, MPROG_DATA, 1);
            if (original)
                original->next = working;
            else
                iMob->mudprogs = working;
            working = mprog_file_read(fread_word(fp), working, iMob);
            working->next = nullptr;
            fread_to_eol(fp);
            break;
        }

    return;
}

/* This procedure is responsible for reading any in_file MUDprograms.
 */

void mprog_read_programs(FILE* fp, MOB_INDEX_DATA* pMobIndex)
{
    MPROG_DATA* mprg;
    char letter;
    bool done = false;

    if ((letter = fread_letter(fp)) != '>')
    {
        bug("Load_mobiles: vnum %d MUDPROG char", pMobIndex->vnum);
        exit(1);
    }
    CREATE(mprg, MPROG_DATA, 1);
    pMobIndex->mudprogs = mprg;

    while (!done)
    {
        mprg->type = mprog_name_to_type(fread_word(fp));
        switch (mprg->type)
        {
        case ERROR_PROG:
            bug("Load_mobiles: vnum %d MUDPROG type.", pMobIndex->vnum);
            exit(1);
            break;
        case IN_FILE_PROG:
            mprg = mprog_file_read(fread_string(fp), mprg, pMobIndex);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_mobiles: vnum %d bad MUDPROG.", pMobIndex->vnum);
                exit(1);
                break;
            }
            break;
        default:
            pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
            mprg->arglist = fread_string(fp);
            fread_to_eol(fp);
            mprg->comlist = fread_string(fp);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_mobiles: vnum %d bad MUDPROG.", pMobIndex->vnum);
                exit(1);
                break;
            }
            break;
        }
    }

    return;
}

/*************************************************************/
/* obj prog functions */
/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */

/* This routine reads in scripts of OBJprograms from a file */

MPROG_DATA* oprog_file_read(char* f, MPROG_DATA* mprg, OBJ_INDEX_DATA* pObjIndex)
{

    char MUDProgfile[MAX_INPUT_LENGTH];
    FILE* progfile;
    char letter;
    MPROG_DATA *mprg_next, *mprg2;
    bool done = false;

    sprintf_s(MUDProgfile, "%s%s", PROG_DIR, f);

    progfile = fopen(MUDProgfile, "r");
    if (!progfile)
    {
        bug("Obj: %d couldnt open mudprog file", pObjIndex->vnum);
        exit(1);
    }

    mprg2 = mprg;
    switch (letter = fread_letter(progfile))
    {
    case '>':
        break;
    case '|':
        bug("empty objprog file.");
        exit(1);
        break;
    default:
        bug("in objprog file syntax error.");
        exit(1);
        break;
    }

    while (!done)
    {
        mprg2->type = mprog_name_to_type(fread_word(progfile));
        switch (mprg2->type)
        {
        case ERROR_PROG:
            bug("objprog file type error");
            exit(1);
            break;
        case IN_FILE_PROG:
            bug("objprog file contains a call to file.");
            exit(1);
            break;
        default:
            pObjIndex->progtypes = pObjIndex->progtypes | mprg2->type;
            mprg2->arglist = fread_string(progfile);
            mprg2->comlist = fread_string(progfile);
            switch (letter = fread_letter(progfile))
            {
            case '>':
                CREATE(mprg_next, MPROG_DATA, 1);
                mprg_next->next = mprg2;
                mprg2 = mprg_next;
                break;
            case '|':
                done = true;
                break;
            default:
                bug("in objprog file syntax error.");
                exit(1);
                break;
            }
            break;
        }
    }
    fclose(progfile);
    return mprg2;
}

/* Load a MUDprogram section from the area file.
 */
void load_objprogs(AREA_DATA* tarea, FILE* fp)
{
    OBJ_INDEX_DATA* iObj;
    MPROG_DATA* original;
    MPROG_DATA* working;
    char letter;
    int value;

    for (;;)
        switch (letter = fread_letter(fp))
        {
        default:
            bug("Load_objprogs: bad command '%c'.", letter);
            exit(1);
            break;
        case 'S':
        case 's':
            fread_to_eol(fp);
            return;
        case '*':
            fread_to_eol(fp);
            break;
        case 'M':
        case 'm':
            value = fread_number(fp);
            if ((iObj = get_obj_index(value)) == nullptr)
            {
                bug("Load_objprogs: vnum %d doesnt exist", value);
                exit(1);
            }

            /* Go to the end of the prog command list if other commands
           exist */

            if ((original = iObj->mudprogs) != nullptr)
                for (; original->next; original = original->next)
                    ;

            CREATE(working, MPROG_DATA, 1);
            if (original)
                original->next = working;
            else
                iObj->mudprogs = working;
            working = oprog_file_read(fread_word(fp), working, iObj);
            working->next = nullptr;
            fread_to_eol(fp);
            break;
        }

    return;
}

/* This procedure is responsible for reading any in_file OBJprograms.
 */

void oprog_read_programs(FILE* fp, OBJ_INDEX_DATA* pObjIndex)
{
    MPROG_DATA* mprg;
    char letter;
    bool done = false;

    if ((letter = fread_letter(fp)) != '>')
    {
        bug("Load_objects: vnum %d OBJPROG char", pObjIndex->vnum);
        exit(1);
    }
    CREATE(mprg, MPROG_DATA, 1);
    pObjIndex->mudprogs = mprg;

    while (!done)
    {
        mprg->type = mprog_name_to_type(fread_word(fp));
        switch (mprg->type)
        {
        case ERROR_PROG:
            bug("Load_objects: vnum %d OBJPROG type.", pObjIndex->vnum);
            exit(1);
            break;
        case IN_FILE_PROG:
            mprg = oprog_file_read(fread_string(fp), mprg, pObjIndex);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_objects: vnum %d bad OBJPROG.", pObjIndex->vnum);
                exit(1);
                break;
            }
            break;
        default:
            pObjIndex->progtypes = pObjIndex->progtypes | mprg->type;
            mprg->arglist = fread_string(fp);
            fread_to_eol(fp);
            mprg->comlist = fread_string(fp);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_objects: vnum %d bad OBJPROG.", pObjIndex->vnum);
                exit(1);
                break;
            }
            break;
        }
    }

    return;
}

/*************************************************************/
/* room prog functions */
/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */

/* This routine reads in scripts of OBJprograms from a file */
MPROG_DATA* rprog_file_read(char* f, MPROG_DATA* mprg, ROOM_INDEX_DATA* RoomIndex)
{

    char MUDProgfile[MAX_INPUT_LENGTH];
    FILE* progfile;
    char letter;
    MPROG_DATA *mprg_next, *mprg2;
    bool done = false;

    sprintf_s(MUDProgfile, "%s%s", PROG_DIR, f);

    progfile = fopen(MUDProgfile, "r");
    if (!progfile)
    {
        bug("Room: %d couldnt open roomprog file", RoomIndex->vnum);
        exit(1);
    }

    mprg2 = mprg;
    switch (letter = fread_letter(progfile))
    {
    case '>':
        break;
    case '|':
        bug("empty roomprog file.");
        exit(1);
        break;
    default:
        bug("in roomprog file syntax error.");
        exit(1);
        break;
    }

    while (!done)
    {
        mprg2->type = mprog_name_to_type(fread_word(progfile));
        switch (mprg2->type)
        {
        case ERROR_PROG:
            bug("roomprog file type error");
            exit(1);
            break;
        case IN_FILE_PROG:
            bug("roomprog file contains a call to file.");
            exit(1);
            break;
        default:
            RoomIndex->progtypes = RoomIndex->progtypes | mprg2->type;
            mprg2->arglist = fread_string(progfile);
            mprg2->comlist = fread_string(progfile);
            switch (letter = fread_letter(progfile))
            {
            case '>':
                CREATE(mprg_next, MPROG_DATA, 1);
                mprg_next->next = mprg2;
                mprg2 = mprg_next;
                break;
            case '|':
                done = true;
                break;
            default:
                bug("in roomprog file syntax error.");
                exit(1);
                break;
            }
            break;
        }
    }
    fclose(progfile);
    return mprg2;
}

/* Load a ROOMprogram section from the area file.
 */
void load_roomprogs(AREA_DATA* tarea, FILE* fp)
{
    ROOM_INDEX_DATA* iRoom;
    MPROG_DATA* original;
    MPROG_DATA* working;
    char letter;
    int value;

    for (;;)
        switch (letter = fread_letter(fp))
        {
        default:
            bug("Load_objprogs: bad command '%c'.", letter);
            exit(1);
            break;
        case 'S':
        case 's':
            fread_to_eol(fp);
            return;
        case '*':
            fread_to_eol(fp);
            break;
        case 'M':
        case 'm':
            value = fread_number(fp);
            if ((iRoom = get_room_index(value)) == nullptr)
            {
                bug("Load_roomprogs: vnum %d doesnt exist", value);
                exit(1);
            }

            /* Go to the end of the prog command list if other commands
           exist */

            if ((original = iRoom->mudprogs) != nullptr)
                for (; original->next; original = original->next)
                    ;

            CREATE(working, MPROG_DATA, 1);
            if (original)
                original->next = working;
            else
                iRoom->mudprogs = working;
            working = rprog_file_read(fread_word(fp), working, iRoom);
            working->next = nullptr;
            fread_to_eol(fp);
            break;
        }

    return;
}

/* This procedure is responsible for reading any in_file ROOMprograms.
 */

void rprog_read_programs(FILE* fp, ROOM_INDEX_DATA* pRoomIndex)
{
    MPROG_DATA* mprg;
    char letter;
    bool done = false;

    if ((letter = fread_letter(fp)) != '>')
    {
        bug("Load_rooms: vnum %d ROOMPROG char", pRoomIndex->vnum);
        exit(1);
    }
    CREATE(mprg, MPROG_DATA, 1);
    pRoomIndex->mudprogs = mprg;

    while (!done)
    {
        mprg->type = mprog_name_to_type(fread_word(fp));
        switch (mprg->type)
        {
        case ERROR_PROG:
            bug("Load_rooms: vnum %d ROOMPROG type.", pRoomIndex->vnum);
            exit(1);
            break;
        case IN_FILE_PROG:
            mprg = rprog_file_read(fread_string(fp), mprg, pRoomIndex);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_rooms: vnum %d bad ROOMPROG.", pRoomIndex->vnum);
                exit(1);
                break;
            }
            break;
        default:
            pRoomIndex->progtypes = pRoomIndex->progtypes | mprg->type;
            mprg->arglist = fread_string(fp);
            fread_to_eol(fp);
            mprg->comlist = fread_string(fp);
            fread_to_eol(fp);
            switch (letter = fread_letter(fp))
            {
            case '>':
                CREATE(mprg->next, MPROG_DATA, 1);
                mprg = mprg->next;
                break;
            case '|':
                mprg->next = nullptr;
                fread_to_eol(fp);
                done = true;
                break;
            default:
                bug("Load_rooms: vnum %d bad ROOMPROG.", pRoomIndex->vnum);
                exit(1);
                break;
            }
            break;
        }
    }

    return;
}

/*************************************************************/
/* Function to delete a room index.  Called from do_rdelete in build.c
  Narn, May/96
  Don't ask me why they return bool.. :).. oh well.. -- Alty
  Don't ask me either, so I changed it to void. - Samson
*/
void delete_room(ROOM_INDEX_DATA* room)
{
    ROOM_INDEX_DATA *prev, *limbo = get_room_index(ROOM_VNUM_LIMBO);
    OBJ_DATA* o;
    CHAR_DATA* ch;
    EXTRA_DESCR_DATA* ed;
    EXIT_DATA* ex;
    MPROG_ACT_LIST* mpact;
    MPROG_DATA* mp;

    UNLINK(room, room->area->first_room, room->area->last_room, next_aroom, prev_aroom);

    while ((ch = room->first_person) != nullptr)
    {
        if (!IS_NPC(ch))
        {
            char_from_room(ch);
            char_to_room(ch, limbo);
        }
        else
            extract_char(ch, true);
    }
    while ((o = room->first_content) != nullptr)
    {
        extract_obj(o);
        wipe_resets(room);
    }

    while ((ed = room->first_extradesc) != nullptr)
    {
        room->first_extradesc = ed->next;
        STRFREE(ed->keyword);
        STRFREE(ed->description);
        DISPOSE(ed);
        --top_ed;
    }
    while ((ex = room->first_exit) != nullptr)
        extract_exit(room, ex);
    while ((mpact = room->mpact) != nullptr)
    {
        room->mpact = mpact->next;
        DISPOSE(mpact->buf);
        DISPOSE(mpact);
    }
    while ((mp = room->mudprogs) != nullptr)
    {
        room->mudprogs = mp->next;
        STRFREE(mp->arglist);
        STRFREE(mp->comlist);
        DISPOSE(mp);
    }
    STRFREE(room->name);
    STRFREE(room->description);

    if (!g_roomIndex.erase(room->vnum))
    {
        bug("delete_room: room %d not in hash bucket.", room->vnum);
    }

    DISPOSE(room);
    return;
}

/* See comment on delete_room. */
void delete_obj(OBJ_INDEX_DATA* obj)
{
    int hash;
    OBJ_INDEX_DATA* prev;
    OBJ_DATA *o, *o_next;
    EXTRA_DESCR_DATA* ed;
    AFFECT_DATA* af;
    MPROG_DATA* mp;

    /* Remove references to object index */
    for (o = first_object; o; o = o_next)
    {
        o_next = o->next;
        if (o->pIndexData == obj)
            extract_obj(o);
    }
    while ((ed = obj->first_extradesc) != nullptr)
    {
        obj->first_extradesc = ed->next;
        STRFREE(ed->keyword);
        STRFREE(ed->description);
        DISPOSE(ed);
        --top_ed;
    }
    while ((af = obj->first_affect) != nullptr)
    {
        obj->first_affect = af->next;
        DISPOSE(af);
        --top_affect;
    }
    while ((mp = obj->mudprogs) != nullptr)
    {
        obj->mudprogs = mp->next;
        STRFREE(mp->arglist);
        STRFREE(mp->comlist);
        DISPOSE(mp);
    }
    STRFREE(obj->name);
    STRFREE(obj->short_descr);
    STRFREE(obj->description);
    STRFREE(obj->action_desc);

    if (!g_objectIndex.erase(obj->vnum))
    {
        bug("delete_mob: object %d not in hash bucket.", obj->vnum);
    }

    DISPOSE(obj);
    return;
}

/* See comment on delete_room. */
void delete_mob(MOB_INDEX_DATA* mob)
{
    MOB_INDEX_DATA* prev;
    CHAR_DATA *ch, *ch_next;
    MPROG_DATA* mp;

    for (ch = first_char; ch; ch = ch_next)
    {
        ch_next = ch->next;
        if (ch->pIndexData == mob)
            extract_char(ch, true);
    }
    while ((mp = mob->mudprogs) != nullptr)
    {
        mob->mudprogs = mp->next;
        STRFREE(mp->arglist);
        STRFREE(mp->comlist);
        DISPOSE(mp);
    }

    if (mob->pShop)
    {
        UNLINK(mob->pShop, first_shop, last_shop, next, prev);
        DISPOSE(mob->pShop);
        --top_shop;
    }

    if (mob->rShop)
    {
        UNLINK(mob->rShop, first_repair, last_repair, next, prev);
        DISPOSE(mob->rShop);
        --top_repair;
    }

    STRFREE(mob->player_name);
    STRFREE(mob->short_descr);
    STRFREE(mob->long_descr);
    STRFREE(mob->description);

    if (!g_mobIndex.erase(mob->vnum))
    {
        bug("delete_mob: mobile %d not in hash bucket.", mob->vnum);
    }

    DISPOSE(mob);
    return;
}

/*
 * Creat a new room (for online building)			-Thoric
 */
ROOM_INDEX_DATA* make_room(int vnum, AREA_DATA* area)
{
    ROOM_INDEX_DATA* pRoomIndex = nullptr;

    CREATE(pRoomIndex, ROOM_INDEX_DATA, 1);
    pRoomIndex->first_person = nullptr;
    pRoomIndex->last_person = nullptr;
    pRoomIndex->first_content = nullptr;
    pRoomIndex->last_content = nullptr;
    pRoomIndex->first_reset = pRoomIndex->last_reset = nullptr;
    pRoomIndex->first_extradesc = nullptr;
    pRoomIndex->last_extradesc = nullptr;
    pRoomIndex->first_ship = nullptr;
    pRoomIndex->last_ship = nullptr;
    pRoomIndex->area = area;
    pRoomIndex->vnum = vnum;
    pRoomIndex->name = STRALLOC("Floating in a void");
    pRoomIndex->description = STRALLOC("");
    pRoomIndex->room_flags = ROOM_PROTOTYPE;
    pRoomIndex->sector_type = 1;
    pRoomIndex->light = 0;
    pRoomIndex->first_exit = nullptr;
    pRoomIndex->last_exit = nullptr;
    LINK(pRoomIndex, area->first_room, area->last_room, next_aroom, prev_aroom);

    g_roomIndex.insert({vnum, pRoomIndex});

    return pRoomIndex;
}

/*
 * Create a new INDEX object (for online building)		-Thoric
 * Option to clone an existing index object.
 */
OBJ_INDEX_DATA* make_object(int vnum, int cvnum, char* name)
{
    OBJ_INDEX_DATA *pObjIndex, *cObjIndex;
    char buf[MAX_STRING_LENGTH];

    if (cvnum > 0)
        cObjIndex = get_obj_index(cvnum);
    else
        cObjIndex = nullptr;
    CREATE(pObjIndex, OBJ_INDEX_DATA, 1);
    pObjIndex->vnum = vnum;
    pObjIndex->name = STRALLOC(name);
    pObjIndex->first_affect = nullptr;
    pObjIndex->last_affect = nullptr;
    pObjIndex->first_extradesc = nullptr;
    pObjIndex->last_extradesc = nullptr;
    if (!cObjIndex)
    {
        sprintf_s(buf, "A %s", name);
        pObjIndex->short_descr = STRALLOC(buf);
        sprintf_s(buf, "A %s is here.", name);
        pObjIndex->description = STRALLOC(buf);
        pObjIndex->action_desc = STRALLOC("");
        pObjIndex->short_descr[0] = LOWER(pObjIndex->short_descr[0]);
        pObjIndex->description[0] = UPPER(pObjIndex->description[0]);
        pObjIndex->item_type = ITEM_TRASH;
        pObjIndex->extra_flags = ITEM_PROTOTYPE;
        pObjIndex->wear_flags = 0;
        pObjIndex->value[0] = 0;
        pObjIndex->value[1] = 0;
        pObjIndex->value[2] = 0;
        pObjIndex->value[3] = 0;
        pObjIndex->value[4] = 0;
        pObjIndex->value[5] = 0;
        pObjIndex->weight = 1;
        pObjIndex->cost = 0;
    }
    else
    {
        EXTRA_DESCR_DATA *ed, *ced;
        AFFECT_DATA *paf, *cpaf;

        pObjIndex->short_descr = QUICKLINK(cObjIndex->short_descr);
        pObjIndex->description = QUICKLINK(cObjIndex->description);
        pObjIndex->action_desc = QUICKLINK(cObjIndex->action_desc);
        pObjIndex->item_type = cObjIndex->item_type;
        pObjIndex->extra_flags = cObjIndex->extra_flags | ITEM_PROTOTYPE;
        pObjIndex->wear_flags = cObjIndex->wear_flags;
        pObjIndex->value[0] = cObjIndex->value[0];
        pObjIndex->value[1] = cObjIndex->value[1];
        pObjIndex->value[2] = cObjIndex->value[2];
        pObjIndex->value[3] = cObjIndex->value[3];
        pObjIndex->value[4] = cObjIndex->value[4];
        pObjIndex->value[5] = cObjIndex->value[5];
        pObjIndex->weight = cObjIndex->weight;
        pObjIndex->cost = cObjIndex->cost;
        for (ced = cObjIndex->first_extradesc; ced; ced = ced->next)
        {
            CREATE(ed, EXTRA_DESCR_DATA, 1);
            ed->keyword = QUICKLINK(ced->keyword);
            ed->description = QUICKLINK(ced->description);
            LINK(ed, pObjIndex->first_extradesc, pObjIndex->last_extradesc, next, prev);
            top_ed++;
        }
        for (cpaf = cObjIndex->first_affect; cpaf; cpaf = cpaf->next)
        {
            CREATE(paf, AFFECT_DATA, 1);
            paf->type = cpaf->type;
            paf->duration = cpaf->duration;
            paf->location = cpaf->location;
            paf->modifier = cpaf->modifier;
            paf->bitvector = cpaf->bitvector;
            LINK(paf, pObjIndex->first_affect, pObjIndex->last_affect, next, prev);
            top_affect++;
        }
    }
    pObjIndex->count = 0;
    g_objectIndex.insert({vnum, pObjIndex});

    return pObjIndex;
}

/*
 * Create a new INDEX mobile (for online building)		-Thoric
 * Option to clone an existing index mobile.
 */
MOB_INDEX_DATA* make_mobile(int vnum, int cvnum, char* name)
{
    MOB_INDEX_DATA *pMobIndex = nullptr, *cMobIndex = nullptr;
    char buf[MAX_STRING_LENGTH];

    if (cvnum > 0)
        cMobIndex = get_mob_index(cvnum);
    else
        cMobIndex = nullptr;
    CREATE(pMobIndex, MOB_INDEX_DATA, 1);
    pMobIndex->vnum = vnum;
    pMobIndex->count = 0;
    pMobIndex->killed = 0;
    pMobIndex->player_name = STRALLOC(name);
    if (!cMobIndex)
    {
        sprintf_s(buf, "A newly created %s", name);
        pMobIndex->short_descr = STRALLOC(buf);
        sprintf_s(buf, "Some god abandoned a newly created %s here.\n\r", name);
        pMobIndex->long_descr = STRALLOC(buf);
        pMobIndex->description = STRALLOC("");
        pMobIndex->short_descr[0] = LOWER(pMobIndex->short_descr[0]);
        pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
        pMobIndex->description[0] = UPPER(pMobIndex->description[0]);
        pMobIndex->act = ACT_IS_NPC | ACT_PROTOTYPE;
        pMobIndex->affected_by = 0;
        pMobIndex->pShop = nullptr;
        pMobIndex->rShop = nullptr;
        pMobIndex->spec_fun = nullptr;
        pMobIndex->spec_2 = nullptr;
        pMobIndex->mudprogs = nullptr;
        pMobIndex->progtypes = 0;
        pMobIndex->alignment = 0;
        pMobIndex->level = 1;
        pMobIndex->mobthac0 = 0;
        pMobIndex->ac = 0;
        pMobIndex->hitnodice = 0;
        pMobIndex->hitsizedice = 0;
        pMobIndex->hitplus = 0;
        pMobIndex->damnodice = 0;
        pMobIndex->damsizedice = 0;
        pMobIndex->damplus = 0;
        pMobIndex->gold = 0;
        pMobIndex->exp = 0;
        pMobIndex->position = 8;
        pMobIndex->defposition = 8;
        pMobIndex->sex = 0;
        pMobIndex->perm_str = 10;
        pMobIndex->perm_dex = 10;
        pMobIndex->perm_int = 10;
        pMobIndex->perm_wis = 10;
        pMobIndex->perm_cha = 10;
        pMobIndex->perm_con = 10;
        pMobIndex->perm_lck = 10;
        pMobIndex->race = 0;
        pMobIndex->xflags = 0;
        pMobIndex->resistant = 0;
        pMobIndex->immune = 0;
        pMobIndex->susceptible = 0;
        pMobIndex->numattacks = 0;
        pMobIndex->attacks = 0;
        pMobIndex->defenses = 0;
    }
    else
    {
        pMobIndex->short_descr = QUICKLINK(cMobIndex->short_descr);
        pMobIndex->long_descr = QUICKLINK(cMobIndex->long_descr);
        pMobIndex->description = QUICKLINK(cMobIndex->description);
        pMobIndex->act = cMobIndex->act | ACT_PROTOTYPE;
        pMobIndex->affected_by = cMobIndex->affected_by;
        pMobIndex->pShop = nullptr;
        pMobIndex->rShop = nullptr;
        pMobIndex->spec_fun = cMobIndex->spec_fun;
        pMobIndex->spec_2 = cMobIndex->spec_2;
        pMobIndex->mudprogs = nullptr;
        pMobIndex->progtypes = 0;
        pMobIndex->alignment = cMobIndex->alignment;
        pMobIndex->level = cMobIndex->level;
        pMobIndex->mobthac0 = cMobIndex->mobthac0;
        pMobIndex->ac = cMobIndex->ac;
        pMobIndex->hitnodice = cMobIndex->hitnodice;
        pMobIndex->hitsizedice = cMobIndex->hitsizedice;
        pMobIndex->hitplus = cMobIndex->hitplus;
        pMobIndex->damnodice = cMobIndex->damnodice;
        pMobIndex->damsizedice = cMobIndex->damsizedice;
        pMobIndex->damplus = cMobIndex->damplus;
        pMobIndex->gold = cMobIndex->gold;
        pMobIndex->exp = cMobIndex->exp;
        pMobIndex->position = cMobIndex->position;
        pMobIndex->defposition = cMobIndex->defposition;
        pMobIndex->sex = cMobIndex->sex;
        pMobIndex->perm_str = cMobIndex->perm_str;
        pMobIndex->perm_dex = cMobIndex->perm_dex;
        pMobIndex->perm_int = cMobIndex->perm_int;
        pMobIndex->perm_wis = cMobIndex->perm_wis;
        pMobIndex->perm_cha = cMobIndex->perm_cha;
        pMobIndex->perm_con = cMobIndex->perm_con;
        pMobIndex->perm_lck = cMobIndex->perm_lck;
        pMobIndex->race = cMobIndex->race;
        pMobIndex->xflags = cMobIndex->xflags;
        pMobIndex->resistant = cMobIndex->resistant;
        pMobIndex->immune = cMobIndex->immune;
        pMobIndex->susceptible = cMobIndex->susceptible;
        pMobIndex->numattacks = cMobIndex->numattacks;
        pMobIndex->attacks = cMobIndex->attacks;
        pMobIndex->defenses = cMobIndex->defenses;
    }

    g_mobIndex.insert({vnum, pMobIndex});

    return pMobIndex;
}

/*
 * Creates a simple exit with no fields filled but rvnum and optionally
 * to_room and vnum.						-Thoric
 * Exits are inserted into the linked list based on vdir.
 */
EXIT_DATA* make_exit(ROOM_INDEX_DATA* pRoomIndex, ROOM_INDEX_DATA* to_room, sh_int door)
{
    EXIT_DATA *pexit, *texit;
    bool broke;

    CREATE(pexit, EXIT_DATA, 1);
    pexit->vdir = door;
    pexit->rvnum = pRoomIndex->vnum;
    pexit->to_room = to_room;
    pexit->distance = 1;
    if (to_room)
    {
        pexit->vnum = to_room->vnum;
        texit = get_exit_to(to_room, rev_dir[door], pRoomIndex->vnum);
        if (texit) /* assign reverse exit pointers */
        {
            texit->rexit = pexit;
            pexit->rexit = texit;
        }
    }
    broke = false;
    for (texit = pRoomIndex->first_exit; texit; texit = texit->next)
        if (door < texit->vdir)
        {
            broke = true;
            break;
        }
    if (!pRoomIndex->first_exit)
        pRoomIndex->first_exit = pexit;
    else
    {
        /* keep exits in incremental order - insert exit into list */
        if (broke && texit)
        {
            if (!texit->prev)
                pRoomIndex->first_exit = pexit;
            else
                texit->prev->next = pexit;
            pexit->prev = texit->prev;
            pexit->next = texit;
            texit->prev = pexit;
            top_exit++;
            return pexit;
        }
        pRoomIndex->last_exit->next = pexit;
    }
    pexit->next = nullptr;
    pexit->prev = pRoomIndex->last_exit;
    pRoomIndex->last_exit = pexit;
    top_exit++;
    return pexit;
}

void fix_area_exits(AREA_DATA* tarea)
{
    ROOM_INDEX_DATA* pRoomIndex;
    EXIT_DATA *pexit, *rev_exit;
    int rnum;
    bool fexit;

    for (rnum = tarea->low_r_vnum; rnum <= tarea->hi_r_vnum; rnum++)
    {
        if ((pRoomIndex = get_room_index(rnum)) == nullptr)
            continue;

        fexit = false;
        for (pexit = pRoomIndex->first_exit; pexit; pexit = pexit->next)
        {
            fexit = true;
            pexit->rvnum = pRoomIndex->vnum;
            if (pexit->vnum <= 0)
                pexit->to_room = nullptr;
            else
                pexit->to_room = get_room_index(pexit->vnum);
        }
        if (!fexit)
            SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
    }

    for (rnum = tarea->low_r_vnum; rnum <= tarea->hi_r_vnum; rnum++)
    {
        if ((pRoomIndex = get_room_index(rnum)) == nullptr)
            continue;

        for (pexit = pRoomIndex->first_exit; pexit; pexit = pexit->next)
        {
            if (pexit->to_room && !pexit->rexit)
            {
                rev_exit = get_exit_to(pexit->to_room, rev_dir[pexit->vdir], pRoomIndex->vnum);
                if (rev_exit)
                {
                    pexit->rexit = rev_exit;
                    rev_exit->rexit = pexit;
                }
            }
        }
    }
}

void load_area_file(AREA_DATA* tarea, char* filename)
{
    /*    FILE *fpin;
        what intelligent person stopped using fpArea?????
        if fpArea isn't being used, then no filename or linenumber
        is printed when an error occurs during loading the area..
        (bug uses fpArea)
          --TRI  */

    if (fBootDb)
        tarea = last_area;
    if (!fBootDb && !tarea)
    {
        bug("Load_area: null area!");
        return;
    }

    std::string file_path = std::string(AREA_DIR) + filename;

    if ((fpArea = fopen(file_path.c_str(), "r")) == nullptr)
    {
        bug("load_area: error loading file (can't open): %s", file_path.c_str());
        return;
    }

    for (;;)
    {
        char* word;

        if (fread_letter(fpArea) != '#')
        {
            bug(tarea->filename);
            bug("load_area: # not found.");
            exit(1);
        }

        word = fread_word(fpArea);

        if (word[0] == '$')
            break;
        else if (!str_cmp(word, "AREA"))
        {
            if (fBootDb)
            {
                load_area(fpArea);
                tarea = last_area;
            }
            else
            {
                DISPOSE(tarea->name);
                tarea->name = fread_string_nohash(fpArea);
            }
        }
        else if (!str_cmp(word, "AUTHOR"))
            load_author(tarea, fpArea);
        else if (!str_cmp(word, "FLAGS"))
            load_flags(tarea, fpArea);
        else if (!str_cmp(word, "RANGES"))
            load_ranges(tarea, fpArea);
        else if (!str_cmp(word, "ECONOMY"))
            load_economy(tarea, fpArea);
        else if (!str_cmp(word, "RESETMSG"))
            load_resetmsg(tarea, fpArea);
        /* Rennard */
        else if (!str_cmp(word, "HELPS"))
            load_helps(tarea, fpArea);
        else if (!str_cmp(word, "MOBILES"))
            load_mobiles(tarea, fpArea);
        else if (!str_cmp(word, "MUDPROGS"))
            load_mudprogs(tarea, fpArea);
        else if (!str_cmp(word, "OBJECTS"))
            load_objects(tarea, fpArea);
        else if (!str_cmp(word, "OBJPROGS"))
            load_objprogs(tarea, fpArea);
        else if (!str_cmp(word, "RESETS"))
            load_resets(tarea, fpArea);
        else if (!str_cmp(word, "ROOMS"))
            load_rooms(tarea, fpArea);
        else if (!str_cmp(word, "SHOPS"))
            load_shops(tarea, fpArea);
        else if (!str_cmp(word, "REPAIRS"))
            load_repairs(tarea, fpArea);
        else if (!str_cmp(word, "SPECIALS"))
            load_specials(tarea, fpArea);
        else
        {
            bug(tarea->filename);
            bug("load_area: bad section name.");
            if (fBootDb)
                exit(1);
            else
            {
                fclose(fpArea);
                fpArea = nullptr;
                return;
            }
        }
    }
    fclose(fpArea);
    fpArea = nullptr;
    if (tarea)
    {
        if (fBootDb)
            sort_area(tarea, false);

        fprintf(stderr, "%-14s: Rooms: %5d - %-5d Objs: %5d - %-5d Mobs: %5d - %d\n", tarea->filename,
                tarea->low_r_vnum, tarea->hi_r_vnum, tarea->low_o_vnum, tarea->hi_o_vnum, tarea->low_m_vnum,
                tarea->hi_m_vnum);
        if (!tarea->author)
            tarea->author = STRALLOC("");
        SET_BIT(tarea->status, AREA_LOADED);
    }
    else
        fprintf(stderr, "(%s)\n", filename);
}

/* Build list of in_progress areas.  Do not load areas.
 * define AREA_READ if you want it to build area names rather than reading
 * them out of the area files. -- Altrag */
void load_buildlist(void)
{
    FILE* fp;
    char buf[MAX_STRING_LENGTH];
    AREA_DATA* pArea;
    char line[81];
    char word[81];
    int low, hi;
    int mlow, mhi, olow, ohi, rlow, rhi;
    bool badfile = false;
    char temp;

    for (auto& entry : std::filesystem::directory_iterator(GOD_DIR))
    {
        if (entry.path().filename().c_str()[0] != '.')
        {
            sprintf_s(buf, "%s%s", GOD_DIR, entry.path().filename().string().c_str());
            if (!(fp = fopen(buf, "r")))
            {
                bug("Load_buildlist: invalid file");
                continue;
            }
            log_string(buf);
            badfile = false;
            rlow = rhi = olow = ohi = mlow = mhi = 0;
            while (!feof(fp) && !ferror(fp))
            {
                low = 0;
                hi = 0;
                word[0] = 0;
                line[0] = 0;
                if ((temp = fgetc(fp)) != EOF)
                    ungetc(temp, fp);
                else
                    break;

                fgets(line, 80, fp);
                sscanf(line, "%s %d %d", word, &low, &hi);
                if (!strcmp(word, "Level"))
                {
                    if (low < LEVEL_AVATAR)
                    {
                        sprintf_s(buf, "%s: God file with level %d < %d", entry.path().filename().string().c_str(), low,
                                  LEVEL_AVATAR);
                        badfile = true;
                    }
                }
                if (!strcmp(word, "RoomRange"))
                    rlow = low, rhi = hi;
                else if (!strcmp(word, "MobRange"))
                    mlow = low, mhi = hi;
                else if (!strcmp(word, "ObjRange"))
                    olow = low, ohi = hi;
            }
            fclose(fp);
            if (rlow && rhi && !badfile)
            {
                sprintf_s(buf, "%s%s.are", BUILD_DIR, entry.path().filename().string().c_str());
                if (!(fp = fopen(buf, "r")))
                {
                    bug("Load_buildlist: cannot open area file for read");
                    continue;
                }
#if !defined(READ_AREA) /* Dont always want to read stuff.. dunno.. shrug */

                strcpy_s(word, fread_word(fp));
                if (word[0] != '#' || strcmp(&word[1], "AREA"))
                {
                    sprintf_s(buf, "Make_buildlist: %s.are: no #AREA found.", entry.path().filename().string().c_str());
                    fclose(fp);
                    continue;
                }
#endif
                CREATE(pArea, AREA_DATA, 1);
                sprintf_s(buf, "%s.are", entry.path().filename().string().c_str());
                pArea->author = STRALLOC(entry.path().filename().string().c_str());
                pArea->filename = str_dup(buf);
#if !defined(READ_AREA)
                pArea->name = fread_string_nohash(fp);
#else
                sprintf_s(buf, "{PROTO} %s's area in progress", dentry->d_name);
                pArea->name = str_dup(buf);
#endif
                fclose(fp);
                pArea->low_r_vnum = rlow;
                pArea->hi_r_vnum = rhi;
                pArea->low_m_vnum = mlow;
                pArea->hi_m_vnum = mhi;
                pArea->low_o_vnum = olow;
                pArea->hi_o_vnum = ohi;
                pArea->low_soft_range = -1;
                pArea->hi_soft_range = -1;
                pArea->low_hard_range = -1;
                pArea->hi_hard_range = -1;
                pArea->first_room = pArea->last_room = nullptr;
                LINK(pArea, first_build, last_build, next, prev);
                fprintf(stderr,
                        "%-14s: Rooms: %5d - %-5d Objs: %5d - %-5d "
                        "Mobs: %5d - %-5d\n",
                        pArea->filename, pArea->low_r_vnum, pArea->hi_r_vnum, pArea->low_o_vnum, pArea->hi_o_vnum,
                        pArea->low_m_vnum, pArea->hi_m_vnum);
                sort_area(pArea, true);
            }
        }
    }
}

/*
 * Sort by room vnums					-Altrag & Thoric
 */
void sort_area(AREA_DATA* pArea, bool proto)
{
    AREA_DATA* area = nullptr;
    AREA_DATA *first_sort, *last_sort;
    bool found;

    if (!pArea)
    {
        bug("Sort_area: nullptr pArea");
        return;
    }

    if (proto)
    {
        first_sort = first_bsort;
        last_sort = last_bsort;
    }
    else
    {
        first_sort = first_asort;
        last_sort = last_asort;
    }

    found = false;
    pArea->next_sort = nullptr;
    pArea->prev_sort = nullptr;

    if (!first_sort)
    {
        pArea->prev_sort = nullptr;
        pArea->next_sort = nullptr;
        first_sort = pArea;
        last_sort = pArea;
        found = true;
    }
    else
        for (area = first_sort; area; area = area->next_sort)
            if (pArea->low_r_vnum < area->low_r_vnum)
            {
                if (!area->prev_sort)
                    first_sort = pArea;
                else
                    area->prev_sort->next_sort = pArea;
                pArea->prev_sort = area->prev_sort;
                pArea->next_sort = area;
                area->prev_sort = pArea;
                found = true;
                break;
            }

    if (!found)
    {
        pArea->prev_sort = last_sort;
        pArea->next_sort = nullptr;
        last_sort->next_sort = pArea;
        last_sort = pArea;
    }

    if (proto)
    {
        first_bsort = first_sort;
        last_bsort = last_sort;
    }
    else
    {
        first_asort = first_sort;
        last_asort = last_sort;
    }
}

/*
 * Display vnums currently assigned to areas		-Altrag & Thoric
 * Sorted, and flagged if loaded.
 */
void show_vnums(CHAR_DATA* ch, int low, int high, bool proto, bool shownl, const char* loadst, const char* notloadst)
{
    AREA_DATA *pArea, *first_sort;
    int count, loaded;

    count = 0;
    loaded = 0;
    set_pager_color(AT_PLAIN, ch);
    if (proto)
        first_sort = first_bsort;
    else
        first_sort = first_asort;
    for (pArea = first_sort; pArea; pArea = pArea->next_sort)
    {
        if (IS_SET(pArea->status, AREA_DELETED))
            continue;
        if (pArea->low_r_vnum < low)
            continue;
        if (pArea->hi_r_vnum > high)
            break;
        if (IS_SET(pArea->status, AREA_LOADED))
            loaded++;
        else if (!shownl)
            continue;
        pager_printf(ch,
                     "&w&W%-15s&B| &GRooms: &W%5d &G- &W%-5d"
                     " &GObjs: &W%5d &G- &W%-5d &GMobs: &W%5d &G- &W%-5d%s\n\r",
                     (pArea->filename ? pArea->filename : "(invalid)"), pArea->low_r_vnum, pArea->hi_r_vnum,
                     pArea->low_o_vnum, pArea->hi_o_vnum, pArea->low_m_vnum, pArea->hi_m_vnum,
                     IS_SET(pArea->status, AREA_LOADED) ? loadst : notloadst);
        count++;
    }
    pager_printf(ch, "&GAreas listed: &W%d  &GLoaded: &W%d\n\r", count, loaded);
    return;
}

/*
 * Shows prototype vnums ranges, and if loaded
 */

void do_vnums(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int low, high;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    low = 0;
    high = (MAX_VNUMS - 1);
    if (arg1[0] != '\0')
    {
        low = atoi(arg1);
        if (arg2[0] != '\0')
            high = atoi(arg2);
    }
    show_vnums(ch, low, high, true, true, " *", "");
}

/*
 * Shows installed areas, sorted.  Mark unloaded areas with an X
 */
void do_zones(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int low, high;

    do_vnums(ch, argument);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    low = 0;
    high = (MAX_VNUMS - 1);

    if (arg1[0] != '\0')
    {
        low = atoi(arg1);
        if (arg2[0] != '\0')
            high = atoi(arg2);
    }

    show_vnums(ch, low, high, false, true, "", " X");
}

/*
 * Show prototype areas, sorted.  Only show loaded areas
 */
void do_newzones(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int low, high;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    low = 0;
    high = (MAX_VNUMS - 1);
    if (arg1[0] != '\0')
    {
        low = atoi(arg1);
        if (arg2[0] != '\0')
            high = atoi(arg2);
    }
    show_vnums(ch, low, high, true, false, "", " X");
}

/*
 * Save system info to data file
 */
void save_sysdata(SYSTEM_DATA sys)
{
    FILE* fp;
    char filename[MAX_INPUT_LENGTH];

    sprintf_s(filename, "%ssysdata.dat", SYSTEM_DIR);

    if ((fp = fopen(filename, "w")) == nullptr)
    {
        bug("save_sysdata: fopen");
    }
    else
    {
        fprintf(fp, "#SYSTEM\n");
        fprintf(fp, "MudName        %s~\n", sys.mudname);
        fprintf(fp, "Acronym        %s~\n", sys.mud_acronym);
        fprintf(fp, "Highplayers    %d\n", sys.alltimemax);
        fprintf(fp, "Highplayertime %s~\n", sys.time_of_max);
        fprintf(fp, "Nameresolving  %d\n", sys.NO_NAME_RESOLVING);
        fprintf(fp, "Waitforauth    %d\n", sys.WAIT_FOR_AUTH);
        fprintf(fp, "Readallmail    %d\n", sys.read_all_mail);
        fprintf(fp, "Readmailfree   %d\n", sys.read_mail_free);
        fprintf(fp, "Writemailfree  %d\n", sys.write_mail_free);
        fprintf(fp, "Takeothersmail %d\n", sys.take_others_mail);
        fprintf(fp, "Muse           %d\n", sys.muse_level);
        fprintf(fp, "Think          %d\n", sys.think_level);
        fprintf(fp, "Build          %d\n", sys.build_level);
        fprintf(fp, "Log            %d\n", sys.log_level);
        fprintf(fp, "Protoflag      %d\n", sys.level_modify_proto);
        fprintf(fp, "Overridepriv   %d\n", sys.level_override_private);
        fprintf(fp, "Msetplayer     %d\n", sys.level_mset_player);
        fprintf(fp, "Stunplrvsplr   %d\n", sys.stun_plr_vs_plr);
        fprintf(fp, "Stunregular    %d\n", sys.stun_regular);
        fprintf(fp, "Damplrvsplr    %d\n", sys.dam_plr_vs_plr);
        fprintf(fp, "Damplrvsmob    %d\n", sys.dam_plr_vs_mob);
        fprintf(fp, "Dammobvsplr    %d\n", sys.dam_mob_vs_plr);
        fprintf(fp, "Dammobvsmob    %d\n", sys.dam_mob_vs_mob);
        fprintf(fp, "Forcepc        %d\n", sys.level_forcepc);
        fprintf(fp, "Guildoverseer  %s~\n", sys.guild_overseer);
        fprintf(fp, "Guildadvisor   %s~\n", sys.guild_advisor);
        fprintf(fp, "Saveflags      %d\n", sys.save_flags);
        fprintf(fp, "Savefreq       %d\n", sys.save_frequency);
        fprintf(fp, "Newbie_purge	%d\n", sys.newbie_purge);
        fprintf(fp, "Regular_purge	%d\n", sys.regular_purge);
        fprintf(fp, "Autopurge		%d\n", sys.CLEANPFILES);
        fprintf(fp, "Wizlock           %d\n", sys.wizlock);
        fprintf(fp, "PrivOverride	%d\n", sys.privwoverride);
        fprintf(fp, "End\n\n");
        fprintf(fp, "#END\n");
    }
    fclose(fp);
    return;
}

void fread_sysdata(SYSTEM_DATA* sys, FILE* fp)
{
    const char* word;
    bool fMatch;

    sys->time_of_max = nullptr;
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
            KEY("Autopurge", sys->CLEANPFILES, fread_number(fp));
            KEY("Acronym", sys->mud_acronym, fread_string_nohash(fp));
            break;
        case 'B':
            KEY("Build", sys->build_level, fread_number(fp));
            break;

        case 'D':
            KEY("Damplrvsplr", sys->dam_plr_vs_plr, fread_number(fp));
            KEY("Damplrvsmob", sys->dam_plr_vs_mob, fread_number(fp));
            KEY("Dammobvsplr", sys->dam_mob_vs_plr, fread_number(fp));
            KEY("Dammobvsmob", sys->dam_mob_vs_mob, fread_number(fp));
            break;

        case 'E':
            if (!str_cmp(word, "End"))
            {
                if (!sys->time_of_max)
                    sys->time_of_max = str_dup("(not recorded)");
                return;
            }
            break;

        case 'F':
            KEY("Forcepc", sys->level_forcepc, fread_number(fp));
            break;

        case 'G':
            KEY("Guildoverseer", sys->guild_overseer, fread_string(fp));
            KEY("Guildadvisor", sys->guild_advisor, fread_string(fp));
            break;

        case 'H':
            KEY("Highplayers", sys->alltimemax, fread_number(fp));
            KEY("Highplayertime", sys->time_of_max, fread_string_nohash(fp));
            break;

        case 'L':
            KEY("Log", sys->log_level, fread_number(fp));
            break;

        case 'M':
            KEY("Msetplayer", sys->level_mset_player, fread_number(fp));
            KEY("MudName", sys->mudname, fread_string_nohash(fp));
            KEY("Muse", sys->muse_level, fread_number(fp));
            break;

        case 'N':
            KEY("Nameresolving", sys->NO_NAME_RESOLVING, fread_number(fp));
            KEY("Newbie_purge", sys->newbie_purge, fread_number(fp));
            break;

        case 'O':
            KEY("Overridepriv", sys->level_override_private, fread_number(fp));
            break;

        case 'P':
            KEY("PrivOverride", sys->privwoverride, fread_number(fp));
            KEY("Protoflag", sys->level_modify_proto, fread_number(fp));
            break;

        case 'R':
            KEY("Readallmail", sys->read_all_mail, fread_number(fp));
            KEY("Readmailfree", sys->read_mail_free, fread_number(fp));
            KEY("Regular_purge", sys->regular_purge, fread_number(fp));
            break;

        case 'S':
            KEY("Stunplrvsplr", sys->stun_plr_vs_plr, fread_number(fp));
            KEY("Stunregular", sys->stun_regular, fread_number(fp));
            KEY("Saveflags", sys->save_flags, fread_number(fp));
            KEY("Savefreq", sys->save_frequency, fread_number(fp));
            break;

        case 'T':
            KEY("Takeothersmail", sys->take_others_mail, fread_number(fp));
            KEY("Think", sys->think_level, fread_number(fp));
            break;

        case 'W':
            KEY("Waitforauth", sys->WAIT_FOR_AUTH, fread_number(fp));
            KEY("Wizlock", sys->wizlock, fread_number(fp));
            KEY("Writemailfree", sys->write_mail_free, fread_number(fp));
            break;
        }

        if (!fMatch)
        {
            bug("Fread_sysdata: no match: %s", word);
        }
    }
}

/*
 * Load the sysdata file
 */
bool load_systemdata(SYSTEM_DATA* sys)
{
    char filename[MAX_INPUT_LENGTH];
    FILE* fp;
    bool found;

    found = false;
    sprintf_s(filename, "%ssysdata.dat", SYSTEM_DIR);

    if ((fp = fopen(filename, "r")) != nullptr)
    {

        found = true;
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
                bug("Load_sysdata_file: # not found.");
                break;
            }

            word = fread_word(fp);
            if (!str_cmp(word, "SYSTEM"))
            {
                fread_sysdata(sys, fp);
                break;
            }
            else if (!str_cmp(word, "END"))
                break;
            else
            {
                bug("Load_sysdata_file: bad section.");
                break;
            }
        }
        fclose(fp);
    }

    if (!sysdata.guild_overseer)
        sysdata.guild_overseer = str_dup("");
    if (!sysdata.guild_advisor)
        sysdata.guild_advisor = str_dup("");
    return found;
}

void load_banlist(void)
{
    BAN_DATA* pban;
    FILE* fp;
    int number;
    char letter;

    if (!(fp = fopen(SYSTEM_DIR BAN_LIST, "r")))
        return;

    for (;;)
    {
        if (feof(fp))
        {
            bug("Load_banlist: no -1 found.");
            fclose(fp);
            return;
        }
        number = fread_number(fp);
        if (number == -1)
        {
            fclose(fp);
            return;
        }
        CREATE(pban, BAN_DATA, 1);
        pban->level = number;
        pban->name = fread_string_nohash(fp);
        if ((letter = fread_letter(fp)) == '~')
            pban->ban_time = fread_string_nohash(fp);
        else
        {
            ungetc(letter, fp);
            pban->ban_time = str_dup("(unrecorded)");
        }
        LINK(pban, first_ban, last_ban, next, prev);
    }
}

/* Check to make sure range of vnums is free - Scryn 2/27/96 */

void do_check_vnums(CHAR_DATA* ch, char* argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    AREA_DATA* pArea;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    bool room, mob, obj, all, area_conflict;
    int low_range, high_range;

    room = false;
    mob = false;
    obj = false;
    all = false;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("Please specify room, mob, object, or all as your first argument.\n\r", ch);
        return;
    }

    if (!str_cmp(arg1, "room"))
        room = true;

    else if (!str_cmp(arg1, "mob"))
        mob = true;

    else if (!str_cmp(arg1, "object"))
        obj = true;

    else if (!str_cmp(arg1, "all"))
        all = true;
    else
    {
        send_to_char("Please specify room, mob, or object as your first argument.\n\r", ch);
        return;
    }

    if (arg2[0] == '\0')
    {
        send_to_char("Please specify the low end of the range to be searched.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char("Please specify the high end of the range to be searched.\n\r", ch);
        return;
    }

    low_range = atoi(arg2);
    high_range = atoi(argument);

    if (low_range < 1 || low_range > MAX_VNUMS)
    {
        send_to_char("Invalid argument for bottom of range.\n\r", ch);
        return;
    }

    if (high_range < 1 || high_range > MAX_VNUMS)
    {
        send_to_char("Invalid argument for top of range.\n\r", ch);
        return;
    }

    if (high_range < low_range)
    {
        send_to_char("Bottom of range must be below top of range.\n\r", ch);
        return;
    }

    if (all)
    {
        sprintf_s(buf, "room %d %d", low_range, high_range);
        do_check_vnums(ch, buf);
        sprintf_s(buf, "mob %d %d", low_range, high_range);
        do_check_vnums(ch, buf);
        sprintf_s(buf, "object %d %d", low_range, high_range);
        do_check_vnums(ch, buf);
        return;
    }
    set_char_color(AT_PLAIN, ch);

    for (pArea = first_asort; pArea; pArea = pArea->next_sort)
    {
        area_conflict = false;
        if (IS_SET(pArea->status, AREA_DELETED))
            continue;
        else if (room)
        {
            if (low_range < pArea->low_r_vnum && pArea->low_r_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_r_vnum && pArea->hi_r_vnum < high_range)
                area_conflict = true;

            if ((low_range >= pArea->low_r_vnum) && (low_range <= pArea->hi_r_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_r_vnum) && (high_range >= pArea->low_r_vnum))
                area_conflict = true;
        }

        if (mob)
        {
            if (low_range < pArea->low_m_vnum && pArea->low_m_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_m_vnum && pArea->hi_m_vnum < high_range)
                area_conflict = true;
            if ((low_range >= pArea->low_m_vnum) && (low_range <= pArea->hi_m_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_m_vnum) && (high_range >= pArea->low_m_vnum))
                area_conflict = true;
        }

        if (obj)
        {
            if (low_range < pArea->low_o_vnum && pArea->low_o_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_o_vnum && pArea->hi_o_vnum < high_range)
                area_conflict = true;

            if ((low_range >= pArea->low_o_vnum) && (low_range <= pArea->hi_o_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_o_vnum) && (high_range >= pArea->low_o_vnum))
                area_conflict = true;
        }

        if (area_conflict)
        {
            sprintf_s(buf, "Conflict:%-15s| ", (pArea->filename ? pArea->filename : "(invalid)"));
            if (room)
                sprintf_s(buf2, "Rooms: %5d - %-5d\n\r", pArea->low_r_vnum, pArea->hi_r_vnum);
            if (mob)
                sprintf_s(buf2, "Mobs: %5d - %-5d\n\r", pArea->low_m_vnum, pArea->hi_m_vnum);
            if (obj)
                sprintf_s(buf2, "Objects: %5d - %-5d\n\r", pArea->low_o_vnum, pArea->hi_o_vnum);

            strcat_s(buf, buf2);
            send_to_char(buf, ch);
        }
    }
    for (pArea = first_bsort; pArea; pArea = pArea->next_sort)
    {
        area_conflict = false;
        if (IS_SET(pArea->status, AREA_DELETED))
            continue;
        else if (room)
        {
            if (low_range < pArea->low_r_vnum && pArea->low_r_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_r_vnum && pArea->hi_r_vnum < high_range)
                area_conflict = true;

            if ((low_range >= pArea->low_r_vnum) && (low_range <= pArea->hi_r_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_r_vnum) && (high_range >= pArea->low_r_vnum))
                area_conflict = true;
        }

        if (mob)
        {
            if (low_range < pArea->low_m_vnum && pArea->low_m_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_m_vnum && pArea->hi_m_vnum < high_range)
                area_conflict = true;
            if ((low_range >= pArea->low_m_vnum) && (low_range <= pArea->hi_m_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_m_vnum) && (high_range >= pArea->low_m_vnum))
                area_conflict = true;
        }

        if (obj)
        {
            if (low_range < pArea->low_o_vnum && pArea->low_o_vnum < high_range)
                area_conflict = true;

            if (low_range < pArea->hi_o_vnum && pArea->hi_o_vnum < high_range)
                area_conflict = true;

            if ((low_range >= pArea->low_o_vnum) && (low_range <= pArea->hi_o_vnum))
                area_conflict = true;

            if ((high_range <= pArea->hi_o_vnum) && (high_range >= pArea->low_o_vnum))
                area_conflict = true;
        }

        if (area_conflict)
        {
            sprintf_s(buf, "Conflict:%-15s| ", (pArea->filename ? pArea->filename : "(invalid)"));
            if (room)
                sprintf_s(buf2, "Rooms: %5d - %-5d\n\r", pArea->low_r_vnum, pArea->hi_r_vnum);
            if (mob)
                sprintf_s(buf2, "Mobs: %5d - %-5d\n\r", pArea->low_m_vnum, pArea->hi_m_vnum);
            if (obj)
                sprintf_s(buf2, "Objects: %5d - %-5d\n\r", pArea->low_o_vnum, pArea->hi_o_vnum);

            strcat_s(buf, buf2);
            send_to_char(buf, ch);
        }
    }

    /*
        for ( pArea = first_asort; pArea; pArea = pArea->next_sort )
        {
            area_conflict = false;
        if ( IS_SET( pArea->status, AREA_DELETED ) )
           continue;
        else
        if (room)
          if((pArea->low_r_vnum >= low_range)
          && (pArea->hi_r_vnum <= high_range))
            area_conflict = true;

        if (mob)
          if((pArea->low_m_vnum >= low_range)
          && (pArea->hi_m_vnum <= high_range))
            area_conflict = true;

        if (obj)
          if((pArea->low_o_vnum >= low_range)
          && (pArea->hi_o_vnum <= high_range))
            area_conflict = true;

        if (area_conflict)
          ch_printf(ch, "Conflict:%-15s| Rooms: %5d - %-5d"
                 " Objs: %5d - %-5d Mobs: %5d - %-5d\n\r",
            (pArea->filename ? pArea->filename : "(invalid)"),
            pArea->low_r_vnum, pArea->hi_r_vnum,
            pArea->low_o_vnum, pArea->hi_o_vnum,
            pArea->low_m_vnum, pArea->hi_m_vnum );
        }

        for ( pArea = first_bsort; pArea; pArea = pArea->next_sort )
        {
            area_conflict = false;
        if ( IS_SET( pArea->status, AREA_DELETED ) )
           continue;
        else
        if (room)
          if((pArea->low_r_vnum >= low_range)
          && (pArea->hi_r_vnum <= high_range))
            area_conflict = true;

        if (mob)
          if((pArea->low_m_vnum >= low_range)
          && (pArea->hi_m_vnum <= high_range))
            area_conflict = true;

        if (obj)
          if((pArea->low_o_vnum >= low_range)
          && (pArea->hi_o_vnum <= high_range))
            area_conflict = true;

        if (area_conflict)
          sprintf_s(ch, "Conflict:%-15s| Rooms: %5d - %-5d"
                 " Objs: %5d - %-5d Mobs: %5d - %-5d\n\r",
            (pArea->filename ? pArea->filename : "(invalid)"),
            pArea->low_r_vnum, pArea->hi_r_vnum,
            pArea->low_o_vnum, pArea->hi_o_vnum,
            pArea->low_m_vnum, pArea->hi_m_vnum );
        }
    */
    return;
}

/* little proggy to center text with whitespace in between. */
std::string centertext(const std::string_view& text, int size)
{
    std::string cleanText = remand(text);
    int filler = (size - cleanText.size()) / 2 - 1;

    std::string result = " ";

    if (size == 0 || size < cleanText.size())
    {
        return "(null)";
    }

    if (size == cleanText.size())
    {
        return cleanText;
    }

    for (int i = 0; i < filler; i++)
        result += " ";

    result += text;

    for (int i = 0; i < filler; i++)
        result += " ";

    return result;
}

/*
 * Read a float number from a file. Turn the result into a float value.
 */
float fread_float(FILE* fp)
{
    float number;
    bool sign, decimal;
    char c;
    double place = 0;

    do
    {
        if (feof(fp))
        {
            bug("%s: EOF encountered on read.", __FUNCTION__);
            if (fBootDb)
            {
                shutdown_mud("Corrupt file somewhere.");
                exit(1);
            }
            return 0;
        }
        c = getc(fp);
    } while (isspace(c));

    number = 0;

    sign = false;
    decimal = false;

    if (c == '+')
        c = getc(fp);
    else if (c == '-')
    {
        sign = true;
        c = getc(fp);
    }

    if (!isdigit(c))
    {
        bug("%s: bad format. (%c)", __FUNCTION__, c);
        if (fBootDb)
            exit(1);
        return 0;
    }

    while (1)
    {
        if (c == '.' || isdigit(c))
        {
            if (c == '.')
            {
                decimal = true;
                c = getc(fp);
            }

            if (feof(fp))
            {
                bug("%s: EOF encountered on read.", __FUNCTION__);
                if (fBootDb)
                    exit(1);
                return number;
            }
            if (!decimal)
                number = number * 10 + c - '0';
            else
            {
                place++;
                number += pow(10, (-1 * place)) * (c - '0');
            }
            c = getc(fp);
        }
        else
            break;
    }

    if (sign)
        number = 0 - number;

    if (c == '|')
        number += fread_float(fp);
    else if (c != ' ')
        ungetc(c, fp);

    return number;
}
