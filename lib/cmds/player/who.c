/* Do not remove the headers from this file! see /USAGE for more info. */

// Rust 2-6-94

//: PLAYERCOMMAND
// USAGE who [ -w -p ]
//
// The who command will give you a list of all user's presently logged
// onto the MUD, and whether or not they are idle.
//
// Used with the given arguments you can alter the way who works:
//
// who -w
// Will show wizards only.
//
// who -p
// Will show players only.

#include <playerflags.h>

inherit CMD;

object frame = new (FRAME, this_object());

string get_who_string(string arg)
{
   object *u;
   int i;
   string name, extra, retval;
   string uptime = time_to_string(uptime() - (uptime() % 3600));
   // If we have bodies() and they have a level>1, the MUD must use levels.
   // This is not set by a DEFINE but a result of implementation in adversary/body.
   int use_levels_guess = sizeof(bodies()) ? bodies()[0]->query_level() > 0 : 0;
   frame->init_user();

   if (uptime == "")
      uptime = "less then an hour";

   extra = retval = "\n";
   if (this_user())
   {
      switch (arg)
      {
      case "-p":
      case "-z":
         u = filter_array(bodies(), ( : !wizardp($1) :));
         extra = frame->accent("(PLAYERS ONLY)");
         break;
      case "-w":
         u = filter_array(bodies(), ( : wizardp:));
         extra = frame->accent("(Wizards only)");
         break;
      case "-i":
         u = filter_array(bodies(), ( : wizardp:));
         extra = frame->accent("(Implementors only)");
         break;
      case "-m":
         if (wizardp(this_user()))
         {
            u = filter_array(children(USER_OB), ( : !interactive($1) :))->query_body();
            extra = frame->accent("(Non-interactives)");
            break;
         }
         /* FALLTHRU */
      default:
         if (arg)
            extra = frame->warning("Unknown flag" + (sizeof(arg) == 2 ? "" : "s") + " '" + arg + "'");
         u = bodies();
         break;
      }
      if (!wizardp(this_user()))
         u = filter_array(u, ( : $1->is_visible() :));
   }
   else
      u = filter_array(bodies(), ( : $1->is_visible() :));

   u -= ({0});
   i = sizeof(u);
   frame->set_title(sprintf("Who - %d user%s", i, (i != 1 ? "s" : "")));
   if (use_levels_guess)
   {
      frame->set_header_content("Level   Role     Name");
   }
   frame->set_footer_content(sprintf("%s has been up for %s.", mud_name(), uptime));
   if (!i)
      retval += "Sorry, no one fits that bill.";
   foreach (object body in u)
   {
      name = body->query_formatted_desc(78);
      if (!name)
         name = capitalize(body->query_userid());

      if (!body->is_visible())
         name = "(" + name + ")";
      if (body->test_flag(F_IN_EDIT))
         name = "*" + name;
      if (body->query_level())
         retval += sprintf(" %-5.5s   %-6.6s   %-68s\n", "" + body->query_level(),
                           (wizardp(body) ? (adminp(body) ? "Admin" : "Wizard") : ""), name);
      else
         retval += sprintf("%-68s\n", name);
   }
   frame->set_content(retval);
   return frame->render();
}

private
void main(string arg)
{
   if (arg == "")
      arg = 0;

   out(get_who_string(arg));
}

void player_menu_entry()
{
   bare_init();
   main(0);
   done_outputing();
}

int clean_up(int instances)
{
   frame->clean_up();
   ::clean_up(instances);
}
