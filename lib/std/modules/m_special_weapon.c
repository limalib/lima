/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// This module can be used to create magical / special weapons for the mud. It includes randomization functions
// for materials and spell-like powers. These powers ranged from common (less powerful) to very rare powers (very
// powerful). The weapons can be upgraded using the
//
//   void upgrade_weapon_material() -- chance to upgrade the material to the next tier.
//   void upgrade_weapon_attack()   -- chance to upgrade the attack level to the next tier.
//
// These functions should be interfaced from a smith or other NPC that takes some materials, money etc. to attempt an
// upgrade. The item will increase in value automatically once upgraded.

/* Tsath 2020.
** Special weapons using damage types, skill restrictions and metal types to change damage and attack levels.
*/

#include "/std/classes/event_info/types.c"
#include <combat_modules.h>

#define MAT_SUM 256
#define ATTK_SUM 230
#define WEAPON_CAN_NEVER_DOWNGRADE 1

string *query_id();                      // obj/names
void clear_adj();                        // obj/names
string *query_adj();                     // obj/names
void set_adj(string *adj...);            // obj/names
void set_value_factor(float f);          // M_VALUEABLE
void set_to_hit_bonus(int x);            // M_DAMAGE_SOURCE
void set_damage_bonus(int x);            // M_DAMAGE_SOURCE
void set_skill_restriction(string, int); // M_DAMAGE_SOURCE
string primary_salvage();                // M_SALVAGEABLE

private
nosave mapping special_mats;
private
nosave mapping special_attk;
private
nosave string *rn;
private
nosave int *mat_sums;
private
nosave int *attk_sums;
private
string material;
private
string special_damage_type;
private
string special_damage_level;
private
int special_mat_level = -1;
private
int special_attk_level = -1;

/* Private functions from here */

private
void init_data()
{
   string prime_mat = primary_salvage();
   mixed *info = CRAFTING_D->query_special_mats(prime_mat);
   special_mats = info[0];
   mat_sums = info[1];

   info = DAMAGE_D->query_special_attk();
   special_attk = info[0];
   attk_sums = info[1];
}

mapping query_special_attk()
{
   return special_attk;
}

string query_special_material()
{
   return material;
}

private
void set_special_restriction()
{
   int max_level = max(({special_mat_level, special_attk_level}));
   set_skill_restriction(this_object()->query_skill_used(), (max_level + 1));
}

private
varargs mixed *random_special(int seed, int max, mapping map)
{
   int sum = 0;
   int level = 0;
   int pick = random(max - seed) + seed;

   foreach (int val in sort_array(keys(map), -1))
   {
      sum += val;
      if (pick <= sum)
         return ({level, map[val]});
      level++;
   }
}

string random_damage_type()
{
   return choice(DAMAGE_D->query_special_damage_types());
}

private
int binary_level()
{
   return sort_array(keys(special_mats), -1)[special_mat_level];
}

private
int binary_attack_level()
{
   return sort_array(keys(special_attk[special_damage_type]), -1)[special_attk_level];
}

private
varargs int summary_mat_level(int level)
{
   if (special_mat_level == -1)
      return 0;
   return mat_sums[level ? level : special_mat_level];
}

private
varargs int summary_attk_level(int level)
{
   if (special_attk_level == -1)
      return 0;
   return attk_sums[level ? level : special_attk_level];
}

private
float calc_value_factor()
{
   return 1 + ((1.0 * MAT_SUM) / (binary_level()) / 50) +
          (special_damage_type ? ((1.0 * ATTK_SUM) / (binary_attack_level()) / 50) : 0);
}

void setup_damage_types()
{
   if (strlen(special_damage_type) && sizeof(this_object()->query_damage_type()) == 1)
      this_object()->set_damage_type(this_object()->query_damage_type()[0], special_damage_type);
}

private
void internal_set_long()
{
   if (special_mat_level >= 0 && special_attk_level >= 0)
      this_object()->set_long("This " + query_id()[0] + " is imbued with " + special_damage_type + " magic at level " +
                              rn[special_attk_level] + " (" + special_damage_level + "), and the " + material +
                              " is a level " + rn[special_mat_level] + " material.");
   setup_damage_types();
}

