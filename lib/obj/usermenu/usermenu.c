/* Do not remove the headers from this file! see /USAGE for more info. */

/*
**	User menu by Tsath
**	For the Lima mudlib.
*/

#include <commands.h>
#include <menu.h>
#include <mudlib.h>
#include <playerflags.h>
#include <security.h>

// Move to config.h perhaps?
#define MAX_CHRACTERS_PER_USER 10
#define AUTO_LOGIN_AFTER 10 // seconds

inherit MENUS;
inherit M_ACCESS;

MENU toplevel;
// submenus of the toplevel (main) menu
MENU_ITEM quit_item;
MENU_ITEM goto_main_menu_item;
MENU_ITEM main_seperator;

// right now, we're just going to call the help command.
// private MENU helpmenu;

private
mapping dispatch = (["w":"who"]);
private
int gender;
private
string name;
private
string race;
private
string fname;
private
int autologin_tag;

private
int name_available(string name)
{
   return !arrayp(LAST_LOGIN_D->query_last(name));
}

private
nomask void simple_cmd(string cmd)
{
   call_other(CMD_DIR_PLAYER "/" + dispatch[cmd], "player_menu_entry");
}

private
void enter_game()
{
   string selected = this_user()->query_selected_body();
   string fname = this_user()->query_body_fname(selected);
   if (!selected)
   {
      write("Select a character first.\n");
      return;
   }
   if (autologin_tag)
   {
      remove_call_out(autologin_tag);
      autologin_tag = 0;
   }
   modal_pop();
   this_user()->enter_game(selected, fname);
   destruct();
}

private
nomask int valid_name(string str)
{
   int len;
   str = lower_case(str);

   if (!name_available(str))
   {
      write("Sorry, that name is already taken.\n");
      write("('quit' will abort).\n");
      return 0;
   }

   if (BANISH_D->check_name(str))
   {
      write("Sorry, that name is forbidden by the implementors.  Please choose another.\n");
      write("('quit' will abort).\n");
      return 0;
   }

   if (BANISH_D->check_name(str))
   {
      write("Sorry, that name is forbidden by the implementors.  Please choose another.\n");
      write("('quit' will abort).\n");
      return 0;
   }

   len = strlen(str);

   if (len < 2)
   {
      write("Sorry, that name's too short.  Try again.\n> ");
      return 0;
   }

   if (len > 12)
   {
      write("Sorry, that name's too long.  Try again.\n");
      write("('quit' will abort).\n");
      return 0;
   }

   /*
   ** We used to rely on the banish code to do this, but that is a
   ** Bad Thing as it is too easy to see those entries deleted.  In
   ** particular, we've been distributing the lib for a while now
   ** without a banish.o that contains this rule.  To use the banish
   ** code to implement a feature here is too convoluted and unintuitive.
   **
   ** So... we'll put a regex here to check the name.
   **
   ** Note that this regex matches the restriction imposed by the
   ** SECURE_D.  Also note the name is in lower case right now.
   */
   if (!regexp(str, "^[a-z]+$"))
   {
      write("Sorry, that name is forbidden by the implementors.  Please\n"
            "choose a name containing only letters.\n");
      return 0;
   }

   return 1;
}

private
void got_entry(function when_done, string line)
{
   int width = 0;
   string format = "%#-75." + (75 / (width + 3)) + "s\n\n";
   mapping races = RACE_D->query_race_data();

   if (line == "list")
   {
      write("Please select a race from the following list:\n");
      printf(format, implode(keys(races), "\n"));
      return;
   }

   if (races[line])
   {
      modal_pop();
      race = line;
      fname = races[line];
      evaluate(when_done);
      return;
   }

   if (sscanf(line, "help %s", line) && races[line])
   {
      write(races[line]->short_description());
      return;
   }

   write("No such race.\n");
}

private
int confirm_decision(string dec)
{
   dec = lower_case(dec);
   if (dec == "yes" || dec = "y")
      return 1;
   return 0;
}

private
void do_select(string name)
{
   mapping bodies = this_user()->query_bodies();
   string *sorted_bodies = sort_array(keys(bodies), 1);
   int indexname = to_int(name) - 1;
   if (indexname >= 0)
   {
      name = sorted_bodies[indexname];
   }

   if (this_user()->set_selected_body(name))
      write("Selected character '" + capitalize(name) + "' for playing.\n");
   else
      write("Failed to select character '" + capitalize(name) + "' for playing.\n");
}

private
string *list_chars()
{
   mapping bodies = this_user()->query_bodies();
   string *sorted_bodies = sort_array(keys(bodies), 1);
   string selected = this_user()->query_selected_body();
   string *genders = ({"None", "Male", "Female", "Non-binary"});
   string format = "%-8.8s %-14.14s %-7.7s %-25.25s %-20.20s\n";
   int count = 1;
   printf(format, "Select", "Name", "Level", "Race", "Gender");
   write("--------------------------------------------------------------------------\n");
   foreach (string name in sorted_bodies)
   {
      mixed *ar = bodies[name];
      string s = "[" + count + (name == selected ? "*" : "") + "]";
      printf(format, s, capitalize(name), "" + ar[1], capitalize(ar[2]), genders[ar[3]]);
      count++;
   }
   write("--------------------------------------------------------------------------\n" + "(* Selected character)\n");
   return sorted_bodies;
}

