/* Do not remove the headers from this file! see /USAGE for more info. */

/**
 * A spell example that creates a dancing lights object that follows the caster
 * around for a period of time.
 */

inherit OBJ;         // This object exists and can move in the world.
inherit M_COMPANION; // We need the dancing lights to be a companion and follow.
inherit M_MESSAGES;  // We need this for targetted_action() messages.
inherit M_STATEFUL;  // We need this for the state.

int duration;

int state_update()
{
   duration--;
   if (!duration)
   {
      tell_from_inside(this_object(), "The lights flicker and fade away.");

      // Delay this a little bit, so STATE_D can clean up.
      call_out("remove", 2);
   }

   return duration > 0;
}

void setup(int d)
{
   set_in_room_desc("A swarm of dancing lights hovering in the air.");
   set_adj("dancing");
   set_id("swarm of lights", "lights", "swarm");
   set_long("A bright swarm of magical lights that dance and flicker in the air."
            " They are warm to the touch, and seem to be made of pure light.");
   set_light(5);
   duration = d;
}

// For leaving and entering messages.
// This is a bit of a hack, but it works.
string a_short()
{
   return "a swarm of dancing lights";
}