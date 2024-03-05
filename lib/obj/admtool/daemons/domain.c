/* Do not remove the headers from this file! see /USAGE for more info. */

/* Tsath 2020, for DOMAIN_D control from admtool */

inherit ADMTOOL_BASE;

nomask string module_name()
{
   return "domain";
}

nomask string module_key()
{
   return "d";
}

private
nomask void list_spawn_limits()
{
   mapping sc;
   mapping domain_sc = ([]);

   sc = DOMAIN_D->query_spawn_controls();

   if (!sizeof(keys(sc)))
   {
      write("There are no spawn controls set.\n");
      return;
   }

   foreach (string key, mapping m in sc)
   {
      if (!arrayp(domain_sc[m["domain"]]))
         domain_sc[m["domain"]] = ({});
      domain_sc[m["domain"]] += ({({key, m["max"]})});
   }

   foreach (string dom in keys(domain_sc))
   {
      printf("Spawn controls for: %s:\n------------\n", capitalize(dom));
      foreach (mixed *m in domain_sc[dom])
      {
         printf("%s - max: %d %s\n", m[0], m[1],
                file_size(m[0] + ".c") <= 0 ? "%%^RED%%^(Does not exist!)%%^RESET%%^" : "");
      }
      printf("\n");
   }
}

private
nomask void add_spawn_control2(string dom, string basename, string max)
{
   if (file_size(basename + ".c") < 0)
   {
      write("Error: The file '" + basename + ".c' does not exist.\n");
      return;
   }

   if (!to_int(max) || to_int(max) < 0)
   {
      write("Error: Illegal number for max spawn '" + max + "'.\n");
      return;
   }

   DOMAIN_D->add_spawn_control(dom, basename, to_int(max));
   write("Spawn control added successfully.\n");
}

private
nomask void add_spawn_control(string dom)
{
   if (file_size("/domains/" + dom + "/") != -2)
   {
      write("Error: Non-existant folder, '/domains/" + dom + "/'.\n");
      return;
   }
   input_two_args("Basename of file (without .c)? ", "Max spawns? ", ( : add_spawn_control2, dom:));
}

private
nomask void remove_spawn_control(string basename)
{
   if (file_size(basename + ".c") < 0)
   {
      write("Error: Non-existant file, '" + basename + ".c'.\n");
      return;
   }

   DOMAIN_D->remove_spawn_control(basename);
   write("Removed spawn control for '" + basename + "' if it existed.\n");
}

private
nomask void add_currency2(string dom, string cur)
{
   string *currencies = MONEY_D->query_currency_types();
   if (dom == "list" || member_array(cur, currencies) == -1)
   {
      write("Possible currencies are:\n" + format_list(currencies) + ".\n");
      input_one_arg("What default currency do you want? ('list') ", ( : add_currency2, dom:));
      return;
   }

   DOMAIN_D->set_currency(dom, cur);
   write("Currency set to '" + cur + "' for '" + dom + "'.\n");
}

private
nomask void add_currency(string dom)
{
   if (file_size("/domains/" + dom + "/") != -2)
   {
      write("Error: Non-existant folder, '/domains/" + dom + "/'.\n");
      return;
   }
   input_one_arg("What default currency do you want? ('list') ", ( : add_currency2, dom:));
}

private
nomask void clear_currency(string dom)
{
   if (file_size("/domains/" + dom + "/") != -2)
   {
      write("Error: Non-existant folder, '/domains/" + dom + "/'.\n");
      return;
   }
   DOMAIN_D->remove_currency(dom);
   write("Domain currency cleared.\n");
}

private
nomask void list_currencies()
{
   DOMAIN_D->print_currencies();
}

nomask class command_info *module_commands()
{
   return ({
       new (class command_info, key
            : "l", desc
            : "list spawn limits", action
            : (
                : list_spawn_limits:)),
       new (class command_info, key
            : "a", proto
            : "[domain]", desc
            : "add spawn control for domain", args
            : ({"Which domain? "}), action
            : (
                : add_spawn_control:)),
       new (class command_info, key
            : "r", proto
            : "[basename]", desc
            : "remove spawn control", args
            : ({"Which basename (without .c)? "}), action
            : (
                : remove_spawn_control:)),
       new (class command_info, key
            : "c", proto
            : "[domain]", desc
            : "add domain currency", args
            : ({"Which domain do you want? "}), action
            : (
                : add_currency:)),
       new (class command_info, key
            : "d", proto
            : "[domain]", desc
            : "clear domain currency", args
            : ({"Which domain do you want? "}), action
            : (
                : clear_currency:)),
       new (class command_info, key
            : "L", desc
            : "list currencies", action
            : (
                : list_currencies:)),
   });
}
