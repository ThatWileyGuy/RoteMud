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
 * Original	 code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,              *
 * Michael Seifert, and Sebastian Hammer.                                           *
 *                                                                                  *
 ***********************************************************************************/

#include <optional>
#include <memory>
#include <algorithm>
#include <boost/format.hpp>

#ifdef WIN32
#include <fcntl.h>
#endif

#include "mud.hxx"
#include "connection.hxx"

#ifdef ECHO
#undef ECHO // TODO where does this come from on Linux?
#endif

using ConnectionContextOpaque = DESCRIPTOR_DATA;

/*
 * Socket and TCP/IP stuff.
 */

#define MAX_NEST 100
static OBJ_DATA* rgObjNest[MAX_NEST];

namespace telnet
{
// see RFC 854 for protocol and IANA (https://www.iana.org/assignments/telnet-options/telnet-options.xhtml) for options
constexpr char IAC = 255;
constexpr char WILL = 251;
constexpr char WONT = 252;
constexpr char GA = 249;

namespace option
{
constexpr unsigned char ECHO = 1;
}

}; // namespace telnet

const char echo_off_str[] = {telnet::IAC, telnet::WILL, telnet::option::ECHO, '\0'};
const char echo_on_str[] = {telnet::IAC, telnet::WONT, telnet::option::ECHO, '\0'};
const char go_ahead_str[] = {telnet::IAC, telnet::GA, '\0'};

void save_sysdata(SYSTEM_DATA sys);
void write_ship_list(void);
void arms(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument);
void send_main_mail_menu(DESCRIPTOR_DATA* d);
/*  from act_info?  */
void show_condition(CHAR_DATA* ch, CHAR_DATA* victim);
void generate_com_freq(CHAR_DATA* ch);

// planets.c

void write_planet_list(void);

/*
 * Global variables.
 */

std::vector<std::shared_ptr<DESCRIPTOR_DATA>> g_descriptors;
bool mud_down; /* Shutdown			*/
time_t boot_time;
HOUR_MIN_SEC set_boot_time_struct;
HOUR_MIN_SEC* set_boot_time;
tm* new_boot_time;
tm new_boot_struct;
char str_boot_time[MAX_INPUT_LENGTH];
char lastplayercmd[MAX_INPUT_LENGTH * 2];
time_t current_time; /* Time of this pulse		*/
std::optional<IOManager> io_manager;

/*
 * OS-dependent local functions.
 */
void game_loop();
boost::asio::ip::tcp::acceptor init_socket(int port);
void handle_new_socket(const boost::system::error_code& error);
void handle_descriptor_error(DESCRIPTOR_DATA* d, const boost::system::error_code& error);
void handle_descriptor_read(DESCRIPTOR_DATA* d, size_t read);

/*
 * Other local functions (OS-independent).
 */
bool check_parse_name(const char* name);
bool check_reconnect(std::shared_ptr<DESCRIPTOR_DATA> d, char* name, bool fConn);
bool check_playing(std::shared_ptr<DESCRIPTOR_DATA> d, char* name, bool kick);
bool check_multi(std::shared_ptr<DESCRIPTOR_DATA> d, char* name);
int main(int argc, char** argv);
void nanny(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument);
void stop_idling(CHAR_DATA* ch);
void display_prompt(std::shared_ptr<DESCRIPTOR_DATA> d);
int make_color_sequence(const char* col, char* buf, DESCRIPTOR_DATA* d);
int make_color_sequence_desc(const char* col, char* buf, DESCRIPTOR_DATA* d);
void handle_pager_input(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument);
void handle_command(ConnectionContext context, const std::string& line);
void handle_window_size_change(ConnectionContext context, int width, int height);
ConnectionContext handle_new_unauthenticated_connection(std::shared_ptr<Connection> connection);
bool authenticate_user(const std::string& username, const std::string& password);
std::vector<Pubkey> get_public_keys_for_user(const std::string& username);
ConnectionContext handle_new_authenticated_connection(std::shared_ptr<Connection> connection,
                                                            const std::string& username);
void connection_closed(ConnectionContext context);
void close_socket(DESCRIPTOR_DATA* dclose, bool startedExternally, bool force);
void send_prompt(DESCRIPTOR_DATA* d);

void mail_count(CHAR_DATA* ch);

std::unique_ptr<boost::asio::ip::tcp::socket> new_socket;

int gamemain(int argc, char** argv)
{

    bool fCopyOver = false;

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug(2);
#endif

#ifdef WIN32
    // turn off Windows EOL translations
    _set_fmode(_O_BINARY);
#endif

    sysdata.NO_NAME_RESOLVING = true;
    sysdata.WAIT_FOR_AUTH = true; // TODO does this go away along with ident?

    /*
     * Init time.
     */

    auto now_time = std::chrono::system_clock::now();
    current_time = std::chrono::system_clock::to_time_t(now_time);
    boot_time = current_time;
    strcpy_s(str_boot_time, ctime(&current_time));

    /*
     * Init boot time.
     */
    set_boot_time = &set_boot_time_struct;
    /*  set_boot_time->hour   = 6;
      set_boot_time->min    = 0;
      set_boot_time->sec    = 0;*/
    set_boot_time->manual = 0;

    new_boot_time = update_time(localtime(&current_time));
    /* Copies *new_boot_time to new_boot_struct, and then points
       new_boot_time to new_boot_struct again. -- Alty */
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;
    new_boot_time->tm_mday += 1;
    if (new_boot_time->tm_hour > 12)
        new_boot_time->tm_mday += 1;
    new_boot_time->tm_sec = 0;
    new_boot_time->tm_min = 0;
    new_boot_time->tm_hour = 6;

    /* Update new_boot_time (due to day increment) */
    new_boot_time = update_time(new_boot_time);
    new_boot_struct = *new_boot_time;
    new_boot_time = &new_boot_struct;

    /* Set reboot time string for do_time */
    get_reboot_string();

    init_pfile_scan_time(); /* Pfile autocleanup initializer - Samson 5-8-99 */

    /*
     * Get the port number.
     */
    const uint16_t telnet_port = 8787;
    const uint16_t ssh_port = 8888;

    /*
     * Run the game.
     */
    log_string("Booting Database");
    boot_db(fCopyOver);
    log_string("Initializing socket");
    if (!fCopyOver) /* We have already the port if copyover'ed */
    {
        IOManagerCallbacks callbacks = {
            &handle_command,           &handle_window_size_change, &handle_new_unauthenticated_connection, &authenticate_user,
            &get_public_keys_for_user, &handle_new_authenticated_connection,   &connection_closed};

        io_manager.emplace(callbacks, telnet_port, ssh_port);
        if (!io_manager.has_value())
        {
            fprintf(stderr, "failed to initialize IO");
            exit(1);
        }
    }

    sprintf_s(log_buf, "%s ready on ports %d and %d.", sysdata.mud_acronym, telnet_port, ssh_port);
    log_string(log_buf);

    using floatseconds = std::chrono::duration<double, std::chrono::seconds::period>;
    sprintf_s(log_buf, "boot took %.06f seconds", floatseconds(std::chrono::system_clock::now() - now_time).count());
    log_string(log_buf);

    game_loop();
    io_manager.reset();

    /*
     * That's all, folks.
     */
    log_string("Normal termination of game.");
    return 0;
}

ConnectionContext handle_new_unauthenticated_connection(std::shared_ptr<Connection> connection)
{
    auto dnew = std::make_shared<DESCRIPTOR_DATA>();

    dnew->connection.swap(connection);
    dnew->connected = CON_GET_NAME;
    dnew->scrlen = 24;
    dnew->user = STRALLOC("unknown");
    dnew->prevcolor = 0x07;

    // TODO this is where IP bans would get processed - skipping for now

    /*
     * Init descriptor data.
     */

    g_descriptors.push_back(dnew);

    /*
     * Send the greeting. Forces new color function - Tawnos
     */
    {
        extern char* help_greeting;
        if (help_greeting[0] == '.')
            send_to_desc_color2(help_greeting + 1, dnew.get());
        else
            send_to_desc_color2(help_greeting, dnew.get());
    }

    char buf[MAX_STRING_LENGTH] = {};

    if (g_descriptors.size() > sysdata.maxplayers)
        sysdata.maxplayers = g_descriptors.size();
    if (sysdata.maxplayers > sysdata.alltimemax)
    {
        if (sysdata.time_of_max)
            DISPOSE(sysdata.time_of_max);
        sprintf_s(buf, "%24.24s", ctime(&current_time));
        sysdata.time_of_max = str_dup(buf);
        sysdata.alltimemax = sysdata.maxplayers;
        sprintf_s(log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax);
        log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
        to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
        save_sysdata(sysdata);
    }

    return dnew;
}

void handle_command(ConnectionContext context, const std::string& command)
{
    auto d = std::static_pointer_cast<DESCRIPTOR_DATA>(context);

    // TODO close the socket instead of asserting
    assert((command.size() + 1) < MAX_INPUT_LENGTH);
    char cmdline[MAX_INPUT_LENGTH];

    std::memcpy(cmdline, command.c_str(), command.length());
    cmdline[command.length()] = '\0';

    d->fcommand = true;
    stop_idling(d->character);

    if (d->character)
        set_cur_char(d->character);

    if (d->pagepoint)
        handle_pager_input(d, cmdline);
    else
    {
        switch (d->connected)
        {
        default:
            nanny(d, cmdline);
            break;
        case CON_PLAYING:
            interpret(d->character, cmdline);
            break;
        case CON_EDITING:
            edit_buffer(d->character, cmdline);
            break;
        case CON_MAIL_BEGIN:
        case CON_MAIN_MAIL_MENU:
        case CON_MAIL_DISPLAY:
        case CON_MAIL_WRITE_START:
        case CON_MAIL_WRITE_TO:
        case CON_MAIL_WRITE_SUBJECT:
            arms(d, cmdline);
            break;
        }
    }
}

void handle_window_size_change(ConnectionContext context, int width, int height)
{
    log("window size change to %d %d", LOG_COMM, sysdata.log_level, width, height);
}

/*
static void SegVio()
{
  CHAR_DATA *ch;
  char buf[MAX_STRING_LENGTH];

  log_string( "SEGMENTATION VIOLATION" );
  log_string( lastplayercmd );
  for ( ch = first_char; ch; ch = ch->next )
  {
    sprintf_s( buf, "%cPC: %-20s room: %d", IS_NPC(ch) ? 'N' : ' ',
            ch->name, ch->in_room->vnum );
    log_string( buf );
  }
  exit(0);
}
*/

/*
 * LAG alarm!							-Thoric
 */
/*
static void caught_alarm()
{
    char buf[MAX_STRING_LENGTH];
    bug("ALARM CLOCK!");
    strcpy_s(buf, "Alas, the hideous malevalent entity known only as 'Lag' rises once more!\n\r");
    echo_to_all(AT_IMMORT, buf, ECHOTAR_ALL);
    if (newdesc)
    {
        FD_CLR(newdesc, &in_set);
        FD_CLR(newdesc, &out_set);
        log_string("clearing newdesc");
    }
    game_loop();
    close(control);

    log_string("Normal termination of game.");
    exit(0);
}
*/

void game_loop()
{
    DESCRIPTOR_DATA* d = nullptr;
    /*  time_t	last_check = 0;  */

    // TODO signal handling is gone
    /*
    signal(SIGPIPE, SIG_IGN);
    signal(SIGALRM, caught_alarm);
    */
    /* signal( SIGSEGV, SegVio ); */

    auto last_time = std::chrono::steady_clock::now();

    /* Main loop */
    while (!mud_down)
    {
        current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        /*
         * Kick out idle descriptors then check for input.
         */
        for (auto d : g_descriptors)
        {
            d->idle++;                                              /* make it so a descriptor can idle out */
            if (((!d->character && d->idle > 360)                   /* 2 mins */
                 || (d->connected != CON_PLAYING && d->idle > 1200) /* 5 mins */
                 || (d->idle > 28800))                              /* 2 hrs */
                && (!IS_IMMORTAL(d->character)))                    /*Possible DC FIX --Keb*/
            {
                // TODO these probably need to be longer
                write_to_buffer(d.get(), "Idle timeout... disconnecting.\n\r", 0);
                close_socket(d.get(), false);
                continue;
            }
            else
            {
                if (d->fcommand)
                {
                    send_prompt(d.get());
                    d->fcommand = false;
                }

                if (d->character && d->character->wait > 0)
                {
                    --d->character->wait;
                    continue;
                }
            }
        }

        /*
         * Autonomous game motion.
         */
        update_handler();

        /*
         * IO
         */

        auto next_pulse_start =
            last_time + std::chrono::steady_clock::duration(std::chrono::nanoseconds(1000000000 / PULSE_PER_SECOND));

        io_manager->runUntil(next_pulse_start);
        std::this_thread::sleep_until(next_pulse_start);

        last_time = next_pulse_start;

        /* Check every 5 seconds...  (don't need it right now)
    if ( last_check+5 < current_time )
    {
      CHECK_LINKS(first_descriptor, last_descriptor, next, prev,
          DESCRIPTOR_DATA);
      last_check = current_time;
    }
    */
    }
    return;
}

