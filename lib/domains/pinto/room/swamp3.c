/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OUTDOOR_ROOM;

void setup()
{
   set_brief("Edge of the Ancient Swamp");
   set_area("behaviour_area", "wiz_area");

   set_long(
       "You find yourself at the edge of the swamp, where the dense trees begin to thin and shafts of sunlight "
       "break through the canopy. The air feels fresher here, carrying the scent of wild mint and blooming lilies. "
       "To the west and east, you notice the moss-covered stones of ancient ruins emerging from the undergrowth, "
       "hinting at a forgotten civilization. Dragonflies skim over clear pools, and a gentle breeze stirs the reeds.");
   set_listen("You hear the gentle rustle of reeds and the distant trickle of water.");
   set_smell("The air is fresh, tinged with the scent of wild mint and lilies.");
   add_item("ruins", "Mossy stones and crumbling walls peek out from the undergrowth to the west and east, "
                     "suggesting the remains of old structures.");
   add_item("stones", "Large, weathered stones covered in moss and lichen, remnants of ancient buildings.");
   add_item("reeds", "Tall green reeds sway gently in the breeze by the edge of the pools.");
   add_item("pools", "Clear pools of water reflect the sky and the surrounding greenery.");
   add_item("lilies", "Delicate white lilies bloom at the water's edge, their fragrance sweet and calming.");
   add_item("mint", "Wild mint grows in small patches, its leaves releasing a refreshing scent when disturbed.");
   add_item("breeze", "A cool, gentle breeze brings relief from the swamp's usual humidity.");
   set_exits((["north":"swamp2.c", "west":"ruins1.c", "east":"ruins2.c"]));
   set_objects((["^std/weapon/longsword":1]));
}
