/* Do not remove the headers from this file! see /USAGE for more info. */

//: PLAYERCOMMAND
//$$ see: inventory, score, equip
// USAGE money
//
// Displays how much money you are carrying. Short form of the 'score' command.

inherit CMD;

private
void main()
{
   write(MONEY_D->money_string(this_body()));
}