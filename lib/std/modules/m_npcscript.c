/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// m_npcscript (M_NPCSCRIPT)
//
// This object provides functionality for MOBILE adversaries to execute scripts at given intervals that involve a series
// of steps and checks along the way. Need your NPC to go buy a sandwich? This is the way.
//
// This modules inherits M_ACTIONS by itself. If triggers are used, inherit M_TRIGGERS in your mob as well.
// Only 1 trigger is supported at a time. Example of script:
//
//   |       create_script("lunch");
//   |       add_steps(
//   |           "lunch",
//   |           ({
//   |               step(SCRIPT_ACTION, (: set_for_sale, 0:)),
//   |               step(SCRIPT_DESC, "Harry the affectionate"),
//   |               step(SCRIPT_ACTION, "say Well, time for some lunch."),
//   |               step(SCRIPT_ACTION, "go south@@go south@@go south@@hum"),
//   |               step(SCRIPT_WAIT, 5),
//   |               step(SCRIPT_ACTION, "push button"),
//   |               step(SCRIPT_TRIGGER, "The elevator door opens.", "go northwest"),
//   |               step(SCRIPT_DESC, "Harry, leaning against the elevator panel."),
//   |               step(SCRIPT_TRIGGER, "The elevator door closes.", "push 8"),
//   |               ...
//   |           }));
//
// Note, ``set_for_sale(0)`` above closes up shop, so a vendor temporarily sells nothing.
//
// .. TAGS: RST

#include <npcscript.h>
inherit "/std/classes/script_step";
inherit M_ACTIONS;

private
mapping scripts = ([]);
int step_pause = 3;
string running_script;
int running_step;
private
int recovery_time = 30;
private
int started_at;

//: FUNCTION create_script
// Creates a new script with 'name'.
void create_script(string name)
{
   scripts[name] = ({});
}

//: FUNCTION query_recovery_time
// Returns the time in minutes before recover() is called
// and the script is aborted.
int query_recovery_time()
{
   return recovery_time;
}

//: FUNCTION set_recovery_time
// Set the time in minutes before the script should have finished
// and the recover() function is called in the NPC and the script
// is aborted.
void set_recovery_time(int rt)
{
   recovery_time = rt;
}

int is_stateful(string state)
{
   return 1;
}

//: FUNCTION recover
// Override this function in your NPC inheriting M_NPCSCRIPT.
// It will be called after the minutes set using ``set_recovery_time()``
// are up.
void recover()
{
   // Override to add your own recovery function.
}

//: FUNCTION add_steps
// Adds a series of steps (an array of class script_step) to a specific script.
// This is typically used in conjuction with the step() function that returns
// script_step classes for easier class creation.
int add_steps(string name, class script_step *steps)
{
   if (!arrayp(scripts[name]))
      return 0;

   scripts[name] += steps;

   return 1;
}

//: FUNCTION step
// Creates a script_step class. The following types are supported:
//
//   - SCRIPT_ACTION: Do things, say things. Actions can be seperated by @@ to include more actions in one step.
//
//   - SCRIPT_TRIGGER: Will create a step waiting for something to happen, and will then do action.
//
//   - SCRIPT_WAIT: Wait a number of seconds.
//
//   - SCRIPT_DESC: Change the in room description for the mob as they move along in the world.
varargs class script_step step(int type, mixed payload, mixed extra)
{
   class script_step ss = new (class script_step);
   ss.type = type;
   switch (type)
   {
   case SCRIPT_ACTION:
      if (stringp(payload))
      {
         ss.action = (string)payload;
      }
      else if (functionp(payload))
         ss.func = (function)payload;
      break;
   case SCRIPT_TRIGGER:
      if (!function_exists("add_pattern", this_object()))
         error("Must inherit M_TRIGGERS to use triggers for NPC scripts.");
      ss.trigger = (string)payload;
      ss.action = (string)extra;
      break;
   case SCRIPT_WAIT:
      ss.wait = (int)payload;
      break;
   case SCRIPT_DESC:
      ss.in_room_desc = (string)payload;
      break;
   }

   return ss;
}

//: FUNCTION started_at
// Returns a timestamp for when the script was launched.
int started_at()
{
   return started_at;
}

