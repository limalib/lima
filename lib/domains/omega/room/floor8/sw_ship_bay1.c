inherit INDOOR_ROOM_DOCKING;

void setup()
{
    ::setup();
    set_area("omega_terminal", "bay_term");
    set_brief("Bay Terminal SW-2");
    set_long("A place where you can rent or buy bays to park your ship");
    set_exits((["northwest": "sw_ship_bay",
    "southeast": "sw_ship_bay2",
    ]));
}
