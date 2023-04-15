inherit INDOOR_ROOM;

void setup()
{
    set_area("omega_terminal", "landing_term");
    // set_weather(1);
    set_light(1);
    set_brief("East Hallway");
    set_long("A hallway leading to the East Landing.");
    set_state_description("electronic_door_off", "closed");
    set_state_description("electronic_door_on", "open");
    set_exits((["west":"e_hall2",
                "east":"e_hall",
    ]));

    set_objects((["^common/obj/auto_door":
    //({2,
    ({"north", "e_shop"}),
//    ({"west", "e_ehall2"}),
//})
    ]));
    set_listen("You hear people running through the terminal. " +
               "It sounds like a busy terminal with people coming and going");
}
