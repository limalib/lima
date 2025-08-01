/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * Daemon which serves the purpose of translating synonyms for "go methods"
 * on exits.
 * Written by Tigran@Illusions, Lima Bean   Feb 4, 1999
 */

inherit M_DAEMON_DATA;

#define METHOD_CONFIG_FILE "/data/config/methods"

private mapping methods = ([]);

void load_config_from_file();

//: FUNCTION add_method
// Add a method and a set of equivalents
// Any arguments after the first are equivalents to the method.
void add_method(string method, string *equivs...)
{
   string *add = ({});
   string verb, prep;
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to add_method()");
   if (sscanf(method, "%s %s", verb, prep) == 2)
   {
      /* There is a preposition */
      prep = PREPOSITION_D->translate_preposition(prep);
      if (member_array(prep, PREPOSITION_D->consolidated_preps()) == -1)
         error("Illegal preposition in method");
      method = sprintf("%s %s", verb, prep);
   }
   equivs = clean_array(({method}) + equivs);
   foreach (string test in equivs)
   {
      if (sscanf(test, "%s %s", verb, prep) == 2)
      {
         /* There is a preposition */
         prep = PREPOSITION_D->translate_preposition(prep);
         if (member_array(prep, PREPOSITION_D->consolidated_preps()) == -1)
            error("Illegal preposition in method");
         add += ({sprintf("%s %s", verb, prep)});
      }
      add += ({test});
   }
   methods[method] = clean_array(add);
   save_me();
}

//: FUNCTION remove_method
// Remove a method and it's equivalents
void remove_method(string method)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to remove_method");
   map_delete(methods, method);
   save_me();
}

//: FUNCTION list_methods
// Return an array of all methods which have equivalents
string *list_methods()
{
   return keys(methods);
}

//: FUNCTION add_method_equivalants
// Add an additional equivalents to a given method.
void add_method_equivalents(string method, string *equivs...)
{
   string *add = ({});
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to add_method_equivalents");
   if (member_array(method, keys(methods)) == -1)
      error("Attempted to add equivalents to a nonexistant method");
   foreach (string test in equivs)
   {
      string verb, prep;
      if (sscanf(test, "%s %s", verb, prep) == 2)
      {
         /* There is a preposition */
         prep = PREPOSITION_D->translate_preposition(prep);
         if (member_array(prep, PREPOSITION_D->consolidated_preps()) == -1)
            error("Illegal preposition in method");
         add += ({sprintf("%s %s", verb, prep)});
      }
      add += ({test});
   }

   methods[method] = clean_array(add + methods[method]);
   save_me();
}

//: FUNCTION remove_method_equivalents
// Remove equivalents from a given method
void remove_method_equivalents(string method, string *equivs...)
{
   if (!check_privilege("Mudlib:daemons"))
      error("Insufficient privileges to remove_method_equivalents");
   if (member_array(method, keys(methods)) == -1)
      error("Attempted to remove equivalents from a nonexistant method");
   methods[method] -= equivs;
   if (!sizeof(methods[method]))
      remove_method(method);
   save_me();
}

//: FUNCTION list_method_equivalents
// Return an array of equivalents to a given method
string *list_method_equivalents(string method)
{
   if (member_array(method, keys(methods)) == -1)
      return 0;
   return methods[method];
}

mapping stat_me()
{
   return copy(methods);
}

void load_config_from_file()
{
   string *input;
   methods = ([]);

   if (!sizeof(stat(METHOD_CONFIG_FILE)))
   {
      write("Error: Missing config file '" + METHOD_CONFIG_FILE + "'.");
      return;
   }

   input = explode(read_file(METHOD_CONFIG_FILE), "\n");

   foreach (string line in input)
   {
      string mthd, equiv, *tmpar;

      // Skip comments
      if (line[0] == '#')
      {
         continue;
      }
      else if (sscanf(line, "%s:%s", mthd, equiv) == 2)
      {
         tmpar = explode(equiv, ",");
         tmpar -= ({""});
         tmpar = map(tmpar, ( : trim($1) :));
         add_method(mthd);
         add_method_equivalents(mthd, explode(equiv, ",")...);
      }
      else
      {
         add_method(line);
      }
   }

   write(__FILE__ + ": " + METHOD_CONFIG_FILE + " loaded.");
   write("Saved.");
   save_me();
}

void create()
{
   ::create();
   if (sizeof(keys(methods)) == 0)
   {
      load_config_from_file();
   }
}