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

extern int top_area;
extern int top_r_vnum;
void write_area_list();
void write_starsystem_list();
extern const char* sector_name[SECT_MAX];

/* local routines */
void fread_planet(PLANET_DATA* planet, FILE* fp);
bool load_planet_file(char* planetfile);
void write_planet_list(void);

const char* cargo_names[CARGO_MAX] = {"None",   "Food",        "Water",       "Medical",
                                      "Metals", "Rare Metals", "Electronics", "Products"};
const char* cargo_names_lower[CARGO_MAX] = {"none",   "food",        "water",       "medical",
                                            "metals", "rare metals", "electronics", "products"};

PLANET_DATA* get_planet(char* name)
{
    PLANET_DATA* planet;

    if (name[0] == '\0')
        return nullptr;

    for (planet = first_planet; planet; planet = planet->next)
        if (!str_cmp(name, planet->name))
            return planet;

    for (planet = first_planet; planet; planet = planet->next)
        if (nifty_is_name(name, planet->name))
            return planet;

    for (planet = first_planet; planet; planet = planet->next)
        if (!str_prefix(name, planet->name))
            return planet;

    for (planet = first_planet; planet; planet = planet->next)
        if (nifty_is_name_prefix(name, planet->name))
            return planet;

    return nullptr;
}

void write_planet_list()
{
    PLANET_DATA* tplanet;
    FILE* fpout;
    char filename[256];

    sprintf_s(filename, "%s%s", PLANET_DIR, PLANET_LIST);
    fpout = fopen(filename, "w");
    if (!fpout)
    {
        bug("FATAL: cannot open planet.lst for writing!\n\r", 0);
        return;
    }
    for (tplanet = first_planet; tplanet; tplanet = tplanet->next)
        fprintf(fpout, "%s\n", tplanet->filename);
    fprintf(fpout, "$\n");
    fclose(fpout);
}

