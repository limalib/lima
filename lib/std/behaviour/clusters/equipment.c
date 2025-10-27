
/* Do not remove the headers from this file! see /USAGE for more info. */

// CLUSTER_EQUIPMENT

/*
   Equipment cluster for behaviour tree.

- Upgrade true:
   - Upgrade equipment (Selector, continue until one of these is true):
     - Find better armours in inventory
     - Find better armours in room
     - Find better weapons in inventory
     - Find better weapons in room
- Hurt true:
   - Hurt (Selector, continue until one of these is true):
     - No need to heal (if not wounded)
     - Use heal (use healing items from inventory)
     - Take from room (get healing item from room and use it)



*/

#include <behaviour.h>

#define MAX_WEAPONS_CARRIED 2
#define MAX_ARMOURS_CARRIED 2

void add_child(string node, string child);
varargs void create_node(int type, string name, mixed offspring);
object *worst_threats_present();
object *query_targets();
string badly_wounded();
string very_wounded();
void do_game_command(string str);

int no_heals = 0;

void init_equipment_cluster()
{
   // If any of these ones return true we stop here, and navigate somewhere else
   create_node(NODE_SEQUENCE, "equipment_seq", ({"upgrade_true", "hurt"}));

   // Add equipment sequence to root sequence.
   add_child("root_sequence", "equipment_seq");
   create_node(NODE_SUCCEEDER, "upgrade_true", ({"upgrade_seq"}));

   // Look for better armour
   create_node(NODE_SELECTOR, "upgrade_seq",
               ({"find_armour_in_inventory", "find_armour_in_room", "find_weapon_in_inventory", "find_weapon_in_room",
                 "clean_inventory"}));

   // Heal ourselves if needed
   create_node(NODE_SELECTOR, "hurt", ({"no_need_to_heal", "use_heal", "take_from_room"}));
   create_node(NODE_LEAF, "no_need_to_heal");
   create_node(NODE_LEAF, "use_heal");

   // Can we find something useful to pick up?
   create_node(NODE_LEAF, "take_from_room");
   create_node(NODE_LEAF, "find_armour_in_inventory");
   create_node(NODE_LEAF, "find_armour_in_room");
   create_node(NODE_LEAF, "find_weapon_in_inventory");
   create_node(NODE_LEAF, "find_weapon_in_room");
   create_node(NODE_LEAF, "clean_inventory");
}

int use_heal()
{
   object *consumables = filter_array(all_inventory(this_object()), ( : $1->is_healing() :));
   object pick;
   string limb = very_wounded() || badly_wounded();

   if (sizeof(consumables))
      pick = choice(consumables);

   if (!pick)
   {
      no_heals = 1;
      return EVAL_FAILURE;
   }

   // Bit defensive code here since things are very interactive and can disappear
   // quickly.
   if (pick->is_bandage() && limb)
   {
      this_object()->do_game_command("apply bandage to " + limb);
      return EVAL_SUCCESS;
   }
   else if (pick->is_food())
   {
      this_object()->digest((10 + pick->get_original_eats() * 5));
      pick->do_eat(this_object());
      return EVAL_SUCCESS;
   }
   else if (pick->is_drink())
   {
      pick->drink_it(this_object());
      return EVAL_SUCCESS;
   }
   return EVAL_FAILURE;
}

int no_need_to_heal()
{
   string wounded = this_object()->badly_wounded() || this_object()->very_wounded();

   // We are wounded.
   if (wounded)
      return EVAL_FAILURE;
   return EVAL_SUCCESS;
}

int take_from_room()
{
   object *consumables;
   object pick;

   if (!environment())
      return EVAL_FAILURE;
   consumables = filter_array(all_inventory(environment()), ( : $1->is_healing() :));

   if (sizeof(consumables))
      pick = choice(consumables);
   if (!pick)
      return EVAL_FAILURE;

   // Grab it!
   do_game_command("get " + pick->short());

   // We got it! Use it!
   if (environment(pick) == this_object())
   {
      return use_heal();
   }
   return EVAL_FAILURE;
}

int find_armour_in_inventory()
{
   object *armours = filter(all_inventory(this_object()), ( : $1->is_wearable() :));
   object *not_worn = filter(armours, ( : !$1->ob_state() :));
   int worn = 0;

   foreach (object nww in not_worn)
   {
      object *armour_obs;
      int armour_cnt;
      string slot = nww ? nww->query_slot() : 0;
      if (!slot)
         continue;
      armour_obs = this_object()->query_armours(slot);
      armour_cnt = sizeof(armour_obs);

      if (!armour_cnt)
      {
         do_game_command("wear " + nww->short());
         worn++;
      }

      // There can be more armours in same slot, but that's too much hassle.
      if (armour_cnt == 1)
      {
         // Defensive, but they can disappear...
         int current_ac = sizeof(armour_obs) ? armour_obs[0]->query_armour_class() : 0;

         // We found a better armour
         if (current_ac < nww->query_armour_class())
         {
            do_game_command("remove " + armour_obs[0]->short());
            do_game_command("wear " + nww->short());
            do_game_command("salvage " + armour_obs[0]->short());
            worn++;
         }
      }
   }
   return worn ? EVAL_SUCCESS : EVAL_FAILURE;
}