/*  From Erwin  */

void log_printf(const char* fmt, ...)
{
    char buf[MAX_STRING_LENGTH * 2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    log_string(buf);
}

// TODO This needs to be a destructor
DESCRIPTOR_DATA::~DESCRIPTOR_DATA()
{
    STRFREE(user); /* identd */
    DISPOSE(pagebuf);
}

void connection_closed(ConnectionContext context)
{
    std::shared_ptr<DESCRIPTOR_DATA> d = std::static_pointer_cast<DESCRIPTOR_DATA>(context);

    if (d == nullptr)
        return;

    close_socket(d.get(), true, true);
}

void close_socket(DESCRIPTOR_DATA* dclose, bool force)
{
    close_socket(dclose, false, force);
}

void close_socket(std::shared_ptr<DESCRIPTOR_DATA> dclose, bool force)
{
    close_socket(dclose.get(), force);
}

void close_socket(DESCRIPTOR_DATA* dclose, bool startedExternally, bool force)
{
    CHAR_DATA* ch = nullptr;

    /* flush outbuf */
    if (!startedExternally)
    {
        if (!force)
            dclose->connection->flushAndClose();
        else
            dclose->connection->close();
    }

    /* say bye to whoever's snooping this descriptor */
    if (dclose->snoop_by)
        write_to_buffer(dclose->snoop_by, "Your victim has left the game.\n\r", 0);

    /* stop snooping everyone else */
    for (auto d : g_descriptors)
    {
        if (d->snoop_by == dclose)
            d->snoop_by = NULL;
    }

    /* Check for switched people who go link-dead. -- Altrag */
    if (dclose->original)
    {
        if ((ch = dclose->character) != NULL)
            do_return(ch, MAKE_TEMP_STRING(""));
        else
        {
            bug("Close_socket: dclose->original without character %s",
                (dclose->original->name ? dclose->original->name : "unknown"));
            dclose->character = dclose->original;
            dclose->original = NULL;
        }
    }

    ch = dclose->character;

    if (dclose->character)
    {
        sprintf_s(log_buf, "Closing link to %s.", ch->name);
        log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));
        /*
            if ( ch->top_level < LEVEL_DEMI )
              to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
        */
        if (dclose->connected == CON_PLAYING || dclose->connected == CON_EDITING)
        {
            act(AT_ACTION, "$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
            ch->desc = NULL;
        }
        else
        {
            /* clear descriptor pointer to get rid of bug message in log */
            dclose->character->desc = NULL;
            free_char(dclose->character);
        }
    }

    auto iter = std::find_if(g_descriptors.begin(), g_descriptors.end(), [&](auto d) { return d.get() == dclose; });

    if (iter == g_descriptors.end())
    {
        bug("tried to remove descriptor that was already removed");
    }
    else
    {
        g_descriptors.erase(iter);
    }
}

void handle_descriptor_error(DESCRIPTOR_DATA* d, const boost::system::error_code& error)
{
    if (d->character && (d->connected == CON_PLAYING || d->connected == CON_EDITING))
        save_char_obj(d->character);
    close_socket(d, true);
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer(DESCRIPTOR_DATA* d, std::string_view string)
{
    if (!d)
    {
        bug("Write_to_buffer: NULL descriptor");
        return;
    }

    // Normally a bug... but can happen if loadup is used.
    if (!d->connection)
        return;

    try
    {
        // Initial \n\r if needed.
        /*
        if (!d->fcommand)
        {
            d->connection->write("\n\r");
        }
        */

        d->connection->write(string);
    }
    catch (...)
    {
        // TODO should probably have a specific exception type here.
        bug("Buffer overflow. Closing (%s).", d->character ? d->character->name : "???");
        close_socket(d, true);
    }
}

void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, std::string_view string)
{
    write_to_buffer(d.get(), string);
}

void write_to_buffer(DESCRIPTOR_DATA* d, const char* string, size_t length)
{
    if (length <= 0)
    {
        length = strlen(string);
    }

    write_to_buffer(d, std::string_view(string, length));
}

void write_to_buffer(std::shared_ptr<DESCRIPTOR_DATA> d, const char* string, size_t length)
{
    write_to_buffer(d.get(), string, length);
}

void show_title(std::shared_ptr<DESCRIPTOR_DATA> d)
{
    CHAR_DATA* ch = d->character;

    if (!IS_SET(ch->pcdata->flags, PCFLAG_NOINTRO))
    {
        if (IS_SET(ch->act, PLR_ANSI))
            send_ansi_title(ch);
        else
            send_ascii_title(ch);
    }
    else
    {
        write_to_buffer(d, "Press enter...\n\r", 0);
    }
    d->connected = CON_PRESS_ENTER;
}

char* smaug_crypt(const char* pwd)
{
    md5_state_t state;
    md5_byte_t digest[16];
    static char passwd[16];
    unsigned int x;

    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)pwd, strlen(pwd));
    md5_finish(&state, digest);

    strncpy_s(passwd, (const char*)digest, 15);
    passwd[15] = '\0';

    /*
     * The listed exceptions below will fubar the MD5 authentication packets, so change them
     */
    for (x = 0; x < strlen(passwd); x++)
    {
        if (passwd[x] == '\n')
            passwd[x] = 'n';
        if (passwd[x] == '\r')
            passwd[x] = 'r';
        if (passwd[x] == '\t')
            passwd[x] = 't';
        if (passwd[x] == ' ')
            passwd[x] = 's';
        if ((int)passwd[x] == 11)
            passwd[x] = 'x';
        if ((int)passwd[x] == 12)
            passwd[x] = 'X';
        if (passwd[x] == '~')
            passwd[x] = '+';
        if (passwd[x] == EOF)
            passwd[x] = 'E';
    }
    return (passwd);
}

class NannyShell : public Shell
{
  private:
    DESCRIPTOR_DATA& m_descriptor;



  public:
    NannyShell(DESCRIPTOR_DATA& d) : Shell(nullptr, d, true), m_descriptor(d)
    {
    }

