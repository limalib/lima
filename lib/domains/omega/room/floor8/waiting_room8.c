inherit INDOOR_ROOM;

void setup()
{
  set_area("omega_terminal", "landing_term");
  // set_weather(1);
  set_light(1);
  set_brief("Waiting Room");
  set_long("A waiting room with an elevator on the north west wall.");
  set_state_description("electronic_door_off", "closed");
  set_state_description("electronic_door_on", "open");
  set_exits((["south":"s_hall4",
              "north":"n_hall4",
               "west":"w_hall4",
               "east":"e_hall4"]));
  set_objects((["/std/elevator_call_button":({"8/Landing Terminal", "^omega/room/elevator_center"}),
                       "/std/elevator_door":({"northwest", "^omega/room/elevator_center"}),
                 "^common/item/node_portal":({"^omega/room/floor8/n_hall1"})]));

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
