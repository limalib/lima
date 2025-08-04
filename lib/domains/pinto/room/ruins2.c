/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OUTDOOR_ROOM;

void setup()
{
   set_brief("Eastern Ruins");
   set_area("behaviour_area", "wiz_area");

   set_long(
       "You enter the eastern ruins, where sunlight glints off shattered archways and fallen statues. "
       "The ground is paved with ancient flagstones, now cracked and overgrown with soft moss. "
       "Fragments of intricate carvings are visible on the stones, and a gentle breeze carries the "
       "scent of lavender and old parchment. The ruins are tranquil, a stark contrast to the wild swamp to the west.");
   set_listen("The ruins are silent except for the occasional flutter of a bird far above.");
   set_smell("You smell lavender and the faint, dry scent of ancient paper.");
   add_item("archways", "Shattered archways hint at the grandeur that once stood here.");
   add_item("statues", "Fallen statues, their features worn smooth by time, lie among the stones.");
   add_item("flagstones", "Ancient flagstones form a broken path through the ruins.");
   add_item("carvings", "Delicate carvings of unknown figures and patterns decorate the stones.");
   add_item("moss", "Soft green moss covers much of the ground, cushioning your steps.");
   add_item("breeze", "A gentle breeze stirs the air, bringing with it the scent of lavender.");
   set_exits((["west":"swamp3.c"]));
}