    void handleCommand(DESCRIPTOR_DATA& d, const std::string& command) override
    {
    }


};

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument)
{
    //	extern int lang_array[];
    //	extern char *lang_names[];
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    CHAR_DATA* ch;
    char mudnamebuf[MSL];

    char* pwdnew;
    char* p;
    int iRace, iClass, iDroid;
    BAN_DATA* pban;
    /*    int iLang;*/
    bool fOld, chk;
    int col = 0;
    while (isspace(*argument))
        argument++;

    ch = d->character;

    switch (d->connected)
    {

    default:
        bug("Nanny: bad d->connected %d.", d->connected);
        close_socket(d, true);
        return;

    case CON_GET_NAME:
        if (argument[0] == '\0')
        {
            close_socket(d, false);
            return;
        }

        argument[0] = UPPER(argument[0]);
        if (!check_parse_name(argument))
        {
            write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
            return;
        }

        if (!str_cmp(argument, "New"))
        {
            if (d->newstate == 0)
            {
                /* New player */
                /* Don't allow new players if DENY_NEW_PLAYERS is true */
                if (sysdata.DENY_NEW_PLAYERS == true)
                {
                    sprintf_s(buf, "The mud is currently preparing for a reboot.\n\r");
                    write_to_buffer(d, buf, 0);
                    sprintf_s(buf, "New players are not accepted during this time.\n\r");
                    write_to_buffer(d, buf, 0);
                    sprintf_s(buf, "Please try again in a few minutes.\n\r");
                    write_to_buffer(d, buf, 0);
                    close_socket(d, false);
                }
                sprintf_s(buf, "\n\rChoosing a name is one of the most important parts of this game...\n\r"
                               "Make sure to pick a name appropriate to the character you are going\n\r"
                               "to role play, and be sure that it suits our theme.\n\r"
                               "If the name you select is not acceptable, you will be asked to choose\n\r"
                               "another one.\n\r\n\rPlease choose a name for your character: ");
                write_to_buffer(d, buf, 0);
                d->newstate++;
                d->connected = CON_GET_NAME;
                return;
            }
            else
            {
                send_to_desc_color2("Illegal name, try another.\n\rName: ", d);
                return;
            }
        }

        if (check_playing(d, argument, false) == BERR)
        {
            write_to_buffer(d, "Name: ", 0);
            return;
        }

        fOld = load_char_obj(*d, argument, true);
        if (!d->character)
        {
            sprintf_s(log_buf, "Bad player file %s@%s.", argument, d->connection->getHostname().c_str());
            log_string(log_buf);
            write_to_buffer(d, "Your playerfile is corrupt...Please notify the Admin.\n\r", 0);
            close_socket(d, false);
            return;
        }
        ch = d->character;

        for (pban = first_ban; pban; pban = pban->next)
        {
            if ((!str_prefix(pban->name, d->connection->getHostname().c_str()) ||
                 !str_suffix(pban->name, d->connection->getHostname().c_str())) &&
                pban->level >= ch->top_level)
            {
                write_to_buffer(d, "Your site has been banned from this Mud.\n\r", 0);
                close_socket(d, false);
                return;
            }
        }
        if (IS_SET(ch->act, PLR_DENY))
        {
            sprintf_s(log_buf, "Denying access to %s@%s.", argument, d->connection->getHostname().c_str());
            log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
            if (d->newstate != 0)
            {
                write_to_buffer(d, "That name is already taken.  Please choose another: ", 0);
                d->connected = CON_GET_NAME;
                return;
            }
            write_to_buffer(d, "You are denied access.\n\r", 0);
            close_socket(d, false);
            return;
        }

        chk = check_reconnect(d, argument, false);
        if (chk == BERR)
            return;

        if (chk)
        {
            fOld = true;
        }
        else
        {

            if (sysdata.wizlock == 1 && !IS_IMMORTAL(ch))
            {
                write_to_buffer(d, "The game is wizlocked.  Only immortals can connect now.\n\r", 0);
                write_to_buffer(d, "Please try back later.\n\r", 0);
                close_socket(d, false);
                return;
            }
        }

        if (fOld)
        {
            if (d->newstate != 0)
            {
                write_to_buffer(d, "That name is already taken.  Please choose another: ", 0);
                d->connected = CON_GET_NAME;
                return;
            }
            /* Old player */
            write_to_buffer(d, "Password: ", 0);
            write_to_buffer(d, echo_off_str, 0);
            d->connected = CON_GET_OLD_PASSWORD;
            return;
        }
        else
        {
            send_to_desc_color2("\n\r&zI don't recognize your name, you must be new here.\n\r\n\r", d);
            sprintf_s(buf, "Did I get that right, &W%s &z(&WY&z/&WN&z)? &w", argument);
            send_to_desc_color2(buf, d);
            d->connected = CON_CONFIRM_NEW_NAME;
            return;
        }
        break;

    case CON_GET_OLD_PASSWORD:
        write_to_buffer(d, "\n\r", 2);
        //        bug(crypt(argument, ch->pcdata->pwd), 0);

        if (strcmp(smaug_crypt(argument), ch->pcdata->pwd))
        {
            write_to_buffer(d, "Wrong password.\n\r", 0);
            /* clear descriptor pointer to get rid of bug message in log */
            d->character->desc = NULL;
            sprintf_s(buf, "%s@%s: Invalid password.", ch->name, d->connection->getHostname().c_str());
            log_string(buf);
            close_socket(d, false);
            return;
        }

        write_to_buffer(d, echo_on_str, 0);

        if (check_playing(d, ch->name, true))
            return;

        chk = check_reconnect(d, ch->name, true);
        if (chk == BERR)
        {
            if (d->character && d->character->desc)
                d->character->desc = NULL;
            close_socket(d, false);
            return;
        }
        if (chk == true)
            return;

        if (check_multi(d, ch->name))
        {
            close_socket(d, false);
            return;
        }

        sprintf_s(buf, ch->name);
        d->character->desc = NULL;
        free_char(d->character);
        fOld = load_char_obj(*d, buf, false);
        ch = d->character;
        sprintf_s(log_buf, "%s@%s(%s) has connected.", ch->name, d->connection->getHostname().c_str(), d->user);
        if (ch->top_level < LEVEL_DEMI)
        {
            /*to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );*/
            log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
        }
        else
            log_string_plus(log_buf, LOG_COMM, ch->top_level);

        {
            tm* tme;
            time_t now;
            char day[50];
            now = time(0);
            tme = localtime(&now);
            strftime(day, 50, "%a %b %d %H:%M:%S %Y", tme);
            sprintf_s(log_buf, "%-20s     %-24s    %s", ch->name, day, d->connection->getHostname().c_str());
            write_last_file(log_buf);
        }

        show_title(d);
        if (ch->pcdata->area)
        {
            do_loadarea(ch, MAKE_TEMP_STRING(""));
            assign_area(ch);
        }
        if (ch->max_mana != (ch->force_control + ch->force_sense + ch->force_alter) * 3 * ch->force_level_status)
            ch->max_mana = (ch->force_control + ch->force_sense + ch->force_alter) * 3 * ch->force_level_status;
        ch->mana = ch->max_mana;
        if (ch->force_identified == 1 && ch->force_level_status == FORCE_MASTER)
        {
            int ft;
            FORCE_SKILL* skill;
            if (ch->force_skill[FORCE_SKILL_PARRY] < 50)
                ch->force_skill[FORCE_SKILL_PARRY] = 50;
            ft = ch->force_type;
            for (skill = first_force_skill; skill; skill = skill->next)
                if ((skill->type == ft || skill->type == FORCE_GENERAL) && ch->force_skill[skill->index] < 50 &&
                    (strcmp(skill->name, "master") && strcmp(skill->name, "student") &&
                     strcmp(skill->name, "promote") && strcmp(skill->name, "instruct")))
                    ch->force_skill[skill->index] = 50;
        }

        break;

    case CON_CONFIRM_NEW_NAME:
        switch (*argument)
        {
        case 'y':
        case 'Y':
            sprintf_s(buf,
                      "\n\r&zMake sure to use a password that won't be easily guessed by someone else."
                      "\n\rPick a good password for %s: &w%s",
                      ch->name, echo_off_str);
            send_to_desc_color2(buf, d);
            d->connected = CON_GET_NEW_PASSWORD;
            break;

        case 'n':
        case 'N':
            send_to_desc_color2("&zOk, what is it, then? &w", d);
            /* clear descriptor pointer to get rid of bug message in log */
            d->character->desc = NULL;
            free_char(d->character);
            d->character = NULL;
            d->connected = CON_GET_NAME;
            break;

        default:
            send_to_desc_color2("&zPlease type &WYes&z or &WNo&z. &w", d);
            break;
        }
        break;

    case CON_GET_NEW_PASSWORD:
        write_to_buffer(d, "\n\r", 2);

        if (strlen(argument) < 5)
        {
            send_to_desc_color2("&zPassword must be at least five characters long.\n\rPassword: &w", d);
            return;
        }

        pwdnew = smaug_crypt(argument);
        for (p = pwdnew; *p != '\0'; p++)
        {
            if (*p == '~')
            {
                send_to_desc_color2("&zNew password not acceptable, try again.\n\rPassword: &w", d);
                return;
            }
        }

        DISPOSE(ch->pcdata->pwd);
        ch->pcdata->pwd = str_dup(pwdnew);
        send_to_desc_color2("\n\r&zPlease retype the password to confirm: &w", d);
        d->connected = CON_CONFIRM_NEW_PASSWORD;
        break;

    case CON_CONFIRM_NEW_PASSWORD:
        write_to_buffer(d, "\n\r", 2);

        if (strcmp(smaug_crypt(argument), ch->pcdata->pwd))
        {
            send_to_desc_color2("&zPasswords don't match.\n\rRetype password: &w", d);
            d->connected = CON_GET_NEW_PASSWORD;
            return;
        }

        write_to_buffer(d, echo_on_str, 0);
        send_to_desc_color2("\n\r&zWhat is your sex (&WM&z/&WF&z/&WN&z)? &w", d);
        d->connected = CON_GET_NEW_SEX;
        break;

    case CON_GET_NEW_SEX:
        switch (argument[0])
        {
        case 'm':
        case 'M':
            ch->sex = SEX_MALE;
            break;
        case 'f':
        case 'F':
            ch->sex = SEX_FEMALE;
            break;
        case 'n':
        case 'N':
            ch->sex = SEX_NEUTRAL;
            break;
        default:
            send_to_desc_color2("&zThat's not a sex.\n\rWhat is your sex?&w ", d);
            return;
        }

        send_to_desc_color2("\n\r&zYou may choose from the following races, or type &Whelp [race]&z to learn more.\n\r",
                            d);
        send_to_desc_color2("Keep in mind that you must ROLEPLAY the race you select. If you choose a\n\r", d);
        send_to_desc_color2("droid, you must RP your character accordingly. If you choose noghri, you\n\r", d);
        send_to_desc_color2("must roleplay your character as the race is expected. This applies to all\n\r", d);
        send_to_desc_color2("races, and if you are unsure about how to RP a race, refer to the helpfile.\n\r", d);
        send_to_desc_color2("If you are still unsure, do not pick that race.\n\r\n\r", d);
        buf[0] = '\0';
        col = 0;
        for (iRace = 0; iRace < MAX_RACE; iRace++)
        {
            if (race_table[iRace].race_name && race_table[iRace].race_name[0] != '\0')
            {
                if (iRace >= 0)
                {
                    /*if ( strlen(buf)+strlen(race_table[iRace].race_name) > 77 )
                    {
                       strcat_s( buf, "\n\r" );

                       write_to_buffer( d, buf, 0 );
                       buf[0] = '\0';
                    }*/
                    sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", race_table[iRace].race_name);
                    strcat_s(buf, buf2);
                    if (++col % 4 == 0)
                    {
                        strcat_s(buf, "\n\r");
                        send_to_desc_color2(buf, d);
                        buf[0] = '\0';
                    }
                }
            }
        }
        if (col % 4 != 0)
            strcat_s(buf, "\n\r");
        strcat_s(buf, "&z:&w ");
        send_to_desc_color2(buf, d);
        d->connected = CON_GET_NEW_RACE;
        break;
    case CON_GET_NEW_RACE:
        argument = one_argument(argument, arg);
        if (!str_cmp(arg, "help"))
        {
            do_help(ch, argument);
            send_to_desc_color2("&zPlease choose a race:&w ", d);
            return;
        }

        for (iRace = 0; iRace < MAX_RACE; iRace++)
        {
            if (toupper(arg[0]) == toupper(race_table[iRace].race_name[0]) &&
                !str_prefix(arg, race_table[iRace].race_name))
            {
                ch->race = iRace;
                break;
            }
        }

        if (iRace == MAX_RACE || !race_table[iRace].race_name || race_table[iRace].race_name[0] == '\0')
        {
            send_to_desc_color2("&zThat's not a race.\n\rWhat is your race?&w ", d);
            return;
        }

        send_to_desc_color2("\n\r&zPlease choose a main ability from the following classes:&w\n\r", d);
        buf[0] = '\0';
        col = 0;
        for (iClass = 0; iClass < MAX_ABILITY; iClass++)
        {
            if (ability_name[iClass] && ability_name[iClass][0] != '\0' && str_cmp(ability_name[iClass], "force"))
            {

                sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", ability_name[iClass]);
                strcat_s(buf, buf2);
                if (++col % 4 == 0)
                {
                    strcat_s(buf, "\n\r");
                    send_to_desc_color2(buf, d);
                    buf[0] = '\0';
                }
            }
        }
        if (col % 4 != 0)
            strcat_s(buf, "\n\r");
        strcat_s(buf, "&z:&w ");

        send_to_desc_color2(buf, d);
        d->connected = CON_GET_NEW_CLASS;
        break;

    case CON_GET_NEW_CLASS:
        argument = one_argument(argument, arg);
        if (!str_cmp(arg, "help"))
        {
            do_help(ch, argument);
            send_to_desc_color2("&zPlease choose an ability class:&w ", 0);
            return;
        }

        for (iClass = 0; iClass < MAX_ABILITY; iClass++)
        {
            if (toupper(arg[0]) == toupper(ability_name[iClass][0]) && !str_prefix(arg, ability_name[iClass]) &&
                str_prefix(arg, "force"))
            {
                ch->main_ability = iClass;
                break;
            }
        }

        if (iClass == MAX_ABILITY || !ability_name[iClass] || ability_name[iClass][0] == '\0')
        {
            send_to_desc_color2("&zThat's not a skill class.\n\rWhat is it going to be? &w", d);
            return;
        }
        send_to_desc_color2("\n\r&zPlease choose a secondary ability from the following classes:&w\n\r", d);
        buf[0] = '\0';
        col = 0;
        for (iClass = 0; iClass < MAX_ABILITY; iClass++)
        {
            if (ability_name[iClass] && ability_name[iClass][0] != '\0' && str_cmp(ability_name[iClass], "force") &&
                ch->main_ability != iClass)
            {

                sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", ability_name[iClass]);
                strcat_s(buf, buf2);
                if (++col % 4 == 0)
                {
                    strcat_s(buf, "\n\r");
                    send_to_desc_color2(buf, d);
                    buf[0] = '\0';
                }
            }
        }
        if (col % 4 != 0)
            strcat_s(buf, "\n\r");
        strcat_s(buf, "&z:&w ");

        send_to_desc_color2(buf, d);
        d->connected = CON_GET_NEW_SECOND;
        break;
    case CON_GET_NEW_SECOND:
        argument = one_argument(argument, arg);
        if (!str_cmp(arg, "help"))
        {
            do_help(ch, argument);
            send_to_desc_color2("&zPlease choose an ability class:&w ", d);
            return;
        }

        for (iClass = 0; iClass < MAX_ABILITY; iClass++)
        {
            if (toupper(arg[0]) == toupper(ability_name[iClass][0]) && !str_prefix(arg, ability_name[iClass]) &&
                str_prefix(arg, "force") && ch->main_ability != iClass)
            {
                ch->secondary_ability = iClass;
                break;
            }
        }

        if (iClass == MAX_ABILITY || !ability_name[iClass] || ability_name[iClass][0] == '\0')
        {
            send_to_desc_color2("&zThat's not a skill class.\n\rWhat is it going to be?&w ", d);
            return;
        }

        send_to_desc_color2("\n\r&zRolling stats...\n\r", d);
    case CON_ROLL_STATS:

        ch->perm_str = number_range(1, 6) + number_range(1, 6) + number_range(1, 6);
        ch->perm_int = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
        ch->perm_wis = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
        ch->perm_dex = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
        ch->perm_con = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
        ch->perm_cha = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);

        ch->perm_str += race_table[ch->race].str_plus;
        ch->perm_int += race_table[ch->race].int_plus;
        ch->perm_wis += race_table[ch->race].wis_plus;
        ch->perm_dex += race_table[ch->race].dex_plus;
        ch->perm_con += race_table[ch->race].con_plus;
        ch->perm_cha += race_table[ch->race].cha_plus;

        sprintf_s(buf, "\n\r&zSTR: &R%d  &zINT: &R%d  &zWIS: &R%d  &zDEX: &R%d  &zCON: &R%d  &zCHA: &R%d\n\r",
                  ch->perm_str, ch->perm_int, ch->perm_wis, ch->perm_dex, ch->perm_con, ch->perm_cha);

        send_to_desc_color2(buf, d);
        send_to_desc_color2("\n\r&zAre these stats OK?&w ", d);
        d->connected = CON_STATS_OK;
        break;

    case CON_STATS_OK:

        switch (argument[0])
        {
        case 'y':
        case 'Y':
            break;
        case 'n':
        case 'N':
            ch->perm_str = number_range(1, 6) + number_range(1, 6) + number_range(1, 6);
            ch->perm_int = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
            ch->perm_wis = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
            ch->perm_dex = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
            ch->perm_con = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
            ch->perm_cha = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);

            ch->perm_str += race_table[ch->race].str_plus;
            ch->perm_int += race_table[ch->race].int_plus;
            ch->perm_wis += race_table[ch->race].wis_plus;
            ch->perm_dex += race_table[ch->race].dex_plus;
            ch->perm_con += race_table[ch->race].con_plus;
            ch->perm_cha += race_table[ch->race].cha_plus;

            sprintf_s(buf, "\n\r&zSTR: &R%d  &zINT: &R%d  &zWIS: &R%d  &zDEX: &R%d  &zCON: &R%d  &zCHA: &R%d\n\r",
                      ch->perm_str, ch->perm_int, ch->perm_wis, ch->perm_dex, ch->perm_con, ch->perm_cha);

            send_to_desc_color2(buf, d);
            send_to_desc_color2("\n\r&zOK?&w ", d);
            return;
        default:
            send_to_desc_color2("&zInvalid selection.\n\r&WYES&z or &WNO&z? ", d);
            return;
        }

        if (!IS_DROID(ch))
        {
            send_to_desc_color2("\n\r&zPlease choose a height from the following list:&w\n\r", d);
            buf[0] = '\0';
            col = 0;
            for (iClass = 0; iClass < 4; iClass++)
            {
                if (height_name[iClass] && height_name[iClass][0] != '\0')
                {
                    sprintf_s(buf3, "%s", height_name[iClass]);
                    buf3[0] = UPPER(buf3[0]);
                    sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", buf3);
                    strcat_s(buf, buf2);
                    if (++col % 4 == 0)
                    {
                        strcat_s(buf, "\n\r");
                        send_to_desc_color2(buf, d);
                        buf[0] = '\0';
                    }
                }
            }
        }
        else
        {
            send_to_desc_color2("\n\r&zPlease choose a droid description from the following list:&w\n\r", d);
            buf[0] = '\0';
            col = 0;
            for (iDroid = 0; iDroid < 8; iDroid++)
            {
                if (droid_name[iDroid] && droid_name[iDroid][0] != '\0')
                {
                    sprintf_s(buf3, "%s", droid_name[iDroid]);
                    buf3[0] = UPPER(buf3[0]);
                    sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", buf3);
                    strcat_s(buf, buf2);
                    if (++col % 4 == 0)
                    {
                        strcat_s(buf, "\n\r");
                        send_to_desc_color2(buf, d);
                        buf[0] = '\0';
                    }
                }
            }
        }
        if (col % 4 != 0)
            strcat_s(buf, "\n\r");
        strcat_s(buf, "&z:&w ");

        send_to_desc_color2(buf, d);
        if (!IS_DROID(ch))
            d->connected = CON_GET_HEIGHT;
        else
            d->connected = CON_GET_DROID;
        break;

    case CON_GET_HEIGHT:
        argument = one_argument(argument, arg);
        for (iClass = 0; iClass < 4; iClass++)
        {
            if (toupper(arg[0]) == toupper(height_name[iClass][0]) && !str_prefix(arg, height_name[iClass]))
            {
                ch->pheight = iClass;
                break;
            }
        }

        if (iClass == 4 || !height_name[iClass] || height_name[iClass][0] == '\0')
        {
            send_to_desc_color2("&zThat's not a height.\n\rWhat is it going to be?&w ", d);
            return;
        }

        send_to_desc_color2("\n\r&zPlease choose a build from the following list:&w\n\r", d);
        buf[0] = '\0';
        col = 0;
        for (iClass = 0; iClass < 6; iClass++)
        {
            if (build_name[iClass] && build_name[iClass][0] != '\0')
            {
                sprintf_s(buf3, "%s", build_name[iClass]);
                buf3[0] = UPPER(buf3[0]);
                sprintf_s(buf2, "&R[&z%-15.15s&R]&w  ", buf3);
                strcat_s(buf, buf2);
                if (++col % 4 == 0)
                {
                    strcat_s(buf, "\n\r");
                    send_to_desc_color2(buf, d);
                    buf[0] = '\0';
                }
            }
        }
        if (col % 4 != 0)
            strcat_s(buf, "\n\r");
        strcat_s(buf, "&z:&w ");

        send_to_desc_color2(buf, d);
        d->connected = CON_GET_BUILD;
        break;

    case CON_GET_BUILD:
        argument = one_argument(argument, arg);
        for (iClass = 0; iClass < 6; iClass++)
        {
            if (toupper(arg[0]) == toupper(build_name[iClass][0]) && !str_prefix(arg, build_name[iClass]))
            {
                ch->build = iClass;
                break;
            }
        }

        if (iClass == 6 || !build_name[iClass] || build_name[iClass][0] == '\0')
        {
            send_to_desc_color2("&zThat's not a build.\n\rWhat is it going to be?&w ", d);
            return;
        }

    case CON_GET_DROID:
        if (IS_DROID(ch))
        {
            argument = one_argument(argument, arg);
            for (iDroid = 0; iDroid < 8; iDroid++)
            {
                if (toupper(arg[0]) == toupper(droid_name[iDroid][0]) && !str_prefix(arg, droid_name[iDroid]))
                {
                    ch->build = iDroid;
                    break;
                }
            }

            if (iDroid == 8 || !droid_name[iDroid] || droid_name[iDroid][0] == '\0')
            {
                send_to_desc_color2("&zThat's not a droid description.\n\rWhat is it going to be?&w ", d);
                return;
            }
        }
        /*  Changing this up a bit...automatically sets PLR_ANSI, skips want ansi/msp.

            write_to_buffer( d, "\n\rWould you like ANSI or no graphic/color support, (R/A/N)? ", 0 ); */
        SET_BIT(ch->act, PLR_ANSI);
        /*	d->connected = CON_GET_WANT_RIPANSI;
                break;

            case CON_GET_WANT_RIPANSI:
            switch ( argument[0] )
            {
            case 'a': case 'A': SET_BIT(ch->act,PLR_ANSI);  break;
            case 'n': case 'N': break;
            default:
                write_to_buffer( d, "Invalid selection.\n\rANSI or NONE? ", 0 );
                return;
            }
                reset_colors(ch);
                write_to_buffer( d, "Does your mud client have the Mud Sound Protocol? ", 0 );
            d->connected = CON_GET_MSP;
             break;


        case CON_GET_MSP:
            switch ( argument[0] )
            {
            case 'y': case 'Y': SET_BIT(ch->act,PLR_SOUND);  break;
            case 'n': case 'N': break;
            default:
                write_to_buffer( d, "Invalid selection.\n\rYES or NO? ", 0 );
                return;
            }

            if ( !sysdata.WAIT_FOR_AUTH )
            {
        */

        sprintf_s(log_buf, "%s@%s new %s.", ch->name, d->connection->getHostname().c_str(),
                  race_table[ch->race].race_name);
        log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
        to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
        sprintf_s(mudnamebuf, "\n\r&R&zWelcome to &R%s&z. Press enter to continue.&w\n\r\n\r", sysdata.mudname);
        send_to_desc_color2(mudnamebuf, d);
        {
            int ability;

            for (ability = 0; ability < MAX_ABILITY; ability++)
            {
                ch->skill_level[ability] = 0;
                ch->bonus[ability] = 0;
            }
        }
        ch->top_level = 0;
        ch->position = POS_STANDING;
        // Threw in commfreq here, hope it doesn't fuck with anything
        sprintf_s(buf, "%d%d%d.%d%d%d", number_range(0, 9), number_range(0, 9), number_range(0, 9), number_range(0, 9),
                  number_range(0, 9), number_range(0, 9));
        ch->comfreq = STRALLOC(buf);
        d->connected = CON_PRESS_ENTER;
        return;
        break;
        /*	}

            write_to_buffer( d, "\n\rYou now have to wait for a god to authorize you... please be patient...\n\r", 0 );
            sprintf_s( log_buf, "(1) %s@%s new %s applying for authorization...",
                        ch->name, d->host,
                        race_table[ch->race].race_name);
            log_string( log_buf );
            to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
            d->connected = CON_WAIT_1;
            break;

             case CON_WAIT_1:
            write_to_buffer( d, "\n\rTwo more tries... please be patient...\n\r", 0 );
            sprintf_s( log_buf, "(2) %s@%s new %s applying for authorization...",
                        ch->name, d->host,
                        race_table[ch->race].race_name);
            log_string( log_buf );
            to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
            d->connected = CON_WAIT_2;
            break;

             case CON_WAIT_2:
            write_to_buffer( d, "\n\rThis is your last try...\n\r", 0 );
            sprintf_s( log_buf, "(3) %s@%s new %s applying for authorization...",
                        ch->name, d->host,
                        race_table[ch->race].race_name);
            log_string( log_buf );
            to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
            d->connected = CON_WAIT_3;
            break;

            case CON_WAIT_3:
            write_to_buffer( d, "Sorry... try again later.\n\r", 0 );
            close_socket( d, false );
            return;
            break;

            case CON_ACCEPTED:

            sprintf_s( log_buf, "%s@%s new %s.", ch->name, d->host,
                        race_table[ch->race].race_name);
            log_string_plus( log_buf, LOG_COMM, sysdata.log_level );
            to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
            write_to_buffer( d, "\n\r", 2 );
            show_title(d);
                {
                   int ability;

                   for ( ability =0 ; ability < MAX_ABILITY ; ability++ )
                {
                      ch->skill_level[ability] = 0;
                  ch->bonus[ability] = 0;
                }
                }
            ch->top_level = 0;
            ch->position = POS_STANDING;
            d->connected = CON_PRESS_ENTER;
            break;
        */
    case CON_PRESS_ENTER:
        /*	if ( IS_SET(ch->act, PLR_ANSI) )
              send_to_pager( "\033[2J", ch );
            else
              send_to_pager( "\014", ch );*/
        if (ch->top_level >= 0)
        {
            send_to_pager("\n\r&WMessage of the Day&w\n\r", ch);
            do_help(ch, MAKE_TEMP_STRING("motd"));
        }
        if (IS_IMMORTAL(ch))
        {
            send_to_pager("&WImmortal Message of the Day&w\n\r", ch);
            do_help(ch, MAKE_TEMP_STRING("imotd"));
        }
        send_to_pager("\n\r&WPress [ENTER] &w", ch);
        d->connected = CON_READ_MOTD;
        break;

    case CON_READ_MOTD:
        write_to_buffer(d, "\n\r\n\r", 0);
        add_char(ch);
        /* if(ch->comfreq == NULL)
         {
          generate_com_freq(ch);
      sprintf_s(buf, "%s has no comfreq. Generating.", ch->name);
      log_string(buf);
     }*/
        sprintf_s(buf, " %s has entered the game.", ch->name);
        log_string_plus(buf, LOG_NORMAL, get_trust(ch));
        d->connected = CON_PLAYING;

        if (ch->top_level == 0)
        {
            OBJ_DATA* obj;
            int iLang;

            ch->pcdata->clan = NULL;
            ch->pcdata->learned[gsn_smallspace] = 25;
            ch->pcdata->learned[gsn_shipsystems] = 25;
            ch->pcdata->learned[gsn_navigation] = 25;
            ch->pcdata->learned[gsn_scan] = 25;

            ch->perm_lck = number_range(6, 18);
            ch->perm_frc = number_range(-1000, 20);
            ch->affected_by = race_table[ch->race].affected;
            ch->perm_lck += race_table[ch->race].lck_plus;
            ch->perm_frc += race_table[ch->race].frc_plus;
            reset_colors(ch);
            if (ch->main_ability == FORCE_ABILITY)
                ch->perm_frc = URANGE(0, ch->perm_frc, 20);
            else
                ch->perm_frc = URANGE(0, ch->perm_frc, 20);

            if (ch->main_ability == HUNTING_ABILITY || ch->main_ability == ASSASSIN_ABILITY || IS_DROID(ch))
                ch->perm_frc = 0;

            /* took out automaticly knowing basic
            if ( (iLang = skill_lookup( "basic" )) < 0 )
                bug( "Nanny: cannot find basic language." );
            else
                ch->pcdata->learned[iLang] = 100;
            */

            for (iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++)
                if (lang_array[iLang] == race_table[ch->race].language)
                    break;
            if (lang_array[iLang] == LANG_UNKNOWN)
            {
                if (IS_DROID(ch) && (iLang = skill_lookup("binary")) >= 0)
                {
                    ch->pcdata->learned[iLang] = 100;
                    ch->speaking = LANG_BINARY;
                    SET_BIT(ch->speaks, LANG_BINARY);
                }
                else
                    bug("Nanny: invalid racial language.");
            }
            else
            {
                if ((iLang = skill_lookup(lang_names[iLang])) < 0)
                    bug("Nanny: cannot find racial language.");
                else
                {
                    ch->pcdata->learned[iLang] = 100;
                    ch->speaking = race_table[ch->race].language;
                    if (ch->race == RACE_QUARREN && (iLang = skill_lookup("quarren")) >= 0)
                    {
                        ch->pcdata->learned[iLang] = 100;
                        SET_BIT(ch->speaks, LANG_QUARREN);
                    }
                    if (ch->race == RACE_MON_CALAMARI && (iLang = skill_lookup("basic")) >= 0)
                        ch->pcdata->learned[iLang] = 100;
                }
            }

            /* ch->resist           += race_table[ch->race].resist;    drats */
            /* ch->susceptible     += race_table[ch->race].suscept;    drats */

            name_stamp_stats(ch);

            {
                int ability;

                for (ability = 0; ability < MAX_ABILITY; ability++)
                {
                    ch->skill_level[ability] = 1;
                    ch->experience[ability] = 0;
                    ch->bonus[ability] = 0;
                }
            }
            ch->top_level = 1;
            ch->hit = ch->max_hit;
            ch->hit += race_table[ch->race].hit;
            ch->move = ch->max_move;
            if (ch->perm_frc > 0)
                ch->max_mana = 100 + 100 * ch->perm_frc;
            else
                ch->max_mana = 0;
            ch->mana = ch->max_mana;
            sprintf_s(buf, "%s the %s", ch->name, race_table[ch->race].race_name);
            set_title(ch, buf);

            /* Newbies get some cash! - Tawnos */
            ch->gold = 10000;

            /* Added by Narn.  Start new characters with autoexit and autgold
               already turned on.  Very few people don't use those. */
            SET_BIT(ch->act, PLR_AUTOGOLD);
            SET_BIT(ch->act, PLR_AUTOEXIT);

            /* New players don't have to earn some eq */

            if (ch->race != RACE_DEFEL)
            {
                obj = create_object(get_obj_index(121), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_HANDS);
                obj = create_object(get_obj_index(122), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_BODY);
                obj = create_object(get_obj_index(123), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_HEAD);
                obj = create_object(get_obj_index(124), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_FEET);
                obj = create_object(get_obj_index(125), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_WAIST);
                obj = create_object(get_obj_index(126), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_ARMS);
                obj = create_object(get_obj_index(127), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_LEGS);
                obj = create_object(get_obj_index(128), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_SHIELD);
                obj = create_object(get_obj_index(129), 0);
                obj_to_char(obj, ch);
                equip_char(ch, obj, WEAR_ABOUT);
            }
            obj = create_object(get_obj_index(130), 0);
            obj_to_char(obj, ch);
            equip_char(ch, obj, WEAR_WIELD);
            obj = create_object(get_obj_index(131), 0);
            obj_to_char(obj, ch);
            equip_char(ch, obj, WEAR_EARS);
            obj = create_object(get_obj_index(132), 0);
            obj_to_char(obj, ch);
            equip_char(ch, obj, WEAR_BACK);

            if (!sysdata.WAIT_FOR_AUTH)
            {
                char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
                ch->pcdata->auth_state = 3;
            }
            else
            {
                char_to_room(ch, get_room_index(ROOM_VNUM_SCHOOL));
                ch->pcdata->auth_state = 1;
                SET_BIT(ch->pcdata->flags, PCFLAG_UNAUTHED);
            }
        }
        else if (!IS_IMMORTAL(ch) && ch->pcdata->release_date > current_time)
        {
            char_to_room(ch, get_room_index(6));
        }

        else if (ch->in_room && !IS_IMMORTAL(ch) && !IS_SET(ch->in_room->room_flags, ROOM_SPACECRAFT) &&
                 ch->in_room != get_room_index(6))
        {
            char_to_room(ch, ch->in_room);
        }
        else if (ch->in_room && !IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags, ROOM_SPACECRAFT) &&
                 ch->in_room != get_room_index(6))
        {
            /*SHIP_DATA *ship;

            for ( ship = first_ship; ship; ship = ship->next )
              if ( ch->in_room->vnum >= ship->firstroom && ch->in_room->vnum <= ship->lastroom )
                    if ( ship->clazz != SHIP_PLATFORM || ship->starsystem ) */
            char_to_room(ch, ch->in_room);
        }
        else
        {
            char_to_room(ch, get_room_index(wherehome(ch)));
        }

        if (get_timer(ch, TIMER_SHOVEDRAG) > 0)
            remove_timer(ch, TIMER_SHOVEDRAG);

        if (get_timer(ch, TIMER_PKILLED) > 0)
            remove_timer(ch, TIMER_PKILLED);
        if (ch->plr_home != NULL)
        {
            char filename[256];
            FILE* fph;
            ROOM_INDEX_DATA* storeroom = ch->plr_home;
            OBJ_DATA* obj;
            OBJ_DATA* obj_next;

            for (obj = storeroom->first_content; obj; obj = obj_next)
            {
                obj_next = obj->next_content;
                extract_obj(obj);
            }

            sprintf_s(filename, "%s%c/%s.home", PLAYER_DIR, tolower(ch->name[0]), capitalize(ch->name).c_str());
            if ((fph = fopen(filename, "r")) != NULL)
            {
                int iNest;
                bool found;
                OBJ_DATA *tobj, *tobj_next;

                rset_supermob(storeroom);
                for (iNest = 0; iNest < MAX_NEST; iNest++)
                    rgObjNest[iNest] = NULL;

                found = true;
                for (;;)
                {
                    char letter;
                    char* word;

                    letter = fread_letter(fph);
                    if (letter == '*')
                    {
                        fread_to_eol(fph);
                        continue;
                    }

                    if (letter != '#')
                    {
                        bug("Load_plr_home: # not found.", 0);
                        bug(ch->name, 0);
                        break;
                    }

                    word = fread_word(fph);
                    if (!str_cmp(word, "OBJECT")) /* Objects	*/
                        fread_obj(supermob, fph, OS_CARRY);
                    else if (!str_cmp(word, "END")) /* Done		*/
                        break;
                    else
                    {
                        bug("Load_plr_home: bad section.", 0);
                        bug(ch->name, 0);
                        break;
                    }
                }

                fclose(fph);

                for (tobj = supermob->first_carrying; tobj; tobj = tobj_next)
                {
                    tobj_next = tobj->next_content;
                    obj_from_char(tobj);
                    obj_to_room(tobj, storeroom);
                }

                release_supermob();
            }
        }

        {
            int ability;

            for (ability = 0; ability < MAX_ABILITY; ability++)
                if (ch->skill_level[ability] > max_level(ch, ability))
                    ch->skill_level[ability] = max_level(ch, ability);
        }

        //    act( AT_ACTION, "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
        do_look(ch, MAKE_TEMP_STRING("auto"));
        mail_count(ch);
        break;

        /* Far too many possible screwups if we do it this way. -- Altrag */
        /*        case CON_NEW_LANGUAGE:
                for ( iLang = 0; lang_array[iLang] != LANG_UNKNOWN; iLang++ )
                if ( !str_prefix( argument, lang_names[iLang] ) )
                    if ( can_learn_lang( ch, lang_array[iLang] ) )
                    {
                        add_char( ch );
                        SET_BIT( ch->speaks, lang_array[iLang] );
                        set_char_color( AT_SAY, ch );
                        ch_printf( ch, "You can now speak %s.\n\r", lang_names[iLang] );
                        d->connected = CON_PLAYING;
                        return;
                    }
            set_char_color( AT_SAY, ch );
            write_to_buffer( d, "You may not learn that language.  Please choose another.\n\r"
                          "New language: ", 0 );
            break;*/
    }

    return;
}