//: FUNCTION execute_script
// Run a script for the mob by "name".
void execute_script(string name)
{
   // If we have ongoing conversations in M_CONVERSATION, exit those.
   // Also, disable conversations
   this_object()->exit_conversations();
   this_object()->set_can_talk(0);
   STATE_D->add_to_queue_at_time(this_object(), (time() + (60 * recovery_time)), "recovery");

   // Stop actions we might have running
   stop_actions();
   running_script = name;
   running_step = 0;
   started_at = time();
   call_out("next_step", step_pause);
}

// This function matches /include/npcscript.h - make sure to change this if you edit the include file.
private
string step_type(int t)
{
   switch (t)
   {
   case 1:
      return "action";
   case 2:
      return "trigger";
   case 3:
      return "wait";
   case 4:
      return "desc";
   default:
      return "unknown";
   }
}

int state_update(string state)
{
   // If this is a recovery update, then check that a script is running, and call
   // recover() if there is.
   if (state == "recovery")
   {
      if (running_script)
      {
         CHANNEL_D->deliver_channel("domains", sprintf("%O", this_object()) + " has been recovered at " +
                                                   add_article(step_type(scripts[running_script][running_step].type)) +
                                                   " step (#" + running_step + ") started at " + ctime(started_at) +
                                                   ".");
         recover();
         running_script = 0;
         running_step = 0;
         started_at = 0;
      }
      return;
   }
   // If this is a script name, execute it.
   else if (member_array(state, keys(scripts)) != -1)
   {
      execute_script(state);
      return;
   }

   // Let people know we were told to do something we do not know how to do.
   this_object()->do_game_command("mumble " + state);
}

// Internal public function so it can be called via call_out().
void next_step()
{
   int next_call_out = step_pause;
   class script_step step;

   if (running_step >= sizeof(scripts[running_script]))
   {
      running_script = 0;
      running_step = 0;
      return;
   }
   step = scripts[running_script][running_step];

   if (running_step + 1 < sizeof(scripts[running_script]))
   {
      class script_step next_step;
      next_step = scripts[running_script][running_step + 1];
      if (next_step.type == SCRIPT_TRIGGER)
      {
         this_object()->add_pattern(next_step.trigger, next_step.action);
      }
   }

   switch (step.type)
   {
   case SCRIPT_ACTION:
      // Parse multiples if they are there
      if (!step.multiple && stringp(step.action) && strsrch(step.action, "@@") != -1)
      {
         string *more = explode(step.action, "@@");
         if (sizeof(more) > 1)
         {
            step.multiple = more;
            scripts[running_script][running_step] = step;
         }
      }

      if (step.multiple && sizeof(step.multiple))
      {
         this_object()->do_game_command(step.multiple[0]);
         if (sizeof(step.multiple) > 1)
         {
            // Shorten the array, and go a step back so we don't progress.
            step.multiple = step.multiple[1..];
            running_step--;
         }
         else
            step.multiple = 0;
      }
      else if (step.action)
      {
         this_object()->do_game_command(step.action);
      }
      else
         evaluate(step.func);
      break;
   case SCRIPT_TRIGGER:
      return;
      break;
   case SCRIPT_WAIT:
      next_call_out = step.wait;
      break;
   case SCRIPT_DESC:
      this_object()->set_in_room_desc(step.in_room_desc);
      break;
   }

   running_step++;
   if (running_step >= sizeof(scripts[running_script]))
   {
      // Reset script tracking.
      running_script = 0;
      running_step = 0;
      started_at = 0;

      // Enable conversation again
      this_object()->set_can_talk(1);
      // Start actions from M_ACTIONS if we had any running.
      start_actions();
      return;
   }

   // Find next step
   step = scripts[running_script][running_step];

   // If next step is not a trigger, run it in call_out().
   if (step.type != SCRIPT_TRIGGER)
      call_out("next_step", next_call_out);
   else
   {
      // Triggers are added immediately.
      next_step();
   }
}

void triggered(string pattern)
{
   this_object()->remove_pattern(pattern);
   running_step++;
   call_out("next_step", step_pause);
}

//: FUNCTION status
// Returns 0 if no scripts are running, or an array of 2 integers ({x,y}),
// where x is the current step in the script, and y is the max number of steps.
// See also ``query_running_script()``.
int *status()
{
   return running_script ? ({running_step, sizeof(scripts[running_script])}) : 0;
}

//: FUNCTION query_running_script
// Returns the current running script if any.
// See also ``status()`` for description on how far the NPC is into the script.
string query_running_script()
{
   return running_script;
}

//: FUNCTION query_scripts
// Returns the scripts contained in the mob.
mapping query_scripts()
{
   return scripts;
}