/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** state_d.c -- Changes states for objects on the MUD
** Tsath 2020-07-10 (Created)
*/

//: MODULE
// This daemon periodically calls objects on the MUD to allow them to change state. This can
// be used to grow crops, make food rot, make critically wounded things die.
//
// This daemon runs on a heartbeat, so is as granular as the heartbeat settings.
// See M_STATEFUL, M_DECAY, LIGHT_OBJECT and ^std/consumable/hamburger.c for some examples of usage.

#define MAX_PROCESSED 10
#define INITIAL_SPREAD 5

int last_time;

mapping queue = ([]);

//: MODULE in_queue
// Checks if ob is in the queue (anywhere in queue).
int in_queue(object ob)
{
   return member_array(ob, flatten_array(values(queue))) != -1;
}

//: FUNCTION add_to_queue
// Adds ob to queue, to be scheduled at NOW + add_to_time in minutes.
// If force=1, the object is added even if already in queue.
varargs void add_to_queue(object ob, int add_to_time, int force, mixed extra)
{
   int update_time;

   if (ob && ob->query_call_interval() && (force || !in_queue(ob)))
   {
      update_time = (ob->query_call_interval() * 60 + time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({({ob, extra})});
   }
}

//: FUNCTION add_to_queue_secs
// Useful for faster testing, should probably not be used in the real mudlib.
// If it's this brief, then use a call_out(). See add_to_queue for documentation.
// This function adds seconds, and not minutes.
varargs void add_to_queue_secs(object ob, int add_to_time, int force, mixed extra)
{
   int update_time;
   if (ob && ob->query_call_interval() && (force || !in_queue(ob)))
   {
      update_time = (time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({({ob, extra})});
   }
}

//: FUNCTION add_to_queue_at_time
// Add to queue at a specific time. No checks, no requirements.
varargs void add_to_queue_at_time(mixed ob, int update_time, mixed extra)
{
   if (!queue[update_time])
      queue[update_time] = ({});

   queue[update_time] += ({({ob, extra})});
}

//: FUNCTION remove_from_queue
// Removes ob from the queue nomatter when scheduled.
varargs void remove_from_queue(object ob)
{
   foreach (int update_time, object * targets in queue)
   {
      foreach (mixed *target in targets)
      {
         if (target[0] == ob)
         {
            queue[update_time] -= ({target});
         }
      }
   }
}

private
void process_queue()
{
   int processed;
   int t = time();
   foreach (int update_time, object * targets in queue)
   {
      // Strip away any objects from the queue that are now 0 (destroyed/removed).
      queue[update_time] -= ({0});

      // If targets array is empty, just delete the entire entry - and on we go.
      if (!sizeof(targets))
      {
         map_delete(queue, update_time);
         continue;
      }
      // Walk through each target in targets and check if they are still stateful and return true on state_update().
      foreach (mixed *target in targets)
      {
         if (stringp(target[0]))
            target[0] = load_object(target[0]);
         if (target[0] && !target[0]->is_stateful(target[1]))
            continue;
         if (update_time < time() && target[0]->state_update(target[1]))
            add_to_queue(target[0], 0, 1, target[1]);
      }

      // Remove old timestamps from the queue
      if (update_time < time())
         map_delete(queue, update_time);

      // We'll be back later for more processing.
      if (processed == MAX_PROCESSED)
         return;
   }
}

private
void capture_all_statefuls()
{
   object *statefuls = filter_array(objects(), ( : clonep($1) && $1->is_stateful() :));
   queue = ([]);
   foreach (object ob in statefuls)
      add_to_queue(ob, random(INITIAL_SPREAD));
}

void heart_beat()
{
   process_queue();
}

void create()
{
   if (clonep())
   {
      destruct(this_object());
      return;
   }
   capture_all_statefuls();
   set_heart_beat(1);
}

string stat_me()
{
   string squeue = "";
   foreach (int update_time, object * targets in queue)
   {
      string *str_targets = map(targets, ( : sprintf("%O", $1[0]) + " " + sprintf("%O", $1[1]) :));
      squeue += " In " + (update_time - time()) + " seconds:\n";
      squeue += "\t" + format_list(str_targets) + "\n";
   }

   return "STATE_D:\n-------\n" + "Queue Length: " + sizeof(keys(queue)) +
          "\n"
          "\n" +
          squeue;
}