bool authenticate_user(const std::string& rawName, const std::string& password)
{
    if (rawName.length() == 0)
        return false;

    std::string name = rawName;
    name[0] = UPPER(name[0]);

    if (!check_parse_name(name.c_str()))
    {
        return false;
    }

    if (name == "New")
    {
        return false; // TODO new player creation via SSH
    }

    // TODO check_playing - looks like a mess

    DESCRIPTOR_DATA d = {};

    bool found = load_char_obj(d, name.c_str(), true);

    CHAR_DATA* ch = d.character;

    if (!found)
    {
        DISPOSE(ch);
        return false;
    }

    if (IS_SET(ch->act, PLR_DENY))
    {
        sprintf_s(log_buf, "Denying access to %s", name.c_str());
        log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
        DISPOSE(ch);
        return false;
    }

    if (sysdata.wizlock == 1 && !IS_IMMORTAL(ch))
    {
        DISPOSE(ch);
        return false;
    }

    if (!strcmp(smaug_crypt(password.c_str()), ch->pcdata->pwd))
    {
        DISPOSE(ch);
        return true;
    }

    DISPOSE(ch);
    return false;
}

std::vector<Pubkey> get_public_keys_for_user(const std::string& rawName)
{
    if (rawName.length() == 0)
        return {};

    std::string name = rawName;
    name[0] = UPPER(name[0]);

    if (!check_parse_name(name.c_str()))
    {
        return {};
    }

    DESCRIPTOR_DATA d = {};
    bool found = load_char_obj(d, name.c_str(), true);
    CHAR_DATA* ch = d.character;

    if (ch == nullptr)
        return {};

    bug("get_public_keys_for_user is unimplemented");

    DISPOSE(ch);
    return {};
}

