inherit INDOOR_ROOM;

void setup()
{
   set_area("omega_terminal", "landing_term");
   // set_weather(1);
   set_light(1);
   set_brief("North Hallway");
   set_long("A hallway heading north of living quarters level. The hallway ends with a metal door.");
   set_state_description("electronic_door_off", "closed");
   set_state_description("electronic_door_on", "open");
   set_exits((["south":"n_hall2"]));
   set_objects((["^common/item/door":({"north", "^omega/room/floor1/wakeup_room"})]));
   set_listen("You hear people running through the terminal. " +
              "It sounds like a busy terminal with people coming and going");
   present("door")->set_locked("wakeuproom_no_entry");
}
