/* Do not remove the headers from this file! see /USAGE for more info. */

inherit SWORD;

void setup()
{
   set_adj("dull");
   set_id("sword");
   set_weapon_class(15);
   set_weight(1.1);
   set_value(15);
   set_skill_used("combat/melee/blade");

   add_combat_message("miss", "$N $vtake a clumsy swipe at $t, but only $vsucceed in making a fool of $r.");
   set_can_dual_wield(1);
}
