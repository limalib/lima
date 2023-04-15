inherit INDOOR_ROOM;

void setup()
{
   set_area("omega_terminal", "bay_term");
   // set_weather(1);
   set_light(1);
   set_brief("Bay Terminal NE-2");
   set_long(
       "This docking bay is a spacious room with smooth, white walls and a high ceiling. " +
       "At one end of the room, there is a docking terminal and an airlock on the wall provides a buffer between the "
       "vacuum of space and the breathable " +
       "atmosphere of the station, ensuring the safety of those who come and go from the ships. " +
       "One of the most striking features of the docking bay is a large panoramic window.");
   add_item("window", (["look":"The windows provide " +
                                   "a breathtaking view of space. These windows line the walls of the room and give a "
                                   "clear view of the ships " +
                                   "as they approach and depart from the station.",
                          "read":"Someone wrote 'Killroy was here!' in the corner.", ]));

   set_exits((["northwest":"n_landing", "southeast":"ne_ship_bay1", ]));
   set_objects((["^common/item/docking_terminal":1]));
}
