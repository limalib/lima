/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// This module provides durability functionality for objects.
// It allows objects to have a durability value that can be set, reset, and modified.
// It also provides functions to check the durability percentage, missing durability,
// and to increase or decrease the durability.

private
int durability = -1;
private
int max_dura = -1;
private
int current_max_dura = -1;

//:FUNCTION has_durability
// Returns 1 if the object has durability, 0 otherwise.
nomask
int has_durability()
{
   return 1;
}

//:FUNCTION set_max_durability
// Sets the maximum durability of the object.
// If md is 0, it does nothing.
nomask
void set_max_durability(int md)
{
   if (md == 0)
      return;
   current_max_dura = md;
   max_dura = md;
}

//:FUNCTION original_max_durability
// Returns the original maximum durability of the object.
// If it has not been set, it is set to the current maximum durability.
// This is used to ensure that the original maximum durability is preserved
// even if the current maximum durability changes.
// This is useful for repairing objects, where the maximum durability may decrease.
nomask int original_max_durability()
{
   if (max_dura == -1)
      max_dura = current_max_dura;
   return max_dura;
}

//:FUNCTION max_durability
// Returns the current maximum durability of the object.
// If it has not been set, it is set to the original maximum durability.
nomask int max_durability()
{
   return current_max_dura;
}

//:FUNCTION set_damaged_durability
// Sets the durability to a random value between 1 and 10% of the current maximum durability.
// This is used when the object is damaged.
nomask
void set_damaged_durability()
{
   durability = 1 + random(current_max_dura * 0.1);
}

//:FUNCTION reset_durability
// Resets the durability to the current maximum durability.
// This is used when the object is repaired or when it is first created.
nomask
void reset_durability()
{
   durability = max_durability();
}

//:FUNCTION durability_after_repair
// This function is called after the object has been repaired.
// It decreases the current maximum durability by a random percentage between 0 and 5%.
// This simulates the wear and tear that occurs during repairs.
nomask
void durability_after_repair()
{
   current_max_dura = current_max_dura * (1 - (random(5) / 100.0));
   durability = max_durability();
}

//:FUNCTION query_durability
// Returns the current durability of the object.
// If it has not been set, it is set to the current maximum durability.
nomask
int query_durability()
{
   if (durability == -1)
      reset_durability();
   return durability;
}

//:FUNCTION direct_repair_obj
// Returns 1 if the object can be repaired, i.e., if its durability is less than the current maximum durability.
// This is used to determine if the object can be repaired.
nomask
int direct_repair_obj()
{
   return (durability < current_max_dura);
}

//:FUNCTION durability_percent
// Returns the percentage of durability remaining.
// This is calculated as (current durability / maximum durability) * 100.
nomask
int durability_percent()
{
   return (100 * query_durability() / max_durability());
}

//:FUNCTION missing_durability
// Returns the amount of durability missing from the object.
// This is calculated as the difference between the current maximum durability and the current durability.
nomask
int missing_durability()
{
   if (durability == -1)
      reset_durability();
   return current_max_dura - durability;
}

//:FUNCTION durability_extra_long
// Returns a string representation of the current durability percentage.
// This is used to display the durability of the object in a long format.
nomask
string durability_extra_long()
{
   return sprintf("The durability is %d%%.", durability_percent());
}

//:FUNCTION decrease_durability
// Decreases the durability by the specified amount.
// If the durability is -1, it resets the durability to the current maximum durability first.
nomask
void decrease_durability(int d)
{
   d = abs(d);
   if (durability == -1)
      reset_durability();
   durability -= d;
   if (durability < 0)
      durability = 0;
}

//:FUNCTION increase_durability
// Increases the durability by the specified amount.
// If the durability is -1, it resets the durability to the current maximum durability first.
// It ensures that the durability does not exceed the current maximum durability.
nomask
void increase_durability(int d)
{
   d = abs(d);
   if (durability == -1)
      reset_durability();
   durability += d;
   if (durability > current_max_dura)
      durability = current_max_dura;
}

// Add durability and current maximum durability to the save list.
// This is used to ensure that the durability and current maximum durability are saved when the object is saved
// and restored when the player loads in again.
// This is important for maintaining the state of the object across sessions.
nomask
void internal_setup()
{
   this_object()->add_save(({"durability", "current_max_dura"}));
}