private
void select_char()
{
   string *sorted_bodies = list_chars();
   int body_count = sizeof(sorted_bodies);

   // We only have one body, select it.
   switch (body_count)
   {
   case 1:
      do_select(sorted_bodies[0]);
      break;
   case 0:
      write("Create a character to select one.\n");
      break;
   default:
      input_one_arg("Name or [1-" + body_count + "]: ", ( : do_select:));
      break;
   }
}

private
void creation_done()
{
   write("Character '" + capitalize(name) + " has been created.\n");
   this_user()->set_body(name, fname, race, gender);
}

private
void input_gender(int inputgender)
{
   string race = DIR_RACES "/human";
   string format;
   string *races;
   function when_done = ( : creation_done:);
   int width = 0;
   int intconvert = to_int(inputgender);

   if (intconvert < 1 || intconvert > 3)
   {
      write("\nSorry? Try again, 1-3 only.\n");
      input_one_arg("Gender: ", ( : input_gender:));
      return;
   }
   gender = intconvert;
#ifdef USE_RACES
   races = RACE_D->query_races();
   if (sizeof(races) == 1)
   {
      string default_race = races[0];
      write("You will be a " + default_race + ".\n");
      creation_done();
   }
   else
   {
      foreach (string racename in races)
      {
         if (strlen(racename) > width)
            width = strlen(racename);
      }

      format = "%#-75." + (75 / (width + 3)) + "s\n\n";

      write("\n\nPlease select a race from the following list:\n");
      printf(format, implode(races, "\n"));

      write("Type 'help race' for a brief description.  Type 'list' to show the choices again.\n");
      modal_push(( : got_entry, when_done:), "Race? ");
   }
#endif /* USE_RACES */
}

private
void char_name(string inputname)
{
   inputname = lower_case(inputname);
   if (!strlen(inputname))
   {
      write("\nName is not optional ('quit' will abort).\n");
      input_one_arg("Name: ", ( : char_name:));
      return;
   }
   if (inputname == "quit")
   {
      return;
   }
   if (!valid_name(inputname))
   {
      input_one_arg("Name: ", ( : char_name:));
      return;
   }
   name = inputname;
   write("Gender for " + capitalize(name) + "?\n\t1. Male\n\t2. Female\n\t3. Non-binary\n\n");
   input_one_arg("Gender: ", ( : input_gender:));
}

private
void create_char()
{
   if (this_user()->query_num_bodies() < MAX_CHRACTERS_PER_USER)
   {
      input_one_arg("Name: ", ( : char_name:));
   }
   else
      write("The maximum number of characters on " + mud_name() + " is currently " + MAX_CHRACTERS_PER_USER +
            ".\n"
            "Alas, you can create no more. Remove an old one to make space for a new.\n");
}

private
confirm_remove(string name, string input)
{
   if (confirm_decision(input))
   {
      this_user()->remove_body(name);
      write(capitalize(name) + " exists no longer.\n");
   }
   else
      write("Cancelled. All is well.\n");
}

private
void input_remove(string name)
{
   mapping bodies = this_user()->query_bodies();
   string *sorted_bodies = sort_array(keys(bodies), 1);
   int indexname = to_int(name) - 1;
   if (indexname >= 0)
   {
      name = sorted_bodies[indexname];
   }
   else
      name = lower_case(name);

   input_one_arg("Are you sure? (Yes/no) ", ( : confirm_remove, name:));
}

private
void remove_char()
{
   string *sorted_bodies = list_chars();
   write("Confirmation will be required before removal.\n");
   input_one_arg("Remove: ", ( : input_remove:));
}

private
void quit_game()
{
   this_user()->save_me();
   this_user()->quit();
   destruct();
}

void create()
{
   set_privilege(1);

   toplevel = new_menu(mud_name() + " User Menu");
   quit_item = new_menu_item("Quit", ( : quit_game:), "q");
   goto_main_menu_item = new_menu_item("Return to main menu", toplevel, "m");

   main_seperator = new_seperator("--------------------------------------------------");

   // Add items to the toplevel (main) menu.
   add_menu_item(toplevel, main_seperator);
   add_menu_item(toplevel, new_menu_item("See who's on (who)", ( : simple_cmd:), "w"));
   add_menu_item(toplevel, new_menu_item("Select character to play", ( : select_char:), "s"));
   add_menu_item(toplevel, new_menu_item("Create new character", ( : create_char:), "c"));
   add_menu_item(toplevel, new_menu_item("Remove a character", ( : remove_char:), "r"));
   add_menu_item(toplevel, new_menu_item("Enter the game", ( : enter_game:), "p"));
   add_menu_item(toplevel, quit_item);
}

void user_is_active()
{
   if (autologin_tag)
   {
      remove_call_out(autologin_tag);
      autologin_tag = 0;
      write("(Cancelled autologin)\n");
   }
}

void auto_login()
{
   string selected;
   string fname;

   // Stop auto login here if user has disappeared.
   if (!this_user() || !this_user()->query_selected_body())
      return;

   selected = this_user()->query_selected_body();
   fname = this_user()->query_body_fname(selected);
   if (selected)
   {
      modal_pop();
      write("(Auto login with " + capitalize(selected) + " after " + AUTO_LOGIN_AFTER + " seconds ...)\n");
      // In case they went away here ...
      if (this_user())
         this_user()->enter_game(selected, fname);
      destruct();
   }
}

void start_menu()
{
   string selected = this_user()->query_selected_body();
   if (selected)
      write("\n(Auto login in " + AUTO_LOGIN_AFTER + " enabled, type anything to stop it)\n\n");
   init_menu_application(toplevel);
   autologin_tag = call_out("auto_login", AUTO_LOGIN_AFTER);
}