std::shared_ptr<void> handle_new_authenticated_connection(std::shared_ptr<Connection> connection, const std::string& rawName)
{
    std::string name = rawName;
    name[0] = UPPER(name[0]);

    auto dnew = std::make_shared<DESCRIPTOR_DATA>();
    bool found = load_char_obj(*dnew, name.c_str(), true);
    assert(found);

    dnew->connection.swap(connection);
    dnew->scrlen = 24;
    dnew->user = STRALLOC(name.c_str());
    dnew->prevcolor = 0x07;

    // TODO this is where IP bans would get processed - skipping for now

    g_descriptors.push_back(dnew);

    /*
     * Send the greeting. Forces new color function - Tawnos
     */
    {
        extern char* help_greeting;
        if (help_greeting[0] == '.')
            send_to_desc_color2(help_greeting + 1, dnew);
        else
            send_to_desc_color2(help_greeting, dnew);
    }

    char buf[MAX_STRING_LENGTH] = {};

    if (g_descriptors.size() > sysdata.maxplayers)
        sysdata.maxplayers = g_descriptors.size();
    if (sysdata.maxplayers > sysdata.alltimemax)
    {
        if (sysdata.time_of_max)
            DISPOSE(sysdata.time_of_max);
        sprintf_s(buf, "%24.24s", ctime(&current_time));
        sysdata.time_of_max = str_dup(buf);
        sysdata.alltimemax = sysdata.maxplayers;
        sprintf_s(log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax);
        log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
        to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
        save_sysdata(sysdata);
    }

    write_to_buffer(dnew, "\n\r", 2);

    write_to_buffer(dnew, echo_on_str, 0);

    CHAR_DATA* ch = dnew->character;

    bool chk = check_reconnect(dnew, ch->name, true);
    if (chk == BERR)
    {
        if (dnew->character && dnew->character->desc)
            dnew->character->desc = NULL;
        close_socket(dnew, false);
        return nullptr;
    }
    if (chk == true)
        return dnew;

    if (check_multi(dnew, ch->name))
    {
        close_socket(dnew, false);
        return nullptr;
    }

    sprintf_s(buf, ch->name);
    dnew->character->desc = NULL;
    free_char(dnew->character);
    bool fOld = load_char_obj(*dnew, buf, false);
    ch = dnew->character;
    sprintf_s(log_buf, "%s@%s(%s) has connected.", ch->name, dnew->connection->getHostname().c_str(), dnew->user);
    if (ch->top_level < LEVEL_DEMI)
    {
        /*to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );*/
        log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
    }
    else
        log_string_plus(log_buf, LOG_COMM, ch->top_level);

    {
        tm* tme;
        time_t now;
        char day[50];
        now = time(0);
        tme = localtime(&now);
        strftime(day, 50, "%a %b %d %H:%M:%S %Y", tme);
        sprintf_s(log_buf, "%-20s     %-24s    %s", ch->name, day, dnew->connection->getHostname().c_str());
        write_last_file(log_buf);
    }

    show_title(dnew);
    if (ch->pcdata->area)
    {
        do_loadarea(ch, MAKE_TEMP_STRING(""));
        assign_area(ch);
    }
    if (ch->max_mana != (ch->force_control + ch->force_sense + ch->force_alter) * 3 * ch->force_level_status)
        ch->max_mana = (ch->force_control + ch->force_sense + ch->force_alter) * 3 * ch->force_level_status;
    ch->mana = ch->max_mana;
    if (ch->force_identified == 1 && ch->force_level_status == FORCE_MASTER)
    {
        int ft;
        FORCE_SKILL* skill;
        if (ch->force_skill[FORCE_SKILL_PARRY] < 50)
            ch->force_skill[FORCE_SKILL_PARRY] = 50;
        ft = ch->force_type;
        for (skill = first_force_skill; skill; skill = skill->next)
            if ((skill->type == ft || skill->type == FORCE_GENERAL) && ch->force_skill[skill->index] < 50 &&
                (strcmp(skill->name, "master") && strcmp(skill->name, "student") && strcmp(skill->name, "promote") &&
                 strcmp(skill->name, "instruct")))
                ch->force_skill[skill->index] = 50;
    }

    return dnew;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name(const char* name)
{
    /*
     * Reserved words.
     */
    if (is_name(name, "all auto someone immortal self god supreme demigod dog guard cityguard cat cornholio spock "
                      "hicaine hithoric death ass fuck shit piss crap quit public phines"))
        return false;

    /*
     * Length restrictions.
     */
    if (strlen(name) < 3)
        return false;

    if (strlen(name) > 12)
        return false;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
        const char* pc;
        bool fIll;

        fIll = true;
        for (pc = name; *pc != '\0'; pc++)
        {
            if (!isalpha(*pc))
                return false;
            if (LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
                fIll = false;
        }

        if (fIll)
            return false;
    }

    /*
     * Code that followed here used to prevent players from naming
     * themselves after mobs... this caused much havoc when new areas
     * would go in...
     */

    return true;
}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect(std::shared_ptr<DESCRIPTOR_DATA> d, char* name, bool fConn)
{
    CHAR_DATA* ch;

    for (ch = first_char; ch; ch = ch->next)
    {
        if (!IS_NPC(ch) && (!fConn || !ch->desc) && ch->name && !str_cmp(name, ch->name))
        {
            if (fConn && ch->switched)
            {
                write_to_buffer(d, "Already playing.\n\rName: ", 0);
                d->connected = CON_GET_NAME;
                if (d->character)
                {
                    /* clear descriptor pointer to get rid of bug message in log */
                    d->character->desc = NULL;
                    free_char(d->character);
                    d->character = NULL;
                }
                return BERR;
            }
            if (fConn == false)
            {
                DISPOSE(d->character->pcdata->pwd);
                d->character->pcdata->pwd = str_dup(ch->pcdata->pwd);
            }
            else
            {
                /* clear descriptor pointer to get rid of bug message in log */
                d->character->desc = NULL;
                free_char(d->character);
                d->character = ch;
                ch->desc = d.get();
                ch->timer = 0;
                send_to_char("Reconnecting.\n\r", ch);
                act(AT_ACTION, "$n has reconnected.", ch, NULL, NULL, TO_ROOM);
                sprintf_s(log_buf, "%s@%s(%s) reconnected.", ch->name, d->connection->getHostname().c_str(), d->user);
                log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));
                /*
                        if ( ch->top_level < LEVEL_SAVIOR )
                          to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
                */
                d->connected = CON_PLAYING;
            }
            return true;
        }
    }

    return false;
}

