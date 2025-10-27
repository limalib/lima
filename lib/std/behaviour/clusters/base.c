/* Do not remove the headers from this file! see /USAGE for more info. */

// Base functionality for behavior_tree
// Emotions inspired by Plitchik's Wheel of Emotions
// https://en.wikipedia.org/wiki/Robert_Plutchik

#include <behaviour.h>
#include <commands.h>

inherit NODE_CLASS;

#ifdef CLUSTER_ASSOCIATION
void init_association_cluster();
string association_features();
#endif
#ifdef CLUSTER_NAVIGATION
void init_navigation_cluster();
string navigation_features();
#endif
#ifdef CLUSTER_EQUIPMENT
void init_equipment_cluster();
string equipment_features();
#endif
#ifdef CLUSTER_HUNGER
void init_hunger_cluster();
string hunger_features();
#endif
#ifdef CLUSTER_COMBAT
void init_combat_cluster();
string combat_features();
#endif

void add_hook(string, function);
void remove_hook(string, function);
object query_target();
object query_link();
varargs int evaluate_node();
void calm_emotions();
string *query_last_failed_leaves();
void reset_last_failed_leaves();

void action_arrival(object);
void action_departure(object);
void register_beings(object *obs);

mapping emotions = ([ECSTASY:0, ADMIRATION:0, TERROR:0, AMAZEMENT:0]);
mapping blackboard = ([]);
mapping node_list, parents;

private function arrival_fn = ( : action_arrival:);
private function departure_fn = ( : action_departure:);

private function my_hook;

private object env;

private int delay_time = 5;   // Default delay time for leaves.
private object debugee = 0;   // The body of the person debugging us.
private string *queue = ({}); // Queue of nodes that must be processed.

private object *arrived = ({});
private object *left = ({});
private int behaviour_call_out;

private int room_has_changed = 1;

int has_room_changed()
{
   return room_has_changed;
}

void room_checked()
{
   room_has_changed = 0;
}

mixed need_hooks()
{
   return 1;
}

object query_body()
{
   return this_object();
}

object query_shell_ob()
{
   return this_object();
}

string status(int s)
{
   switch (s)
   {
   case -2:
      return "<226>NEVER RUN<res>";
   case -1:
      return "<135>NONE<res>";
   case 0:
      return "<165>FAILURE<res>";
   case 1:
      return "<046>SUCCESS<res>";
   case 2:
      return "<118>RUNNING<res>";
   case 5:
      return "<009>LAST FAILED<res>";
   }
}

void backpush(mixed node)
{
   if (arrayp(node))
      queue += node;
   else
      queue += ({node});
}

void frontpush(string node)
{
   queue = ({node}) + queue;
}

string print_queue()
{
   string out = "";
   int i = 1;
   return format_list(queue);
}

mapping query_nodes()
{
   return node_list;
}

mapping query_parents()
{
   return parents;
}

// Reset the tree states as well as the queue.
void reset_tree()
{
   foreach (string name, class node node in node_list)
   {
      if (node.status != EVAL_NEVER_RAN)
         node.status = EVAL_NONE;
      node.node_num = 0;
   }
   queue = ({});
   reset_last_failed_leaves();
}

class node front()
{
   return sizeof(queue) ? node_list[queue[0]] : 0;
}

class node parent(class node node)
{
   return node && parents[node.name] ? node_list[parents[node.name]] : 0;
}

class node pop()
{
   class node n = front();
   queue = sizeof(queue) ? queue[1..] : ({});
   return n;
}

string discover_parent(string node)
{
   string parent;
   string *names = keys(node_list);
   int i = 0;
   while (!parent && i < sizeof(names))
   {
      if (member_array(node, node_list[names[i]].children) != -1)
         parent = names[i];
      i++;
   }
   return parent;
}

// Accepts offstring as a single string or as an array of strings.
// If offspring is 0, the array is set to ({}).
varargs void create_node(int type, string name, mixed offspring)
{
   node_list[name] = new(class node, type : type, name : name,
                         children : arrayp(offspring) ? offspring
                         : offspring                  ? ({offspring})
                                                      : ({}),
                         delay : type == NODE_LEAF ? LEAF_NODE_PAUSE : 0, status : -2);
   parents[name] = discover_parent(name);
}

void do_game_command(string str)
{
   object save_tu;
   mixed *winner;
   string verb, argument;

   save_tu = this_user();
   set_this_user(this_object());

   verb = str;
   sscanf(verb, "%s %s", verb, argument);

   winner = CMD_D->find_cmd_in_path(verb, ({CMD_DIR_PLAYER "/"}));
   if (arrayp(winner))
      winner[0]->call_main("", "", "", "", "", "", argument);
   else if (environment())
   {
      mixed result = parse_sentence(str);

      if (stringp(result))
         write(result);
   }

   set_this_user(save_tu);
}

