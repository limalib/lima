/* Do not remove the headers from this file! see /USAGE for more info. */

inherit SIGN;

void setup()
{
   set_adj("small", "weathered");
   set_in_room_desc("A small sign is here, trenched deeply in the sand.");
   set_long("The sign is quite weathered, but still readable, nonetheless.");
   set_text("Warning:\n\n"
            "DON'T USE THIS AREA ON YOUR MUD!!!\n\n"
            "This area is part of every LIMA based MUD out there. We are providing this area as an example only!  You "
            "can use the code to the extent that you want to code something similar by example, but otherwise, get "
            "your own ideas =P\n\n"
            "As for this area, if you want to play it before you read the code, there are nine rooms and a treasure to "
            "find.  See if you can do it...\n\n"
            "-- The LIMA guys.\n");
}