void save_planet(PLANET_DATA* planet)
{
    FILE* fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
    int i = 0;

    if (!planet)
    {
        bug("save_planet: null planet pointer!", 0);
        return;
    }

    if (!planet->filename || planet->filename[0] == '\0')
    {
        sprintf_s(buf, "save_planet: %s has no filename", planet->name);
        bug(buf, 0);
        return;
    }

    sprintf_s(filename, "%s%s", PLANET_DIR, planet->filename);

    if ((fp = fopen(filename, "w")) == nullptr)
    {
        bug("save_planet: fopen", 0);
        perror(filename);
    }
    else
    {
        AREA_DATA* pArea;

        fprintf(fp, "#PLANET\n");
        fprintf(fp, "Name         %s~\n", planet->name);
        fprintf(fp, "Filename     %s~\n", planet->filename);
        fprintf(fp, "X            %d\n", planet->x);
        fprintf(fp, "Y            %d\n", planet->y);
        fprintf(fp, "Z            %d\n", planet->z);
        fprintf(fp, "Sector       %d\n", planet->sector);
        fprintf(fp, "Type    	   %d\n", planet->controls);
        fprintf(fp, "PopSupport   %d\n", (int)(planet->pop_support));
        if (planet->starsystem && planet->starsystem->name)
            fprintf(fp, "Starsystem   %s~\n", planet->starsystem->name);
        if (planet->governed_by && planet->governed_by->name)
            fprintf(fp, "GovernedBy   %s~\n", planet->governed_by->name);
        // pArea = planet->area;
        // if (pArea->filename)
        //	fprintf( fp, "Area         %s~\n",	pArea->filename  );
        for (pArea = planet->first_area; pArea; pArea = pArea->next_on_planet)
            fprintf(fp, "Area         %s~\n", pArea->filename);

        for (i = 1; i < CARGO_MAX; i++)
        {
            fprintf(fp, "Resource %d %d\n", i, planet->price[i]);
        }
        fprintf(fp, "Base_value   %d\n", planet->base_value);
        fprintf(fp, "End\n\n");
        fprintf(fp, "#END\n");
    }
    fclose(fp);
    return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY(literal, field, value)                                                                                     \
    if (!str_cmp(word, literal))                                                                                       \
    {                                                                                                                  \
        field = value;                                                                                                 \
        fMatch = true;                                                                                                 \
        break;                                                                                                         \
    }

void fread_planet(PLANET_DATA* planet, FILE* fp)
{
    char buf[MAX_STRING_LENGTH];
    char const* word;
    char* line;
    int x0, x1;
    bool fMatch;

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
            /*	    if ( !str_cmp( word, "Area" ) )
                    {
                        char aName[MAX_STRING_LENGTH];
                            AREA_DATA *pArea;

                        sprintf_s (aName, fread_string(fp));
                    for( pArea = first_area ; pArea ; pArea = pArea->next )
                          if (pArea->filename && !str_cmp(pArea->filename , aName ) )
                          {
                             ROOM_INDEX_DATA *room;

                             planet->size = 0;
                             planet->citysize = 0;
                             planet->wilderness = 0;
                             planet->farmland = 0;
                             planet->barracks = 0;
                             planet->controls = 0;
                             pArea->planet = planet;
                             planet->area = pArea;
                             for( room = pArea->first_room ; room ; room = room->next_in_area )
                                 {
                                      planet->size++;
                                      if ( room->sector_type <= SECT_CITY )
                                         planet->citysize++;
                                      else if ( room->sector_type == SECT_FARMLAND )
                                         planet->farmland++;
                                      else if ( room->sector_type != SECT_DUNNO )
                                         planet->wilderness++;

                                      if ( IS_SET( room->room_flags , ROOM_CONTROL ))
                                         planet->controls++;
                                      if ( IS_SET( room->room_flags , ROOM_BARRACKS ))
                                         planet->barracks++;
                                 }
                          }      */
            // fMatch = true;
            //}
            if (!str_cmp(word, "area"))
            {
                planet->area = get_area(fread_string(fp));
                if (planet->area)
                {
                    AREA_DATA* area = planet->area;
                    area->planet = planet;
                    LINK(area, planet->first_area, planet->last_area, next_on_planet, prev_on_planet);
                }
                fMatch = true;
            }

            break;

        case 'B':
            KEY("Base_value", planet->base_value, fread_number(fp));
        case 'E':
            if (!str_cmp(word, "End"))
            {
                if (!planet->name)
                    planet->name = STRALLOC("");
                return;
            }
            break;

        case 'F':
            KEY("Filename", planet->filename, fread_string_nohash(fp));
            break;

        case 'G':
            if (!str_cmp(word, "GovernedBy"))
            {
                planet->governed_by = get_clan(fread_string(fp));
                fMatch = true;
            }
            break;

        case 'N':
            KEY("Name", planet->name, fread_string(fp));
            break;

        case 'P':
            KEY("PopSupport", planet->pop_support, fread_float(fp));
            break;

        case 'R':
            if (!str_cmp(word, "Resource"))
            {
                line = fread_line(fp);
                x0 = 0;
                sscanf(line, "%d %d\n", &x0, &x1);
                planet->price[x0] = x1;
            }
            break;

        case 'S':
            KEY("Sector", planet->sector, fread_number(fp));
            if (!str_cmp(word, "Starsystem"))
            {
                planet->starsystem = starsystem_from_name(fread_string(fp));
                if (planet->starsystem)
                {
                    SPACE_DATA* starsystem = planet->starsystem;

                    LINK(planet, starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system);
                }
                fMatch = true;
            }
            break;
        case 'T':
            KEY("Type", planet->controls, fread_number(fp));
            break;
        case 'X':
            KEY("X", planet->x, fread_number(fp));
            break;

        case 'Y':
            KEY("Y", planet->y, fread_number(fp));
            break;

        case 'Z':
            KEY("Z", planet->z, fread_number(fp));
            break;
        }

        if (!fMatch)
        {
            sprintf_s(buf, "Fread_planet: no match: %s", word);
            bug(buf, 0);
        }
    }
}

