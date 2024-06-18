/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** stand.c

   Added standing in and standing on objects
   Vette  April 11, 1997

   Basically rewritten in March 1999 by Tigran
*/

inherit VERB_OB;

/* This is more than likely a temporary fix -- Tigran */
/* Using default_checks here is bad.  Complaining about not standing when
 * trying to and the like */
mixed can_verb_rule(string verb, string rule)
{
   return 1;
}

void do_stand()
{
   if (!this_body()->query_prone())
   {
      write("You're already standing.");
      return;
   }
   this_body()->simple_action("$N $vstand up.");
   this_body()->stand_up();
}

void do_stand_wrd(string prep)
{
   if (prep != "up")
   {
      write("Stand what?");
      return;
   }

   if (!this_body()->query_prone())
   {
      write("You're already standing.");
      return;
   }
   this_body()->simple_action("$N $vstand up.");
   this_body()->stand_up();
}

void do_stand_wrd_obj(string prep, object ob)
{
   ob->do_verb_rule("stand", "WRD OBJ", prep, ob);
}

void create()
{
   add_rules(({""}));
   add_rules(({"WRD"}));
   add_rules(({"WRD OBJ"}), ({}));
}
