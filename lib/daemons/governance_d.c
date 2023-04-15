/*
** Governance daemon for controlling elections and governance 
** for many different things.
** Tsath 2020 (another Corona lock-up creation)
*/

inherit M_DAEMON_DATA;
inherit M_WIDGETS;

private
mapping leaders = ([]);
private
mapping managers = ([]);

void set_leader(string what, string leader)
{
    leaders[what] = leader;
    save_me();
}

string query_leader(string what)
{
    return leaders[what];
}

void add_manager(string what, string manager)
{
    if (!managers[what])
        managers[what] = ({});
    managers[what] += ({manager});
    save_me();
}

void remove_manager(string what, string manager)
{
    if (!managers[what])
        return;
    managers[what] -= ({manager});
    save_me();
}

string *query_managers(string what)
{
    return managers[what] || ({});
}

string leader_board()
{
    string out = "";
    out += sprintf("  %16s  %40s\n", "Leader", "Association");
    out += simple_divider();
    foreach (string leader, string assoc in leaders)
    {
        out += sprintf("  %16s  %40s\n", capitalize(leader), capitalize(assoc));
    }

    return out;
}

void create()
{
    ::create();
}
