/* Do not remove the headers from this file! see /USAGE for more info. */

#include <behaviour.h>

inherit ADVERSARY;
inherit BEHAVIOUR_TREE;

void setup()
{
   set_name("George");
   set_id("george", "geo");
   set_race("goblin");
   set_gender(1);
   set_proper_name("George the Goblin");
   set_in_room_desc("A small ugly goblin called George.");
   set_long("George is a small goblin, but with a strange glint in his eye.");
   set_max_health(30);
   set_wielding("/domains/std/weapon/sword");
   set_level(3);
   start_behaviour();
   set_wander_area(({"behaviour_area","wiz_area"}));
   set_objects((["^std/consumable/peanuts":1, "^std/consumable/bandage":1]));
   set_will_attack_friendly(1);
}

int association_for(object liv)
{
   string met_race = liv->query_race();
   // George really likes other goblins
   if (met_race == "goblin" || met_race == "badger")
      return FRIENDLY;
   // But is suspicious about everyone else.
   return SUSPICIOUS;
}
