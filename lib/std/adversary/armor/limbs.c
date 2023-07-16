/* Do not remove the headers from this file! see /USAGE for more info. */

inherit CLASS_EVENT_INFO;

int is_limb(string);
string *query_limbs();
string query_random_limb();
string *query_non_limbs();
varargs int query_max_health(string);

class wear_info
{
   object primary;
   object *others;
   object secondary;
}

private mapping armors = ([]);

mapping query_armor_map()
{
   return armors;
}

class wear_info find_wi(string s)
{
   class wear_info wi;

   wi = armors[s];

   // Upgrade class with secondary if needed
   if (sizeof(wi) == 2)
   {
      class wear_info wi_upgrade = new (class wear_info);
      wi_upgrade.primary = wi.primary;
      wi_upgrade.others = wi.others;
      wi = wi_upgrade;
      armors[s] = wi;
   }

   if (!wi)
   {
      if (!is_limb(s))
         return 0;
      wi = armors[s] = new (class wear_info);
   }
   return wi;
}

//: FUNCTION query_armors
// object *query_armors(string s);
// Returns the armors that are covering limb 's'.
object *query_armors(string s)
{
   class wear_info wi;
   object *armors = ({});

   if (query_max_health(s) == -1)
      return 0;

   wi = find_wi(s);
   if (!wi)
      return 0;

   armors += ({wi.primary}) + (arrayp(wi.others) ? wi.others : ({0})) + ({wi.secondary});
   armors -= ({0});
   return sizeof(armors) ? armors : 0;

   /*
   if (wi.primary)
      return wi.others ? ({wi.primary}) + wi.others : ({wi.primary});
   else
      return wi.others ? wi.others : 0;
      */
}

//: FUNCTION wear_item
// nomask int wear_item(object what, string where);
// Forces the adversary to wear 'what' on its 'where' limb.
nomask int wear_item(object what, string where)
{
   class wear_info wi;
   string orig_w = where;
   mixed *also;

   // Find the limb we want to use.
   if (is_limb(where))
      wi = find_wi(where);
   else
   {
      if (is_limb("right " + where))
      {
         where = "right " + where;
         wi = find_wi(where);
         // If this is a primary slot item and we're already
         // wearing something there, try left limb instead.
         if (wi && wi.primary && !what->query_worn_under())
         {
            where = "left " + orig_w;
            wi = find_wi(where);
         }
      }
   }

   // Check if this is worn under other things.
   if (what->query_worn_under())
   {
      if (!wi || wi.secondary)
         return 0;

      wi.secondary = what;

      also = what->also_covers();
      if (also)
         foreach (string limb in also)
            if (wi = find_wi(limb))
            {
               if (wi.others)
                  wi.others += ({what});
               else
                  wi.others = ({what});
               wi.others -= ({0});
            }
      return 1;
   }

   if (!wi || wi.primary)
      return 0;

   wi.primary = what;

   also = what->also_covers();
   if (also)
      foreach (string limb in also)
         if (wi = find_wi(limb))
         {
            if (wi.others)
               wi.others += ({what});
            else
               wi.others = ({what});
            wi.others -= ({0});
         }

   return 1;
}

//: FUNCTION remove_item
// nomask int remove_item(object what, string where);
// Removes armor 'what' from the 'where' limb.
nomask int remove_item(object what, string where)
{
   class wear_info wi;
   string orig_w = where;
   string *also;

   if (!(wi = armors[where]) || (wi.primary != what && wi.secondary != what))
   {
      where = "left " + where;
      if (!(wi = armors[where]) || (wi.primary != what && wi.secondary != what))
      {
         where = "right " + orig_w;
         if (!(wi = armors[where]) || (wi.primary != what && wi.secondary != what))
         {
            return 0;
         }
      }
   }

   wi.primary = 0;
   if (wi.others == 0)
      map_delete(armors, where);

   also = what->also_covers();
   if (also)
      foreach (string limb in also)
         if (wi = find_wi(limb))
         {
            wi.others -= ({what});
            wi.others -= ({0});

            if (sizeof(wi.others) == 0)
            {
               if (wi.primary == 0)
                  map_delete(armors, limb);
               else
                  wi.others = 0;
            }
         }
   return 1;
}

//: FUNCTION has_body_slot
// int has_body_slot(string slot);
// Returns 1 if the body slot is a valid one.
int has_body_slot(string slot)
{
   return is_limb(slot);
}

//: FUNCTION query_armor_slots
// string *query_armor_slots()
// Returns all valid armor slots on an adversary.
string *query_armor_slots()
{
   return query_limbs() - query_non_limbs();
}

string query_random_armor_slot()
{
   return query_random_limb();
}

object *event_get_armors(class event_info evt)
{
   // Following ifdef added so this would update nicely always
   return query_armors(evt.target_extra);
}
