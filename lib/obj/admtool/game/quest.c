/* Do not remove the headers from this file! see /USAGE for more info. */

inherit "/obj/admtool/internal/base.c";

nomask string module_name()
{
   return "quest";
}

nomask string module_key()
{
   return "q";
}

private
nomask void list_quests()
{
   more(QUEST_D->quest_dump());
}

private
nomask void get_milestone(string domain, string quest_name, int value, string file, string s)
{
   if (!stringp(s) || !strlen(s))
   {
      s = 0;
   }
   QUEST_D->add_quest(domain, quest_name, value, file, s);

   printf("Quest %s added to domain '%s'.\n", quest_name, domain);
}

private
nomask void get_master(string domain, string quest_name, int value, string master)
{
   string file = evaluate_path(master);

   if (!is_file(file) && !is_file(file + ".c"))
   {
      write("That file doesn't exist.\n");
      return;
   }

   modal_simple((
                    : get_milestone, domain, quest_name, value, file:),
                "If this is a quest endpoint, give a one line description of the quest for\nplayers, or else hit enter "
                "(and nothing else!): ");
}

private
nomask void get_value(string domain, string quest_name, string pts_as_string)
{
   int value = to_int(pts_as_string);
   printf("Quest %s' value is set at %d points.\n", quest_name, value);
   modal_simple(( : get_master, domain, quest_name, value:), "What file can dole out these points? ");
}

private
nomask void get_quest_name(string domain, string quest_name, string qname)
{
   printf("Quest %s' name is set to %s.\n", quest_name, qname);
   QUEST_D->set_quest_name(domain, quest_name, qname);
}

private
nomask void add_quest(string domain, string name)
{
   if (!stringp(name) || !strlen(name))
   {
      write("Not a valid quest name.\n");
      return;
   }
   if (QUEST_D->quest_exists(name))
   {
      write("That quest already exists.\n");
      return;
   }

   modal_simple(( : get_value, domain, name:), "How many points is this quest item worth? ");
}

private
nomask void add_quest_name(string domain, string name)
{
   if (!stringp(name) || !strlen(name))
   {
      write("Not a valid quest name.\n");
      return;
   }
   if (QUEST_D->quest_exists(name))
   {
      write("That quest already exists.\n");
      return;
   }

   modal_simple(( : get_quest_name, domain, name:), "What name should players see? ");
}

private
nomask void add_quest_domain(string domain)
{
   if (!stringp(domain) || !strlen(domain))
   {
      write("Not a valid domain name.\n");
      return;
   }

   modal_simple(( : add_quest, domain:), "Format: [quest]:[taskname], example 'pirate:foundCave'.\nWhat quest/task name?");
}

private
nomask void quest_name(string domain)
{
   if (!stringp(domain) || !strlen(domain))
   {
      write("Not a valid domain name.\n");
      return;
   }

   modal_simple(( : add_quest_name, domain:), "What quest name? (base part excluding :part)  ");
}

private
nomask void remove_quest(string domain, string name)
{
   if (!QUEST_D->delete_quest(domain, name))
   {
      write("No such quest.\n");
      return;
   }
   else
   {
      write("Deleted.\n");
   }
}

private
nomask void remove_quest_domain(string domain)
{
   modal_simple(( : remove_quest, domain:), "Which quest? ");
}

private
dump_domain(string domain)
{
   more(QUEST_D->dump_final_goals(domain));
}

nomask class command_info *module_commands()
{
   return ({new (class command_info, key
                 : "l", desc
                 : "list all places to get points", action
                 : (
                     : list_quests:)),
            new (class command_info, key
                 : "L", proto
                 : "[domain]", desc
                 : "list all quests for domain", args
                 : ({"Which domain? "}), action
                 : (
                     : dump_domain:)),
            new (class command_info, key
                 : "a", proto
                 : "[domain]", desc
                 : "add quest points for a goal", args
                 : ({"Which domain? "}), action
                 : (
                     : add_quest_domain:)),
            new (class command_info, key
                 : "r", proto
                 : "[domain]", desc
                 : "remove quest points for a goal", args
                 : ({"Which domain? "}), action
                 : (
                     : remove_quest_domain:)),
            new (class command_info, key
                 : "n", proto
                 : "[domain]", desc
                 : "add quest name", args
                 : ({"Which domain? "}), action
                 : (
                     : quest_name:))});
}