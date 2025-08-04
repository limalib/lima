/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OUTDOOR_ROOM;

void setup()
{
   set_brief("Western Ruins");
   set_area("behaviour_area", "wiz_area");

   set_long("You stand among the western ruins, where ancient stone columns rise from the earth, "
            "their surfaces etched with faded symbols. The ground is firm and dry, scattered with "
            "fragments of pottery and smooth marble tiles. Sunlight streams through gaps in the crumbling "
            "walls, illuminating patches of wildflowers that have taken root in the cracks. The air is "
            "still and quiet, filled only with the faint scent of old stone and blooming thyme.");
   set_listen("It is peaceful here, with only the soft whisper of wind through the ruins.");
   set_smell("The scent of sun-warmed stone and wild thyme fills the air.");
   add_item("columns", "Tall, weathered columns stand in silent testimony to a lost civilization.");
   add_item("symbols", "Ancient, faded symbols are carved into the stone, their meaning long forgotten.");
   add_item("pottery", "Broken pieces of pottery lie scattered about, remnants of daily life from ages past.");
   add_item("tiles", "Smooth marble tiles, some still intact, form a mosaic beneath your feet.");
   add_item("wildflowers", "Bright wildflowers grow in the cracks, adding color to the ruins.");
   add_item("walls", "Crumbling stone walls, now home to moss and flowers, enclose the space.");
   set_exits((["east":"swamp3.c"]));
   set_objects((["^std/weapon/club":1]));
}
