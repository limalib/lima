/* Do not remove the headers from this file! see /USAGE for more info. */

//:PLAYERCOMMAND
//$$ see: color, colours, ansi, simplify, emoji
//USAGE hints
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
      hintstate = "on";
    else
      hintstate = "off";

    out("Hints is " + hintstate + ".\n");
    return;
  }

  if (arg == "on")
  {
    this_user()->query_shell_ob()->set_variable("hints", 1);
    new ("/domains/std/item/hint_wand")->move(this_body());
    out("Turning hints on - you received a hint wand.\n");
  }
  else if (arg == "off")
  {
    this_user()->query_shell_ob()->unset_variable("hints");
    if (present("hint wand", this_body()))
    {
      this_body()->do_game_command("drop wand");
    }
    out("Turning hints off.\n");
  }
  else
  {
    out("Usage: hints <on/off>.\n");
    return;
  }
}

void player_menu_entry(string str)
{
  bare_init();
  main(str);
  done_outputing();
}