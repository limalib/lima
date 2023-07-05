inherit ARMOR;

void setup()
{
   set_id("jacket");
   set_adj("bomber");
   set_armor_class(random(3) + 1);
   set_wearmsg("$N $vslip into $p thick green bomber jacket.");
   set_slot("torso");
   set_resistances((["slashing":10, "bludgeon":10]));
   set_salvageable((["skin":100]));
   set_also_covers("left arm", "right arm");
}
