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

/****************************************************************************
 *                   ^     +----- |  / ^     ^ |     | +-\                  *
 *                  / \    |      | /  |\   /| |     | |  \                 *
 *                 /   \   +---   |<   | \ / | |     | |  |                 *
 *                /-----\  |      | \  |  v  | |     | |  /                 *
 *               /       \ |      |  \ |     | +-----+ +-/                  *
 ****************************************************************************
 * AFKMud Copyright 1997-2003 by Roger Libiez (Samson),                     *
 * Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),           *
 * Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine, and Adjani.    *
 * All Rights Reserved.                                                     *
 *                                                                          *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,        *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,        *
 * Grishnakh, Fireblade, and Nivek.                                         *
 *                                                                          *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                      *
 *                                                                          *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,      *
 * Michael Seifert, and Sebastian Hammer.                                   *
 ****************************************************************************
 *               Color Module -- Allow user customizable Colors.            *
 *                                   --Matthew                              *
 *                      Enhanced ANSI parser by Samson                      *
 ****************************************************************************/

#define SAMSONCOLOR /* To interact with other snippets */

#define COLOR_DIR "../color/"

DECLARE_DO_FUN(do_color);

void reset_colors(CHAR_DATA* ch);
void set_char_color(short AType, CHAR_DATA* ch);
void set_pager_color(short AType, CHAR_DATA* ch);
const char* color_str(short AType, CHAR_DATA* ch);
const char* const_color_align(const char* argument, int size, int align);

/*
 * Color Alignment Parameters
 */
#define ALIGN_LEFT 1
#define ALIGN_CENTER 2
#define ALIGN_RIGHT 3

/* These are the ANSI codes for foreground text colors */
#define ANSI_BLACK "\x1b[0;30m"
#define ANSI_DRED "\x1b[0;31m"
#define ANSI_DGREEN "\x1b[0;32m"
#define ANSI_ORANGE "\x1b[0;33m"
#define ANSI_DBLUE "\x1b[0;34m"
#define ANSI_PURPLE "\x1b[0;35m"
#define ANSI_CYAN "\x1b[0;36m"
#define ANSI_GREY "\x1b[1;37m"
#define ANSI_DGREY "\x1b[1;30m"
#define ANSI_RED "\x1b[1;31m"
#define ANSI_GREEN "\x1b[1;32m"
#define ANSI_YELLOW "\x1b[1;33m"
#define ANSI_BLUE "\x1b[1;34m"
#define ANSI_PINK "\x1b[1;35m"
#define ANSI_LBLUE "\x1b[1;36m"
#define ANSI_WHITE "\x1b[0;37m"
#define ANSI_RESET "\x1b[0m"

/* These are the ANSI codes for blinking foreground text colors */
#define BLINK_BLACK "\x1b[0;5;30m"
#define BLINK_DRED "\x1b[0;5;31m"
#define BLINK_DGREEN "\x1b[0;5;32m"
#define BLINK_ORANGE "\x1b[0;5;33m"
#define BLINK_DBLUE "\x1b[0;5;34m"
#define BLINK_PURPLE "\x1b[0;5;35m"
#define BLINK_CYAN "\x1b[0;5;36m"
#define BLINK_GREY "\x1b[0;5;37m"
#define BLINK_DGREY "\x1b[1;5;30m"
#define BLINK_RED "\x1b[1;5;31m"
#define BLINK_GREEN "\x1b[1;5;32m"
#define BLINK_YELLOW "\x1b[1;5;33m"
#define BLINK_BLUE "\x1b[1;5;34m"
#define BLINK_PINK "\x1b[1;5;35m"
#define BLINK_LBLUE "\x1b[1;5;36m"
#define BLINK_WHITE "\x1b[1;5;37m"

