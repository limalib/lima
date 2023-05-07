/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** wizfuncs.c -- wizard functions for the body
**
** Positions are simple strings assigned by admins to a wizard to describe
** their relation/job/position/whatever to the mud.
**
** 950820, Deathblade: created.
** 230319, Tsath: Merged wiz_positions into this file.
*/

object query_link();

string query_home()
{
   string home;

   home = wiz_dir(this_object()) + "/workroom";
   if (file_size(home + ".c") > 0)
      return home;

   return "CLONE";
}

nomask void wiz_switch_body(string obname)
{
   query_link()->switch_body(obname, 0);
}

#ifdef USE_WIZ_POSITION
private
string wiz_position;

nomask string query_wiz_position()
{
   return wiz_position;
}

nomask void set_wiz_position(string new_position)
{
   if (!check_previous_privilege(1))
      error("Illegal attempt to set a position.\n");

   wiz_position = new_position;
}

#endif
