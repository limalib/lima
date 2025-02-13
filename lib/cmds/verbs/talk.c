/* Do not remove the headers from this file! see /USAGE for more info. */

inherit VERB_OB;

//: COMMAND
// USAGE
//
//  |  ``talk``
//  |  ``talk with <someone>``
//  |  ``talk to <someone>``
//
//  Talk to noone in particular, or to someone you have met.
//
// .. TAGS: RST

void do_talk()
{
   this_body()->simple_action("$N $vstart babbling about nothing (and to noone) in particular.");
}

void do_talk_to_liv(object liv)
{
   liv->begin_conversation();
}

void do_talk_with_liv(object liv)
{
   liv->begin_conversation();
}
void create()
{
   add_rules(({"", "with LIV", "to LIV"}));
}