/* These are the ANSI codes for background colors */
#define BACK_BLACK "\x1b[40m"
#define BACK_DRED "\x1b[41m"
#define BACK_DGREEN "\x1b[42m"
#define BACK_ORANGE "\x1b[43m"
#define BACK_DBLUE "\x1b[44m"
#define BACK_PURPLE "\x1b[45m"
#define BACK_CYAN "\x1b[46m"
#define BACK_GREY "\x1b[47m"

/* Other miscelaneous ANSI tags that can be used */
#define ANSI_BOLD "\x1b[1m"      /* For bright color stuff */
#define ANSI_ITALIC "\x1b[3m"    /* Italic text */
#define ANSI_UNDERLINE "\x1b[4m" /* Underline text */
#define ANSI_BLINK "\x1b[5m"     /* Blinking text */
#define ANSI_REVERSE "\x1b[7m"   /* Reverse colors */
#define ANSI_STRIKEOUT "\x1b[9m" /* Overstrike line */

#define AT_BLACK 0
#define AT_BLOOD 1
#define AT_DGREEN 2
#define AT_ORANGE 3
#define AT_DBLUE 4
#define AT_PURPLE 5
#define AT_CYAN 6
#define AT_GREY 7
#define AT_DGREY 8
#define AT_RED 9
#define AT_GREEN 10
#define AT_YELLOW 11
#define AT_BLUE 12
#define AT_PINK 13
#define AT_LBLUE 14
#define AT_WHITE 15

/*People, Objects or Room Related*/
#define AT_IMMORT 16
#define AT_NOTE 17
#define AT_OBJECT 18
#define AT_PERSON 19
#define AT_RMDESC 20
#define AT_RMNAME 21
#define AT_SHIP 22

/*Actions or Commands*/
#define AT_ACTION 23
#define AT_BLINK 24
#define AT_CONSIDER 25
#define AT_EXITS 26
#define AT_GOLD 27
#define AT_HELP 28 /* Added by Samson 1-15-01 for helpfiles */
#define AT_LIST 29
#define AT_OLDSCORE 30
#define AT_PLAIN 31
#define AT_QUIT 32
#define AT_REPORT 33
#define AT_SKILL 34
#define AT_SLIST 35

/*Fighting Stuffs*/
#define AT_DAMAGE 36
#define AT_FLEE 37
#define AT_HIT 38
#define AT_HITME 39
#define AT_HURT 40

/*Continual Messages*/
#define AT_DEAD 41
#define AT_DYING 42
#define AT_FALLING 43
#define AT_HUNGRY 44
#define AT_POISON 45
#define AT_RESET 46
#define AT_SOBER 47
#define AT_THIRSTY 48
#define AT_WEAROFF 49

/*Mortal Channels*/
#define AT_ARENA 50
#define AT_AUCTION 51 /* Added by Samson 12-25-98 for auction channel */
#define AT_CHAT 52
#define AT_CLAN 53
#define AT_GOSSIP 54
#define AT_GTELL 55
#define AT_HOLONET 56
#define AT_OOC 57
#define AT_MUSIC 58
#define AT_SAY 59
#define AT_SHIPTALK 60
#define AT_SHOUT 61 /* Added by Samson 9-29-98 for shout channel */
#define AT_SOCIAL 62
#define AT_TELL 63
#define AT_WARTALK 64
#define AT_WHISPER 65 /* Added by Samson 9-29-98 for version 1.4 code */
#define AT_YELL 66

/*Imm Only Colors*/
#define AT_AVATAR 67
#define AT_BUILD 68
#define AT_COMM 69
#define AT_IMMTALK 70
#define AT_LOG 71
#define AT_RFLAGS1 72
#define AT_RFLAGS2 73
#define AT_RVNUM 74

/* Should ALWAYS be one more than the last numerical value in the list */
#define MAX_COLORS 75

#define AT_MAGIC AT_WHITE
#define AT_FIRE AT_RED
#define AT_DIEMSG AT_BLOOD
#define AT_DANGER AT_RED
extern const short default_set[MAX_COLORS];
