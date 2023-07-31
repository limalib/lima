/* Do not remove the headers from this file! see /USAGE for more info. */

/* Skills command by Tsath */

//: PLAYERCOMMAND
//$$ see: hp, stats, score, spells
// USAGE: skills
//
// Prints out a list of your skills and skill ranks.
// - Skill ranks are shown by roman numerals (the higher the better from 0-15)
// - Training points are shown at the end of the skill bars (in yellow)
//
// Optional arguments are: combat, magic or misc to only show a subsection
// of the skill tree.
//
// The more you use your skills, the better you get. So things get better all
// the time! When you have a lot of training points, search for a trainer to
// train you. They both need to have the right skills and at the right skill
// rank to train you.

#include <classes.h>
#include <config.h>

inherit CMD;
inherit CLASS_SKILL;
inherit M_WIDGETS;
inherit M_FRAME;

private
void main(string arg)
{
   mapping skills = this_body()->query_skills();
   int width = this_user()->query_screen_width() - 6;
   int skill_bar = width - 35;
   string barchar = uses_unicode() ? "▅" : "=";
   string nobarchar = uses_unicode() ? "▅" : ".";
   string bend = uses_unicode() ? "└" : " ";
   string contbend = uses_unicode() ? "├" : " ";
   string content;
   string *names;
   object target;
   int self_view;

   if (strlen(arg) > 0 && wizardp(this_user()))
   {
      target = present(arg, environment(this_body()));
      if (!target)
         target = find_body(arg);
      if (!target)
      {
         out("Cannot find '" + arg + "'.\n");
         return;
      }
      out("Skills for " + capitalize(arg) + ":\n");
      self_view = 0;
      arg = 0;
   }

   if (strlen(arg) && arg != "combat" && arg != "misc" && !target)
   {
      out("Valid arguments are: combat or misc.\n");
      return;
   }

   if (!target)
      target = this_body();

   skills = target->get_skills() || target->query_skills();

   if (sizeof(skills) == 0)
   {
      if (target == this_body())
         out("You have no skills yet.\n");
      else
         out(target->short() + " has not skills yet.\n");
      return;
   }
   names = sort_array(keys(skills), 1);

   if (!i_simplify())
   {
      int i = 0;
      while (i < sizeof(names))
      {
         string name = names[i];
         class skill skill = skills[name];
         string *parts = explode(name, "/");
         int level = sizeof(parts);
         int next_level = (i + 1) < sizeof(names) ? sizeof(explode(names[i + 1], "/")) : 0;
         string name2 = repeat_string("   ", sizeof(parts) - 1) + parts[ < 1];
         string pretty_name =
             target->is_body() ? SKILL_D->skill_rank_pretty(target, name) : SKILL_D->monster_skill_rank_pretty(target, name);
         int percentage = target->is_body() ? SKILL_D->percent_for_next_rank(target, name)
                                          : SKILL_D->monster_percent_for_next_rank(target, name);
         int green = skill_bar * percentage / 100;
         int red = skill_bar - green;
         frame_init_user();
         if (strlen(arg) && strsrch(name, arg) != 0)
            continue;

         if (level == 1)
         {
            if (content)
            {
               set_frame_content(content);
               out(frame_render());
            }
            content = "";
            set_frame_title(pretty_name);
         }
         else if (percentage || target->is_body())
            content += sprintf("%-25s %4s [<040>%s<238>%s<res>] %-5s\n",
                               repeat_string(" " + (level == next_level ? contbend : bend), level - 2) + pretty_name,
                               percentage + "%", repeat_string(barchar, green), repeat_string(nobarchar, red),
                               target->is_body() ? accent(skill.training_points) : "", );
         i++;
      }
      if (content)
      {
         set_frame_content(content);
         out(frame_render());
      }
   }
   else
   {
      outf("%-30s %-12s  %-8s\n", "Skill name", "Progress", "Training points");
      foreach (string name in names)
      {
         class skill skill = skills[name];
         int percentage = SKILL_D->percent_for_next_rank(this_body(), name);
         string *parts = explode(name, "/");
         string name2 = repeat_string("   ", sizeof(parts) - 1) + parts[ < 1];
         if (strlen(arg) && strsrch(name, arg) != 0)
            continue;

         if (sizeof(parts) == 1)
         {
            outf("%s:\n", SKILL_D->skill_rank_simple(this_body(), name));
         }
         else
            outf("%-30s %-12s  %-8s\n", SKILL_D->skill_rank_simple(this_body(), name), percentage + "%",
                 sizeof(parts) > 1 ? skill.training_points + "" : "-");
      }
   }
}
