/* Do not remove the headers from this file! see /USAGE for more info. */

inherit VERB_OB;

#include <driver/type.h>

void do_wear_obj(object ob)
{
   if (!try_to_acquire(ob))
      return;

   ob->do_wear();
}

void do_wear_obs(object *obs)
{
   foreach (object ob in obs)
   {
      // TBUG("Wear: "+ob);
      if (typeof(ob) == T_STRING)
         continue;

      // Do not try to wear living things or things not in inventory.
      if (ob->is_living() || environment(ob) != this_body())
         continue;
      ob->do_wear();
   }
}

void create()
{
   add_rules(({"OBJ", "OBS"}), ({"put on"}));
}
