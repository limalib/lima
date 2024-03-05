/* Do not remove the headers from this file! see /USAGE for more info. */


inherit OBJ;
inherit M_GETTABLE;
inherit M_WEARABLE;


void setup()
{
   set_adj("red");
   set_id("scarf");
#ifdef USE_BODYSLOTS
   set_slot("head");
#endif
   set_value(10000);
#ifdef USE_SIZE
   set_size(SMALL);
#endif
#ifdef USE_MASS
   set_mass(SMALL);
#endif
}
