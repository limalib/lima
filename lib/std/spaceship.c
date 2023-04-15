/*
** Ship object for player ship and perstent rooms.
** Interacts with SHIP_D.
*/

#include <move.h>
#define SHIP_PERSISTED 1
#define SHIP_DIRTY 2

inherit ROOM;
inherit M_ACCESS;

private
string owner;
private
int unique_id;
private
int state = SHIP_PERSISTED;
private
string inventory;
private
nosave int restored = 0;
private
nosave string ship_brief;
private
nosave string ship_long;
private
nosave string ship_type;
private
nosave int weekly_cost = 10;
private
nosave object possible_owner;
nosave object base_obj;
private
int room_on_ship = 10;

void object_arrived(object);
void object_left(object);
void set_owner(string str);
void before_save();
void init_room(object r);
void reinit_room();

void setup()
{
   set_save_recurse(1);
   add_hook("object_left", ( : object_left:));
   add_hook("object_arrived", ( : object_arrived:));
}

object *on_ship()
{
   object *here = filter_array(all_inventory(),
                               (: base_name($1)[0..21] != "/domains/common/ship/" &&
                                         base_name($1)[0..11] != "/obj/mudlib/" && base_name($1)[0..9] != "/std/race/"
                                   :));
   return here;
}

mixed cannot_drop_here()
{
   if (sizeof(on_ship()) > room_on_ship)
      return "The room would be too crammed if you left that lying around.";
   return 0;
}

void set_ship_type(string st)
{
   ship_type = st;
}

string query_ship_type()
{
   return ship_type;
}

void set_ship_brief(string b)
{
   ship_brief = b;
}

void set_room_on_ship(int rof)
{
   room_on_ship = rof;
}

int query_room_on_ship()
{
   return room_on_ship;
}

void set_ship_long(string l)
{
   ship_long = l;
}

void set_ship_cost(int ac)
{
   weekly_cost = ac;
}

string query_ship_brief()
{
   return ship_brief;
}

string query_ship_long()
{
   return ship_long;
}

int query_ship_cost()
{
   return weekly_cost;
}

int query_state()
{
   return state;
}

string query_owner()
{
   return owner;
}

void set_relations()
{
   object *on_ship;

   on_ship = on_ship();
   foreach (object ob in on_ship)
   {
      this_object()->reinsert_object(ob, "in");
   }

   on_ship = all_inventory();
   on_ship = filter(on_ship, ( : sizeof(all_inventory($1)) :));
   foreach (object container in on_ship)
   {
      string default_rel = container->query_default_relation();
      foreach (object in_container in all_inventory(container))
      {
         container->reinsert_object(in_container, default_rel);
      }
   }
}

void object_arrived(object ob)
{
   if (ob->is_body() && !owner)
   {
      possible_owner = ob;
      owner = lower_case(possible_owner->query_name());
   }
   if (!restored && owner)
   {
      SHIP_D->restore_ship_state(this_object());
      this_object()->set_restored(1);
      reinit_room();
      set_relations();
   }
   state = SHIP_DIRTY;
}

void set_restored(int r)
{
   restored = r;
}

string save_to()
{
   return SHIP_PATH(query_owner()) + ".o";
}

void unguarded_save()
{
   before_save();
   unguarded(1, ( : write_file, save_to(), save_to_string(), 1 :));
   state = SHIP_PERSISTED;
}

int restore_ship(string save_str)
{
   load_from_string(save_str, 1);
}

int is_ship_empty()
{
   object *players = filter_array(deep_inventory(), ( : $1->is_body() :));
   players -= ({0});
   return !sizeof(players);
}

int count_objects(string file)
{
   object *contents = all_inventory();
   return sizeof(filter(contents, ( : cannonical_form($1) == $(file) :)));
}

void set_saved_objects(mapping m)
{
   if (!restored)
      return;
   foreach (string file, mixed parameters in m)
   {
      int num = 1;
      mixed *rest = ({});
      int matches = 0;
      file = absolute_path(file, this_object());
      if (intp(parameters))
         num = parameters;
      else if (arrayp(parameters))
      {
         if (intp(parameters[0]))
         {
            num = parameters[0];
            rest = parameters[1..];
         }
         else
            rest = parameters;
      }
      else
         continue;

      matches = count_objects(file);
      while (matches < num)
      {
         int ret;
         object ob = new (absolute_path(file), rest...);
         if (!ob)
            error("Couldn't find file '" + file + "' to clone!\n");
         ret = ob->move(this_object(), "#CLONE#");
         if (ret != MOVE_OK)
            error("Initial clone failed for '" + file + "': " + ret + "\n");

         ob->on_clone(rest...);
         matches++;
      }
   }
}

int is_ship()
{
   return 1;
}

void object_left(object ob)
{
   if (is_ship_empty() && state == SHIP_DIRTY)
   {
      object *obs = all_inventory();
      if (obs)
         inventory = save_to_string();
      SHIP_D->save_ship_state(this_object());
   }
   else
   {
   }
}

void before_save()
{
   while (present("ship_furniture"))
      present("ship_furniture")->remove();
   restored = 0;
}

void init_room(object r)
{
   // Override to perform custom setup for person.
}

void reinit_room()
{
   string base_file = implode(explode(base_name(this_object()), "/")[0.. < 3], "/");
   object base_obj = load_object(base_file);
   if (base_obj)
      base_obj->init_room(this_object());
}

object virtual_create(string arg)
{
   object room;
   if (sscanf(arg, "%s/%d", owner, unique_id) != 2)
      return 0;
   room = new (__FILE__, owner, unique_id);
   init_room(room);
   return room;
}

void do_listen()
{
   string listen_str = "";
   foreach (object b in all_inventory())
   {
      string sound = b->furniture_sound();
      if (sound)
         listen_str += sound + " ";
   }
   write(listen_str + "\n");
}