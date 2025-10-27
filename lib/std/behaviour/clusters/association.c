/* Do not remove the headers from this file! see /USAGE for more info. */

#include <behaviour.h>

// Associations towards other livings

void add_child(string node, string child);
varargs void create_node(int type, string name, mixed offspring);
void set_blackboard(string key, mixed value);
void del_blackboard(string key);
int is_on_blackboard(string key, mixed value);
void add_to_blackboard(string key, mixed value);
mixed blackboard(string key);
void do_game_command(string str);
int has_room_changed();
void room_checked();
void mod_emotion(mixed emotion, int mod);
void neutralize_emotion(mixed emotion);
void set_emotion(mixed emotion, int mod);
object *living_arrived();
object *left();

private mapping assoc = ([]);
private int will_attack_friendly = 0;

// The init function *must* create a node that is adds to the root sequence,
// e.g.
//    create_node(NODE_SEQUENCE, "my_seq", ({"step1", "step2"}));
//    add_child("root_sequence", "my_seq");
// The root of the tree contains the "root_sequence" that is run at the top of
// the tree.
void init_association_cluster()
{
   // Add nodes
   create_node(NODE_SEQUENCE, "association_seq", ({"cool_down", "look_around", "own_status", "study_beings"}));
   add_child("root_sequence", "association_seq");
   create_node(NODE_LEAF, "cool_down");
   create_node(NODE_LEAF, "look_around");
   create_node(NODE_LEAF, "study_beings");
   create_node(NODE_LEAF, "own_status");
}

void set_will_attack_friendly(int value)
{
   will_attack_friendly = value;
   TBUG(will_attack_friendly);
}

string attitude_to_string(int a)
{
   switch (a)
   {
   case 2:
      return "<190>Friendly<res>";
   case 1:
      return "<190>Positive<res>";
   case 0:
      return "<190>Indifferent<res>";
   case -1:
      return "<190>Suspicious<res>";
   case -2:
      return "<190>Hostile<res>";
   }
}

mapping query_assoc()
{
   return assoc;
}

// Threat level <0 are threats, >0 = likely not threats
void recalc_threat_level(object ob)
{
   int level_diff = ob->query_level() - this_object()->query_level();

   // This handles if attitude is positive, we treat them as non-threats even
   // if higher levels. It's called trust.
   assoc[ob]["threat_level"] = assoc[ob]["attitude"] > 0 ? abs(level_diff) : level_diff * assoc[ob]["attitude"];
}

object *worst_threats()
{
   if (!sizeof(keys(assoc)))
      return ({});
   map_delete(assoc, 0);
   return sort_array(keys(assoc), ( : assoc[$1]["threat_level"] :));
}

object *worst_threats_present()
{
   return filter(worst_threats(), ( : $1 && present($1, environment()) :));
}

void register_beings(object *obs)
{
   foreach (object ob in obs)
   {
      string id = ob->query_id()[0];
      // Things we only evaluate first time we meet.
      if (!assoc[ob])
      {
         assoc[ob] = ([]);

         assoc[ob]["attitude"] = this_object()->association_for(ob);
         assoc[ob]["level"] = ob->query_level();
         recalc_threat_level(ob);

         // Check if know something about this kind of being.
         // If we have previously been surprised attacked by this being/id,
         // set our attitude to suspicious. If it is attacking me again right now,
         // then we set it to hostile.
         if (blackboard(id))
         {
            if (is_on_blackboard(id, "hostile"))
               assoc[ob]["attitude"] = HOSTILE;
            else if (is_on_blackboard(id, "surprise attacks"))
            {
               assoc[ob]["attitude"] = SUSPICIOUS;
               if (this_object()->is_attacking_me(ob))
               {
                  // A few sample commands to use when surprised attacked.
                  do_game_command(choice(({"scream", "yell " + id, "snapout " + id})));
                  add_to_blackboard(id, "hostile");
               }
            }
         }
      }

      // Things we evaluate every time.

      // Are we being attacked by this person?
      if (this_object()->is_attacking_me(ob))
      {
         int surprise_attack = 0;
         // If we were not expecting this person to attack us, we are surprised.
         if (assoc[ob]["attitude"] >= 0)
         {
            surprise_attack = 1;
            do_game_command("surprised");
            add_to_blackboard(id, "surprise attacks");
         }

         switch (assoc[ob]["threat_level"])
         {
         case -20.. - 5:
            mod_emotion(TERROR, 2);
            break;
         case -4.. - 1:
            mod_emotion(TERROR, 1);
            break;
         case 0..5:
            mod_emotion(RAGE, 1);
            break;

         case 6..10:
            mod_emotion(RAGE, 2);
            break;
         case 11..1000:
            mod_emotion(RAGE, 3);
            break;
         }
         assoc[ob]["attitude"] = HOSTILE;
      }

      recalc_threat_level(ob);
   }
}

