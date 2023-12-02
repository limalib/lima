/* Do not remove the headers from this file! see /USAGE for more info. */

inherit ADVERSARY;
inherit M_WANDER;
inherit M_SMARTMOVE;
inherit M_ACTIONS;
inherit M_AGGRESSIVE;

void setup()
{
   object carapace = new ("/domains/omega/mob/tech_gremlin_carapace");
   int size = 1 + random(3);
   string size_str;
   ::mudlib_setup();

   switch (size)
   {
   case 1:
      size_str = "small";
      break;
   case 2:
      size_str = "adult";
      break;
   case 3:
      size_str = "large";
      break;
   }
   set_name("tech gremlin");
   set_id("tech gremlin","gremlin");
   set_combat_messages("combat-claws-bites");
   add_adj(size_str);
   set_gender(0);
   set_wander_time(20);
   set_wander_area("maintmaze");
   set_aggressive(40);
   set_in_room_desc(capitalize(add_article(size_str + " tech gremlin")));
   set_long("A little box like droid with 6 metallic legs, about the size of a terrestrial cat - this one is " +
            size_str + " for the species.");
   update_body_style("insect");
   set_level(size);
   set_max_health((3 * size));
   set_skill("combat/defense/dodge", 200 * size);

   //Handle shell
   carapace->move(this_object());
   carapace->do_wear();
}
