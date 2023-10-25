/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// Module for salvageable items handling breaking them down into
// "realistic" components so you don't get metal out of a wooden shields.
//
// Initial version by Tsath@PLEXUS, 2020.

/*
** M_SALVAGEABLE by Tsath 2019
**
*/

void add_adj(string *adj...);

mapping salvageable = ([]);
int tattered = 0;
private
string primary_material;

//: FUNCTION query_salvage_level
// int query_salvage_level()
// Returns the salvage level determining how much material an object
// will drop when salvaged. This function returns sane values for
// weapons, armour and pelts.
int query_salvage_level()
{
   if (this_object()->is_weapon())
      return this_object()->query_weapon_class();
   if (this_object()->is_armour())
      return this_object()->query_armour_class();
   if (this_object()->is_pelt())
      return this_object()->query_pelt_size();
   return 0;
}

//: FUNCTION set_tattered
// void set_tattered()
// Sets the object as "tattered" meaning reduced durability,
// and gives it a new adjective.
void set_tattered()
{
   tattered = 1;
   add_adj("tattered");
   this_object()->set_tattered_durability();
}

//: FUNCTION is_tattered
// int is_tattered()
// Returns true is the object is tattered, else 0.
int is_tattered()
{
   return tattered;
}

//: FUNCTION primary_salvage
// Returns the material that is the most prevalent in the item.
//  example:
//    set_salvageable((["wood":15, "metal":85, ]));
//    would return:
//    primary_salvage() -> "metal"
string primary_salvage()
{
   return primary_material;
}

//: FUNCTION set_salvageable
// int set_salvageable(mapping s)
// Sets the salvage mapping for object. Salvage mappings contains
// valid categories are defined by being part of:
//  CRAFTING_D->query_salvage_categories();
// example:
//   set_salvageable((["wood":15, "metal":85, ]));
//
// The numbers must sum to 100, or the salvage mapping will be rejected.
int set_salvageable(mapping s)
{
   int sum = 0;
   int largest = 0;
   foreach (string key in keys(s))
   {
      int p = s[key];
      if (CRAFTING_D->valid_category(key))
      {
         if (p > largest)
         {
            primary_material = key;
            largest = p;
         }
         sum += p;
      }
   }
   if (sum != 100)
      return 0;

   salvageable = s;
   return 1;
}

//: FUNCTION is_salvageable
// int is_salvageable()
// Returns 1 if the object is salvageable, otherwise 0. If the object doesn't
// have a valid salvage mapping (see set_salvageable()) it always returns 0.
int is_salvageable()
{
   return sizeof(keys(salvageable));
}

//: FUNCTION query_salvageable
// mapping query_salvageable()
// Returns the salvage mapping set by set_salvageable().
mapping query_salvageable()
{
   return salvageable;
}

// This always returns 1 to let the salvage verb do the walking.
int direct_salvage_obj()
{
   return 1;
}