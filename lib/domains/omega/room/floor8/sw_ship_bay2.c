inherit ROOM_DOCKING;

void setup()
{
    ::setup();
    set_area("omega_terminal", "bay_term");
    // set_weather(1);
    set_light(1);
    set_brief("Bay Terminal SW-3");
    set_long("A place where you can rent or buy bays to park your ship");
    set_exits((["northwest": "sw_ship_bay1",
    "southeast": "s_landing2",
    ]));
}
