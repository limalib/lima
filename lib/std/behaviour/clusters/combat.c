/* Do not remove the headers from this file! see /USAGE for more info. */

// CLUSTER_HUNGER

#include <behaviour.h>

void add_child(string node, string child);
varargs void create_node(int type, string name, mixed offspring);
void set_blackboard(string key, mixed value);
mixed blackboard(string key);
void do_game_command(string str);
int has_room_changed();
int im_bored();
void room_checked();
void do_wander();
int player_did_arrive(string dir);
void moving();
int query_emotion(mixed emotion);
void mod_emotion(mixed emotion, int mod);
object *worst_threats_present();

void init_combat_cluster()
{
   // If any of these ones return true we stop here, and navigate somewhere else
   create_node(NODE_SUCCEEDER, "combat_true");
   add_child("root_sequence", "combat_true");
   // Sequence of combat related actions
   create_node(NODE_SEQUENCE, "combat_seq", ({"food_attack"}));
   add_child("combat_true", "combat_seq");
   create_node(NODE_LEAF, "food_attack");
}

int food_attack()
{
   if (this_object()->desperately_hungry())
   {
      object *targets = this_object()->potential_targets();
      if (sizeof(targets))
      {
         object target = targets[random(sizeof(targets))];
         this_object()->start_fight(targets[0]);
         return EVAL_SUCCESS;
      }
   }
   return EVAL_FAILURE;
}

string combat_features()
{
   return "<039>Combat:\n<res>" + "\t- Roar!\n\n";
}