/* Do not remove the headers from this file! see /USAGE for more info. */

inherit MENUS;
inherit M_ACCESS;

// Most of these fields can be left blank (i.e. 0) if irrelevant or unwanted.
// key == 0 means "leave a blank line"
// proto == 0 means no prototype
// priv == 0 means no restriction
// who == 0 means no info
// args == 0 means no arguments
class command_info
{
   string key; // A one character string giving the key to be pressed to
   // invoke this command.
   string proto; // A short [12 char or less] summary of the args, if any
   string desc;  // A short [~40 chars] description of the command
   string priv;  // priv required to use this command
   string who;   // A short [~10 chars] description of who can use the
   // command
   string *args; // An *of prompts to use to ask for arguments not
   // given on the line.  Max == 2.
   function action; // The function to be called.
}

class menu toplevel;
// submenus of the toplevel (main) menu
class menu_item quit_item;
class menu_item help_item;
class menu_item goto_main_menu_item;
class section main;
class section other;

// Overload these
string module_name();
string module_key();
class command_info *module_commands();
// setup
void begin_menu();

mixed module_priv()
{
   return 0;
}

// our variables
private string prompt;
private class command_info *commands;
private object parent;

private nomask string parent_name()
{
   string path = base_name();
   int idx;

   // ### should be OB_ADMTOOL or something...
   //  the top level object has no parent
   if (path == "/obj/admtool/admtool3")
      return 0;

   idx = strsrch(path, "/", -1);
   path = path[0..idx - 1];

   // anything in /obj/admtool should have admtool2 as the parent
   if (path == "/obj/admtool")
      return "/obj/admtool/admtool3";

   return path;
}

private nomask void do_main_menu()
{
   modal_pop();
   parent->display_current_menu();
   destruct();
}

nomask void do_quit()
{
   modal_pop();
   if (parent)
      parent->do_quit();
   destruct();
}

protected nomask int write_error(string err)
{
   if (err)
   {
      write("Error: " + err + ".\n");
      return 1;
   }
   return 0;
}

protected nomask void do_help()
{
   display_current_menu();
}

void create()
{
   string module_name = this_object()->module_name();
   set_privilege(1);

   // abstract class
   if (strsrch(base_name(), "internal") != -1)
      return;

   toplevel = new_menu(mud_name() + " Administration Tool");
   quit_item = new_menu_item("Quit", ( : do_quit:), "q");
   help_item = new_menu_item("Help", ( : do_help:), "?");
   goto_main_menu_item = new_menu_item("Return to main menu", ( : do_main_menu:), "m");

   main = new_section("Main", "accent");
   other = new_section("Other", "warning");
   add_section_item(toplevel, main);
   add_section_item(toplevel, other);
   add_menu_item(other, quit_item);
   add_menu_item(other, help_item);

   // If the module name is not "main", add a menu item to return to the main menu.
   if (module_name != "main")
      add_menu_item(other, goto_main_menu_item);

   // For each of the commands in the module, add a menu item.
   foreach (class command_info mc in module_commands())
   {
      add_menu_item(
          main, new_menu_item(mc.desc + (mc.proto ? " " + mc.proto : "") + (mc.who ? " (" + mc.who[1.. < 2] + ")" : ""),
                              mc.action, mc.key, 0, 0, mc.args, mc.priv));
   }

   if (clonep())
   {
      if (module_priv() && !check_privilege(module_priv()))
      {
         write("Permission denied: need priv " + module_priv() + "\n");
         return;
      }

      begin_menu();
   }
}

private void handle_input(string str)
{
   string *parts = ({});
   string arg;
   if (stringp(str))
      parts = explode(str, " ");
   else
      str = "";

   if (sizeof(parts) > 1)
   {
      arg = implode(parts[1..], " ");
      str = parts[0];
   }

   if (str == "")
   {
      if (parent)
         str = "m";
      else
         str = "q";
   }

   foreach (class command_info comm in commands)
   {
      if (str == comm.key)
      {
         if (comm.priv && !check_previous_privilege(comm.priv))
         {
            printf("Permission denied; need privilege '%O'.\n", comm.priv);
            return;
         }
         switch (sizeof(comm.args))
         {
         case 0:
            if (arg)
            {
               write("** No argument required.\n");
               return;
            }

            evaluate(comm.action);
            return;
         case 1:
            input_one_arg(comm.args[0], comm.action, arg);
            return;
         case 2:
            input_two_args(comm.args[0], comm.args[1], comm.action, arg);
            return;
         default:
            error("No support for >2 args\n");
         }
      }
   }

   write("** Unknown option (? for help)\n");
}

protected nomask void do_modal_func()
{
   modal_func(( : handle_input:), prompt);
}

void begin_menu()
{
   if (parent_name())
   {
      parent = previous_object();

      if (base_name(parent) != parent_name())
         error("Illegal call to begin_menu()\n");
   }

   frame_init_user();
   if (query_style() == "ascii")
      set_style("none");

   init_menu_application(toplevel);
}
