/* Do not remove the headers from this file! see /USAGE for more info. */

inherit ARMOR;

void setup()
{
    set_id("uniform");
    set_adj("yakitori","smudged");
    set_armor_class(10);
    set_slot("torso");
    set_resistances((["blade":10, "electricity":5]));
    set_weaknesses((["acid":5]));
    set_salvageable((["chemical":10, "textile":40, "plastic":50]));
    set_also_covers("left arm", "right arm");
    set_long("A standard-issue Yakitori red/white uniform used by delivery boys. It's a bit smudged.");
}