bool load_planet_file(const char* planetfile)
{
    PLANET_DATA* planet;
    FILE* fp;
    bool found;

    CREATE(planet, PLANET_DATA, 1);

    planet->governed_by = nullptr;
    planet->next_in_system = nullptr;
    planet->prev_in_system = nullptr;
    planet->starsystem = nullptr;
    planet->area = nullptr;
    planet->first_guard = nullptr;
    planet->last_guard = nullptr;

    found = false;

    auto filename = std::string(PLANET_DIR) + planetfile;

    if ((fp = fopen(filename.c_str(), "r")) != nullptr)
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
                bug("Load_planet_file: # not found.", 0);
                break;
            }

            word = fread_word(fp);
            if (!str_cmp(word, "PLANET"))
            {
                fread_planet(planet, fp);
                break;
            }
            else if (!str_cmp(word, "END"))
                break;
            else
            {
                char buf[MAX_STRING_LENGTH];

                sprintf_s(buf, "Load_planet_file: bad section: %s.", word);
                bug(buf, 0);
                break;
            }
        }
        fclose(fp);
    }

    if (!found)
        DISPOSE(planet);
    else
        LINK(planet, first_planet, last_planet, next, prev);

    return found;
}

void load_planets()
{
    FILE* fpList;
    const char* filename;
    char planetlist[256];
    char buf[MAX_STRING_LENGTH];

    first_planet = nullptr;
    last_planet = nullptr;

    log_string("Loading planets...");

    sprintf_s(planetlist, "%s%s", PLANET_DIR, PLANET_LIST);
    if ((fpList = fopen(planetlist, "r")) == nullptr)
    {
        perror(planetlist);
        exit(1);
    }

    for (;;)
    {
        filename = feof(fpList) ? "$" : fread_word(fpList);
        log_string(filename);
        if (filename[0] == '$')
            break;

        if (!load_planet_file(filename))
        {
            sprintf_s(buf, "Cannot load planet file: %s", filename);
            bug(buf, 0);
        }
    }
    fclose(fpList);
    log_string(" Done planets ");
    return;
}

AREA_DATA* get_area(char* argument)
{
    AREA_DATA* pArea;
    for (pArea = first_area; pArea; pArea = pArea->next)
        if (pArea->filename && !str_cmp(pArea->filename, argument))
            return pArea;

    return nullptr;
}

