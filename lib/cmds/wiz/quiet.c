/* Do not remove the headers from this file! see /USAGE for more info. */

// Davmar/Nevyn

//: COMMAND
// USAGE:  ``quiet <object>``
//
// This command silences an NPC in the room you are in if they use M_ACTIONS for
// all their chatter.
//
// .. TAGS: RST

inherit CMD;

private
void main(mixed *arg)
{
   object ob;

   ob = arg[0];
   ob->set_actions(100, 0);
   outf("%s has been quieted.",ob->short());

   return;
}
