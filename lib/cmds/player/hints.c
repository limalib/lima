/* Do not remove the headers from this file! see /USAGE for more info. */

//: PLAYERCOMMAND
//$$ see: color, colours, ansi, simplify, emoji
// USAGE hints
//      hints on
//      hints off
//

inherit CMD;
inherit M_COLOURS;

private
void main(string arg)
{
   if (!arg)
   {
      string hintstate;
      if (get_user_variable("hints") == 1)
         hintstate = "<010>on<res>";
      else
         hintstate = "<009>off<res>";

      out("Hints are " + hintstate + ".\n");
   }

   if (arg == "on")
   {
      object tmp;
      this_user()->query_shell_ob()->set_variable("hints", 1);
      tmp = new ("/domains/std/item/hint_token");
      tmp->move(this_body());
      out("Turning hints <010>on<res> - you received a hint token.\n");
      tmp->hook_func();
   }
   else if (arg == "off")
   {
      this_user()->query_shell_ob()->unset_variable("hints");
      if (present("hint token", this_body()))
      {
         this_body()->do_game_command("drop hint token");
      }
      out("Turning hints <009>off<res>.\n");
   }
   else if (arg && present(arg, this_body()))
   {
      object tmp,token;
      token=present("hint_token", this_body());
      if (!token)
         main("on");
      tmp = present(arg, this_body());
      if (!tmp->query_hint())
      {
         out("No hints on that item, sorry.");
         return;
      }
      token->hint_from(tmp);
   }
   else
   {
      out("Usage:\n"
          "  hints <on/off> - to turn hints on and off\n"
          "  hints <item>   - to check for hints on a specific item near you\n");
      return;
   }
}

void player_menu_entry(string str)
{
   bare_init();
   main(str);
   done_outputing();
}