int find_armour_in_room()
{
   object *armours = filter(all_inventory(this_object()), ( : $1->is_wearable() :));
   object *in_room = filter(all_inventory(environment()), ( : $1->is_wearable() :));
   int worn = 0;

   foreach (object ira in in_room)
   {
      object *armour_obs;
      int armour_cnt;
      string slot = ira ? ira->query_slot() : 0;
      if (!slot)
         continue;
      armour_obs = this_object()->query_armours(slot);
      armour_cnt = sizeof(armour_obs);

      if (!armour_cnt)
      {
         do_game_command("get " + ira->short());
         do_game_command("wear " + ira->short());
         worn++;
      }

      // There can be more armours in same slot, but that's too much hassle.
      if (armour_cnt == 1)
      {
         // Defensive, but they can disappear...
         int current_ac = sizeof(armour_obs) ? armour_obs[0]->query_armour_class() : 0;

         // We found a better armour
         if (current_ac < ira->query_armour_class())
         {
            do_game_command("get " + ira->short());
            do_game_command("remove " + armour_obs[0]->short());
            do_game_command("wear " + ira->short());
            do_game_command("salvage " + armour_obs[0]->short());
            worn++;
         }
      }
   }
   return worn ? EVAL_SUCCESS : EVAL_FAILURE;
}

int best_weapon(object ob1, object ob2)
{
   return ob1->query_weapon_class() < ob2->query_weapon_class() ? 1 : -1;
}

int find_weapon_in_inventory()
{
   object *weapons = filter(all_inventory(this_object()), ( : $1->valid_wield() :));
   object *wielded_weapons = filter(all_inventory(this_object()), ( : $1->query_wielding() :));
   int best_wc = max(weapons->query_weapon_class());
   int best_wielded = sizeof(wielded_weapons) ? max(wielded_weapons->query_weapon_class()) : 0;
   int wielded = 0;

   weapons = sort_array(weapons, ( : best_weapon:));
   wielded_weapons = sort_array(wielded_weapons, ( : best_weapon:));

   // We got a better weapon that what we are wielding
   if (best_wc > best_wielded)
   {
      do_game_command("unwield " + wielded_weapons[ < 1]->short());
      do_game_command("salvage " + wielded_weapons[ < 1]->short());
      if (wielded_weapons[ < 1])
         wielded_weapons[ < 1]->remove();
      do_game_command("wield " + weapons[0]->short());
      wielded = 1;
   }

   if (wielded)
   {
      return EVAL_SUCCESS;
   }

   return EVAL_FAILURE;
}

int find_weapon_in_room()
{
   object *weapons = filter(all_inventory(this_object()), ( : $1->valid_wield() :));
   object *in_room = filter(all_inventory(environment()), ( : $1->valid_wield() :));
   int best_wc = max(weapons->query_weapon_class());
   int wielded = 0;

   in_room = sort_array(in_room, ( : best_weapon:));

   if (sizeof(in_room) > 0 && in_room[0]->query_weapon_class() >= best_wc)
   {
      do_game_command("get " + in_room[0]->short());
      wielded = 1;
   }

   if (wielded)
   {
      do_game_command("wield " + in_room[0]->short());
      return EVAL_SUCCESS;
   }

   return EVAL_FAILURE;
}

int clean_inventory()
{
   object *armours = filter(all_inventory(this_object()), ( : $1->is_wearable() && !$1->ob_state() :));
   object *weapons = filter(all_inventory(this_object()), ( : $1->valid_wield() && !$1->query_wielding() :));

   if (sizeof(weapons) > MAX_WEAPONS_CARRIED)
   {
      do_game_command("salvage " + choice(weapons)->short());
      return EVAL_SUCCESS;
   }
   if (sizeof(armours) > MAX_ARMOURS_CARRIED)
   {
      do_game_command("salvage " + choice(armours)->short());
      return EVAL_SUCCESS;
   }

   return EVAL_FAILURE;
}

string equipment_features()
{
   return "<033>Equipment:\n<res>" + "\t- Heal from inventory\n"
                                     "\t- Find healing from rooms and claim\n"
                                     "\t- Wear better armours\n"
                                     "\t- Find better armours and wear them\n"
                                     "\t- Wear better weapons\n"
                                     "\t- Find better weapons and wear them\n";
}