/*
 * Check if already playing.
 */

bool check_multi(std::shared_ptr<DESCRIPTOR_DATA> d, char* name)
{
    for (auto dold : g_descriptors)
    {
        if (dold != d && (dold->character || dold->original) &&
            str_cmp(name, dold->original ? dold->original->name : dold->character->name) &&
            !str_cmp(dold->connection->getHostname().c_str(), d->connection->getHostname().c_str()))
        {
            // TODO hardcoded IPs FTW!
            const char* ok = "194.234.177";
            const char* ok2 = "209.183.133.229";
            int iloop;

            auto host = d->connection->getHostname().c_str();

            if (get_trust(d->character) >= LEVEL_SUPREME ||
                get_trust(dold->original ? dold->original : dold->character) >= LEVEL_SUPREME)
                return false;
            for (iloop = 0; iloop < 11; iloop++)
            {
                if (ok[iloop] != host[iloop])
                    break;
            }
            if (iloop >= 10)
                return false;
            for (iloop = 0; iloop < 11; iloop++)
            {
                if (ok2[iloop] != host[iloop])
                    break;
            }
            if (iloop >= 10)
                return false;
            write_to_buffer(d, "Sorry multi-playing is not allowed ... have you other character quit first.\n\r", 0);
            sprintf_s(log_buf, "%s attempting to multiplay with %s.",
                      dold->original ? dold->original->name : dold->character->name, d->character->name);
            log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
            d->character = NULL;
            free_char(d->character);
            return true;
        }
    }

    return false;
}

bool check_playing(std::shared_ptr<DESCRIPTOR_DATA> d, char* name, bool kick)
{
    for (auto dold : g_descriptors)
    {
        if (dold != d && (dold->character || dold->original) &&
            !str_cmp(name, dold->original ? dold->original->name : dold->character->name))
        {
            int cstate = dold->connected;
            auto ch = dold->original ? dold->original : dold->character;
            if (!ch->name || (cstate != CON_PLAYING && cstate != CON_EDITING))
            {
                write_to_buffer(d, "Already connected - try again.\n\r", 0);
                sprintf_s(log_buf, "%s already connected.", ch->name);
                log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
                return BERR;
            }
            if (!kick)
                return true;
            write_to_buffer(d, "Already playing... Kicking off old connection.\n\r", 0);
            write_to_buffer(dold, "Kicking off old connection... bye!\n\r", 0);
            close_socket(dold, false);
            /* clear descriptor pointer to get rid of bug message in log */
            d->character->desc = NULL;
            free_char(d->character);
            d->character = ch;
            ch->desc = d.get();
            ch->timer = 0;
            if (ch->switched)
                do_return(ch->switched, MAKE_TEMP_STRING(""));
            ch->switched = NULL;
            send_to_char("Reconnecting.\n\r", ch);
            act(AT_ACTION, "$n has reconnected, kicking off old link.", ch, NULL, NULL, TO_ROOM);
            sprintf_s(log_buf, "%s@%s reconnected, kicking off old link.", ch->name,
                      d->connection->getHostname().c_str());
            log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));
            /*
                    if ( ch->top_level < LEVEL_SAVIOR )
                      to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
            */
            d->connected = cstate;
            return true;
        }
    }

    return false;
}

void stop_idling(CHAR_DATA* ch)
{
    if (!ch || !ch->desc || ch->desc->connected != CON_PLAYING || !ch->was_in_room ||
        ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
        return;

    ch->timer = 0;
    char_from_room(ch);
    char_to_room(ch, ch->was_in_room);
    ch->was_in_room = NULL;
    act(AT_ACTION, "$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
    return;
}

/*
 * Write to one char. Commented out in favour of colour
 * Update: Need a use for this, no color, removes &'s from string
 * Used for infrared viewing, bright red on players/mobiles
 * (sets color before sending text, act_info.c send_char_to_char_0)
 */
void send_to_char_noand(const char* txt, CHAR_DATA* ch)
{
    char buf[MAX_STRING_LENGTH];
    if (!ch)
    {
        bug("Send_to_char: NULL *ch");
        return;
    }
    if (txt && ch->desc)
    {
        sprintf_s(buf, "%s", txt);
        sprintf_s(buf, "%s", remand(buf).c_str());
        write_to_buffer(ch->desc, buf, strlen(buf));
    }
    return;
}

/*
 * Same as above, but for Descriptors, uses make_color_sequence_desc, for
 * descriptors. -Tawnos
 */
void send_to_desc_color2(const char* txt, DESCRIPTOR_DATA* d)
{
    const char* colstr = nullptr;
    const char* prevstr = txt;
    char colbuf[20];
    int ln;

    if (!d)
    {
        bug("send_to_desc_color2: NULL *d");
        return;
    }
    if (!txt || !d)
        return;

    /* Clear out old color stuff */
    while ((colstr = strpbrk(prevstr, "&^")) != NULL)
    {
        if (colstr > prevstr)
            write_to_buffer(d, prevstr, (colstr - prevstr));
        ln = make_color_sequence_desc(colstr, colbuf, d);
        if (ln < 0)
        {
            prevstr = colstr + 1;
            break;
        }
        else if (ln > 0)
            write_to_buffer(d, colbuf, ln);
        prevstr = colstr + 2;
    }
    if (*prevstr)
        write_to_buffer(d, prevstr, 0);
    return;
}

void send_to_desc_color2(const char* txt, std::shared_ptr<DESCRIPTOR_DATA> d)
{
    send_to_desc_color2(txt, d.get());
}

std::string obj_short(OBJ_DATA* obj)
{
    if (obj->count > 1)
        return (boost::format{"%s (%d)"} % obj->short_descr % obj->count).str();
    return obj->short_descr;
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */
/* Changed so it shows PERS instead of ch->name... sometimes -- Tawnos */
#define NAME(ch) (IS_NPC(ch) ? ch->short_descr : ch->name)
char* act_string(const char* format, CHAR_DATA* to, CHAR_DATA* ch, const void* arg1, const void* arg2)
{
    static const char* he_she[] = {"it", "he", "she"};
    static const char* him_her[] = {"it", "him", "her"};
    static const char* his_her[] = {"its", "his", "her"};
    static char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    char* point = buf;
    const char* str = format;
    const char* i;
    CHAR_DATA* vch = (CHAR_DATA*)arg2;
    OBJ_DATA* obj1 = (OBJ_DATA*)arg1;
    OBJ_DATA* obj2 = (OBJ_DATA*)arg2;

    while (*str != '\0')
    {
        std::string tempString;
        if (*str != '$')
        {
            *point++ = *str++;
            continue;
        }
        ++str;
        if (!arg2 && *str >= 'A' && *str <= 'Z')
        {
            bug("Act: missing arg2 for code %c:", *str);
            bug(format);
            i = " <@@@> ";
        }
        else
        {
            switch (*str)
            {
            default:
                bug("Act: bad code %c.", *str);
                i = " <@@@> ";
                break;
            case 't':
                i = (char*)arg1;
                break;
            case 'T':
                i = (char*)arg2;
                break;
            case 'n':
                i = (to ? PERS(ch, to) : NAME(ch));
                break;
            case 'N':
                i = (to ? PERS(vch, to) : NAME(vch));
                break;
            case 'e':
                if (ch->sex > 2 || ch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", ch->name, ch->sex);
                    i = "it";
                }
                else
                    i = he_she[URANGE(0, ch->sex, 2)];
                break;
            case 'E':
                if (vch->sex > 2 || vch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", vch->name, vch->sex);
                    i = "it";
                }
                else
                    i = he_she[URANGE(0, vch->sex, 2)];
                break;
            case 'm':
                if (ch->sex > 2 || ch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", ch->name, ch->sex);
                    i = "it";
                }
                else
                    i = him_her[URANGE(0, ch->sex, 2)];
                break;
            case 'M':
                if (vch->sex > 2 || vch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", vch->name, vch->sex);
                    i = "it";
                }
                else
                    i = him_her[URANGE(0, vch->sex, 2)];
                break;
            case 's':
                if (ch->sex > 2 || ch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", ch->name, ch->sex);
                    i = "its";
                }
                else
                    i = his_her[URANGE(0, ch->sex, 2)];
                break;
            case 'S':
                if (vch->sex > 2 || vch->sex < 0)
                {
                    bug("act_string: player %s has sex set at %d!", vch->name, vch->sex);
                    i = "its";
                }
                else
                    i = his_her[URANGE(0, vch->sex, 2)];
                break;
            case 'q':
                i = (to == ch) ? "" : "s";
                break;
            case 'Q':
                i = (to == ch) ? "your" : his_her[URANGE(0, ch->sex, 2)];
                break;
            case 'p':
                tempString = obj_short(obj1);
                i = (!to || can_see_obj(to, obj1) ? tempString.c_str() : "something");
                break;
            case 'P':
                tempString = obj_short(obj2);
                i = (!to || can_see_obj(to, obj2) ? tempString.c_str() : "something");
                break;
            case 'd':
                if (!arg2 || ((char*)arg2)[0] == '\0')
                    i = "door";
                else
                {
                    one_argument((char*)arg2, fname);
                    i = fname;
                }
                break;
            }
        }
        ++str;
        while ((*point = *i) != '\0')
            ++point, ++i;
    }
    strcpy(point, "\n\r");
    buf[0] = UPPER(buf[0]);
    return buf;
}
#undef NAME

void act(sh_int AType, const char* format, CHAR_DATA* ch, const void* arg1, const void* arg2, int type)
{
    char* txt;
    CHAR_DATA* to;
    CHAR_DATA* vch = (CHAR_DATA*)arg2;

    /*
     * Discard null and zero-length messages.
     */
    if (!format || format[0] == '\0')
        return;

    if (!ch)
    {
        bug("Act: null ch. (%s)", format);
        return;
    }

    if (!ch->in_room)
        to = NULL;
    else if (type == TO_CHAR)
        to = ch;
    else if (type == TO_MUD)
        to = first_char;
    else
        to = ch->in_room->first_person;

    /*
     * ACT_SECRETIVE handling
     */
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_SECRETIVE) && type != TO_CHAR)
        return;

    if (type == TO_VICT)
    {
        if (!vch)
        {
            bug("Act: null vch with TO_VICT.");
            bug("%s (%s)", ch->name, format);
            return;
        }
        if (!vch->in_room)
        {
            bug("Act: vch in NULL room!");
            bug("%s -> %s (%s)", ch->name, vch->name, format);
            return;
        }
        to = vch;
        /*	to = vch->in_room->first_person;*/
    }

    if (MOBtrigger && type != TO_CHAR && type != TO_VICT && to)
    {
        OBJ_DATA* to_obj;

        txt = act_string(format, NULL, ch, arg1, arg2);

        if (to && IS_SET(to->in_room->progtypes, ACT_PROG))
            rprog_act_trigger(txt, to->in_room, ch, (OBJ_DATA*)arg1, (void*)arg2);
        for (to_obj = to->in_room->first_content; to_obj; to_obj = to_obj->next_content)
            if (IS_SET(to_obj->pIndexData->progtypes, ACT_PROG))
                oprog_act_trigger(txt, to_obj, ch, (OBJ_DATA*)arg1, (void*)arg2);
    }

    /* Anyone feel like telling me the point of looping through the whole
       room when we're only sending to one char anyways..? -- Alty */
    for (; to; to = (type == TO_MUD) ? to->next : (type == TO_CHAR || type == TO_VICT) ? NULL : to->next_in_room)

    {
        if ((!to->desc && (IS_NPC(to) && !IS_SET(to->pIndexData->progtypes, ACT_PROG))) || !IS_AWAKE(to))
            continue;
        if (type == TO_MUD && (to == ch || to == vch))
            continue;
        if (type == TO_CHAR && to != ch)
            continue;
        if (type == TO_VICT && (to != vch || to == ch))
            continue;
        if (type == TO_ROOM && to == ch)
            continue;
        if (type == TO_NOTVICT && (to == ch || to == vch))
            continue;

        txt = act_string(format, to, ch, arg1, arg2);
        if (to->desc)
        {
            set_char_color(AType, to);
            send_to_char_color(txt, to);
        }
        if (MOBtrigger)
        {
            /* Note: use original string, not string with ANSI. -- Alty */
            mprog_act_trigger(txt, to, ch, (OBJ_DATA*)arg1, (void*)arg2);
        }
    }
    MOBtrigger = true;
    return;
}

void do_name(CHAR_DATA* ch, char* argument)
{
    char fname[1024];
    struct stat fst;
    CHAR_DATA* tmp;

    if (!NOT_AUTHED(ch) || ch->pcdata->auth_state != 2)
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }

    argument[0] = UPPER(argument[0]);

    if (!check_parse_name(argument))
    {
        send_to_char("Illegal name, try another.\n\r", ch);
        return;
    }

    if (!str_cmp(ch->name, argument))
    {
        send_to_char("That's already your name!\n\r", ch);
        return;
    }

    for (tmp = first_char; tmp; tmp = tmp->next)
    {
        if (!str_cmp(argument, tmp->name))
            break;
    }

    if (tmp)
    {
        send_to_char("That name is already taken.  Please choose another.\n\r", ch);
        return;
    }

    sprintf_s(fname, "%s%c/%s", PLAYER_DIR, tolower(argument[0]), capitalize(argument).c_str());
    if (stat(fname, &fst) != -1)
    {
        send_to_char("That name is already taken.  Please choose another.\n\r", ch);
        return;
    }

    STRFREE(ch->name);
    ch->name = STRALLOC(argument);
    send_to_char("Your name has been changed.  Please apply again.\n\r", ch);
    ch->pcdata->auth_state = 0;
    return;
}

