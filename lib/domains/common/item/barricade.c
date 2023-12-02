/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CLIMB_CHALLENGE;

void setup()
{
    set_fail_climb_message(({
        "$N $vstumble over the barricade, but $vmake it across - a bit hurt.",
        "$N $vstumble over the barricade, but $vmake it across - quite hurt.",
    }));
}

string query_hint()
{
    return "The barricade may be climbable?";
}