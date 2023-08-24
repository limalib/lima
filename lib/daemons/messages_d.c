/* Do not remove the headers from this file! see /USAGE for more info. */

/**
 * MESSAGING_D for holding different messages for the MUD in the standard format.
 * The files in this daemon is held in /data/messages/ in a simple format.
 * Lines can be repeated for random selections of messages.
 */

#define MESSAGE_DIR "/data/messages/"

inherit M_DAEMON_DATA;
#define PRIV_NEEDED "Mudlib:daemons"

private
mapping messages;
private
nosave string *mandatory_cmbt_msgs = ({"none", "disarm", "knockout", "knockdown", "miss", "dam10", "fatal", "dam1",
                                       "dam2", "dam3", "dam4", "dam5", "dam6", "dam7", "dam8", "dam9", "dam10"});

mixed query_messages(string type)
{
   return messages[type];
}

void save_messages()
{
   save_me();
   write("MESSAGES_D saved.");
}

varargs void load_messages(string type, int batch)
{
   string *content = explode(read_file(MESSAGE_DIR + type), "\n");
   int combat = type[0..5] == "combat";
   mapping m = ([]);

   foreach (string c in content)
   {
      string name, action;
      if (sscanf(c, "%s:%s", name, action) != 2 || c[0] == '#')
         continue;
      action = punctuate(trim(action));

      // If action is too short or doesn't contain a $n or a verb $v, it's not a valid combat action.
      if (strlen(action) < 3 ||
          (combat && (strsrch(lower_case(action), "$n") == -1 || strsrch(lower_case(action), "$v") == -1)))
         error("Bad action in " + type + " for " + name);

      if (arrayp(m[name]))
         m[name] += ({action});
      else if (stringp(m[name]))
         m[name] = ({m[name], action});
      else if (!m[name])
         m[name] = action;
   }

   // Combat messages
   if (combat)
   {
      if (sizeof(mandatory_cmbt_msgs - keys(m)) != 0)
         error("Missing combat message types in " + type + ": " + format_list(mandatory_cmbt_msgs - keys(m)));
      if (sizeof(keys(m) - mandatory_cmbt_msgs) != 0)
         error("Unknown message types in " + type + ": " + format_list(keys(m) - mandatory_cmbt_msgs));
      messages[type] = m;
   }
   // Defeault messages for living
   else if (type == "living-default")
   {
      messages[type] = m;
   }
   // Base messages are inserted directly into messages mapping.
   else if (type == "base-default")
   {
      messages += m;
   }
   // Fail explicitly on other file types for now.
   else
      error("Unknown file type: " + type);

   write("Messages '" + type + "' updated from file.");
   if (!batch)
      save_messages();
}

void refresh_messages()
{
   string *files = get_dir("/data/messages/");
   messages = ([]);
   foreach (string fi in files)
   {
      load_messages(fi, 1 /* batch - to skip saving before the end */);
   }
   save_messages();
}

void create()
{
   ::create();
   if (!mapp(messages))
   {
      messages = ([]);
      refresh_messages();
   }
}

void clean_up()
{
   destruct();
}

/* Count the messages in the messages mapping recursively. Likely not a very reusable function since it's fitted to this
 * exact mapping. */
private
int count_msgs(mixed m)
{
   if (arrayp(m))
      return sizeof(m);
   if (mapp(m))
   {
      int c = 0;
      foreach (mixed smth in values(m))
      {
         c += count_msgs(smth);
      }
      return c;
   }
   if (stringp(m))
      return 1;
}

void stat_me()
{
   int msg_cnt = count_msgs(messages);

   write("MESSAGE_D stats:\n----------------\n\n");
   write("Combat message types:\n\t" + format_list(filter(keys(messages), ( : strsrch($1, "combat") != -1 :))));
   write("\n\nOther message types:\n\t" + format_list(filter(keys(messages), ( : strsrch($1, "combat") == -1 :))));
   write("\n\nA total of " + msg_cnt + " messages.");
}