/*

Added the 'master' command which reloads the master object.
Jezu @ LIMA July 6, 2023

*/

//: ADMINCOMMAND
//USAGE: ``master``
//
//  Reload the master object.
//
//  .. TAGS: RST

#include <mudlib.h>

inherit CMD;

private
void main()
{
  destruct(master());
  master();
  tell(this_user(), "Master object reloaded.\n");
}