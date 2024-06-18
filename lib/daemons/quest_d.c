/* Do not remove the headers from this file! see /USAGE for more info. */

#include <log.h>
#include <mudlib.h>

/*
 * Rewritten and expanded to handle domains, new data structure and more concepts.
 * Tsath 2019-11-09
 */

inherit M_DAEMON_DATA;

mapping quests = (["std":(["pirate:secretPsg":({
                                                  "/domains/std/object/dock_wall",
                                                  5,
                                                  0,
                                                  0,
                                              }),
                                "pirate:treasure":({
                                                      "/domains/std/object/sand_with_treasure",
                                                      10,
                                                      0,
                                                      "Congratulations! You completed the sample Pirate quest.",
                                                  }),
                             " pirate:foundCave ":({
                                                      "/domains/std/room/beach/Sandy_Beach ",
                                                      10,
                                                      0,
                                                      0,
                                                  }),
]),
]);
mapping quest_names = ([]);
nosave mapping d_quests = ([]); // Temp storage
nosave int total_points;

#define PRIV_REQUIRED "Mudlib:daemons"
#define Q_BASE 0
#define Q_VALUE 1
// #define Q_SOLVED 2
#define Q_MSTONE 2
#define QP_TO_XP_RATIO 10

private
void calculate_total_points()
{
   int i;
   string *domains;
   string *keys;

   total_points = 0;

   domains = keys(quests);
   foreach (string domain in keys(quests))
   {
      keys = keys(quests[domain]);
      i = sizeof(keys);
      while (i--)
         total_points += quests[domain][keys[i]][Q_VALUE];
   }
}

void set_quest_name(string domain, string main, string name)
{
   if (!quest_names[domain])
      quest_names[domain] = ([main:name]);
   else
      quest_names[domain][main] = name;
   save_me();
}

string query_quest_name(string domain, string main)
{
   if (quest_names[domain] && stringp(quest_names[domain][main]))
      return quest_names[domain][main];
   return main;
}

void remove_quest_name(string domain, string main)
{
   map_delete(quest_names[domain], main);
   if (sizeof(keys(quest_names[domain])) == 0)
      map_delete(quest_names, domain);
   save_me();
}

mapping query_quest_names()
{
   return quest_names;
}

int add_quest(string domain, string quest, int value, string base, string major_milestone)
{
   if (!check_privilege(PRIV_REQUIRED))
      return 0;
   if (!quests[domain])
      quests[domain] = ([]);

   if (quests[domain][quest])
   {
      write("Woops, that quest already exists.\n");
      return 0;
   }
   LOG_D->log(LOG_QUEST, sprintf("%-30s worth %d pts, added by %s.\n", base, value, this_body()->query_name()));

   if (base[ < 2..] == ".c")
   {
      base = base[0.. < 3];
   }
   quests[domain][quest] = ({base, value, 0, major_milestone});
   calculate_total_points();
   save_me();
   return 1;
}

int delete_quest(string domain, string quest)
{
   TBUG("Domain: " + domain + " Quest: " + quest);
   if (!check_privilege(PRIV_REQUIRED) || !quests[domain][quest])
      return 0;
   map_delete(quests[domain], quest);
   if (sizeof(keys(quests[domain])) == 0)
      map_delete(quests, domain);
   save_me();
   calculate_total_points();
   return 1;
}

int quest_exists(string d, string q)
{
   if (!quests[d])
      return 0;
   return quests[d][q];
}

mapping query_quests()
{
   return quests;
}

mapping query_domain_quests(string domain)
{
   if (quests[domain])
      return quests[domain];
   return ([]);
}

int begin_quest(object solver, string domain, string main)
{
   string quest = main + ":begin";
   if (!arrayp(quests[domain][quest]))
      return -1;
   if (base_name(previous_object()) != quests[domain][quest][Q_BASE])
      return 0;
   if (!solver->begin_quest(domain, main, quests[domain][quest][Q_VALUE]))
      return -2;

   return 1;
}

int grant_points(object solver, string domain, string main, string task, string prev_task)
{
   string quest = main + (task && sizeof(task) > 0 ? ":" + task : "");
   if (!arrayp(quests[domain][quest]))
      return -1;
   if (base_name(previous_object()) != quests[domain][quest][Q_BASE] && !check_privilege(1))
      return 0;
   if (!solver->update_quest(domain, main, task, prev_task, quests[domain][quest][Q_VALUE]))
      return -2;
   tell(solver, sprintf("\n>>>Quest step completed! You just gained %d XP!<<<\n",
                        QP_TO_XP_RATIO * quests[domain][quest][Q_VALUE]));

   return 1;
}

string *get_goals_for_quests_cmd(string domain)
{
   d_quests = quests[domain];
   return map(filter(keys(d_quests), (
                                         : d_quests[$1][Q_MSTONE]
                                         :)),
              (
                  : sprintf("%s (%s)", $1, d_quests[$1][Q_MSTONE])
                  :));
}

string dump_final_goals(string domain)
{
   string *keys;
   int i;
   string output;
   int total1;
   int total2;

   keys = keys(quests[domain]);
   keys = sort_array(keys, -1);

   i = sizeof(keys);
   output = "";
   while (i--)
   {
      if (quests[domain][keys[i]][Q_MSTONE])
      {
         output += sprintf("%-17s.%-2d pts, %s\n", keys[i], quests[domain][keys[i]][Q_VALUE],
                           quests[domain][keys[i]][Q_BASE]);
         total1 += quests[domain][keys[i]][Q_VALUE];
      }
      total2 += quests[domain][keys[i]][Q_VALUE];
   }

   output += sprintf("\nTotal points: %d in main goals, %d total\n", total1, total2);

   return output;
}

// Dumps all the quest items, and not just the final goals
string quest_dump(string fname)
{
   string *keys;
   string *quest_ar;
   int i;
   string output = "";
   int total;
   string quest;
   string *domains;
   string task;

   domains = keys(quests);
   domains = sort_array(domains, -1);

   foreach (string d in domains)
   {
      keys = keys(quests[d]);
      keys = sort_array(keys, -1);

      i = sizeof(keys);
      output +=
          "Domain: " + d + "\n----------------------------------------------------------------------------------\n";
      output += sprintf("%-15s  %-15s %-6s %s\n", "Quest", "Task", "Points", "File");

      while (i--)
      {
         quest_ar = explode(keys[i], ":");
         quest = quest_ar[0];
         task = (sizeof(quest_ar) > 1 ? quest_ar[Q_VALUE] : "");
         output += sprintf("%-15s  %-15s %-6s %s\n", "" + quest, "" + task, "" + quests[d][keys[i]][Q_VALUE],
                           "" + replace_string(quests[d][keys[i]][Q_BASE], "/domains/", "^"));
         total += quests[d][keys[i]][Q_VALUE];
      }
      output += "\n";
   }
   output += sprintf("\nTotal points: %d\n", total);

   if (fname)
      write_file(fname, output, 1);

   return output;
}

string show_quest(string domain, string quest_name)
{
   if (!quests[domain][quest_name])
      return sprintf("%s: no such quest milestone in domain '%s'.\n", quest_name, domain);

   return sprintf("%-20s %-20s.%-2d points, home: %O", domain, quest_name, quests[domain][quest_name][Q_VALUE],
                  quests[domain][quest_name][Q_BASE]);
}

void create()
{
   ::create();
   if (!quests)
      quests = ([]);
   calculate_total_points();
}

int total_points()
{
   return total_points;
}
