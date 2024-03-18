/* Do not remove the headers from this file! see /USAGE for more info. */

inherit WEAPON;

void mudlib_setup()
{
   ::mudlib_setup();
   set_id("sword");
   set_combat_messages("combat-sword");
   set_damage_type("slashing");
   set_can_dual_wield(1);
#ifdef USE_SKILLS
   set_skill_used("combat/melee/blade");
#endif
}
