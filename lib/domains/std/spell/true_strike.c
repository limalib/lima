/* Do not remove the headers from this file! see /USAGE for more info. */

#include <spells.h>

inherit COMBAT_SPELL;
inherit M_DICE;

// Inspired by D&D SRD, but modified for an interactive MUD.

void setup()
{
   set_level(0);
   set_name("true strike");
   set_category("divination");
   set_targets(TARGET_LIVING);
   set_cast_time(5);
   set_description("You extend your hand and point a finger at a target "
                   "in range. Your magic grants you a brief insight into "
                   "the target’s defenses.");
}

void cast_spell(object ob, object reagent, int success)
{
   this_body()->targetted_action("$N $vextend a hand and point a finger at $t.",ob);
   transient("true_strike", 10)->move(ob);
}