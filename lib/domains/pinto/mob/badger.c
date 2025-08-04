/* Do not remove the headers from this file! see /USAGE for more info. */

inherit ADVERSARY;

void setup()
{
   set_name("badger");
   set_id("badger");
   set_in_room_desc("A sturdy badger is nosing around here, searching for food.");
   set_combat_messages("combat-claws-bites");
   set_long("This is a stocky, black-and-white badger with sharp claws and a determined look. Its fur is thick and "
            "coarse, and it seems quite at home in the wild, always alert for danger or a tasty meal.");
   update_body_style("quadruped");
   set_level(1);
   set_combat_message("miss", "$N lunges with badger-like ferocity, but $n dodges nimbly out of the way.");
}

string query_race()
{
   return "badger";
}

string query_drops_pelt()
{
   return "badger skin";
}

string query_drops_meat()
{
   return "badger meat";
}