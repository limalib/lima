/* Do not remove the headers from this file! see /USAGE for more info. */

/*
**
** Provides various adminstration facilities for admins, domain lords, and
** wizards.
**
** 07-Jun-95.  Deathblade.  Created.
*/

#include <commands.h>
#include <daemons.h>
#include <security.h>

inherit __DIR__ "internal/base";

string module_name()
{
   return "main";
}

class command_info *module_commands()
{
   class command_info *ret = ({});
   string dir = base_name();
   string ourname;
   int idx = strsrch(dir, "/", -1);

   ourname = dir[idx + 1..];
   dir = dir[0..idx];

   foreach (string file in get_dir(dir + "*.c") - ({ourname + ".c"}))
   {
      string key, name, who;

      file = dir + file;

      key = file->module_key();
      name = file->module_name();
      who = file->module_user();

      if (key)
         ret += ({new (class command_info, key : key, desc : name, action : ( : clone_object, file:), who : who)});
   }

   return ret;
}

void create()
{
   if (clonep())
   {
      if (file_name(previous_object()) != CMD_OB_ADMTOOL)
      {
         write("Illegal attempt to clone ADMTOOL\n");
         destruct(this_object());
         return;
      }

      ::create();
   }
}