private
void change_material()
{
   mixed *mats;
   int s_level = special_mat_level - 1;
   if (s_level < 0)
      s_level = 0;
   if (!special_mats)
      init_data();
   mats = random_special(WEAPON_CAN_NEVER_DOWNGRADE + summary_mat_level(s_level), MAT_SUM, special_mats);
   special_mat_level = mats[0];
   material = mats[1];

   clear_adj();
   set_adj(material);
   set_value_factor(calc_value_factor());
   set_to_hit_bonus(1 + special_mat_level);
   set_damage_bonus(1 + special_mat_level);
   set_special_restriction();
   internal_set_long();
}

private
void special_attack()
{
   mixed *attcks;
   int s_level = special_attk_level - 1;
   if (s_level < 0)
      s_level = 0;
   if (!special_mats)
      init_data();
   attcks = random_special(WEAPON_CAN_NEVER_DOWNGRADE + summary_attk_level(s_level), ATTK_SUM,
                           special_attk[special_damage_type]);
   special_attk_level = attcks[0];
   special_damage_level = attcks[1];
   set_value_factor(calc_value_factor());
   set_special_restriction();
   internal_set_long();
}

/* Public functions from here */

//: FUNCTION query_special_mat_level
// int query_special_mat_level()
// Returns the material level of the special weapon.
int query_special_mat_level()
{
   return special_mat_level;
}

//: FUNCTION query_special_att_level
// int query_special_att_level()
// Returns the special attack level of the weapon.
int query_special_att_level()
{
   return special_attk_level;
}

//: FUNCTION roll_item
// void roll_item()
// Must be called from weapons inherting M_SPECIAL_WEAPON in setup()
// to randomize the weapon so it's ready for player pickup. This function
// is automatically not called more than once, and not when player quits
// and loads again.
void roll_item()
{
   call_out("internal_roll_item", 0);
}

//: FUNCTION upgrade_weapon_material
// int upgrade_weapon_material()
// Call to attempt a material upgrade. Value of item is adjusted automatically.
// Returns the value incrase if any.
int upgrade_weapon_material()
{
   int value = this_object()->query_value();
   change_material();
   return this_object()->query_value() - value;
}

//: FUNCTION upgrade_weapon_attack
// int upgrade_weapon_attack()
// Call to attempt an attack power upgrade. Value of item is adjusted automatically.
// Returns the value incrase if any.
int upgrade_weapon_attack()
{
   int value = this_object()->query_value();
   special_attack();
   return this_object()->query_value() - value;
}

/* Internal public functions here, not documented, called by other things externally */

string short()
{
   return this_object()->ob_state()[0.. < 3] + (special_damage_level ? " of " + special_damage_level : "");
}

/* Must be public to allow call by call_out() */
void internal_roll_item()
{
   if (special_mat_level == -1)
   {
      change_material();
      special_damage_type = random_damage_type();
      special_attack();
   }

   if (sizeof(query_adj()))
   {
      clear_adj();
      set_adj(material);
   }

   internal_set_long();
}

/* Does the special damage */
class event_info source_modify_event(class event_info evt)
{
   int opp_health;
   int damage;
   int excess;
   string *system_limbs;

   if (evt->data == "miss" || evt->data == "disarm")
      return evt;

   damage = evt->data[sizeof(evt->data) - 1];

   /*
   if (special_damage_type)
       evt->data[0] += ({special_damage_type});
       */
   evt->data[1] += 1 + random(special_attk_level);
   if (evt->data[1] <= 0)
      evt->data[1] = 1;
   // do_effect_info();

   return evt;
}

int is_special_weapon()
{
   return 1;
}

void internal_setup()
{
   rn = SKILL_D->titles()[1..];
   this_object()->add_save(
       ({"material", "special_damage_type", "special_damage_level", "special_mat_level", "special_attk_level"}));
}
