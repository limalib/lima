/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CLASS_BODYSLOTS;

private
mapping default_slots();

private
mapping slots = default_slots();

// Set some defaults. If you want other ones, override this.
private
mapping default_slots()
{
   return (["torso":new (class slot, howmany : 1, name
                         : "torso", max_size
                         : LARGE, stuff
                         : ({})),
             "head":new (class slot, howmany : 1, name
                         : "head", max_size
                         : SMALL, stuff
                         : ({})),
             "arms":new (class slot, howmany : 1, name
                         : "arms", max_size
                         : MEDIUM, stuff
                         : ({})),
             "legs":new (class slot, howmany : 1, name
                         : "legs", max_size
                         : MEDIUM, stuff
                         : ({})),
            "hands":new (class slot, howmany : 1, name
                         : "hands", max_size
                         : SMALL, stuff
                         : ({})),
             "feet":new (class slot, howmany : 1, name
                         : "feet", max_size
                         : SMALL, stuff
                         : ({})),

       /* These are non-armour slots. You can wear things here, but they
          don't offer any standard protection. */
       "right hand":new (class slot, howmany : 1, name
                         : "right hand", max_size
                         : SMALL, non_armour : 1, stuff
                         : ({})),
        "left hand":new (class slot, howmany : 1, name
                         : "left hand", max_size
                         : SMALL, non_armour : 1, stuff
                         : ({})),
             "neck":new (class slot, howmany : 1, name
                         : "neck", max_size
                         : SMALL, non_armour : 1, stuff
                         : ({})),
   ]);
}

void reset_slots()
{
   slots = default_slots();
}

protected
nomask void set_body_slots(mapping what)
{
   slots = what;
}

protected
nomask void add_body_slot(string sname, int num, int maxsize)
{
   if (slots[sname])
      error("Duplicate slot\n");

   slots[sname] = new (class slot, howmany : num, name : sname, stuff : ({}), max_size : maxsize);
}

#define SLOT(x) ((class slot)slots[x])

//: FUNCTION wear_item
// nomask int wear_item(object what, string sname);
// Wear object 'what' on slot 'sname'.
nomask int wear_item(object what, string sname)
{
   object *athere;
   int num;
   int slot_max_size;

   if (!slots[sname])
      return 0;

   SLOT(sname)->stuff -= ({0});

   athere = SLOT(sname)->stuff;
   num = SLOT(sname)->howmany;
   if (sizeof(athere) >= num)
      return 0;
   slot_max_size = SLOT(sname)->max_size;
   if (what->get_size() > slot_max_size && slot_max_size)
      return 0;

   SLOT(sname)->stuff += ({what});
   return 1;
}

nomask int remove_item(object what, string sname)
{
   int num;

   if (!slots[sname])
      return 0;

   num = sizeof(SLOT(sname)->stuff);

   SLOT(sname)->stuff -= ({what});

   return sizeof(SLOT(sname)->stuff) < num;
}

nomask string query_random_armour_slot()
{
   string tmp;

   if (!sizeof(slots))
      return 0;

   tmp = choice(keys(slots));
   if (!tmp)
      return 0;

   return tmp;
}

nomask mixed get_random_clothing()
{
   class slot tmp;

   if (!sizeof(slots))
      return 0;

   tmp = choice(values(slots));
   if (!tmp)
      return 0;

   if (!sizeof(tmp.stuff))
      return 0;

   return choice(tmp.stuff);
}

protected
nomask void reset_body_slots()
{
   slots = ([]);
}

//: FUNCTION has_body_slot
// nomask int has_body_slot(string what);
// Returns 1 if the adversary has a 'what' body slot.
nomask int has_body_slot(string what)
{
   return !!slots[what];
}

//: FUNCTION query_armour_slots
// string *query_armour_slots()
// Returns a list of all armour slots on the adversary.
string *query_armour_slots()
{
   return keys(slots);
}

varargs object *event_get_armours()
{
   object *tmp = ({get_random_clothing()}) - ({0});
   if (!sizeof(tmp))
      return 0;
   else
      return tmp;
}
