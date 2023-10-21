/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OBJ;
inherit M_WEARABLE;
inherit M_GETTABLE;
inherit M_SALVAGEABLE;
inherit M_DAMAGE_SINK;

inherit M_DURABILITY;

#define ARMOR_LVL_PRICE 50

//: MODULE
// This is the base for creating a piece of armor.  It uses M_WEARABLE to
// allow it to be worn, and M_DAMAGE_SINK to allow it to absorb damage.

void mudlib_setup()
{
   object::mudlib_setup();
   m_wearable::mudlib_setup();
   set_salvageable((["skin":50, "metal":50]));
   add_id("armor", "armour");
   set_mass(1);
   set_max_durability(500 * query_armor_class());
}

int query_value()
{
   int value = query_value();
   int level = query_armor_class() + array_sum(values(query_stat_mods()));
   if (!value)
      value = ARMOR_LVL_PRICE * level - random(ARMOR_LVL_PRICE);
   set_value(value);
   return value;
}

//: FUNCTION set_worn
// set_worn(1) causes this object to be worn by whatever is holding it.
// set_worn(0) takes it back off again.
void set_worn(int g)
{
   if (g)
      environment()->add_armor(this_object());
   else
      environment()->remove_armor(this_object());
   ::set_worn(g);
}

void remove()
{
   m_wearable::remove();
   object::remove();
}

mixed ob_state()
{
   return m_wearable::ob_state();
}
