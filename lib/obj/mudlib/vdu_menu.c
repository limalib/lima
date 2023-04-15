/*	
**	VDU menu by Tsath 2020 for Nuke Mudlib
**
*/

#include <combat_config.h>
#include <commands.h>
#include <menu.h>
#include <mudlib.h>
#include <playerflags.h>
//### for now
#include <security.h>

inherit MENUS;
//### for now
inherit M_ACCESS;

MENU toplevel;
// submenus of the toplevel (main) menu
MENU soulmenu;
MENU ccmenu;
MENU reportmenu;
MENU remotemenu;
MENU personalmenu;
// submenus of personalmenu
MENU biffmenu;
MENU snoopablemenu;

MENU_ITEM quit_item;
MENU_ITEM goto_main_menu_item;
MENU_ITEM main_seperator;

void toggle_cc(int flag, string state);

// right now, we're just going to call the help command.
//private MENU helpmenu;

private
mapping dispatch = (["o":"Options",
                     "?":"help",
]);

void start_player_menu()
{
    new (PLAYER_MENU)->start_menu();
}

class menu
build_guild_menu(string guild)
{
    class menu m;
    m = new_menu("[-- " + sprintf("%-30s", capitalize(guild) + " Sub Module") + "                   -----]");
    return m;
}

void start_mission(int tier, string name, string guild, mapping mats, string reply)
{
    if (reply == "y" || reply == "Y")
    {
        write("Starting mission.\n");
        foreach (string mat, int cnt in mats)      
        {
            GUILD_D->remove_material(guild, mat, cnt);
        }
        GUILD_D->run_mission(tier, name, guild);
    }
    else
    {
        write("Aborted.\n");
    }
}

void start_favor(int tier, string name, int cost, string guild, string reply)
{
    if (reply == "y" || reply == "Y")
    {
        if (GUILD_D->query_favor_score(guild) >= cost)
        {
            write("Purchasing favor.\n");

            if (GUILD_D->spend_favor(guild, cost))
                GUILD_D->start_favor(tier, name, guild);
            else
                write("Aborted, suddenly not enough favor.\n");
        }
        else
            write("Aborted, suddenly not enough favor.\n");
    }
    else
    {
        write("Aborted.\n");
    }
}

mixed pretty_materials(string guild, string mats)
{
    string out = "";
    mapping guild_mats = GUILD_D->query_materials(guild);
    mapping used_mats = ([]);
    string *pairs = explode(mats, ",");
    int gotall = 1;
    foreach (string mat in pairs)
    {
        string *req = explode(mat, ":");
        string material = req[0];
        int count = to_int(req[1]);
        used_mats[material] = count;

        if (CRAFTING_D->valid_material(material))
        {
            if (guild_mats[material] >= count)
                out += " %^GREEN%^+%^RESET%^ " + count + "/" + guild_mats[material] + " " + material + " available.\n";
            else
            {
                out += " %^RED%^-%^RESET%^ " + count + "/" + guild_mats[material] + " " + material + " not available.\n";
                gotall = 0;
            }
        }
    }
    return ({gotall, out, used_mats});
}

void guild_mission(string guild, int tier, string name)
{
    mapping mish_info = GUILD_D->query_mission(tier, name);
    mixed materials = pretty_materials(guild, mish_info["materials"]);
    string *mgmt = GOVERNANCE_D->query_managers(guild) + ({GOVERNANCE_D->query_leader(guild)});

    write("[MISSION INFO]\n" + replace_string(mish_info["description"], "<guild>", capitalize(guild)) + "\n\n");
    write("[MISSION COST]\n" + materials[1] + "\n\n");

    if (materials[0] && member_array(lower_case(this_body()->query_name()), mgmt) != -1)
    {
        modal_simple((
                         : start_mission, tier, name, guild, materials[2]
                         :),
                     "Start the mission? (y/N) ",
                     1);
    }
}

