// Do not remove the headers from this file! see /USAGE for more info.

inherit ELEVATOR;

void setup()
{
    // Call setup() in ELEVATOR
    ::setup();
    // Description of the elevator make sure to mention the buttons you can press.
    // Cmds like 'look at cryo button' also works.
    set_long("You are in the elevator.  There are many buttons on a panel on the wall.");
    // Set destinations with filenames of where we can go and shorthands.
    set_destinations((["1/waiting1":"^omega/room/floor1/waiting_room1",
                       "2/waiting2":"^omega/room/floor2/waiting_room2",
                       "3/waiting3":"^omega/room/floor3/waiting_room3",
                       "4/waiting4":"^omega/room/floor4/waiting_room4",
                       "5/waiting5":"^omage/room/floor5/waiting_room5",
                       "6/Upper Living Quarters":"^omega/room/floor6/waiting_room6",
                       "7/Omega Trading":"^omega/room/floor7/waiting_room7",
                       "8/Landing Terminal":"^omega/room/floor8/waiting_room8",
                       "9/Stargate Terminal":"^omega/room/floor9/stargate_term",
    ]));

    // Set start position for elevator
    move_to("8/Landing Terminal");

    // Set the distance from and to each destination in seconds
    set_distance("1/waiting1", "2/waiting2", 10);
    set_distance("2/waiting1", "1/waiting1", 10);
    set_distance("2/waiting2", "3/waiting3", 10);
    set_distance("3/waiting3", "2/waiting2", 10);
    set_distance("3/waiting3", "4/waiting4", 10);
    set_distance("4/waiting4", "3/waiting3", 10);
    set_distance("4/waiting4", "5/waiting5", 10);
    set_distance("5/waiting5", "4/waiting4", 10);
    set_distance("5/waiting5", "6/waiting6", 10);
    set_distance("6/waiting6", "5/waiting5", 10);
    set_distance("6/waiting6", "7/waiting7", 10);
    set_distance("7/waiting7", "6/waiting6", 10);
    set_distance("7/waiting7", "8/waiting8", 10);
    set_distance("8/waiting8", "7/waiting7", 10);
    set_distance("9/Stargate Terminal", "8/waiting8", 10);

    // Set an elevator door direction and default location
    set_objects((["/std/elevator_door":({"southeast", "waiting_room8"})]));

    // Spawn the buttons on the wall automatically.
    // This *must* be done after the set_destinations() cal.
    setup_buttons();
}
