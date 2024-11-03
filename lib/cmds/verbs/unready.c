/* Do not remove the headers from this file! see /USAGE for more info. */

inherit VERB_OB;

// Rely on default rules for "", "OBJ" and "OBS"

//: COMMAND
// USAGE
//
//  |  ``unready <something>``
//
//  Unready your crossbow.
//
// .. TAGS: RST

void create()
{
   add_rules(({"OBJ", "OBS", ""}), ({}));
}
