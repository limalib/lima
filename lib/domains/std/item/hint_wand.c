/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OBJ;
inherit M_GETTABLE;
inherit M_GRAMMAR;

void show_hints(string *hints)
{
   tell(environment(this_object()), "%^HINTS%^Hints%^RESET%^:\n  " + implode(hints, "\n  ") + "\n");
}

void hook_func()
{
   object *items;
   string *hints = ({});
   items = all_inventory(environment(environment(this_object())));
   foreach (object item in items)
   {
      hints += ({punctuate(item->query_hint())});
   }
   hints = filter_array(hints, ( : $1 && strlen($1) > 1 :));
   if (sizeof(hints))
      call_out("show_hints", 0, hints);
}

void dropped()
{
   if (!this_body())
      return;
   if (environment(this_object())->is_living())
      return;
   // Clean up the hooks
   remove_hook("move", ( : dropped:));
   this_body()->remove_hook("move", ( : hook_func:));
   this_body()->simple_action("$N $vdrop " + short() + " which disappears in a puff of smoke.");

   this_object()->remove();
}

string get_extra_long()
{
   return "\n%^HINTS%^Hints:\n%^RESET%^  This is an example hint.";
}

void mudlib_setup()
{
   ::mudlib_setup();
   set_id("hint wand", "wand");
   set_proper_name("a hint wand");
   set_in_room_desc("An ivory hinting wand");
   set_weight(0.01);
   set_long("This foot long ivory wand will provide hints whereever you go. The wand will "
            "magically disappear if you drop it. The wand will automatically give you hints "
            "as you explore the world.\n\nYou can use 'hints on' to get a new one should you "
            "lose this one.\n");
   this_body()->add_hook("move", ( : hook_func:));
   add_hook("move", ( : dropped:));
}
