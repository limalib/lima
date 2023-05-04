/*
** Ship object for player ship and perstent rooms.
** Interacts with SHIP_D.
*/

inherit ROOM;
inherit SPACE_CLASSES;
inherit M_ACCESS;
inherit __DIR__ "spaceship/persistence";
inherit __DIR__ "spaceship/status";

private
int unique_id;
private
nosave string ship_brief;
private
nosave string ship_long;
private
nosave string ship_type;
private
nosave int weekly_cost = 10;
nosave object base_obj;
private
nosave int size = 1;
private
int room_on_ship = 10;
private
class ship_info ship_i;

string query_owner();
void set_owner(string str);
void setup_persistence();
void object_arrived(object ob);
void object_left(object ob);

void mudlib_setup()
{
   ::mudlib_setup();
   set_save_recurse(1);
   add_hook("object_left", ( : object_left:));
   add_hook("object_arrived", ( : object_arrived:));
   set_privilege("Common:ships");
}

object *on_ship()
{
   object *here = filter_array(all_inventory(),
                               (
                                   : base_name($1)[0..21] != "/domains/common/ship/" &&
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

void set_ship_size(int sz)
{
   size = sz;
}

int query_ship_size()
{
   return size;
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

class ship_info query_ship_info()
{
   return ship_i || ship_i = SHIP_D->find_ship(base_name(this_object()));
}

int is_docked()
{
   return SHIP_D->query_docked(query_ship_info()->name);
}

object virtual_create(string ship_file, string arg)
{
   object ship;
   string owner;
   if (sscanf(arg, "%s/%d", owner, unique_id) != 2)
      return 0;
   set_owner(owner);
   ship = new (ship_file);
   ship_i = 0;
   return ship;
}

// Function to smash together all the sounds from all the furniture and comma separate them. The sounds should return
// sentences not starting with a capital and ending with a punctuation.
void do_listen()
{
   string *listens = ({});
   foreach (object b in all_inventory())
   {
      string sound = b->furniture_sound();
      if (sound)
         listens += ({sound});
   }
   if (sizeof(listens))
      write(punctuate(capitalize(format_list(listens))));
   else
      ::do_listen();
}

int is_ship()
{
   return 1;
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
