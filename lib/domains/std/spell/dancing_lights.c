/* Do not remove the headers from this file! see /USAGE for more info. */

#include <spells.h>

#define DANCING_LIGHTS_OBJ "/domains/std/spell/item/dancing_lights"

inherit COMBAT_SPELL;
inherit M_DICE;

// Inspired by D&D SRD, but modified for an interactive MUD.

void setup()
{
   set_level(0);
   set_name("dancing lights");
   set_category("evocation");
   set_targets(TARGET_ROOM);
   set_cast_time(1);
   set_description("You create up to four lights that resemble floating candles. "
                   "They follow you about and shed light as candles. ");
}

void cast_spell(object ob, object reagent, int success)
{
   int duration = (spell_skill_rank(this_body(), "magic/evocation/dancing lights") + 2) * (1 + success);
   object dl;

   // Only one dancing lights at a time.
   // Call unique() to check if the object is unique to the caster.
   if (!STATE_D->unique(DANCING_LIGHTS_OBJ, this_body()))
   {
      // Should be safe to use write() as it goes to this_body().
      write("You fail to cast a second dancing lights at a time.");
      return;
   }

   // Create the dancing lights object, it's unique to the caster.
   dl = new (DANCING_LIGHTS_OBJ, duration);
   this_body()->simple_action("$N $vsummon a swarm " + (success ? "of bright " : "") + "of dancing lights.");
   dl->move(environment(this_body()));
   dl->set_tamed_by(this_body());
   dl->setup_follow(this_body());
   dl->set_owner(this_body());
   dl->no_chip();
}