char* hit_prompt(CHAR_DATA* ch)
{
    CHAR_DATA* victim;
    int percent;
    static char pbuf[MAX_STRING_LENGTH];
    if ((victim = who_fighting(ch)) != NULL)
    {
        if (victim->max_hit > 0)
            percent = (100 * victim->hit) / victim->max_hit;
        else
            percent = -1;
        if (percent >= 100)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m++\x1b[1;33m++\x1b[1;32m++");
        else if (percent >= 90)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m++\x1b[1;33m++\x1b[1;32m+\x1b[1;30m+");
        else if (percent >= 80)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m++\x1b[1;33m++\x1b[1;30m++");
        else if (percent >= 70)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m++\x1b[1;33m+\x1b[1;30m+++");
        else if (percent >= 60)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m++\x1b[1;30m++++");
        else if (percent >= 50)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[0;33m+\x1b[1;30m+++++");
        else if (percent >= 40)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m++\x1b[1;30m++++++");
        else if (percent >= 30)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;31m+\x1b[1;30m+++++++");
        else if (percent >= 20)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m++\x1b[1;30m++++++++");
        else if (percent >= 10)
            sprintf_s(pbuf, "\x1b[1;30m\x1b[0;31m+\x1b[1;30m+++++++++");
        else
            sprintf_s(pbuf, "\x1b[1;30m++++++++++");
    }
    return pbuf;
}

char* default_prompt(CHAR_DATA* ch)
{
    static char buf[MAX_STRING_LENGTH];
    strcpy_s(buf, "");
    if (ch->skill_level[FORCE_ABILITY] > 1 || get_trust(ch) >= LEVEL_IMMORTAL)
        strcat_s(buf, "&pForce:&P%m/&p%M  &pAlign:&P%a\n\r");
    //  strcat_s(buf, "&BHealth:&C%h&B/%H  &BMovement:&C%v&B/%V  &w%e");
    //  strcat_s(buf, "&C >&w");

    strcat_s(buf, "&G[&zHp:&w%h&G/&w%H&G] &G[&zMv:&w%v&G/&w%V&G] &G(&zAlign:&w%a&G) &w");
    if (ch->position == POS_FIGHTING)
        strcat_s(buf, "&W&G[%e&W&G] &w");
    return buf;
}

int getcolor(char clr)
{
    static const char* colors = "xrgObpcwzRGYBPCW";
    int r;

    for (r = 0; r < 16; r++)
        if (clr == colors[r])
            return r;
    return -1;
}

void display_prompt(DESCRIPTOR_DATA* d)
{
    CHAR_DATA* ch = d->character;
    CHAR_DATA* och = (d->original ? d->original : d->character);
    bool ansi = (!IS_NPC(och) && IS_SET(och->act, PLR_ANSI));
    const char* prompt;
    char buf[MAX_STRING_LENGTH];
    char* pbuf = buf;
    int stat;

    if (!ch)
    {
        bug("display_prompt: NULL ch");
        return;
    }

    if (!IS_NPC(ch) && ch->substate != SUB_NONE && ch->pcdata->subprompt && ch->pcdata->subprompt[0] != '\0')
        prompt = ch->pcdata->subprompt;
    else if (IS_NPC(ch) || !ch->pcdata->prompt || !*ch->pcdata->prompt)
        prompt = default_prompt(ch);
    else
        prompt = ch->pcdata->prompt;

    if (ansi)
    {
        strcpy(pbuf, ANSI_RESET);
        d->prevcolor = 0x08;
        pbuf += 4;
    }

    /* Clear out old color stuff */
    /*  make_color_sequence(NULL, NULL, NULL);*/
    for (; *prompt; prompt++)
    {
        /*
         * '%' = prompt commands
         * Note: foreground changes will revert background to 0 (black)
         */
        if (*prompt != '%')
        {
            *(pbuf++) = *prompt;
            continue;
        }
        ++prompt;
        if (!*prompt)
            break;
        if (*prompt == *(prompt - 1))
        {
            *(pbuf++) = *prompt;
            continue;
        }
        switch (*(prompt - 1))
        {
        default:
            bug("Display_prompt: bad command char '%c'.", *(prompt - 1));
            break;
        case '%':
            *pbuf = '\0';
            stat = 0x80000000;
            switch (*prompt)
            {
            case '%':
                *pbuf++ = '%';
                *pbuf = '\0';
                break;
            case 'a':
                if (ch->top_level >= 10)
                    stat = ch->alignment;
                else if (IS_GOOD(ch))
                    strcpy(pbuf, "good");
                else if (IS_EVIL(ch))
                    strcpy(pbuf, "evil");
                else
                    strcpy(pbuf, "neutral");
                break;
            case 'e':
                if (ch->position == POS_FIGHTING)
                    strcpy(pbuf, hit_prompt(ch));
                // sprintf_s(pbuf, "%s", hit_prompt(ch));
                break;
            case 'h':
                stat = ch->hit;
                break;
            case 'H':
                stat = ch->max_hit;
                break;
            case 'm':
                if (IS_IMMORTAL(ch) || ch->skill_level[FORCE_ABILITY] > 1)
                    stat = ch->mana;
                else
                    stat = 0;
                break;
            case 'M':
                if (IS_IMMORTAL(ch) || ch->skill_level[FORCE_ABILITY] > 1)
                    stat = ch->max_mana;
                else
                    stat = 0;
                break;
            case 'n':
                sprintf(pbuf, "\n\r");
                break;
            case 'U':
                stat = sysdata.maxplayers;
                break;
            case 'v':
                stat = ch->move;
                break;
            case 'V':
                stat = ch->max_move;
                break;
            case 'g':
                stat = ch->gold;
                break;
            case 'r':
                if (IS_IMMORTAL(och))
                    stat = ch->in_room->vnum;
                break;
            case 'R':
                if (IS_SET(och->act, PLR_ROOMVNUM))
                    sprintf(pbuf, "<#%d> ", ch->in_room->vnum);
                break;
            case 'i':
                if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_WIZINVIS)) || (IS_NPC(ch) && IS_SET(ch->act, ACT_MOBINVIS)))
                    sprintf(pbuf, "(Invis %d) ", (IS_NPC(ch) ? ch->mobinvis : ch->pcdata->wizinvis));
                else if (IS_AFFECTED(ch, AFF_INVISIBLE))
                    sprintf(pbuf, "(Invis) ");
                break;
            case 'I':
                stat = (IS_NPC(ch) ? (IS_SET(ch->act, ACT_MOBINVIS) ? ch->mobinvis : 0)
                                   : (IS_SET(ch->act, PLR_WIZINVIS) ? ch->pcdata->wizinvis : 0));
                break;
            }
            if (stat != 0x80000000)
                sprintf(pbuf, "%d", stat);
            pbuf += strlen(pbuf);
            break;
        }
    }
    *pbuf = '\0';
    send_to_char(buf, ch);
    return;
}

int make_color_sequence_desc(const char* col, char* buf, DESCRIPTOR_DATA* d)
{
    int ln;
    const char* ctype = col;
    unsigned char cl;

    col++;
    if (!*col)
        ln = -1;
    else if (*ctype != '&' && *ctype != '^')
    {
        bug("Make_color_sequence: command '%c' not '&' or '^'.", *ctype);
        ln = -1;
    }
    else if (*col == *ctype)
    {
        buf[0] = *col;
        buf[1] = '\0';
        ln = 1;
    }
    else
    {
        cl = d->prevcolor;
        switch (*ctype)
        {
        default:
            bug("Make_color_sequence: bad command char '%c'.", *ctype);
            ln = -1;
            break;
        case '&':
            if (*col == '-')
            {
                buf[0] = '~';
                buf[1] = '\0';
                ln = 1;
                break;
            }
        case '^': {
            int newcol;

            if ((newcol = getcolor(*col)) < 0)
            {
                ln = 0;
                break;
            }
            else if (*ctype == '&')
                cl = (cl & 0xF0) | newcol;
            else
                cl = (cl & 0x0F) | (newcol << 4);
        }
            if (cl == d->prevcolor)
            {
                ln = 0;
                break;
            }
            strcpy(buf, "\033[");
            if ((cl & 0x88) != (d->prevcolor & 0x88))
            {
                if (cl == 0x07)
                {
                    strcpy(buf, "\033[0;37");
                }
                else
                {
                    if ((cl & 0x08))
                        strcat(buf, "1;");
                    else
                        strcat(buf, "0;");
                    if ((cl & 0x80))
                        strcat(buf, "5;");
                }
                d->prevcolor = 0x07 | (cl & 0x88);
                ln = strlen(buf);
            }
            else
                ln = 2;

            if ((cl & 0x07) != (d->prevcolor & 0x07))
            {
                sprintf(buf + ln, "3%d;", cl & 0x07);
                ln += 3;
            }
            if ((cl & 0x70) != (d->prevcolor & 0x70))
            {
                sprintf(buf + ln, "4%d;", (cl & 0x70) >> 4);
                ln += 3;
            }
            if (buf[ln - 1] == ';')
                buf[ln - 1] = 'm';
            else
            {
                buf[ln++] = 'm';
                buf[ln] = '\0';
            }
            d->prevcolor = cl;
        }
    }
    if (ln <= 0)
        *buf = '\0';
    return ln;
}

void send_prompt(DESCRIPTOR_DATA* d)
{
    CHAR_DATA* ch = d->original ? d->original : d->character;
    if (IS_SET(ch->act, PLR_BLANK))
        write_to_buffer(d, "\n\r", 2);

    if (IS_SET(ch->act, PLR_PROMPT))
        display_prompt(d);
    if (IS_SET(ch->act, PLR_TELNET_GA))
        write_to_buffer(d, go_ahead_str, 0);
}

void handle_pager_input(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument)
{
    while (isspace(*argument))
        argument++;
    char pagecmd = *argument;

    char* last = nullptr;
    CHAR_DATA* ch = nullptr;
    int pclines = 0;
    int lines = 0;

    if (!d || !d->pagepoint || pagecmd == -1)
        return;

    ch = d->original ? d->original : d->character;
    pclines = UMAX(ch->pcdata->pagerlen, 5) - 1;
    switch (LOWER(pagecmd))
    {
    default:
        lines = 0;
        break;
    case 'b':
        lines = -1 - (pclines * 2);
        break;
    case 'r':
        lines = -1 - pclines;
        break;
    case 'q':
        d->pagetop = 0;
        d->pagepoint = NULL;
        send_prompt(d.get());
        DISPOSE(d->pagebuf);
        d->pagesize = MAX_STRING_LENGTH;
        return;
    }
    while (lines < 0 && d->pagepoint >= d->pagebuf)
        if (*(--d->pagepoint) == '\n')
            ++lines;
    if (*d->pagepoint == '\n' && *(++d->pagepoint) == '\r')
        ++d->pagepoint;
    if (d->pagepoint < d->pagebuf)
        d->pagepoint = d->pagebuf;
    for (lines = 0, last = d->pagepoint; lines < pclines; ++last)
        if (!*last)
            break;
        else if (*last == '\n')
            ++lines;
    if (*last == '\r')
        ++last;
    if (last != d->pagepoint)
    {
        write_to_buffer(d, d->pagepoint, (last - d->pagepoint));
        d->pagepoint = last;
    }
    while (isspace(*last))
        ++last;
    if (!*last)
    {
        d->pagetop = 0;
        d->pagepoint = NULL;
        send_prompt(d.get());
        DISPOSE(d->pagebuf);
        d->pagesize = MAX_STRING_LENGTH;
        return;
    }

    if (IS_SET(ch->act, PLR_ANSI))
        write_to_buffer(d, ANSI_LBLUE, 7);
    write_to_buffer(d, "(C)ontinue, (R)efresh, (B)ack, (Q)uit: [C] ", 0);

    if (IS_SET(ch->act, PLR_ANSI))
    {
        char buf[32];

        snprintf(buf, 32, "%s", color_str(d->pagecolor, ch));

        write_to_buffer(d, buf, 0);
    }
}

//  Warm reboot stuff, gotta make sure to thank Erwin for this :)
/*
void do_copyover(CHAR_DATA* ch, char* argument)
{
    FILE* fp;
    DESCRIPTOR_DATA* d, * d_next;
    SHIP_DATA* ship;
    PLANET_DATA* planet;
    char buf[100], buf2[100], buf3[100], buf4[100], buf5[100];

    if (str_cmp(argument, "now") && str_cmp(argument, "warn") && str_cmp(argument, "poscrash") && str_cmp(argument,
"nosave"))
    {
        send_to_char("Syntax: copyover (warn/now/nosave)\n\r", ch);
        return;
    }

    fp = fopen(COPYOVER_FILE, "w");

    if (!fp)
    {
        send_to_char("Copyover file not writeable, aborted.\n\r", ch);
        log_printf("Could not write to copyover file: %s", COPYOVER_FILE);
        perror("do_copyover:fopen");
        return;
    }

    // In memory of ||/Korey/Nathan/Eleven. -Tawnos

    if (!str_cmp(argument, "warn"))
    {
        do_echo(ch, "^g ^x &WCopyover Warning ^g ^x");
        return;
    }

    if (!str_cmp(argument, "poscrash"))
    {
        do_echo(ch, "^r ^x &WPossible Crash ^r ^x");
        return;
    }


    // Consider changing all saved areas here, if you use OLC

    // do_asave (NULL, ""); - autosave changed areas

    // Save ships

    if (str_cmp(argument, "nosave"))
        for (ship = first_ship; ship; ship = ship->next)
            save_ship(ship);

    // Save planets
    if (str_cmp(argument, "nosave"))
        for (planet = first_planet; planet; planet = planet->next)
            save_planet(planet);

    sprintf_s(buf, "\n\rYou have an intense feeling of Deja Vu...\n\r");
    //    sprintf_s (buf, "\n\r *** COPYOVER by %s - please remain seated!\n\r", ch->name);
    // For each playing descriptor, save its state
    for (d = first_descriptor; d; d = d_next)
    {
        CHAR_DATA* och = CH(d);
        d_next = d->next; // We delete from the list , so need to save this
        if (!d->character || d->connected != CON_PLAYING) // drop those logging on
        {
            write_to_descriptor(d, "\n\rSorry, we are rebooting."
                " Come back in a few minutes.\n\r", 0);
            close_socket(d, false); // throw'em out
        }
        else
        {
            do_save(d->character, "");
            fprintf(fp, "%d %s %s\n", d->descriptor, och->name, d->host);
            if (och->top_level == 1)
            {
                write_to_descriptor(d, "Since you are level one,"
                    "and level one characters do not save, you gain a free level!\n\r",
                    0);
                advance_level(och, 2);
                och->top_level++; // Advance_level doesn't do that
            }
            save_char_obj(och);
            write_to_descriptor(d, buf, 0);
        }
    }
    fprintf(fp, "-1\n");
    fclose(fp);

    // Close reserve and other always-open files and release other resources
    fclose(fpLOG);

    // exec - descriptors are inherited

    sprintf_s(buf, "%d", port);
    sprintf_s(buf2, "%d", control);

    execl(EXE_FILE, "swr", buf, "copyover", buf2, buf3,
        buf4, buf5, (char*)NULL);

    // Failed - sucessful exec will not return

    perror("do_copyover: execl");
    send_to_char("Copyover FAILED!\n\r", ch);

    // Here you might want to reopen fpReserve
    // Since I'm a neophyte type guy, I'll assume this is
    // a good idea and cut and past from main()

    if ((fpLOG = fopen(NULL_FILE, "r")) == NULL)
    {
        perror(NULL_FILE);
        exit(1);
    }

}
*/

// Recover from a copyover - load players
/*
void copyover_recover()
{
    DESCRIPTOR_DATA* d;
    FILE* fp;
    char name[100];
    char host[MAX_STRING_LENGTH];
    int desc;
    bool fOld;

    log_string("Copyover recovery initiated");

    fp = fopen(COPYOVER_FILE, "r");

    if (!fp) // there are some descriptors open which will hang forever then?
    {
        perror("copyover_recover:fopen");
        log_string("Copyover file not found. Exitting.\n\r");
        exit(1);
    }

    unlink(COPYOVER_FILE); // In case something crashes - doesn't prevent reading
    for (;;)
    {
        fscanf(fp, "%d %s %s\n", &desc, name, host);
        if (desc == -1)
            break;

        // Write something, and check if it goes error-free
        if (!write_to_descriptor(desc, "\n\rYou definitely remember being here before..\n\r", 0))
        {
            close(desc); // nope
            continue;
        }

        CREATE(d, DESCRIPTOR_DATA, 1);
        init_descriptor(d, desc); // set up various stuff

        d->host = STRALLOC(host);

        LINK(d, first_descriptor, last_descriptor, next, prev);
        d->connected = CON_COPYOVER_RECOVER; // negative so close_socket will cut them off

        // Now, find the pfile

        fOld = load_char_obj(d, name, false);

        if (!fOld) // Player file not found?!
        {
            write_to_descriptor(desc, "\n\rSomehow, your character was lost in the copyover sorry.\n\r", 0);
            close_socket(d, false);
        }
        else // ok!
        {
            write_to_descriptor(desc, "\n\rYour feeling of Deja Vu has subsided.\n\r", 0);
            //          write_to_descriptor (desc, "\n\rCopyover recovery complete.\n\r",0);

            // Just In Case,  Someone said this isn't necassary, but _why_ do we want to dump someone in limbo?
            if (!d->character->in_room)
                d->character->in_room = get_room_index(ROOM_VNUM_TEMPLE);

            // Insert in the char_list
            LINK(d->character, first_char, last_char, next, prev);

            char_to_room(d->character, d->character->in_room);
            do_look(d->character, "auto noprog");
            act(AT_ACTION, "$n materializes!", d->character, NULL, NULL, TO_ROOM);
            d->connected = CON_PLAYING;
        }

    }
    fclose(fp);
}
*/

void do_idealog(CHAR_DATA* ch, char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    one_argument(argument, arg);

    if (arg[0] == '\0')
    {
        send_to_pager("         &B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&w&WIDEA "
                      "LOG&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-&w&W\n\r",
                      ch);
        show_file(ch, IDEA_FILE);
        return;
    }

    if (!str_cmp(arg, "clear"))
    {
        sprintf_s(buf, "%sideas.txt", SYSTEM_DIR);
        sprintf_s(buf2, "%sideas.bak", SYSTEM_DIR);
        rename(buf, buf2);
        send_to_char("Idealog removed, saved as ideas.bak\n\r", ch);
        return;
    }
    else
    {
        send_to_char("         &B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&B-=&r-=&WIDEA "
                     "LOG&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-&B=-&r=-\n\r",
                     ch);
        show_file(ch, IDEA_FILE);
    }
    return;
}

void do_giveslug(CHAR_DATA* ch, char* argument)
{
    /*   SHIP_PROTOTYPE * prototype;
       CHAR_DATA	  * victim;

       if(argument[0] == '\0')
       {
        send_to_char("Give who a slug?\n\r", ch);
        return;
       }

       if ( (victim = get_char_world(ch, argument)) == NULL)
       {
        send_to_char("They aren't here.\n\r", ch);
        return;
       }

       if (IS_NPC(victim))
       {
        send_to_char("Only players can have slugs given to them.\n\r", ch);
        return;
       }

       prototype = get_ship_prototype( "Slug" );

                if ( prototype )
                {
                  SHIP_DATA *ship;
                  char shipname[MAX_STRING_LENGTH];
                  ship = make_ship( prototype );
                  ship_to_room( ship, 178 );
                  ship->location = 178;
                  ship->lastdoc = 178;
                  sprintf_s( shipname , "%s's %s %s" , victim->name , prototype->name , ship->filename );
                  STRFREE( ship->owner );
                  ship->owner = STRALLOC( victim->name );
                  STRFREE( ship->name );
                  ship->name = STRALLOC( shipname );
              ship->password = number_range(1111,9999);
              ship->shipyard = 1050;
                  save_ship( ship );
                  write_ship_list();
                }

        send_to_char("&w&GA slug has been created for you at the newbie docking bay.\n\r", victim);
        send_to_char("Done.\n\r", ch);
    */
}

FELLOW_DATA* knowsof(CHAR_DATA* ch, CHAR_DATA* victim)
{
    FELLOW_DATA* fellow;

    for (fellow = ch->first_fellow; fellow; fellow = fellow->next)
    {
        if (fellow->victim == victim->name)
            return fellow;
    }

    return NULL;
}

const char* PERS(CHAR_DATA* ch, CHAR_DATA* looker)
{
    static char buf[MAX_STRING_LENGTH];
    char race[MAX_STRING_LENGTH];
    FELLOW_DATA* fellow;

    if (can_see(looker, ch))
    {
        if (IS_NPC(ch))
            return ch->short_descr;
        else
        {
            if (IS_IMMORTAL(looker) || ch == looker)
                return ch->name;
            else if (ch->pcdata->disguise && ch->pcdata->disguise[0] != '\0')
                return ch->pcdata->disguise;
            else if ((fellow = knowsof(looker, ch)) != NULL)
                return fellow->knownas;
            else
            {
                if (IS_IMMORTAL(ch))
                    return ch->name;
                sprintf_s(race, "%s", npc_race[ch->race]);
                race[0] = tolower(race[0]);
                if (!IS_DROID(ch))
                    sprintf_s(buf, "%s %s %s of %s height", aoran(build_name[ch->build]).c_str(), race,
                              ch->sex == 1   ? "male"
                              : ch->sex == 2 ? "female"
                                             : "neutral",
                              height_name[ch->pheight]);
                else
                    sprintf_s(buf, "%s %s", aoran(droid_name[ch->build]).c_str(), race);
                return buf;
            }
        }
    }
    else
    {
        if (IS_IMMORTAL(ch))
            return "Immortal";
        else
            return "someone";
    }
}

void arms(std::shared_ptr<DESCRIPTOR_DATA> d, char* argument)
{
    CHAR_DATA* ch;
    char dmsgcmd[50];
    char tocmd[90];
    char to[80];
    char subj[100];
    char subjcmd[110];
    ch = d->character;

    switch (d->connected)
    {
    case CON_MAIN_MAIL_MENU:
        switch (*argument)
        {
        case 'H':
        case 'h':
            send_to_char("In ARIMS, there are 5 basic commands.\n\rl - lists all your mail messages\n\rw - writes a "
                         "message\n\rd - displays a message\n\rh - displays this help message\n\rq - quits ARIMS\n\r",
                         ch);
            send_main_mail_menu(d.get());
            break;

        case 'l':
        case 'L':
            do_mailroom(ch, MAKE_TEMP_STRING("list"));
            send_to_char("\n\r\n\rPlease enter a command or enter h for help: ", ch);
            break;
        case 'd':
        case 'D':
            send_to_char("Which Message? ", ch);
            d->connected = CON_MAIL_DISPLAY;
            break;
        case 'q':
        case 'Q':
            d->connected = CON_PLAYING;
            break;
        case 'w':
        case 'W':
            d->connected = CON_MAIL_WRITE_START;
        default:
            send_to_char("That's not a command!\n\r", ch);
            send_main_mail_menu(d.get());
            break;
        }
        break;

    case CON_MAIL_DISPLAY:
        strcpy_s(dmsgcmd, "read ");
        strcat_s(dmsgcmd, argument);
        do_mailroom(ch, dmsgcmd);
        send_to_char("\n\r\n\rPlease enter a command or enter h for help: ", ch);
        d->connected = CON_MAIN_MAIL_MENU;
        break;
    case CON_MAIL_WRITE_START:
        send_to_char("To: ", ch);
        d->connected = CON_MAIL_WRITE_TO;
        break;
    case CON_MAIL_WRITE_TO:
        strcpy_s(to, argument);
        strcpy_s(tocmd, "to ");
        strcat_s(tocmd, to);
        do_mailroom(ch, tocmd);
        send_to_char("\n\rSubject: ", ch);
        d->connected = CON_MAIL_WRITE_SUBJECT;
        break;
    case CON_MAIL_WRITE_SUBJECT:
        strcpy_s(subj, argument);
        strcpy_s(subjcmd, "subject ");
        strcat_s(subjcmd, subj);
        do_mailroom(ch, subjcmd);
        send_to_char("/n/rBody: ", ch);
        do_mailroom(ch, MAKE_TEMP_STRING("write"));
        do_mailroom(ch, MAKE_TEMP_STRING("post"));
        d->connected = CON_MAIN_MAIL_MENU;
        break;
    default:
        send_main_mail_menu(d.get());
        d->connected = CON_MAIN_MAIL_MENU;
        break;
    }
}

void send_main_mail_menu(DESCRIPTOR_DATA* d)
{
    CHAR_DATA* ch;
    ch = d->character;
    send_to_char("Please enter a command or enter h for help: ", ch);
}