void do_setplanet(CHAR_DATA* ch, char* argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    PLANET_DATA* planet;
    int i = 0;
    char arg3[MAX_INPUT_LENGTH];

    if (IS_NPC(ch))
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char("Usage: setplanet <planet> <field> [value]\n\r", ch);
        send_to_char("\n\rField being one of:\n\r", ch);
        send_to_char(" name filename area starsystem governed_by x y z\n\r", ch);
        send_to_char("NEW: basevalue", ch);
        return;
    }

    planet = get_planet(arg1);
    if (!planet)
    {
        send_to_char("No such planet.\n\r", ch);
        return;
    }

    if (!strcmp(arg2, "name"))
    {
        STRFREE(planet->name);
        planet->name = STRALLOC(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        return;
    }
    if (!strcmp(arg2, "type"))
    {
        if (!argument)
            planet->controls = 0;
        else
            planet->controls = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        return;
    }
    if (!strcmp(arg2, "sector"))
    {
        planet->sector = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        return;
    }

    if (!strcmp(arg2, "area"))
    {
        planet->area = get_area(argument);
        if (planet->area)
        {
            AREA_DATA* area = planet->area;
            LINK(area, planet->first_area, planet->last_area, next_on_planet, prev_on_planet);
        }
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        return;
    }

    if (!strcmp(arg2, "governed_by"))
    {
        CLAN_DATA* clan;
        clan = get_clan(argument);
        if (clan)
        {
            planet->governed_by = clan;
            send_to_char("Done.\n\r", ch);
            save_planet(planet);
        }
        else
            send_to_char("No such clan.\n\r", ch);
        return;
    }

    if (!strcmp(arg2, "starsystem"))
    {
        SPACE_DATA* starsystem;

        if ((starsystem = planet->starsystem) != nullptr)
            UNLINK(planet, starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system);
        if ((planet->starsystem = starsystem_from_name(argument)))
        {
            starsystem = planet->starsystem;
            LINK(planet, starsystem->first_planet, starsystem->last_planet, next_in_system, prev_in_system);
            send_to_char("Done.\n\r", ch);
        }
        else
            send_to_char("No such starsystem.\n\r", ch);
        save_planet(planet);
        return;
    }

    if (!strcmp(arg2, "x"))
    {
        planet->x = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        write_planet_list();
        return;
    }

    if (!strcmp(arg2, "y"))
    {
        planet->y = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        write_planet_list();
        return;
    }

    if (!strcmp(arg2, "z"))
    {
        planet->z = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        write_planet_list();
        return;
    }
    if (!strcmp(arg2, "basevalue"))
    {
        planet->base_value = atoi(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        write_planet_list();
        return;
    }
    if (!strcmp(arg2, "price"))
    {
        argument = one_argument(argument, arg3);

        for (i = 0; i < CARGO_MAX; i++)
        {
            if (!str_cmp(arg3, cargo_names[i]))
            {
                planet->price[i] = atoi(argument);
                send_to_char("done.\n\r", ch);
                save_planet(planet);
                return;
            }
        }
        send_to_char("No such resource type\r\n", ch);
        return;
    }

    /*    if ( !strcmp( arg2, "export"))
        {
        argument = one_argument( argument, arg3 );

           for (i = 0; i < CARGO_MAX; i++)
           {
              if (!str_cmp( arg3, cargo_names[i]))
              {
                 planet->export[i] = atoi(argument);
                 planet->import[i] = 0;
                 send_to_char("done.\n\r", ch );
                 save_planet( planet );
                 return;
              }
           }
           send_to_char("No such resource type\r\n", ch);
           return;
        }

        if ( !strcmp( arg2, "resource"))
        {
        argument = one_argument( argument, arg3 );

           for (i = 0; i < CARGO_MAX; i++)
           {
              if (!str_cmp( arg3, cargo_names[i]))
              {
                 planet->resource[i] = atoi(argument);
                 send_to_char("done.\n\r", ch );
                 save_planet( planet );
                 return;
              }
           }
           send_to_char("No such resource type\r\n", ch);
           return;
        }

        if ( !strcmp( arg2, "produces"))
        {
        argument = one_argument( argument, arg3 );

           for (i = 0; i < CARGO_MAX; i++)
           {
              if (!str_cmp( arg3, cargo_names[i]))
              {
                 planet->produces[i] = atoi(argument);
                 send_to_char("done.\n\r", ch );
                 save_planet( planet );
                 return;
              }
           }
           send_to_char("No such resource type\r\n", ch);
           return;
        }

        if ( !strcmp( arg2, "consumes"))
        {
        argument = one_argument( argument, arg3 );

           for (i = 0; i < CARGO_MAX; i++)
           {
              if (!str_cmp( arg3, cargo_names[i]))
              {
                 planet->consumes[i] = atoi(argument);
                 send_to_char("done.\n\r", ch );
                 save_planet( planet );
                 return;
              }
           }
           send_to_char("No such resource type\r\n", ch);
           return;
        }*/

    if (!strcmp(arg2, "filename"))
    {
        DISPOSE(planet->filename);
        planet->filename = str_dup(argument);
        send_to_char("Done.\n\r", ch);
        save_planet(planet);
        write_planet_list();
        return;
    }

    do_setplanet(ch, MAKE_TEMP_STRING(""));
    return;
}

void do_showplanet(CHAR_DATA* ch, char* argument)
{
    GUARD_DATA* guard;
    PLANET_DATA* planet;
    AREA_DATA* pArea;
    char area[MAX_STRING_LENGTH];
    int num_guards = 0;
    int pf = 0;
    int pc = 0;
    int pw = 0;

    if (IS_NPC(ch))
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char("Usage: showplanet <planet>\n\r", ch);
        return;
    }

    planet = get_planet(argument);
    if (!planet)
    {
        send_to_char("No such planet.\n\r", ch);
        return;
    }

    for (guard = planet->first_guard; guard; guard = guard->next_on_planet)
        num_guards++;

    if (planet->size > 0)
    {
        float tempf;

        tempf = planet->citysize;
        pc = tempf / planet->size * 100;

        tempf = planet->wilderness;
        pw = tempf / planet->size * 100;

        tempf = planet->farmland;
        pf = tempf / planet->size * 100;
    }

    ch_printf(ch, "&W%s\n\r", planet->name);
    if (IS_IMMORTAL(ch))
        ch_printf(ch, "&WFilename: &G%s\n\r", planet->filename);

    ch_printf(ch, "&WTerrain: &G%s\n\r", sector_name[planet->sector]);
    ch_printf(ch, "&WGoverned by: &G%s\n\r", planet->governed_by ? planet->governed_by->name : "");
    ch_printf(ch, "&WPlanet Size: &G%d\n\r", planet->size);
    ch_printf(ch, "&WPercent Civilized: &G%d\n\r", pc);
    ch_printf(ch, "&WPercent Wilderness: &G%d\n\r", pw);
    ch_printf(ch, "&WPercent Farmland: &G%d\n\r", pf);
    ch_printf(ch, "&WBarracks: &G%d\n\r", planet->barracks);
    ch_printf(ch, "&WPatrols: &G%d&W/%d\n\r", num_guards, planet->barracks * 5);
    ch_printf(ch, "&WPopulation: &G%d&W\n\r", planet->population);
    ch_printf(ch, "&WPopular Support: &G%.2f\n\r", planet->pop_support);
    ch_printf(ch, "&WCurrent Monthly Revenue: &G%ld\n\r", get_taxes(planet));
    area[0] = '\0';
    for (pArea = planet->first_area; pArea; pArea = pArea->next_on_planet)
    {
        strcat_s(area, pArea->filename);
        strcat_s(area, ", ");
    }
    ch_printf(ch, "&WAreas: &G%s\n\r", area);
    if (IS_IMMORTAL(ch) && !planet->area)
    {
        ch_printf(ch, "&RWarning - this planet is not attached to an area!&G");
        ch_printf(ch, "\n\r");
    }

    return;
}

