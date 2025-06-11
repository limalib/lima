/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
//$$ see: I, didlog
// USAGE:
//
//     |  ``lima ``                    (Show versions of the LIMA mudlib)
//     |  ``lima <version>``           (Show changes in a specific version)
//     |  ``lima <version> <pattern>`` (Show changes in a specific version that matches a pattern)
//
// Shows you the versions of the LIMA mudlib that you have change
// information about. If you specify a version, it will show you
// the changes in that version. If you specify a version and a
// pattern, it will show you the changes in that version that
// match the pattern.
//
// .. TAGS: RST

#include <daemons.h>

inherit CMD;

private
void main(string str)
{
   string header;
   string pattern;
   string version;

   if (str == "help" || str == "/help" && this_body())
   {
      out("lima:\n"
          "\tlima                     - Show the versions stored.\n" +
          "\tlima <version> <pattern> - Show all changes in <version> that matches <pattern>.\n" +
          "\tlima <version>           - Show all changes in <version>.\n");
      return;
   }
   else if (str && sscanf(str, "%s", version) == 1)
   {
      sscanf(str, "%s %s", version, pattern);

      if (member_array(version, LIMA_D->versions()) == -1)
      {
         out("Unknown LIMA version '" + version + "'.");
         return;
      }
   }
   else if (!version)
   {
      out("The following LIMA versions are known: \n\t" + implode(sort_array(LIMA_D->versions(), 1), "\n\t") + "\n");
      return;
   }
   else
   {
      out("Please specify version.");
      return;
   }

   header = "LIMA changes " + (version ? " for " + version : "");

   out(LIMA_D->get_did_info(time() + 10000, ({header, repeat_string("-", sizeof(header)), ""}), pattern, 0, version));
}