// Return the number of listeners (player in room)
int query_observers()
{
   object *inv;
   if (!env)
      return 0;
   if (inv = all_inventory(env))
      return sizeof(filter(inv, ( : $1->query_link() :)));
   return 0;
}

// The periodic action call_out
void behaviour_call()
{
   if (query_observers() && behaviour_call_out == 0)
      behaviour_call_out = call_out("behaviour_call", delay_time);
   evaluate_node();
   behaviour_call_out = 0;
}

object *living_arrived()
{
   return filter_array(arrived, ( : $1 && $1->is_living() :));
}

object *left()
{
   return left;
}

void person_attacked_person(object attacker, object target)
{
   if (target == this_object())
      register_beings(({attacker}));
}

void something_arrived(object ob)
{
   // Ignore if I spot myself arriving.
   if (ob == this_object())
      return;
   left -= ({ob});
   arrived += ({ob});
}

void something_left(object ob)
{
   // Ignore if I spot myself leaving.
   if (ob == this_object())
      return;
   left += ({ob});
   arrived -= ({ob});
}

// If last player leaves, remove call_out
void action_departure(object who)
{
   room_has_changed = 1;
   something_left(who);
}

// If first player arrives, add call_out
void action_arrival(object who)
{
   room_has_changed = 1;
   something_arrived(who);
   if (who->query_link())
   {
      if (query_observers() == 1 && behaviour_call_out == 0)
         behaviour_call_out = call_out("behaviour_call", delay_time);
   }
}

// It moves the "object_arrived/left" hooks (which are associated with a room)
// when the action monster moves
void action_movement()
{
   if (env)
   {
      env->remove_hook("object_arrived", arrival_fn);
      env->remove_hook("object_left", departure_fn);
   }

   env = environment();
   env->add_hook("object_arrived", arrival_fn);
   env->add_hook("object_left", departure_fn);
   if (behaviour_call_out == 0 && query_observers())
      behaviour_call_out = call_out("behaviour_call", delay_time);
}

int debugging()
{
   return objectp(debugee);
}

void debug(mixed s)
{
   if (debugee && debugee->is_body())
      tell(debugee, sprintf("%s: %O\n", obname(this_object()), (s)));
}

void debug_me(int i)
{
   debugee = i ? this_body() : 0;
}

void set_blackboard(string key, mixed value)
{
   blackboard[key] = value;
}

void add_to_blackboard(string key, mixed value)
{
   if (blackboard[key])
   {
      if (arrayp(blackboard[key]))
      {
         if (member_array(value, blackboard[key]) == -1)
            blackboard[key] += ({value});
      }
      else
         blackboard[key] = ({value});
   }
   else
   {
      blackboard[key] = ({value});
   }
}

int is_on_blackboard(string key, mixed value)
{
   if (!blackboard[key])
      return 0;
   if (arrayp(blackboard[key]))
      return member_array(value, blackboard[key]) != -1;
   return blackboard[key] == value;
}

void del_blackboard(string key)
{
   map_delete(blackboard, key);
}

mixed blackboard(string key)
{
   return blackboard[key];
}

mapping query_blackboard()
{
   return blackboard;
}

mapping query_emotions()
{
   return emotions;
}

string emotion_string()
{
   string *ems = ({});
   foreach (int emotion, int level in emotions)
   {
      if (!level)
         continue; // Don't show emotions that are not present.
      // "boredom","disgust","loathing","","acceptance","trust","admiration"
      //   0          1           2      3     4          5        6
      ems += ({EMOTION_NAMES[emotion][level + 3]});
   }

   if (!sizeof(ems))
      ems = ({"indifference"});
   return (sizeof(ems) > 1 ? "a mixture of " : "") + format_list(ems);
}

mapping emotion_names()
{
   return EMOTION_NAMES;
}

private void raw_mod_emotion(int emotion, int mod)
{
   emotions[emotion] = clamp(emotions[emotion] + mod, -3, 3); // ## Correct now?
}

//: FUNCTION neutralize_emotion
// Neutralize an emotion by one point. This is useful for
// calming down an NPC.
void neutralize_emotion(int emotion)
{
   if (emotion < 0 || emotion > 7)
      error("Unknown emotion state '" + emotion + "'. States defined in /include/behaviour.h.\n" +
            format_list(EMOTION_STRINGS));
   if (emotion > 3)
      emotion -= 4;

   if (emotions[emotion] > 0)
      emotions[emotion] -= 1;
   else
      emotions[emotion] += 1;
}

//: FUNCTION mod_emotion
// Function for modifying the behavior of the brainee.
// Examples::
//    orc->mod_emotion("rage",3);
//    orc->mod_emotion("ter",2);
//
// Are valid examples. Emotion names can be abbreviated to first 3 letters, or you
// can use integers as per defined in behaviour.h.
void mod_emotion(int emotion, int mod)
{
   // Some guard rails
   if (emotion < 0 || emotion > 7)
      error("Unknown emotion state '" + emotion + "'. States defined in /include/behaviour.h.\n" +
            format_list(EMOTION_STRINGS));
   if (emotion > 3)
   {
      emotion -= 4;
      mod = -mod;
   }
   if (mod < -3 || mod > 3)
      error("Can only modify emotions -3 - 3 points.");

   raw_mod_emotion(emotion, mod);
}

