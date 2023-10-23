/* Do not remove the headers from this file! see /USAGE for more info. */

// Adapted to new user menu system, Tsath 2022

//: PLAYERCOMMAND
// Quit to the user menu.

#include <config/user_menu.h>

inherit CMD;

private
void main()
{
   this_body()->quit();
#ifdef USE_USER_MENU
   new (USER_MENU)->start_menu();
#else
   this_user()->save_me();
   this_user()->quit();
#endif
}