/* Do not remove the headers from this file! see /USAGE for more info. */

// Class used to store data inside adversaries.

class limb
{
   int health;
   int max_health;
#ifdef LIMB_SHIELDS
   int shield;
   int max_shield;
#endif
   string parent;
   int flags;
}
