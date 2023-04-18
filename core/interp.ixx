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

#include "mud.hxx"

export module interp;

import mud;
import mud_prog;
import comm;

export bool check_social(CHAR_DATA* ch, const char* command, const char* argument);

/*
 * Log-all switch.
 */
export bool fLogAll = false;
export bool fLogPC = false;

CMDTYPE* command_hash[126];   /* hash table for cmd_table */
SOCIALTYPE* social_index[27]; /* hash table for socials   */

/*
 * Character not in position for command?
 */
export bool check_pos(CHAR_DATA* ch, sh_int position)
{
    if (ch->position < position)
    {
        switch (ch->position)
        {
        case POS_DEAD:
            send_to_char("A little difficult to do when you are DEAD...\n\r", ch);
            break;

        case POS_MORTAL:
        case POS_INCAP:
            send_to_char("You are hurt far too bad for that.\n\r", ch);
            break;

        case POS_STUNNED:
            send_to_char("You are too stunned to do that.\n\r", ch);
            break;

        case POS_SLEEPING:
            send_to_char("In your dreams, or what?\n\r", ch);
            break;

        case POS_RESTING:
            send_to_char("Nah... You feel too relaxed...\n\r", ch);
            break;

        case POS_SITTING:
            send_to_char("You can't do that sitting down.\n\r", ch);
            break;

        case POS_FIGHTING:
            send_to_char("No way!  You are still fighting!\n\r", ch);
            break;
        }
        return false;
    }
    return true;
}

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
export void interpret(CHAR_DATA* ch, char* argument)
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    char logname[MAX_INPUT_LENGTH];
    TIMER* timer = NULL;
    CMDTYPE* cmd = NULL;
    int trust;
    int loglvl;
    bool found;
    std::chrono::steady_clock::duration time_used;

    if (!ch)
    {
        bug("interpret: null ch!", 0);
        return;
    }

    found = false;
    if (ch->substate == SUB_REPEATCMD)
    {
        DO_FUN* fun;

        if ((fun = ch->last_cmd) == NULL)
        {
            ch->substate = SUB_NONE;
            bug("interpret: SUB_REPEATCMD with NULL last_cmd", 0);
            return;
        }
        else
        {
            int x;

            /*
             * yes... we lose out on the hashing speediness here...
             * but the only REPEATCMDS are wizcommands (currently)
             */
            for (x = 0; x < 126; x++)
            {
                for (cmd = command_hash[x]; cmd; cmd = cmd->next)
                    if (cmd->do_fun == fun)
                    {
                        found = true;
                        break;
                    }
                if (found)
                    break;
            }
            if (!found)
            {
                cmd = NULL;
                bug("interpret: SUB_REPEATCMD: last_cmd invalid", 0);
                return;
            }
            sprintf_s(logline, "(%s) %s", cmd->name, argument);
        }
    }

    if (!cmd)
    {
        /* Changed the order of these ifchecks to prevent crashing. */
        if (!argument || !strcmp(argument, ""))
        {
            bug("interpret: null argument!", 0);
            return;
        }

        /*
         * Strip leading spaces.
         */
        while (isspace(*argument))
            argument++;
        if (argument[0] == '\0')
            return;

        timer = get_timerptr(ch, TIMER_DO_FUN);

        /* REMOVE_BIT( ch->affected_by, AFF_HIDE ); */

        /*
         * Implement freeze command.
         */
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE))
        {
            send_to_char("You're totally frozen!\n\r", ch);
            return;
        }

        /*
         * Grab the command word.
         * Special parsing so ' can be a command,
         *   also no spaces needed after punctuation.
         */
        strcpy_s(logline, argument);
        if (!isalpha(argument[0]) && !isdigit(argument[0]))
        {
            command[0] = argument[0];
            command[1] = '\0';
            argument++;
            while (isspace(*argument))
                argument++;
        }
        else
            argument = one_argument(argument, command);

        /*
         * Look for command in command table.
         * Check for council powers and/or bestowments
         */
        trust = get_trust(ch);
        for (cmd = command_hash[LOWER(command[0]) % 126]; cmd; cmd = cmd->next)
            if (!str_prefix(command, cmd->name) &&
                (cmd->level <= trust || (!IS_NPC(ch) && ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0' &&
                                         is_name(cmd->name, ch->pcdata->bestowments) && cmd->level <= (trust + 5))))
            {
                found = true;
                break;
            }

        /*
         * Turn off afk bit when any command performed.
         */
        if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AFK) && (str_cmp(command, "AFK")))
        {
            REMOVE_BIT(ch->act, PLR_AFK);
            act(AT_GREY, "$n is no longer afk.", ch, NULL, NULL, TO_ROOM);
        }
    }

    /*
     * Log and snoop.
     */
    sprintf_s(lastplayercmd, "** %s: %s", ch->name, logline);

    if (found && cmd->log == LOG_NEVER)
        strcpy_s(logline, "XXXXXXXX XXXXXXXX XXXXXXXX");

    loglvl = found ? cmd->log : LOG_NORMAL;

    // Log all PC's
    if (!IS_NPC(ch) && fLogPC)
    {
        sh_int newloglvl = loglvl;

        if (ch->desc && ch->desc->original)
            sprintf_s(log_buf, "Log %s (%s): %s", ch->name, ch->desc->original->name, logline);
        else
            sprintf_s(log_buf, "Log %s: %s", ch->name, logline);

        if (newloglvl == LOG_NORMAL)
            newloglvl = LOG_ALL;

        log_string_plus(log_buf, newloglvl, get_trust(ch));
    }
    else if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_LOG)) || fLogAll || loglvl == LOG_BUILD || loglvl == LOG_HIGH ||
             loglvl == LOG_ALWAYS)
    {
        /* Added by Narn to show who is switched into a mob that executes
           a logged command.  Check for descriptor in case force is used. */
        if (ch->desc && ch->desc->original)
            sprintf_s(log_buf, "Log %s (%s): %s", ch->name, ch->desc->original->name, logline);
        else
            sprintf_s(log_buf, "Log %s: %s", ch->name, logline);

        /*
         * Make it so a 'log all' will send most output to the log
         * file only, and not spam the log channel to death	-Thoric
         */
        if (fLogAll && loglvl == LOG_NORMAL && (IS_NPC(ch) || !IS_SET(ch->act, PLR_LOG)))
            loglvl = LOG_ALL;

        /* This is handled in get_trust already */
        /*	if ( ch->desc && ch->desc->original )
              log_string_plus( log_buf, loglvl,
                ch->desc->original->level );
            else
        */
        log_string_plus(log_buf, loglvl, get_trust(ch));
    }

    /*
     * Secret Log. Added by Tawnos.
     */
    if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_SLOG)))
    {
        /* Added by Narn to show who is switched into a mob that executes
           a logged command.  Check for descriptor in case force is used. */
        if (ch->desc && ch->desc->original)
            sprintf_s(log_buf, "SecretLog %s (%s): %s", ch->name, ch->desc->original->name, logline);
        else
            sprintf_s(log_buf, "SecretLog %s: %s", ch->name, logline);

        append_to_file(SLOG_FILE, log_buf);
    }

    if (ch->desc && ch->desc->snoop_by)
    {
        sprintf_s(logname, "%s", ch->name);
        write_to_buffer(ch->desc->snoop_by, logname, 0);
        write_to_buffer(ch->desc->snoop_by, "% ", 2);
        write_to_buffer(ch->desc->snoop_by, logline, 0);
        write_to_buffer(ch->desc->snoop_by, "\n\r", 2);
    }

    if (cmd && timer && cmd->ooc == 0)
    {
        int tempsub;

        tempsub = ch->substate;
        ch->substate = SUB_TIMER_DO_ABORT;
        (timer->do_fun)(ch, MAKE_TEMP_STRING(""));
        if (char_died(ch))
            return;
        if (ch->substate != SUB_TIMER_CANT_ABORT)
        {
            ch->substate = tempsub;
            extract_timer(ch, timer);
        }
        else
        {
            ch->substate = tempsub;
            return;
        }
    }

    // Check for force skill
    if (check_force_skill(ch, command, argument))
        return;
    /*
     * Look for command in skill and socials table.
     */
    if (!found)
    {
        if (!check_skill(ch, command, argument) && !check_social(ch, command, argument))
        {
            EXIT_DATA* pexit;

            /* check for an auto-matic exit command */
            if ((pexit = find_door(ch, command, true)) != NULL && IS_SET(pexit->exit_info, EX_xAUTO))
            {
                if (IS_SET(pexit->exit_info, EX_CLOSED) &&
                    (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info, EX_NOPASSDOOR)))
                {
                    if (!IS_SET(pexit->exit_info, EX_SECRET))
                        act(AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
                    else
                        send_to_char("You cannot do that here.\n\r", ch);
                    return;
                }
                move_char(ch, pexit, 0);
                return;
            }
            if (rprog_custom_trigger(command, argument, ch))
                return;
            if (mprog_custom_trigger(command, argument, ch))
                return;
            if (oprog_custom_trigger(command, argument, ch))
                return;
            send_to_char("Huh?\n\r", ch);
        }
        return;
    }

    /*
     * Character not in position for command?
     */
    if (!check_pos(ch, cmd->position))
        return;

    /* Berserk check for flee.. maybe add drunk to this?.. but too much
       hardcoding is annoying.. -- Altrag */
    if (!str_cmp(cmd->name, "flee") && IS_AFFECTED(ch, AFF_BERSERK))
    {
        send_to_char("You aren't thinking very clearly..\n\r", ch);
        return;
    }

    /*
     * Dispatch the command.
     */
    ch->prev_cmd = ch->last_cmd; /* haus, for automapping */
    ch->last_cmd = cmd->do_fun;

    auto start_time = std::chrono::steady_clock::now();

    (*cmd->do_fun)(ch, argument);

    time_used = std::chrono::steady_clock::now() - start_time;

    /*
     * Update the record of how many times this command has been used (haus)
     */
    update_userec(time_used, &cmd->userec);

    /* laggy command notice: command took longer than 1 second */
    if (time_used > std::chrono::seconds(1))
    {
        using namespace std::chrono; // TODO clean up the duration_casts to subtract the seconds from the microseconds

        using floatseconds = std::chrono::duration<double, std::chrono::seconds::period>;
        sprintf_s(log_buf, "[*****] LAG: %s: %s %s (R:%d S:%.06f)", ch->name, cmd->name,
                  (cmd->log == LOG_NEVER ? "XXX" : argument), ch->in_room ? ch->in_room->vnum : 0,
                  floatseconds(time_used).count());
        log_string_plus(log_buf, LOG_NORMAL, get_trust(ch));
    }
}

