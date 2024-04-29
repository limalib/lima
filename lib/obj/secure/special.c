/* Do not remove the headers from this file! see /USAGE for more info. */

/*
**
** Special commands used by guilds.
** Tsath 2020.
**
*/

inherit CMD;

int reflex_cost = 1;
int starts_fight = 0;
int target_required = 0;
int obj_required = 0;
string name;
string other_skill_name;
int other_skill_rank;

void set_name(string n)
{
   name = n;
}

string query_name()
{
   return name;
}

int query_reflex_cost()
{
   return reflex_cost;
}

void set_other_skill(string on, int or)
{
   other_skill_name = on;
   other_skill_rank = or ;
}

int query_target_required()
{
   return target_required;
}

void set_target_required(int tr)
{
   target_required = tr;
}

int query_obj_required()
{
   return obj_required;
}

void set_obj_required(int or)
{
   obj_required = or ;
}

void set_starts_fight(int sf)
{
   starts_fight = sf;
}

void set_reflex_cost(int c)
{
   reflex_cost = c;
}

void setup()
{
}

void special(object attacker, object target)
{
   // Override me
   write("Nothing really.\n");
}

string skill_used()
{
   return "combat/special/" + name;
}

varargs int skill_rank(object b, string s)
{
   return SKILL_D->skill_rank(b, s || skill_used());
}

private
object *targets(object adv)
{
   return adv->query_targets() - ({0});
}

void check_config()
{
   // If not this file, do special config checks for the special abilities.
   if (!name && file_name(this_object()) + ".c" != __FILE__)
   {
      write("* Missing special name\n");
      // Add more here
   }
}

int check_reflex(object b)
{
   return b->query_reflex() >= reflex_cost;
}

int spend_reflex(object b)
{
   return b->use_reflex(reflex_cost);
}

int skill_check(object attacker, object target)
{
   int rank = skill_rank(attacker);
   string attack_skill = skill_used();
   string defend_skill = target->query_defend_skill_used();
   int defend_value = target->aggregate_skill(defend_skill);
   int sc;

   sc = attacker->test_skill(skill_used(), defend_value);
   return sc;
}

private
deferred_start_fight(object adv, object target)
{
   adv->start_fight(target);
}

int special_check(object attacker, object target)
{
   // Override me.
   return 1;
}

private
void main(string arg)
{
   object b = this_body();
   object selected_target;

   // We cannot be called without checking rank of other skill
   if (other_skill_rank && other_skill_name)
   {
      if (skill_rank(b, other_skill_name) < other_skill_rank)
      {
         write("You cannot use " + name + " before you are rank " + other_skill_rank + " in " + other_skill_name +
               ".\n");
         return;
      }
   }

   if (!special_check(b, selected_target))
      return;

   // Target aquired at this point, must check reflex last since we spend reflex
   // points when w check for them
   if (!check_reflex(b))
   {
      write("You do not have " + reflex_cost + " reflex points to do this.\n");
      return;
   }

   // No argument, and we have no target and it is required.
   if (!arg && target_required && !sizeof(targets(b)))
   {
      write("Use " + name + " on whom?\n");
      return;
   }
   // No argument, and we have no obj and it is required.
   if (!arg && obj_required && !sizeof(targets(b)))
   {
      write("Use " + name + " on what?\n");
      return;
   }
   // No argument, target required and we have targets
   else if (!arg && target_required && sizeof(targets(b)))
   {
      selected_target = choice(targets(b));
   }
   // No argument, obj required and we have targets
   else if (!arg && obj_required && sizeof(targets(b)))
   {
      selected_target = choice(targets(b));
   }
   // Argument and target required
   else if (arg && target_required)
   {
      if (present(arg, environment(b)))
      {
         selected_target = present(arg, environment(b));
         if (selected_target->attackable() && selected_target != b)
         {
            if (starts_fight)
               defer(( : deferred_start_fight, b, selected_target:));
         }
         else
         {
            if (selected_target == b)
               write(capitalize("You cannot attack yourself.\n"));
            else
               write(capitalize(selected_target->short()) + " cannot be attacked.\n");
            selected_target = 0;
            return;
         }
      }
      else
      {
         write("There is no " + arg + " here.\n");
         return;
      }
   }
   else if (arg && obj_required)
   {
      if (present(arg, environment(b)))
      {
         selected_target = present(arg, environment(b));
         if (selected_target->attackable())
         {
            write("You cannot use that on living things.\n");
         }
         else if (selected_target == b)
            write("You cannot do that to yourself.\n");
      }
      else
      {
         write("There is no " + arg + " here.\n");
         return;
      }
   }

   if (!selected_target)
   {
      TBUG("No target?");
   }

   // Target aquired at this point, must check reflex last since we spend reflex
   // points when w check for them
   if (!spend_reflex(b))
   {
      write("You do not have " + reflex_cost + " reflex points to do this.\n");
      return;
   }

   special(b, selected_target);
}

void create()
{
   setup();
   check_config();
}