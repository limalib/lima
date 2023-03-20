/*
** equip.c -- material pouch command
**
** Tsath, 2020
*/

//: PLAYERCOMMAND
//$$ see: skills, hp, stats, score, pouch
// USAGE equip
//
// Shows your wielded and worn weapons and armors, their durability
// and some primary stats. The command also shows your spell failure chance
// which increases with the more medium and heavy armor you wear, the latter
// having a greater impact.
//
// Reparing and salvaging
//---
// You can repair your gear using the 'repair' verb using components from
//'salvage' if you are next to a crafting bench. The components can be
// seen in your 'pouch'.
//
// You can easily salvage by using 'salvage all' which will salvage all
// your tattered equipment. You can salvage other things by salvaging
// them one by one, i.e. 'salvage mace' to salvage your non-tattered mace.
//
// Use 'repair all' to repair all your equipment. The command will give you
// an estimate of the cost in materials and coins (if needed) before you
// decide to repair or not. Be careful not to repair things you do not want
// to keep. It does not pay to repair things you want to sell.

#include <config.h>

inherit CMD;

class wear_info
{
   object primary;
   object *others;
}

mapping short_names = DAMAGE_D->query_short_names();

string abbreviate(string s)
{
   return filter_array(s, ( : member_array($1, "eyuioa") == -1 :));
}

private
void main(string arg)
{
   int width = this_user()->query_screen_width() - 7;
   object *weapons = ({});
   object *armors = ({});
   object frame = new (FRAME);
   object body = this_body();
   int nothing_worn = 1;
   int nothing_wielded = 1;

   if (strlen(arg) > 0 && wizardp(this_user()))
   {
      body = find_body(arg);
      if (!body)
      {
         out("Cannot find '" + arg + "'.\n");
         return;
      }
      write("Equip for " + capitalize(arg) + ":\n");
   }

   foreach (string limb in body->query_wielding_limbs())
   {
      if (member_array(body->query_weapon(limb), weapons) == -1)
         weapons += ({body->query_weapon(limb)});
   }

   // foreach (string limb in body->query_armor_slots())
   foreach (string limb in body->query_limbs())
   {
      if (body->find_wi(limb) && member_array(body->find_wi(limb), armors) == -1)
         armors += ({((class wear_info)body->find_wi(limb))->primary});
   }

   weapons = filter_array(weapons, ( : $1:));
   armors = filter_array(armors, ( : $1:));

   if (sizeof(weapons))
   {
      string *props = ({});
      string content = "";
      frame->set_title("Weapons");
      frame->set_header_content(
          sprintf("%-24s  %-7s  %-5s %-11s   %s", "Weapon", "WC", "Dura", "Damage Type", "Properties"));
      foreach (object w in weapons)
      {
         string *types =
             w->query_loadable() && w->loaded_with() ? w->loaded_with()->query_damage_type() : w->query_damage_type();
         types -= ({0});
         types = map_array(types, ( : short_names[$1] :));
         props += w->query_properties() || ({});
         content += sprintf(" %-24s  %-7s  %-5s  %11-s  %s\n", capitalize(w->short()),
                            "" + w->query_weapon_class() +
                                (w->query_to_hit_bonus() > 0
                                     ? "(+" + w->query_to_hit_bonus() + ")"
                                     : (w->query_to_hit_bonus() < 0 ? "(" + w->query_to_hit_bonus() + ")" : "")),
                            "" + w->durability_percent() + "%", implode(types, ","), format_list(props));
      }
      frame->set_content(content);
      write(frame->render());
      nothing_wielded = 0;
   }

   if (sizeof(armors))
   {
      string content = "";
      frame = new (FRAME);
      frame->set_title("Armours");
      frame->set_header_content(sprintf("%-25s %-21s  %-3s  %-5s  %s", "Item", "Worn on", "AC", "Dura", "Stat mod."));
      foreach (object a in armors)
      {
         string *slots = ({a->query_slot()});

         if (a->also_covers())
            slots += a->also_covers();

         if (member_array("left foot", slots) != -1 && member_array("right foot", slots) != -1)
         {
            slots -= ({"left foot", "right foot"});
            slots += ({"feet"});
         }

         if (member_array("left leg", slots) != -1 && member_array("right leg", slots) != -1)
         {
            slots -= ({"left leg", "right leg"});
            slots += ({"legs"});
         }

         if (member_array("left arm", slots) != -1 && member_array("right arm", slots) != -1)
         {
            slots -= ({"left arm", "right arm"});
            slots += ({"arms"});
         }

         if (member_array("left hand", slots) != -1 && member_array("right hand", slots) != -1)
         {
            slots -= ({"left hand", "right hand"});
            slots += ({"hands"});
         }

         if (member_array("arms", slots) != -1 && member_array("legs", slots) != -1 &&
             member_array("torso", slots) != -1)
         {
            slots -= ({"arms", "legs", "torso"});
            slots += ({"full body"});
         }

         slots = map_array(slots, ( : capitalize($1) :));

         content += sprintf(" %-24s  %-21s  %-3s  %-5s  %s\n", capitalize(a->short()), format_list(slots),
                            "" + (a->query_armor_class() || "-"), a->query_armor_class() ? "" + a->durability_percent() + "%" : "-",
                            (a->query_stat_bonus() ? capitalize(a->query_stat_bonus()) + " " +
                                                         (a->query_stat_mod() >= 0 ? "+" : "") + a->query_stat_mod()
                             : a->stat_mods_string(1) ? a->stat_mods_string(1)
                                                      : ""), );
      }
      frame->set_content(content);
      write(frame->render());
      nothing_worn = 0;
   }

   if (nothing_worn & nothing_wielded)
      printf("You are wielding and wearing nothing.\n");
   else if (nothing_worn)
      printf("\n\n(You are wearing nothing)\n");
   else if (nothing_wielded)
      printf("\n\n(You are wielding nothing)\n");
}