export CMDTYPE* find_command(const char* command)
{
    CMDTYPE* cmd;
    int hash = LOWER(command[0]) % 126;

    for (cmd = command_hash[hash]; cmd; cmd = cmd->next)
        if (!str_prefix(command, cmd->name))
            return cmd;

    return NULL;
}

export SOCIALTYPE* find_social(const char* command)
{
    SOCIALTYPE* social;
    int hash;

    if (command[0] < 'a' || command[0] > 'z')
        hash = 0;
    else
        hash = (command[0] - 'a') + 1;

    for (social = social_index[hash]; social; social = social->next)
        if (!str_prefix(command, social->name))
            return social;

    return NULL;
}

bool check_social(CHAR_DATA* ch, const char* command, const char* argument)
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA* victim;
    SOCIALTYPE* social;

    if ((social = find_social(command)) == NULL)
        return false;

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE))
    {
        send_to_char("You are anti-social!\n\r", ch);
        return true;
    }

    switch (ch->position)
    {
    case POS_DEAD:
        send_to_char("Lie still; you are DEAD.\n\r", ch);
        return true;

    case POS_INCAP:
    case POS_MORTAL:
        send_to_char("You are hurt far too bad for that.\n\r", ch);
        return true;

    case POS_STUNNED:
        send_to_char("You are too stunned to do that.\n\r", ch);
        return true;

    case POS_SLEEPING:
        /*
         * I just know this is the path to a 12" 'if' statement.  :(
         * But two players asked for it already!  -- Furey
         */
        if (!str_cmp(social->name, "snore"))
            break;
        send_to_char("In your dreams, or what?\n\r", ch);
        return true;
    }

    one_argument(argument, arg);
    victim = NULL;
    if (arg[0] == '\0')
    {
        act(AT_SOCIAL, social->others_no_arg, ch, NULL, victim, TO_ROOM);
        act(AT_SOCIAL, social->char_no_arg, ch, NULL, victim, TO_CHAR);
    }
    else if ((victim = get_char_room(ch, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r", ch);
    }
    else if (victim == ch)
    {
        act(AT_SOCIAL, social->others_auto, ch, NULL, victim, TO_ROOM);
        act(AT_SOCIAL, social->char_auto, ch, NULL, victim, TO_CHAR);
    }
    else
    {
        act(AT_SOCIAL, social->others_found, ch, NULL, victim, TO_NOTVICT);
        act(AT_SOCIAL, social->char_found, ch, NULL, victim, TO_CHAR);
        act(AT_SOCIAL, social->vict_found, ch, NULL, victim, TO_VICT);

        if (!IS_NPC(ch) && IS_NPC(victim) && !IS_AFFECTED(victim, AFF_CHARM) && IS_AWAKE(victim) &&
            !IS_SET(victim->pIndexData->progtypes, ACT_PROG))
        {
            switch (number_bits(4))
            {
            case 0:
                if (!IS_SET(ch->in_room->room_flags, ROOM_SAFE) || IS_EVIL(ch))
                    multi_hit(victim, ch, TYPE_UNDEFINED);
                else if (IS_NEUTRAL(ch))
                {
                    act(AT_ACTION, "$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
                    act(AT_ACTION, "You slap $N.", victim, NULL, ch, TO_CHAR);
                    act(AT_ACTION, "$n slaps you.", victim, NULL, ch, TO_VICT);
                }
                else
                {
                    act(AT_ACTION, "$n acts like $N doesn't even exist.", victim, NULL, ch, TO_NOTVICT);
                    act(AT_ACTION, "You just ignore $N.", victim, NULL, ch, TO_CHAR);
                    act(AT_ACTION, "$n appears to be ignoring you.", victim, NULL, ch, TO_VICT);
                }
                break;

            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                act(AT_SOCIAL, social->others_found, victim, NULL, ch, TO_NOTVICT);
                act(AT_SOCIAL, social->char_found, victim, NULL, ch, TO_CHAR);
                act(AT_SOCIAL, social->vict_found, victim, NULL, ch, TO_VICT);
                break;

            case 9:
            case 10:
            case 11:
            case 12:
                act(AT_ACTION, "$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
                act(AT_ACTION, "You slap $N.", victim, NULL, ch, TO_CHAR);
                act(AT_ACTION, "$n slaps you.", victim, NULL, ch, TO_VICT);
                break;
            }
        }
    }

    return true;
}

/*
 * Return true if an argument is completely numeric.
 */
export bool is_number(const char* arg)
{
    if (*arg == '\0')
        return false;

    for (; *arg != '\0'; arg++)
    {
        if (!isdigit(*arg))
            return false;
    }

    return true;
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
export int number_argument(const char* argument, char* arg)
{
    char* pdot;
    int number;
    char buffer[MAX_STRING_LENGTH] = {};

    strcpy_s(buffer, argument);

    for (pdot = buffer; *pdot != '\0'; pdot++)
    {
        if (*pdot == '.')
        {
            *pdot = '\0';
            number = atoi(buffer);
            *pdot = '.';
            strcpy(arg, pdot + 1);
            return number;
        }
    }

    strcpy(arg, argument);
    return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
export const char* one_argument(const char* argument, char* arg_first)
{
    char cEnd;
    sh_int count;

    count = 0;

    while (isspace(*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0' || ++count >= 255)
    {
        if (*argument == cEnd)
        {
            argument++;
            break;
        }
        *arg_first = LOWER(*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while (isspace(*argument))
        argument++;

    return argument;
}

export char* one_argument(char* argument, char* arg_first)
{
    return const_cast<char*>(one_argument(const_cast<const char*>(argument), arg_first));
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.  Delimiters = { ' ', '-' }
 */
export const char* one_argument2(const char* argument, char* arg_first)
{
    char cEnd;
    sh_int count;

    count = 0;

    while (isspace(*argument))
        argument++;

    cEnd = ' ';
    if (*argument == '\'' || *argument == '"')
        cEnd = *argument++;

    while (*argument != '\0' || ++count >= 255)
    {
        if (*argument == cEnd || *argument == '-')
        {
            argument++;
            break;
        }
        *arg_first = LOWER(*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while (isspace(*argument))
        argument++;

    return argument;
}

char* one_argument2(char* argument, char* arg_first)
{
    return const_cast<char*>(one_argument2(const_cast<const char*>(argument), arg_first));
}

void do_timecmd(CHAR_DATA* ch, char* argument)
{
    static bool timing;
    extern CHAR_DATA* timechar;
    char arg[MAX_INPUT_LENGTH];

    send_to_char("Timing\n\r", ch);
    if (timing)
        return;
    one_argument(argument, arg);
    if (!*arg)
    {
        send_to_char("No command to time.\n\r", ch);
        return;
    }
    if (!str_cmp(arg, "update"))
    {
        if (timechar)
            send_to_char("Another person is already timing updates.\n\r", ch);
        else
        {
            timechar = ch;
            send_to_char("Setting up to record next update loop.\n\r", ch);
        }
        return;
    }
    set_char_color(AT_PLAIN, ch);
    send_to_char("Starting timer.\n\r", ch);
    timing = true;

    auto start_time = std::chrono::steady_clock::now();

    interpret(ch, argument);

    auto end_time = std::chrono::steady_clock::now();

    timing = false;
    set_char_color(AT_PLAIN, ch);
    send_to_char("Timing complete.\n\r", ch);

    auto time_taken = end_time - start_time;
    ch_printf(ch, "Timing took %d.%06d seconds.\n\r", std::chrono::duration_cast<std::chrono::seconds>(time_taken),
              std::chrono::duration_cast<std::chrono::microseconds>(time_taken).count() % 1000000);
    return;
}

export void send_timer(TIMERSET* vtime, CHAR_DATA* ch)
{
    using namespace std::chrono;

    if (vtime->num_uses == 0)
        return;

    auto ntime = vtime->total_time / vtime->num_uses;
    ch_printf(ch, "Has been used %d times this boot.\n\r", vtime->num_uses);
    ch_printf(ch,
              "Time (in secs): min %d.%0.6d; avg: %d.%0.6d; max %d.%0.6d"
              "\n\r",
              duration_cast<seconds>(vtime->min_time), duration_cast<microseconds>(vtime->min_time),
              duration_cast<seconds>(ntime), duration_cast<microseconds>(ntime),
              duration_cast<seconds>(vtime->max_time), duration_cast<microseconds>(vtime->max_time));
    return;
}

export void update_userec(std::chrono::steady_clock::duration time_used, TIMERSET* userec)
{
    userec->num_uses++;
    if (userec->num_uses == 1 || time_used < userec->min_time)
    {
        userec->min_time = time_used;
    }
    if (userec->num_uses == 1 || time_used > userec->max_time)
    {
        userec->max_time = time_used;
    }

    userec->total_time += time_used;

    return;
}
