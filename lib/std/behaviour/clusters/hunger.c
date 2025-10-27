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

private int hunger_level = 0; // 0-100

void init_hunger_cluster()
{
   // If any of these ones return true we stop here, and navigate somewhere else
   create_node(NODE_SUCCEEDER, "hunger_true");
   add_child("root_sequence", "hunger_true");
   // Sequence of hunger related actions
   create_node(NODE_SEQUENCE, "hunger_seq", ({"digest", "eat_food"}));
   add_child("hunger_true", "hunger_seq");
   create_node(NODE_LEAF, "digest");
   create_node(NODE_LEAF, "eat_food");
}

varargs int digest(int food)
{
   // Food is > 0, as it removes hunger.
   if (!food)
      food = -1;
   hunger_level = clamp(hunger_level - food, 0, 100);
   // if (hunger_level > 50)
   //       do_game_command("hungry");
   return EVAL_SUCCESS;
}

int eat_food()
{
   object *food = filter_array(all_inventory(environment()), ( : $1->query_num_eats() > 0 :));
   object target;
   if (!sizeof(food))
      return EVAL_FAILURE;

   target = choice(food);

   // If we are not hungry, we don't eat.
   hunger_level = clamp(hunger_level - (10 + target->get_original_eats() * 5), 0, 100);
   do_game_command("eat " + target->query_id()[0]);
   return EVAL_SUCCESS;
}

int query_hunger_level()
{
   return hunger_level;
}

int hungry()
{
   return hunger_level > 30;
}

int desperately_hungry()
{
   return hunger_level > 70;
}

string hunger_features()
{
   return "<039>Hunger\n<res>" + "\t- Become more hungry\n\n";
}