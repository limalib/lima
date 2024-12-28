/* Do not remove the headers from this file! see /USAGE for more info. */

// This is an example of a transient spell object that is held transparently in the
// players inventory. This shield spell provides a magical shield that reduces incoming
// bludgeon damage by intercepting the combat event and modifying it if the damage
// type is "bludgeon".
//
// The strength, duration, etc of the shield is determined upon clone of the shield.
// If more shield spells are cast, the duration is extended. (Thanks TRANSIENT).

inherit TRANSIENT_SPELL;

int spent;

int do_effect()
{
   if (spent)
      remove();
}

class event_info effect_modify_event(class event_info evt)
{
   // Only modify events that are combat events from the caster.
   if (this_body() == query_caster() && !spent)
   {
      this_body()->targetted_action("$N $vstrike true against $t!",environment());
      evt.data[1] = evt.data[1] > 0 ? evt.data[1] * 2 : 1;
      spent = 1;
      call_out("do_effect", 1);
   }
   return evt;
}

void setup(int duration_par)
{
   set_effect_type("true strike");
   set_effect_name("true strike spell");
   set_effect_duration(duration_par);
}

// @new("/domains/std/spell/transient/true_strike",5,120,"Blarg")->move(.here:troll)