void do_makeplanet(CHAR_DATA* ch, char* argument)
{
    char filename[256];
    PLANET_DATA* planet;
    bool found;

    if (!argument || argument[0] == '\0')
    {
        send_to_char("Usage: makeplanet <planet name>\n\r", ch);
        return;
    }

    found = false;
    sprintf_s(filename, "%s%s", PLANET_DIR, strlower(argument).c_str());

    CREATE(planet, PLANET_DATA, 1);
    LINK(planet, first_planet, last_planet, next, prev);
    planet->governed_by = nullptr;
    planet->next_in_system = nullptr;
    planet->prev_in_system = nullptr;
    planet->starsystem = nullptr;
    planet->first_area = nullptr;
    planet->last_area = nullptr;
    planet->first_guard = nullptr;
    planet->last_guard = nullptr;
    planet->name = STRALLOC(argument);
    planet->flags = 0;
}

void do_planets(CHAR_DATA* ch, char* argument)
{
    PLANET_DATA* planet;
    int count = 0;
    SPACE_DATA* starsystem;

    set_char_color(AT_WHITE, ch);
    send_to_char("Planet             Starsystem    Governed By                  Popular Support\n\r", ch);

    for (starsystem = first_starsystem; starsystem; starsystem = starsystem->next)
        for (planet = starsystem->first_planet; planet; planet = planet->next_in_system)
        {
            if (planet->controls != 0)
                continue;
            ch_printf(ch, "&G%-18s %-12s  %-25s    ", planet->name, starsystem->name,
                      planet->governed_by ? planet->governed_by->name : "");
            ch_printf(ch, "%.1f\n\r", planet->pop_support);
            if (IS_IMMORTAL(ch) && !planet->area)
            {
                ch_printf(ch, "&RWarning - this planet is not attached to an area!&G");
                ch_printf(ch, "\n\r");
            }

            count++;
        }

    for (planet = first_planet; planet; planet = planet->next)
    {
        if (planet->starsystem)
            continue;

        ch_printf(ch, "&G%-15s %-12s  %-25s    ", planet->name, "",
                  planet->governed_by ? planet->governed_by->name : "");
        ch_printf(ch, "%.1f\n\r", !str_cmp(planet->governed_by->name, "Neutral") ? 100.0 : planet->pop_support);
        if (IS_IMMORTAL(ch) && !planet->area)
        {
            ch_printf(ch, "&RWarning - this planet is not attached to an area!&G");
            ch_printf(ch, "\n\r");
        }

        count++;
    }

    if (!count)
    {
        set_char_color(AT_BLOOD, ch);
        send_to_char("There are no planets currently formed.\n\r", ch);
    }
    send_to_char("&WUse SHOWPLANET for more information.\n\r", ch);
}

