/* Do not remove the headers from this file! see /USAGE for more info.    */
/*
 * Clan Daemon
 *
 * This daemon manages clan data, clan rooms, and clan doorways.
 *
 * Functions:
 * - query_clan_info(clan_name): Returns a copy of the clan's information mapping.
 * - init_clan(clan_name): Initializes a new clan's data.
 * - upgrade_clan_hall(clan_name, new_level): Upgrades the clan hall's level.
 * - add_clan_option(clan_name, option_name): Adds a purchased option to a clan.
 * - remove_clan_option(clan_name, option_name): Removes an option from a clan.
 * - spawn_clan_room(clan_name, room_type): Creates a new clan room of a specific type.
 * - spawn_clan_stone(clan_name, room_path): Places the clan stone in the main hall.
 * - load_all_clan_rooms(clan_name): Ensures all clan rooms are loaded.
 * - query_clan_entrance(clan_name): Returns the main entrance room path for a clan.
 * - destroy_clan_doorway(clan_name): Removes an active clan doorway.
 * - remove_clan_area(clan_name): Removes all rooms, stones, and doorways for a clan.
 * - query_active_doorway(clan_name): Returns info about an active doorway.
 * - clean_up(): Saves clan and doorway data.
 * - create_clan_doorway(clan_name, player_obj, current_room): Creates a temporary doorway to the clan hall.
 * - reset(): Recreates doorways after a reboot.
 */

#include <log.h>

inherit M_DAEMON_DATA;

mapping clan_data;
mapping active_doorways; //([ clan_name :({ doorway_object_path, room_where_door_is})])

#define CLAN_SAVE_DATA "/data/clan_d_data"
#define DOORWAY_SAVE_DATA "/data/clan_doorway_data"

void create()
{
   clan_data = ([]);
   active_doorways = ([]);
   restore_object(CLAN_SAVE_DATA);
   restore_object(DOORWAY_SAVE_DATA);
   if (!mapp(clan_data))
      clan_data = ([]);
   if (!mapp(active_doorways))
      active_doorways = ([]);
}

// Public Functions

//: FUNCTION query_clan_info
// Returns information about a clan by its name.
// If the clan does not exist, returns 0.
mapping query_clan_info(string clan_name)
{
   if (!clan_name || !stringp(clan_name))
      return 0;
   return copy(clan_data[clan_name]);
}

//: FUNCTION init_clan
// Initializes a new clan with the given name.
// Returns 1 on success, 0 if the clan already exists or the name is invalid.
// The clan data includes level, rooms, options, and main entrance.
// The level starts at 1, rooms is an empty array, options is an empty array, and main_entrance is an empty string.
int init_clan(string clan_name)
{
   if (!clan_name || !stringp(clan_name))
      return 0;
   if (clan_data[clan_name])
      return 0;

   clan_data[clan_name] = (["level":1, "rooms":({}), "options":({}), "main_entrance":""]);

   save_object(CLAN_SAVE_DATA);
   return 1;
}

// Upgrade a clan hall's level.
// Thinking this upgrade only allows for an increase of members
int upgrade_clan_hall(string clan_name, int new_level)
{
   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return 0;
   if (new_level <= clan_data[clan_name]["level"])
      return 0;

   clan_data[clan_name]["level"] = new_level;
   // Define what happens at new levels here

   save_object(CLAN_SAVE_DATA);
   return 1;
}

// Adds a purchased option to a clan
int add_clan_option(string clan_name, string option_name)
{

   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return 0;
   if (member_array(option_name, clan_data[clan_name]["options"]) != -1)
      return 0;

   clan_data[clan_name]["options"] += ({option_name});

   // Spawn associated options
   // switch (option_name)

   save_object(CLAN_SAVE_DATA);
   return 1;
}

// Removes an option
int remove_clan_option(string clan_name, string option_name)
{
   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return 0;
   if (member_array(option_name, clan_data[clan_name]["options"]) == -1)
      return 0;

   clan_data[clan_name]["options"] -= ({option_name});

   // Destruct associated room. May need a more robust method here
   foreach (string room_path in clan_data[clan_name]["rooms"])
   {
      if (room_path == sprintf("domains/clans/%s/%s_room", clan_name, option_name))
      {
         object room_to_destruct = find_object(room_path);
         if (room_to_destruct)
         {
            destruct(room_to_destruct);
            clan_data[clan_name]["rooms"] -= ({room_path});
         }
         break;
      }
   }
   save_object(CLAN_SAVE_DATA);
   return 1;
}

// Spawns a specific type of clan room.
private string spawn_clan_room(string clan_name, string room_type)
{
   string base_room_object_path;
   string specific_room_blueprint;
   string room_instance_path;
   object new_room;

   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return 0;

   base_room_object_path = "";
   specific_room_blueprint = 0;

   // switch (room_type)
   //{
   // }

   room_instance_path = sprintf("/domains/clans/%s/%s_room_%d", clan_name, room_type, time());

   if (find_object(room_instance_path))
      return 0;

   if (is_file(specific_room_blueprint + ".c"))
   {
      new_room = new(specific_room_blueprint);
   }
   else
   {
      new_room = new(base_room_object_path);
   }

   if (!new_room)
   {
      LOG_D->log(LOG_CLAN_ERROR, sprintf("Failed to clone room for clan %s, type %s.\n", clan_name, room_type));
      return 0;
   }

   new_room->set_clan_owner(clan_name);
   new_room->set_room_type(room_type);
   new_room->set_long("This is the " + replace_string(room_type, "_", " ") + " of the " + capitalize(clan_name) +
                      " clan hall.");

   clan_data[clan_name]["rooms"] += ({room_instance_path});
   save_object(CLAN_SAVE_DATA);

   return room_instance_path;
}