void get_all_from_corpse()
{
   do_game_command("get all from corpse");
}

void register_kill(object mykill, int xp)
{
   string id = mykill->query_id()[0];
   add_to_blackboard(id, "killed");
   neutralize_emotion(TERROR);
   neutralize_emotion(RAGE);
   mod_emotion(ECSTASY, 1);
   mod_emotion(ADMIRATION, 1);
   call_out("get_all_from_corpse", 1 + random(2));
}

void i_met(object who)
{
   TBUG(who);
}

int attitude(object ob)
{
   // We don't know this person.
   if (!assoc[ob])
      return INDIFFERENT;
   return assoc[ob]["attitude"];
}

// Default behavior.  Livings that want to behave differently should
// override this.
mixed indirect_give_obj_to_liv(object ob, object liv)
{
   if (attitude(ob) == HOSTILE)
      return capitalize(liv->the_short()) + " refuses.\n";
   return 1;
}

void clean_assoc()
{
   assoc = filter_mapping(assoc, ( : $1:));
}

varargs void threat_emotions(object *people, int left)
{
   int total_threat = 0;
   foreach (object p in people)
   {
      if (p && !p->is_visible())
         continue;
      if (assoc[p])
         total_threat += assoc[p]["threat_level"];
   }
   total_threat = clamp(total_threat, -1000, 1000);
   if (left)
      total_threat = total_threat * -1;

   switch (total_threat)
   {
   case -1000.. - 20:
      mod_emotion(TERROR, 3);
      break;
   case -19.. - 10:
      mod_emotion(TERROR, 2);
      break;
   case -9.. - 1:
      mod_emotion(TERROR, 1);
      break;
   case 0..10:
      mod_emotion(TERROR, -1);
      break;
   case 11..50:
      mod_emotion(TERROR, -2);
      mod_emotion(ADMIRATION, 1);
      break;
   case 51..1000:
      mod_emotion(TERROR, -3);
      mod_emotion(ADMIRATION, 2);
      break;
   }
}

private evaluate_targets(object ob1, object ob2)
{
   if (assoc[ob1]["threat_level"] > assoc[ob2]["threat_level"])
      return -1;
   return 1;
}

object *potential_targets()
{
   object *potentials = ({});
   object *easy_targets = ({});
   if (!sizeof(keys(assoc)))
      return ({});
   map_delete(assoc, 0);
   potentials = keys(assoc);
   potentials = filter(potentials, ( : $1 && present($1, environment()) :));
   potentials = sort_array(keys(assoc), ( : evaluate_targets:), -1);
   easy_targets = filter(potentials, ( : assoc[$1]["attitude"] < 0 && assoc[$1]["threat_level"] > 0 :));
   if (sizeof(easy_targets))
   {
      return easy_targets;
   }
   if (will_attack_friendly && sizeof(potentials))
   {
      potentials = filter(potentials, ( : assoc[$1]["attitude"] >= 0 :));
      return potentials;
   }
   return ({});
}