void do_capture(CHAR_DATA* ch, char* argument)
{
    CLAN_DATA* clan;
    PLANET_DATA* planet;
    PLANET_DATA* cPlanet;
    float support = 0.0;
    int pCount = 0;
    char buf[MAX_STRING_LENGTH];

    if (!ch->in_room || !ch->in_room->area)
        return;

    if (IS_NPC(ch) || !ch->pcdata)
    {
        send_to_char("huh?\n\r", ch);
        return;
    }

    if (!ch->pcdata->clan)
    {
        send_to_char("You need to be a member of an organization to do that!\n\r", ch);
        return;
    }

    clan = ch->pcdata->clan;

    if ((planet = ch->in_room->area->planet) == nullptr)
    {
        send_to_char("You must be on a planet to capture it.\n\r", ch);
        return;
    }

    if (clan == planet->governed_by)
    {
        send_to_char("Your organization already controls this planet.\n\r", ch);
        return;
    }

    if (clan->clan_type == CLAN_CRIME || clan->clan_type == CLAN_GUILD)
    {
        send_to_char("Your clan can't capture planets!\n\r", ch);
        return;
    }

    if (planet->starsystem)
    {
        SHIP_DATA* ship;
        CLAN_DATA* sClan;

        for (ship = planet->starsystem->first_ship; ship; ship = ship->next_in_starsystem)
        {
            sClan = get_clan(ship->owner);
            if (!sClan)
                continue;
            if (sClan == planet->governed_by)
            {
                send_to_char("A planet cannot be captured while protected by orbiting spacecraft.\n\r", ch);
                return;
            }
        }
    }

    if (planet->first_guard)
    {
        send_to_char("This planet is protected by soldiers.\n\r", ch);
        send_to_char("You will have to eliminate all enemy forces before you can capture it.\n\r", ch);
        return;
    }
    if (planet->governed_by->name && planet->governed_by->name != nullptr)
    {
        if (!str_cmp(planet->governed_by->name, "Neutral"))
        {
            send_to_char("This planet cannot be captured.\n\r", ch);
            return;
        }
    }
    if (planet->pop_support > 0)
    {
        send_to_char("The population is not in favour of changing leaders right now.\n\r", ch);
        return;
    }

    for (cPlanet = first_planet; cPlanet; cPlanet = cPlanet->next)
        if (clan == cPlanet->governed_by)
        {
            pCount++;
            support += cPlanet->pop_support;
        }

    if (support < 0)
    {
        send_to_char("There is not enough popular support for your organization!\n\rTry improving loyalty on the "
                     "planets that you already control.\n\r",
                     ch);
        return;
    }

    planet->governed_by = clan;
    planet->pop_support = 50;

    sprintf_s(buf, "%s has been captured by %s!", planet->name, clan->name);
    echo_to_all(AT_RED, buf, 0);

    save_planet(planet);

    return;
}

long get_taxes(PLANET_DATA* planet)
{
    long gain;

    gain = planet->base_value;
    gain += planet->base_value * (planet->pop_support / 100);
    gain += (planet->population * 150);
    gain += UMAX(0, planet->pop_support / 10 * (planet->population * 20));

    return gain;
}

void do_imports(CHAR_DATA* ch, char* argument)
{
    PLANET_DATA* planet;
    int i;

    if (argument[0] == '\0')
    {
        send_to_char("Usage: imports <planet>\r\n", ch);
        return;
    }

    planet = get_planet(argument);

    if (!planet)
    {
        send_to_char("&RNo such planet\r\n", ch);
        return;
    }
    ch_printf(ch, "&BImport and Export data for %s:\r\n", planet->name);
    ch_printf(ch, "&GResource           &CPrice\r\n");
    ch_printf(ch, "&G---------------    ------\r\n");
    for (i = 1; i < CARGO_MAX; i++)
        ch_printf(ch, "&G%-15.15s    &C%5d/ton\r\n", cargo_names[i], planet->price[i]);
    return;
}
