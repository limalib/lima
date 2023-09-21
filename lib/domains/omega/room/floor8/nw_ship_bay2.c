inherit ROOM_DOCKING;

void setup()
{
    ::setup();
    set_area("omega_terminal", "bay_term");
    set_brief("Bay Terminal NW-3");
    set_long("A place where you can rent or buy bays to park your ship");
    set_exits((["northeast": "nw_ship_bay1",
    "southwest": "w_landing",
    ]));
    // set_objects(([""]));
}