int own_status()
{
#ifdef HEALTH_USES_LIMBS
   // Limbs code
   mixed *worst_vital = this_object()->query_worst_limb(1);
   mixed *worst_limb = this_object()->query_worst_limb();
   if (worst_vital[0] != "none")
      set_blackboard("worst_vital_limb", worst_vital);
   else
      del_blackboard("worst_vital_limb");
   if (worst_limb[0] != "none")
      set_blackboard("worst_limb", worst_limb);
   else
      del_blackboard("worst_limb");
#else
   // Non limbs
   int hp, max_hp;
   hp = this_object()->query_health();
   max_hp = this_object()->query_max_health();
   set_blackboard("health", (100 * hp) / max_hp);
#endif
   return EVAL_SUCCESS;
}

int study_beings()
{
   object *things = filter_array(all_inventory(environment()) - ({this_object()}),
                                 ( : !$1->is_simple_ob() && $1->is_living() && $1->is_visible() :));
   object *beings, *new_people, *leavers;

   if (!environment())
      return EVAL_FAILURE;

   clean_assoc();

   // If room is the same and we're not in combat, exit here.
   if (!has_room_changed() && !this_object()->is_attacking())
      return EVAL_SUCCESS;

   beings = filter_array(things, ( : $1->is_living() && environment($1) == environment() :));
   new_people = living_arrived();
   leavers = left();

   // Do we have new people?
   if (sizeof(new_people))
   {
      threat_emotions(new_people);
   }
   // Someone left?
   if (sizeof(leavers))
   {
      threat_emotions(leavers, 1);
   }
   // Are we alone? :(
   if (!sizeof(beings))
   {
      this_object()->alone();
      mod_emotion(LOATHING, 1);
   }
   else
   {
      mod_emotion(LOATHING, 0);
      mod_emotion(GRIEF, 1);
   }

   // Tell them that we noticed that they arrived.
   if (sizeof(new_people))
      do_game_command("look at " + choice(new_people)->query_id()[0]);

   register_beings(beings);
   set_blackboard("environment", things - blackboard("exits") - blackboard("gettable") - beings);
   room_checked();

   return EVAL_SUCCESS;
}

int cool_down()
{
   // If we are in combat, we don't want to cool down.
   if (this_object()->is_attacking())
      return EVAL_FAILURE;

   // If we are not in combat, we can cool down.
   if (random(2))
      this_object()->neutralize_emotion(ECSTASY);
   if (random(2))
      this_object()->neutralize_emotion(ADMIRATION);
   if (random(2))
      this_object()->neutralize_emotion(TERROR);
   if (random(2))
      this_object()->neutralize_emotion(AMAZEMENT);

   return EVAL_SUCCESS;
}

int look_around()
{
   object *things =
       filter_array(all_inventory(environment()) - ({this_object()}), ( : !$1->is_simple_ob() && $1->is_visible() :));
   if (!environment())
      return EVAL_FAILURE;
   if (!has_room_changed())
      return EVAL_SUCCESS;
   if (!blackboard("beings"))
      set_blackboard("beings", ({}));

   set_blackboard("gettable", filter_array(things, ( : $1->is_gettable() :)));
   set_blackboard("doors", filter_array(things, ( : $1->id("door") :)));
   set_blackboard("exits", environment()->query_exit_directions(0));
   set_blackboard("hidden_exits", environment()->query_exit_directions(1) - blackboard("exits"));
   set_blackboard("beings",
                  filter_array(things, ( : $1->is_living() && $1->is_visible() && environment($1) == environment() :)));
   set_blackboard("environment", things - blackboard("doors") - blackboard("gettable") - blackboard("beings"));
   return EVAL_SUCCESS;
}

string association_features()
{
   return "<045>Association:\n<res>" + "\t- Relations to other beings\n" + "\t- Basic emotion settings\n" +
          "\t- Assessing threats\n" + "\t- Assess own status\n\n";
}