void guild_favor(string guild, int tier, string name)
{
    mapping favor_info = GUILD_D->query_favor(tier, name);
    string *mgmt = GOVERNANCE_D->query_managers(guild) + ({GOVERNANCE_D->query_leader(guild)});
    int favor = to_int(favor_info["favor"]);
    write("[FAVOR INFO]   " + replace_string(favor_info["description"], "<guild>", capitalize(guild)) + "\n");
    write("[FAVOR COST]   " + favor + "\n");
    write("[FAVOR LENGTH] " + time_to_string(to_int(favor_info["length"]) * 60) + "\n\n");

    if (member_array(lower_case(this_body()->query_name()), mgmt) != -1)
    {
        if (GUILD_D->query_favor_score(guild) >= favor)
            modal_simple((
                             : start_favor, tier, name, favor, guild:),
                         "Buy this favor for " + favor + " favor? (y/N) ",
                         1);
        else
        {
            write("\n" + capitalize(guild) + " does not have enough favor to purchase this yet.\n\n");
        }
    }
}

class menu
build_guild_mission_menu(string guild)
{
    class menu m;
    int count = 1;
    int guild_tier = GUILD_D->query_guild_tier(guild);
    mapping missions = GUILD_D->query_missions(guild_tier);
    m = new_menu("[-- " + sprintf("%-30s", capitalize(guild) + " Missions") + "                   -----]");
    foreach (string name, mapping data in missions)
    {
        class menu_item mi = new_menu_item(sprintf("%-30s",name) + " Favor gain: " + data["favor"], (
                                                                                       : guild_mission, guild, guild_tier, name:),
                                           count + "");
        add_menu_item(m, mi);
        count++;
    }
    return m;
}

class menu
build_guild_favor_menu(string guild)
{
    class menu m;
    int count = 1;
    int guild_tier = GUILD_D->query_guild_tier(guild);
    mapping favors = GUILD_D->query_favors(guild_tier);

    m = new_menu("[-- " + sprintf("%-30s", capitalize(guild) + " Favor") + "                   -----]");
    foreach (string name, mapping data in favors)
    {
        class menu_item mi = new_menu_item(name + " Favor Cost: " + data["favor"], (
                                                                                       : guild_favor, guild, guild_tier, name:),
                                           count + "");
        add_menu_item(m, mi);
        count++;
    }
    return m;
}

void init_guild_modules(object *modules)
{
    if (!sizeof(modules))
        return;
    foreach (object module in modules)
    {
        string guild = module->query_module_name();
        class menu gm = build_guild_menu(guild);
        class menu g_mish = build_guild_mission_menu(guild);
        class menu g_favor = build_guild_favor_menu(guild);
        class menu_item mi = new_menu_item(capitalize(guild) + " vdu module", gm, module->query_letter());
        class menu_item mish_item = new_menu_item("Missions", g_mish, "s");
        class menu_item favor_item = new_menu_item("Favor", g_favor, "f");
        class menu_item goto_guild_menu_item = new_menu_item("Return to " + guild + " menu",
                                                             gm,
                                                             "m");
        add_menu_item(g_mish, goto_guild_menu_item);
        add_menu_item(g_favor, goto_guild_menu_item);
        add_menu_item(gm, mish_item);
        add_menu_item(gm, favor_item);
        add_menu_item(gm, goto_main_menu_item);
        add_menu_item(toplevel, mi);
        add_menu_item(gm, quit_item);
        add_menu_item(g_mish, quit_item);
        add_menu_item(g_favor, quit_item);
    }
}

void create(int level)
{
    set_privilege(1);

    toplevel = new_menu("[-- Freamon Electronics Ltd. VDU                     -----]");
    main_seperator = new_seperator("[-- Version 3.41 - Level " + level + "                           -----]");

    // Since we'll use these things more than once, we can just
    // call new_menu_item once, and insert them wherever we want
    // to use them.
    quit_item = new_menu_item("Quit", (
                                          : quit_menu_application:),
                              "q");
    goto_main_menu_item = new_menu_item("Return to main menu",
                                        toplevel,
                                        "m");

    // Add items to the toplevel (main) menu.
    add_menu_item(toplevel, main_seperator);
    add_menu_item(toplevel, new_menu_item("MUD Options", (
                                                             : start_player_menu:),
                                          "o"));
    add_menu_item(toplevel, quit_item);
}

void start_menu()
{
    init_menu_application(toplevel);
}
