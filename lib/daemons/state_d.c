/*
** state_d.c -- Changes states for objects on the MUD
**
** This daemon periodically calls objects on the MUD to allow them to change state. This can
** be used to grow crops, make food rot, make critically wounded things die.
**
** Tsath 2020-07-10 (Created)
*/

#define MAX_PROCESSED 10
#define INITIAL_SPREAD 20

int last_time;

mapping queue = ([]);

int in_queue(object ob)
{
   return member_array(ob, flatten_array(values(queue))) != -1;
}

varargs void add_to_queue(object ob, int add_to_time)
{
   int update_time;
   if (ob && ob->query_call_interval() && !in_queue(ob))
   {
      update_time = (ob->query_call_interval() * 60 + time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({ob});
   }
}

// Useful for faster testing, should probably not be used in the real mudlib.
// If it's this brief, then use a call_out().
varargs void add_to_queue_secs(object ob, int add_to_time)
{
   int update_time;
   if (ob && ob->query_call_interval())
   {
      update_time = (time()) + add_to_time;

      if (!queue[update_time])
         queue[update_time] = ({});

      queue[update_time] += ({ob});
   }
}

varargs void remove_from_queue(object ob)
{
   foreach (int update_time, object * targets in queue)
   {
      foreach (object target in targets)
      {
         if (target == ob)
            queue[update_time] -= ({ob});
      }
   }
}

void process_queue()
{
   int processed;
   int t = time();
   foreach (int update_time, object * targets in queue)
   {
      queue[update_time] -= ({0});
      if (!sizeof(targets))
      {
         map_delete(queue, update_time);
         continue;
      }
      foreach (object target in targets)
      {
         if (target && !target->is_stateful())
            continue;
         if (update_time < time() && target->state_update())
            add_to_queue(target);
      }
      if (update_time < time())
         map_delete(queue, update_time);
      processed++;
      if (processed == MAX_PROCESSED)
         return;
   }
}

mapping queue()
{
   return queue;
}

void heart_beat()
{
   process_queue();
}

void capture_all_statefuls()
{
   object *statefuls = filter_array(objects(), ( : clonep($1) && $1->is_stateful() :));
   queue = ([]);
   foreach (object ob in statefuls)
      add_to_queue(ob, random(INITIAL_SPREAD));
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
   return "STAT_D:\n-------\n" + "Queue Length: " + sizeof(keys(queue)) +
          "\n"
          "\n";
}