// Spawns the Clan Stone inside the main hall.
private object spawn_clan_stone(string clan_name, string room_path)
{
   object clan_stone = new("/domain/std/clan/clan_stone");
   if (clan_stone)
   {
      clan_stone->set_clan_owner(clan_name);
      clan_stone->move(room_path);
      return clan_stone;
   }
   return 0;
}

// Ensures all clan rooms are loaded when the daemon starts or requested.
void load_all_clan_rooms(string clan_name)
{
   object room;

   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return;

   foreach (string room_path in clan_data[clan_name]["rooms"])
   {
      if (!find_object(room_path))
      {
         // Re-clone the base, assuming the room object handles its own data loading
         room = new("/obj/clan_room_base");
         if (room)
         {
            // room->restore_object(room_path); // If rooms save unique data
         }
         else
         {
            LOG_D->log(LOG_CLAN_ERROR,
                       sprintf("Failed to load or recreate room %s for clan %s.\n", room_path, clan_name));
         }
      }
   }
}

// Returns the full path to the main entrance room for a clan.
string query_clan_entrance(string clan_name)
{
   string main_room_path;

   if (!clan_name || !stringp(clan_name) || !clan_data[clan_name])
      return 0;

   main_room_path = clan_data[clan_name]["main_entrance"];

   // If no main entrance is set, create one.
   if (!main_room_path || main_room_path == "")
   {
      main_room_path = spawn_clan_room(clan_name, "main_hall");
      if (main_room_path)
      {
         clan_data[clan_name]["main_entrance"] = main_room_path;
         spawn_clan_stone(clan_name, main_room_path); // Place clan stone
         save_object(CLAN_SAVE_DATA);
      }
   }
   return main_room_path;
}

// Destroys an existing clan doorway.
int destroy_clan_doorway(string clan_name)
{
   string doorway_path;
   object doorway_obj;

   if (!clan_name || !stringp(clan_name) || !active_doorways[clan_name])
      return 0;

   doorway_path = active_doorways[clan_name][0];
   doorway_obj = find_object(doorway_path);

   if (doorway_obj)
   {
      doorway_obj->remove_doorway();
      destruct(doorway_obj);
   }

   map_delete(active_doorways, clan_name);
   save_object(DOORWAY_SAVE_DATA);
   return 1;
}

// Removes a clan's entire area (rooms, stone, doorway).
void remove_clan_area(string clan_name)
{
   object room;
   object clan_stone;

   if (!clan_data[clan_name])
      return;

   foreach (string room_path in clan_data[clan_name]["rooms"])
   {
      room = find_object(room_path);
      if (room)
         destruct(room);
   }
   clan_stone = find_object(sprintf("/obj/clan_stone#%s", clan_name));
   if (clan_stone)
      destruct(clan_stone);

   destroy_clan_doorway(clan_name);

   map_delete(clan_data, clan_name);
   save_object(CLAN_SAVE_DATA);
   LOG_D->log(LOG_CLAN, sprintf("%s: Removed clan area for %s.\n", ctime(time()), clan_name));
}

// Returns information about an active doorway.
mixed *query_active_doorway(string clan_name)
{
   if (!clan_name || !stringp(clan_name))
      return 0;
   return active_doorways[clan_name];
}

// Persistence functions.
void clean_up()
{
   save_object(CLAN_SAVE_DATA);
   save_object(DOORWAY_SAVE_DATA);
}

// Creates a temporary doorways to the clan hall in a public room
string create_clan_doorway(string clan_name, object player_obj, object current_room)
{
   string doorway_path;
   object doorway_obj;

   if (!clan_name || !stringp(clan_name))
      return 0;
   if (!clan_data[clan_name])
      return 0; // CLan data must be initialized first
   if (active_doorways[clan_name])
      return 0; // Doorway already exists

   doorway_path = sprintf("/domains/clan/doorways/clan_doorway/%s_doorway_%d", clan_name, time());
   doorway_obj = new("/domains/std/clan/clan_doorway");

   if (!doorway_obj)
   {
      LOG_D->log(LOG_CLAN_ERROR, sprintf("Failed to clone doorways for %s.\n", clan_name));
      return 0;
   }

   doorway_obj->set_clan_owner(clan_name);
   doorway_obj->set_in_room_description(" A slowly shimmering portal is here");
   doorway_obj->set_long(
       sprintf("A shimmering portal stands here, leading to the %s Clan Hall.\n", capitalize(clan_name)));
   doorway_obj->set_exits(query_clan_entrance(clan_name)); // Set where the doorway leads

   if (doorway_obj->move(current_room))
   {
      active_doorways[clan_name] = ({file_name(doorway_obj), file_name(current_room)});
      save_object(DOORWAY_SAVE_DATA);
      return file_name(doorway_obj);
   }
   else
   {
      destruct(doorway_obj);
      return 0;
   }
}

void reset()
{
   // Re-check and potentially recreate doorways after a reboot.
   foreach (string clan_name, mixed * data in active_doorways)
   {
      string doorway_path = data[0];
      string room_path = data[1];
      if (!find_object(doorway_path))
      {
         object current_room = find_object(room_path);
         if (current_room)
         {
            LOG_D->log(LOG_CLAN, sprintf("Re-creating doorway for %s in %s.\n", clan_name, room_path));
            create_clan_doorway(clan_name, 0, current_room);
         }
         else
         {
            LOG_D->log(LOG_CLAN_ERROR, sprintf("Doorway for %s in %s could not be re-created: room not found.\n",
                                               clan_name, room_path));
            map_delete(active_doorways, clan_name);
            save_object(DOORWAY_SAVE_DATA);
         }
      }
   }
}
