inherit INDOOR_ROOM;

void setup()
{
   set_area("omega_terminal", "landing_term");
   // set_weather(1);
   set_light(1);
   set_brief("Stargate Terminal");
   set_long("The room is vast, stretching up to the high ceiling in a dome-like shape. "
            "The walls are made of a gleaming metal that reflected the light of the star-filled"
            "sky outside, casting a soft glow into the room. In the center of the room is a circular"
            " platform that looked like a massive metal disc, with a pulsating energy field swirling in the center.");
   set_state_description("electronic_door_off", "closed");
   set_state_description("electronic_door_on", "open");
   set_exits(([]));
  set_objects((["/std/elevator_call_button":({"9/Stargate Terminal", "^omega/room/elevator_center"}),
                       "/std/elevator_door":({"northwest", "^omega/room/elevator_center"}),
                );

  set_room_chat(({"Some busy travellers pass through the waiting room in a hurry",
                  "It seems like the elevator is moving again",
                  "Something, somewhere, is beeping",
                  "A small cockroach scuttles quickly into hiding under a floor tile",
                  "A light in the ceiling flickers"}),
                10, 25);
}

// Function needed to tell people about the elevator lamp in any room with an elevator.
void arrived()
{
  if (query_state("lamp"))
     tell_from_inside(this_object(), "The lamp on the button beside the elevator goes out.\n");
  clear_room_state("lamp");
}