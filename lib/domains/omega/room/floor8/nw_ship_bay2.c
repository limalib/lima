inherit INDOOR_ROOM;

void setup()
{
    set_area("omega_terminal", "bay_term");
    // set_weather(1);
    set_light(1);
    set_brief("Bay Terminal");
    set_long("A place where you can rent or buy bays to park your ship");
    set_state_description("electronic_door_off", "closed");
    set_state_description("electronic_door_on", "open");
    set_exits((["northeast": "nw_ship_bay1",
    "southwest": "w_landing",
    ]));
    // set_objects(([""]));
}
