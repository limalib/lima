/* Do not remove the headers from this file! see /USAGE for more info. */

// Tsath @ Nuke 2, 2020

//: COMMAND
// USAGE ``killmobs``
//
// Kill mobs by name
//
// .. TAGS: RST

inherit CMD;

void main(string arg)
{

   object *mobs;

   if (!arg || strlen(arg) == 0)
   {
      write("killmobs <id|filename>\n");
      return;
   }

   if (arg[ < 2..] == ".c")
      arg = arg[0.. < 3];
   arg = evaluate_path(arg);

   if (is_file(evaluate_path(arg) + ".c"))
      mobs = filter_array(objects(), ( : !$1->is_body() && clonep($1) && base_name($1) == $(arg) :));
   else
      mobs = filter_array(objects(), ( : !$1->is_body() && clonep($1) && $1->query_name() == $(arg) :));
   mobs->remove();
   write("Killed " + sizeof(mobs) + " '" + arg + "'.\n");
}
