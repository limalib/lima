inherit ROOM_DOCKING;

void setup()
{
    ::setup();
    set_area("omega_terminal", "bay_term");
    set_brief("Bay Terminal SE-2");
    set_long("A place where you can rent or buy bays to park your ship");
    set_exits((["northeast": "se_ship_bay",
    "southwest": "se_ship_bay2",
    ]));
    // set_objects(([""]));
}