//: FUNCTION set_emotion
// Function for setting a specific emotion. This sets the emotion to this value and doesn't raise it above
// the given value.
// Examples::
//    orc->set_emotion("rage",3);
//    orc->set_emotion("ter",2);
//
// Are valid examples. Emotion names can be abbreviated to first 3 letters, or you
// can use integers as per defined in behaviour.h.
void set_emotion(int emotion, int value)
{
   int current_val = emotions[emotion];
   value = value - current_val;
   mod_emotion(emotion, value);
}

int im_bored()
{
   return array_sum(values(emotions)) <= 2;
}

int query_emotion(int emotion)
{
   return emotions[emotion];
}

void calm_emotions()
{
   for (int i = 0; i < 8; i++)
      raw_mod_emotion(i, -1);
}

void add_child(string node, string child)
{
   if (!child)
      error("Cannot add empty child.");
   if (!arrayp(node_list[node].children))
      node_list[node].children = ({child});
   else
      node_list[node].children += ({child});
   parents[child] = node;
}

private void init_tree()
{
   node_list = ([]);
   parents = ([]);

   // Mandatory root node
   create_node(NODE_ROOT, "root", "root_sequence");
   create_node(NODE_SEQUENCE, "root_sequence");

   // The order of these matters.
#ifdef CLUSTER_ASSOCIATION
   init_association_cluster();
#endif
#ifdef CLUSTER_EQUIPMENT
   init_equipment_cluster();
#endif
#ifdef CLUSTER_HUNGER
   init_hunger_cluster();
#endif
#ifdef CLUSTER_COMBAT
   init_combat_cluster();
#endif
#ifdef CLUSTER_NAVIGATION
   init_navigation_cluster();
#endif
}

void start_behaviour()
{
   if (!node_list)
      init_tree();
   add_hook("move", ( : action_movement:));
   if (env = environment(this_object()))
   {
      env->add_hook("object_arrived", arrival_fn);
      env->add_hook("object_left", departure_fn);
   }

   if (query_observers())
      behaviour_call_out = call_out("behaviour_call", delay_time);
}

void stop_behaviour_call()
{
   remove_call_out(behaviour_call_out);
}

string base_features()
{
   return "<051>Base functionality: \n<res>" + "\t- Emotions\n" + "\t- Basic call out\n" +
          "\t- Establish root sequence\n\n";
}

string features()
{
   return "<bld>Behaviour tree functionality loaded.<res>\n\n" + base_features() +
#ifdef CLUSTER_ASSOCIATION
          association_features() +
#endif
#ifdef CLUSTER_NAVIGATION
          navigation_features() +
#endif
#ifdef CLUSTER_EQUIPMENT
          equipment_features() +
#endif
#ifdef CLUSTER_HUNGER
          hunger_features() +
#endif
#ifdef CLUSTER_COMBAT
          combat_features() +
#endif
          "";
}

string node_type(int type)
{
   switch (type)
   {
   case NODE_ROOT:
      return "<190>ROOT<res>";
   case NODE_SEQUENCE:
      return "<214>SEQUENCE<res>";
   case NODE_SELECTOR:
      return "<206>SELECTOR<res>";
   case NODE_LEAF:
      return "<118>LEAF<res>";
   case NODE_SUCCEEDER:
      return "<070>SUCCEEDER<res>";
   case NODE_INVERTER:
      return "<015>INVERTER<res>";
   case NODE_REPEAT_UNTIL_FAIL:
      return "<134>REPEAT UNTIL FAIL<res>";
   default:
      return "<171>UNKNOWN<res>";
   }
}

private varargs string print_children(class node node, int level, int has_siblings)
{
   string out = "";
   int last_child = 0;
   int i = 0;
   foreach (string name in node.children)
   {
      class node child = node_list[name];
      string bend;
      i++;
      if (i == sizeof(node.children))
         last_child = 1;
      bend = last_child ? "└" : "├";
      if (classp(child) && sizeof(child.children))
         bend = "└";
      else if (!classp(child))
      {
         out += "\n<161>Node <043>" + name + "<161> not correctly in tree<res>";
         continue;
      }

      out += sprintf("\n%-20.20s %s%s %s", node_type(child.type),
                     repeat_string(" ", level * 2) + "<050>" + bend + "─ <res>", name,
                     member_array(child.name, query_last_failed_leaves()) != -1 ? status(5) : status(child.status));

      if (sizeof(child.children))
         out += print_children(child, level + 1, sizeof(node.children) > 1);
   }
   return out;
}

void print_tree()
{
   string out = "";

   out = node_type(NODE_ROOT) + "       <res>root";

   out += print_children(node_list["root"]);

   printf("%s\n", out);
}