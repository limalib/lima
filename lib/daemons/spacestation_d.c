/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// Daemon for space station controlling.

inherit M_DAEMON_DATA;
inherit SPACE_CLASSES;

private
nosave mapping docked_ships = ([]);
private
nosave int dock_count = 0;

int claim_dock(string room_file, string docked_ship, int docking_time, string who)
{
   class docking_info di = new (class docking_info);

   di->vfile = docked_ship;
   di->docking_time = docking_time;
   di->who = lower_case(who);
   if (!docked_ships[room_file])
   {
      docked_ships[room_file] = di;
      dock_count++;
      return 1;
   }
   return 0;
}

class docking_info query_dock(string room_file)
{
   if (docked_ships[room_file])
   {
      return docked_ships[room_file];
   }
   return 0;
}

int release_dock(string room_file)
{
   if (docked_ships[room_file])
   {
      dock_count--;
      map_delete(docked_ships, room_file);
      return 1;
   }
   return 0;
}

string stat_me()
{
   string out = "Docked ships:\n";
   mapping ship_count = ([]);
   foreach (string room in keys(docked_ships))
   {
      ship_count[domain_file(room)]++;
   }

   foreach (string domain, int count in ship_count)
      out += domain + ": " + count + "\n";

   return out;
}
