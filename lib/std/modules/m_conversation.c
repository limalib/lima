/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * By Beek.  Merged in some functionality written by Valentino.
 *
 * Extended by Tsath to check for stats and skill ranks before options are shown.
 */

inherit M_ACTIONS;
inherit M_INPUT;

private
mapping stats = (["str":"Strength", "agi":"Agility", "int":"Intelligence", "con":"Constitution", "cha":"Charisma"]);

void simple_action(string);
string query_name();

mapping options = ([]);
mapping responses = ([]);

mapping current = ([]);
mapping start = ([]);
mixed *default_start = ({});
mixed goodbye_action;

void set_goodbye(mixed arg)
{
   goodbye_action = arg;
}

private
int check_option(string key)
{
   string stat_chk, s;
   int req;
   s = options[key];

   if (sscanf(s, "%s[%s>%d]", s, stat_chk, req) == 3)
   {
      if (strlen(stat_chk) == 3)
      {
         int stat = call_other(this_body(), "query_" + stat_chk);
         if (req > stat)
            return 0;
         s = "[<229>" + stats[stat_chk] + "<res>] " + s;
      }
      else
      {
         int skill_rank = SKILL_D->skill_rank(this_body(), stat_chk);
         if (req > skill_rank)
            return 0;
         s = "[<209>" + explode(stat_chk,"/")[<1] + "<res>] " + s;
      }
      options[key] = s;
      return 1;
   }
   else if (sscanf(s, "%s[%s<%d]", s, stat_chk, req) == 3)
   {
      if (strlen(stat_chk) == 3)
      {
         int stat = call_other(this_body(), "query_" + stat_chk);
         if (req < stat)
            return 0;
         s = "[<229>" + stats[stat_chk] + "<res>] " + s;
         options[key] = s;
         return 1;
      }
      else
      {
         int skill_rank = SKILL_D->skill_rank(this_body(), stat_chk);
         if (req < skill_rank)
            return 0;
         s = "[<209>" + explode(stat_chk,"/")[<1] + "<res>] " + s;
      }
   }
   return 1;
}

void set_options(mapping m)
{
   options = m;
}

void set_responses(mapping m)
{
   responses = m;
}

varargs void set_start(mixed *a, object target)
{
   if (target)
      start[target] = a;
   else
      default_start = a;
}

mixed direct_talk_to_liv()
{
   return 1;
}

mixed direct_talk_with_liv()
{
   return 1;
}

void continue_conversation(object, string);

void show_menu(object ob)
{
   int n = 1;

   write("\n");

   foreach (string option in current[ob])
   {
      if (check_option(option))
         printf("   %%^MENU_CHOICE%%^%2d%%^RESET%%^: %s\n", n++, options[option]);
   }
   printf("    %%^MENU_CHOICE%%^q%%^RESET%%^: Quit talking to " + query_name() + ".\n");

   modal_simple(( : continue_conversation, ob:), "[choice] :>");
}

void do_action(object ob, mixed action)
{
   string add;
   string remove;

   if (arrayp(action))
      foreach (mixed act in action)
         do_action(ob, act);
   else
   {
      if (functionp(action))
         evaluate(action);
      else if (stringp(action))
      {
         if (sscanf(action, "%s@@%s", action, add) == 2)
         {
            if (sscanf(add, "%s@@%s", add, remove) == 2)
               current[ob] -= explode(remove, ",");
            current[ob] += explode(add, ",");
         }

         if (action[0] == '!')
            do_game_command(action[1..]);
         else
            do_game_command("say " + action);
      }
      else
      {
         if (wizardp(ob))
         {
            if (!action)
               write("Error: no response found\n");
            else
               write("Error: invalid type response\n");
         }
         simple_action("$N $vare not sure what to say in response to that.");
         show_menu(ob);
      }
   }
}

void bye(object ob)
{
   map_delete(current, ob);

   if (goodbye_action)
      do_action(ob, goodbye_action);
}

void continue_conversation(object ob, string input)
{
   int num;
   string tmp;
   string response;
   string option;
   string tag;

   if (input == "q")
   {
      return bye(ob);
   }
   if (sscanf(input, "%d%s", num, tmp) != 2 || tmp != "")
   {
      simple_action("$N $vsuggest that you use the numbers to " + "indicate what you want to say.");
      show_menu(ob);
      return;
   }
   if (num <= 0 || num > sizeof(current[ob]))
   {
      simple_action("$N $vsuggest that you pick a legal option.");
      show_menu(ob);
      return;
   }
   num--;
   tag = current[ob][num];
   //Strip away [Blargh] tags infront of options.
   if (sscanf(options[tag], "[%s] %s", tmp, option) != 2)
      option = options[tag];
   ob->simple_action("$N $vsay: " + option);
   response = responses[tag];

   do_action(ob, response);

   current[ob] -= ({tag});

   if (sizeof(current[ob]))
      show_menu(ob);
   else
      return bye(ob);
}

void begin_conversation()
{
   current[this_body()] = start[this_body()] || default_start;

   show_menu(this_body());
}
