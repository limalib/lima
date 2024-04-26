/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** guild_d.c -- daemon for recording guild information2
**
** Ohara: Design and original implementation
** Deathblade: updated, installed, etc
** Tsath: Updated, rearranged, extended and wrangled.
*/

inherit M_DAEMON_DATA;
inherit M_STATEFUL;

#define MISSION_DIR "mission/"
#define FAVOUR_DIR "favour/"
#define GUILD_LOG_HISTORY_SIZE 50

class guild_defn
{
   string guild_prospectus;  /* info about the guild */
   int guild_begone;         /* what to do upon leaving guild */
   int guild_suspend_level;  /* suspend level upon suspension */
   string *guild_attributes; /* the guild's attributes */
   string *guild_exclusive;  /* exclusive attributes */
   string *guild_allies;     /* allied guilds */
   string *guild_prereq;     /* prerequisite guilds */
   string *guild_banned;     /* exclusive guilds */
   string guild_title;       /* formal title of this guild */
   int guild_sees_secret;    /* can see other guilds' attrs */
   int guild_is_secret;      /* attrs are (generally) secret */
   int guild_need_all;       /* need ALL prereq guilds */
   int guild_tier;           /* The tier of guild this is */
}

private mapping guilds = ([]);
private
mapping guild_assets = ([]);
private
mapping materials = ([]);
private
mapping guild_contribution = ([]);
private
mapping guild_favour_score = ([]);
private
mapping guild_log = ([]);
private
mapping buff_list = ([]);
private
mapping guild_favours = ([]);
private
mapping guild_skills = ([]);
private
mapping guild_abbrevs = ([]);
private
string *guild_dirs = ({"/domains/std/guild/"});

/* No saves */
private
nosave object *active_missions = ({});
private
nosave object *active_buffs = ({});
private
nosave mapping mission_ctrl = ([]);
private
nosave mapping guild_missions = ([]);
private
nosave mapping guild_mission_obs = ([]);
private
nosave mapping guild_favours = ([]);
private
nosave mapping guild_favour_obs = ([]);

#define DEFN(name) ((class guild_defn)guilds[name])

#define SAVE_FILE "/data/daemons/guild_d"
#define PRIV_NEEDED "Mudlib" // owner priv implies Arch-type

void set_guild_title(string name, string title);
void set_guild_banned(string name, string *banned);
void set_guild_prereq(string name, string *prereq);
void set_guild_allies(string name, string *allies);
void set_guild_exclusive(string name, string *exclusive);
void set_guild_attributes(string name, string *attributes);
void set_guild_begone(string name, int begone);
void set_guild_suspend_level(string name, int suspend_level);
void set_guild_prospectus(string name, string prospectus);
void set_guild_sees_secret(string name, int sees_secret);
void set_guild_is_secret(string name, int is_secret);
void set_guild_need_all(string name, int need_all);
void set_guild_tier(string name, int tier);

string query_guild_title(string name);
string *query_guild_banned(string name);
string *query_guild_prereq(string name);
string *query_guild_allies(string name);
string *query_guild_exclusive(string name);
string *query_guild_attributes(string name);
int query_guild_begone(string name);
int query_guild_suspend_level(string name);
string query_guild_prospectus(string name);
int query_guild_sees_secret(string name);
int query_guild_is_secret(string name);
int query_guild_need_all(string name);
int query_guild_tier(string name);
void add_log(string guild, string log);

void load_missions();
void load_favours();
varargs int start_favour(int tier, string name, string guild, int end_at);

void create()
{
   set_privilege(1);
   restore_me();
   call_out("internal_add_to_queue", 1);
   load_missions();
   load_favours();

   foreach (int tier, mapping guilds in buff_list)
      foreach (string guild, mapping buffs in guilds)
      {
         foreach (string buff, int end_at in buffs)
         {
            TBUG("buff: " + buff + " end_at: " + end_at);
            if (time() < end_at - 30)
            {
               if (guild_favour_obs[tier] && guild_favour_obs[tier][buff])
                  start_favour(tier, buff, guild, end_at);
               else
               {
                  map_delete(buff_list[tier][guild], buff);
               }
            }
            else
            {
               map_delete(buff_list[tier][guild], buff);
            }
         }
      }
   save_me();
}

string *query_guild_dirs()
{
   return guild_dirs;
}

void remove_guild_dir(string d)
{
   guild_dirs -= ({d});
   save_me();
}

void add_guild_dir(string d)
{
   guild_dirs += ({d});
   save_me();
}

void set_guild_abbrev(string guild, string abbrev)
{
   guild_abbrevs[guild] = abbrev;
   save_me();
}

string query_guild_abbrev(string guild)
{
   if (!guild_abbrevs[guild])
      return guild;
   return guild_abbrevs[guild];
}

void clear_buff_list()
{
   buff_list = ([]);
   save_me();
}

string find_guild_dir(string g)
{
   foreach (string gdir in guild_dirs)
   {
      string *gs = get_dir(gdir);
      if (gs && member_array(g, gs) != -1)
         return gdir;
   }
   return 0;
}

object *belongs_to(string guild)
{
   return filter_array(bodies(), ( : member_array($(guild), $1->guilds_belong()) != -1 :));
}

int check_previous(string needed)
{
   if (check_previous_privilege(needed))
      return 1;
   if (base_name(previous_object()) == "/obj/admtool/daemons/guild")
      return 1;
   return 0;
}

private
int check_guild_skill_values(string guild, string skill)
{
   if (!SKILL_D->valid_skill(skill))
   {
      error("Invalid skill '" + skill + "'");
      return 0;
   }
   if (member_array(guild, keys(guilds)) == -1)
   {
      error("Invalid guild '" + guild + "'");
      return 0;
   }
   return 1;
}

void add_guild_skill(string guild, string skill)
{
   if (!check_guild_skill_values(guild, skill))
      return;
   if (!guild_skills[guild])
      guild_skills[guild] = ({});

   if (member_array(skill, guild_skills[guild]) == -1)
   {
      guild_skills[guild] += ({skill});
      save_me();
   }
}

void remove_guild_skill(string guild, string skill)
{
   if (!check_guild_skill_values(guild, skill))
      return;
   if (!guild_skills[guild])
      return;
   guild_skills[guild] -= ({skill});
   save_me();
}

mixed query_guild_skills(mixed guild)
{
   if (arrayp(guild))
   {
      string *skills = ({});
      foreach (string g in guild)
      {
         skills += guild_skills[g];
      }
      return skills;
   }
   if (stringp(guild))
      return guild_skills[guild];
   return guild_skills;
}

void guild_add(string name)
{
   class guild_defn gi;

   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   gi = new (class guild_defn);
   gi.guild_title = "Title Error";
   gi.guild_prospectus = "We Have Nothing To Offer";
   gi.guild_suspend_level = 1;
   guilds[name] = gi;
   save_me();
}

void guild_remove(string name)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   map_delete(guilds, name);
   save_me();
}

void remove_all_guilds()
{
   if (!check_previous(PRIV_NEEDED))
      error("Insufficient priv to remove all guilds.");
   guilds = ([]);
   save_me();
   write("Guilds Wiped.\n");
}

void guild_check(string name)
{
   if (guilds[name])
      return;

   error("Non-existant guild - " + name + ".\n");
}

void set_guild_title(string name, string title)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_title = title;
   save_me();
}

void set_guild_banned(string name, string *banned...)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_banned = banned;
   save_me();
}

void set_guild_prereq(string name, string *prereq...)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_prereq = prereq;
   save_me();
}

void set_guild_allies(string name, string *allies...)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_allies = allies;
   save_me();
}

void set_guild_exclusive(string name, string *exclusive...)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_exclusive = exclusive;
   save_me();
}

void set_guild_attributes(string name, string *attributes...)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_attributes = attributes;
   save_me();
}

void set_guild_begone(string name, int begone)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_begone = begone;
   save_me();
}

void set_guild_suspend_level(string name, int suspend_level)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_suspend_level = suspend_level;
   save_me();
}

void set_guild_prospectus(string name, string prospectus)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_prospectus = prospectus;
   save_me();
}

void set_guild_sees_secret(string name, int sees_secret)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_sees_secret = sees_secret;
   save_me();
}

void set_guild_is_secret(string name, int is_secret)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_is_secret = is_secret;
   save_me();
}

void set_guild_need_all(string name, int need_all)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_need_all = need_all;
   save_me();
}

void set_guild_tier(string name, int tier)
{
   if (!check_previous(PRIV_NEEDED))
      error("insufficient privilege");

   guild_check(name);
   DEFN(name)->guild_tier = tier;
   save_me();
}

string query_guild_title(string name)
{
   guild_check(name);
   return DEFN(name)->guild_title;
}

string *query_guild_banned(string name)
{
   guild_check(name);
   return DEFN(name)->guild_banned;
}

string *query_guild_prereq(string name)
{
   guild_check(name);
   return DEFN(name)->guild_prereq;
}

string *query_guild_allies(string name)
{
   guild_check(name);
   return DEFN(name)->guild_allies;
}

string *query_guild_exclusive(string name)
{
   guild_check(name);
   return DEFN(name)->guild_exclusive;
}

string *query_guild_attributes(string name)
{
   guild_check(name);
   return DEFN(name)->guild_attributes;
}

int query_guild_begone(string name)
{
   guild_check(name);
   return DEFN(name)->guild_begone;
}

int query_guild_suspend_level(string name)
{
   guild_check(name);
   return DEFN(name)->guild_suspend_level;
}

string query_guild_prospectus(string name)
{
   guild_check(name);
   return DEFN(name)->guild_prospectus;
}

int query_guild_sees_secret(string name)
{
   guild_check(name);
   return DEFN(name)->guild_sees_secret;
}

int query_guild_is_secret(string name)
{
   guild_check(name);
   return DEFN(name)->guild_is_secret;
}

int query_guild_need_all(string name)
{
   guild_check(name);
   return DEFN(name)->guild_need_all;
}

int query_guild_tier(string name)
{
   guild_check(name);
   return DEFN(name)->guild_tier;
}

/* given the player's current guilds, can they join the new one? */
mixed guild_is_ok(string name, string *player_guilds)
{
   string *prereq;
   string *banned;
   string *exclusive;
   int sees_secret;

   guild_check(name);

   /* check the new guild's prerequisites against the current guilds */
   prereq = DEFN(name)->guild_prereq;
   if (prereq)
   {
      string *common = prereq & player_guilds;

      /* do we have to have ALL the prereqs? or just one? */
      if (DEFN(name)->guild_need_all)
      {
         if (sizeof(common) != sizeof(prereq))
         {
            // ### should return the missing prereqs
            // ### missing = prereq - common
            return "not all prerequisites have been filled";
         }
      }
      else if (!sizeof(common))
      {
         // ### should return the list of prereqs
         return "need to fill one of the prerequisites";
      }
   }
   else
   {
      /* initialize as empty just in case the "banned" check needs it */
      prereq = ({});
   }

   /*
   ** If the banned list contains the single guild "all" then all guilds
   ** are banned (the player cannot be in ANY guilds except the guilds
   ** that are prerequisites).
   */
   banned = DEFN(name)->guild_banned;
   if (sizeof(banned) && banned[0] == "all" && sizeof(player_guilds - prereq))
   {
      return "cannot join while a member of other guilds";
   }

   exclusive = DEFN(name)->guild_exclusive;
   sees_secret = DEFN(name)->guild_sees_secret;

   foreach (string item in player_guilds)
   {
      string *attributes;

      /* if this item is in the new guild's banned list, punt */
      if (banned && member_array(item, banned) != -1)
      {
         return DEFN(item)->guild_title;
      }

      /* if the guild in question has secret attrs and we can't see them,
     then skip it and go on to the next guild */
      if (!sees_secret && DEFN(item)->guild_is_secret)
         continue;

      /* if any attributes are in common between the excluded list and
     the guild's attributes, then punt */
      attributes = DEFN(item)->guild_attributes;
      if (exclusive && attributes && sizeof(exclusive & attributes) > 0)
      {
         return DEFN(item)->guild_title;
      }
   }

   return 0;
}

void define_from_file(string fname)
{
   string *lines;
   string which_guild;

   if (file_size(fname) < 1)
      error(fname + " is not a file.");
   lines = filter(explode(read_file(fname), "\n"), ( : $1[0] != '#' :));

   foreach (string line in lines)
   {
      int idx = member_array('=', line);
      string attr;
      mixed value;

      /* hmm. malformed. just skip it -- probably blank. */
      if (idx == -1)
         continue;

      attr = trim(line[0..idx - 1]);
      value = trim(line[idx + 1..]);

      if (attr == "name")
      {
         write("Defining '" + value + "'...\n");
         guild_add(value);
         which_guild = value;
         continue;
      }

      if (!which_guild)
         error("attribute specified before guild name\n");

      switch (attr)
      {
      case "prospectus":
         set_guild_prospectus(which_guild, value);
         break;
      case "begone":
         set_guild_begone(which_guild, to_int(value));
         break;
      case "suspend_level":
         set_guild_suspend_level(which_guild, to_int(value));
         break;
      case "attributes":
         value = map(explode(value, ","), ( : trim:));
         set_guild_attributes(which_guild, value...);
         break;
      case "exclusive":
         value = map(explode(value, ","), ( : trim:));
         set_guild_exclusive(which_guild, value...);
         break;
      case "allies":
         value = map(explode(value, ","), ( : trim:));
         set_guild_allies(which_guild, value...);
         break;
      case "prereq":
         value = map(explode(value, ","), ( : trim:));
         set_guild_prereq(which_guild, value...);
         break;
      case "banned":
         value = map(explode(value, ","), ( : trim:));
         set_guild_banned(which_guild, value...);
         break;
      case "title":
         set_guild_title(which_guild, value);
         break;
      case "sees_secret":
         set_guild_sees_secret(which_guild, to_int(value));
         break;
      case "is_secret":
         set_guild_is_secret(which_guild, to_int(value));
         break;
      case "need_all":
         set_guild_need_all(which_guild, to_int(value));
         break;
      }
   }
}

string *query_guilds()
{
   return keys(guilds);
}

/***************************************************************
 * *************** * * ********* * * * ********************* ***
 ***************************************************************/

int state_update()
{
   active_missions -= ({0});
   foreach (object mis_ob in active_missions)
   {
      if (mis_ob && !mis_ob->run_mission())
         mis_ob->end_mission();
   }
   active_buffs -= ({0});
   foreach (object buff in active_buffs)
   {
      if (buff && !buff->apply_favour())
         buff->end_favour();
   }
   foreach (int tier, mapping guilds in buff_list)
   {
      foreach (string guild, mapping buffs in guilds)
      {
         foreach (string buff, int end_at in buffs)
         {
            if (time() > end_at - 30)
            {
               map_delete(buffs, buff);
               buff_list[tier][guild] = buffs;
            }
         }
         if (!sizeof(values(buff_list[tier][guild])))
            map_delete(buff_list[tier], guild);
      }
      if (!sizeof(values(buff_list[tier])))
         map_delete(buff_list, tier);
   }
   return 1;
}

void add_favour(object body, string guild, int favour)
{
   string who;
   if (!body)
      return;
   who = body->query_name();
   guild=lower_case(guild);

   if (!guild_favour_score[guild])
      guild_favour_score[guild] = 0;

   guild_favour_score[guild] += favour;

   if (!guild_contribution[guild])
      guild_contribution[guild] = ([]);

   if (!guild_contribution[guild][who])
      guild_contribution[guild][who] = 0;
   guild_contribution[guild][who] += favour;
   save_me();
}

/*
void set_favour_score(mapping m)
{
    guild_favour_score =m;
    save_me();
}
*/

int query_favour_score(string guild)
{
   if (!guild_favour_score[guild])
      return 0;
   return guild_favour_score[guild];
}

int spend_favour(string guild, int f)
{
   if (!guild_favour_score[guild])
      return 0;
   if (guild_favour_score[guild] >= f)
   {
      guild_favour_score[guild] -= f;
      return 1;
   }
   return 0;
}

mapping query_favour_contribution(string guild)
{
   if (!guild_contribution[guild])
      return ([]);
   return guild_contribution[guild] || ([]);
}

void load_missions()
{
   string *files = ({});
   string contents;
   string *ar_contents;
   string err;

   foreach (string gdir in guild_dirs)
   {
      TBUG(gdir + MISSION_DIR + "*.c");
      files += map(get_dir(gdir + MISSION_DIR + "*.c"), ( : $(gdir) + MISSION_DIR + $1:));
   }

   if (!files)
   {
      write("No missions defined for GUILD_D.");
      return;
   }

   foreach (string item in files)
   {
      int tier, favour;
      string description, name, materials;

      object obj;
      if (item[ < 14..] == "/std_mission.c")
         continue;
      if (err = catch (obj = load_object(item)))
      {
         write("Error while loading mission:\n" + err + "\n");
         continue;
      }

      tier = obj->query_tier();
      favour = obj->query_favour();
      description = obj->query_description();
      materials = obj->query_materials();

      contents = read_file(item);
      if (strlen(contents))
      {
         int found = 0;
         ar_contents = explode(contents, "\n");
         foreach (string line in ar_contents)
         {
            if (line[0..10] == "// MISSION:")
            {
               name = trim(line[11..]);
            }
         }
      }

      if (!name)
      {
         write("Missing // MISSION: string for '" + item + "'.\n");
         continue;
      }

      //        guild_favours[name] = item;
      if (!guild_mission_obs[tier])
      {
         guild_mission_obs[tier] = ([]);
      }
      guild_mission_obs[tier][name] = obj;

      if (!guild_missions[tier])
      {
         guild_missions[tier] = ([]);
      }

      guild_missions[tier][name] = ([]);
      guild_missions[tier][name]["description"] = description;
      guild_missions[tier][name]["materials"] = materials;
      guild_missions[tier][name]["favour"] = favour;
   }
}

void load_favours()
{
   string *files = ({});
   string contents;
   string *ar_contents;
   string err;

   foreach (string gdir in guild_dirs)
   {
      files += map(get_dir(gdir + FAVOUR_DIR + "*.c"), ( : $(gdir) + FAVOUR_DIR + $1:));
   }

   if (!files)
   {
      write("No favours defined for GUILD_D.");
      return;
   }

   foreach (string item in files)
   {
      int tier, favour, length;
      string description, name;

      object obj;
      if (item[ < 13..] == "/std_favour.c")
         continue;
      if (err = catch (obj = load_object(item)))
      {
         write("Error while favour object:\n" + err + "\n");
         continue;
      }

      tier = obj->query_tier();
      favour = obj->query_favour();
      description = obj->query_description();
      length = obj->query_length();

      contents = read_file(item);
      if (strlen(contents))
      {
         int found = 0;
         ar_contents = explode(contents, "\n");
         foreach (string line in ar_contents)
         {
            if (line[0..9] == "// FAVOUR:")
            {
               name = trim(line[10..]);
            }
         }
      }

      if (!name)
      {
         write("Missing // FAVOUR: string for '" + item + "'.\n");
         continue;
      }

      //        guild_favours[name] = item;
      if (!guild_favour_obs[tier])
      {
         guild_favour_obs[tier] = ([]);
      }
      guild_favour_obs[tier][name] = obj;

      if (!guild_favours[tier])
      {
         guild_favours[tier] = ([]);
      }

      guild_favours[tier][name] = ([]);
      guild_favours[tier][name]["description"] = description;
      guild_favours[tier][name]["length"] = length;
      guild_favours[tier][name]["favour"] = favour;
   }
}

mapping query_guild_favour_obs()
{
   return guild_favour_obs;
}

mapping query_favours(int tier)
{
   return guild_favours[tier || 1];
}

mapping query_favour(int tier, string name)
{
   return guild_favours[tier][name];
}

mapping query_missions(int tier)
{
   if (!tier)
      return copy(guild_missions);
   else
      return copy(guild_missions[tier]);
}

mapping query_mission(int tier, string name)
{
   return copy(guild_missions[tier][name]);
}

object mission_ctrl_npc(string guild)
{
   string gdir = find_guild_dir(guild);
   object room_mis = load_object(gdir + guild + "/room/mission");
   object room_lead = load_object(gdir + guild + "/room/lead");
   object npc = present("mission_ctrl_npc_guild", room_mis);

   // Give it another try
   if (!npc)
   {
      object *a = all_inventory(room_mis);
      a->move(room_lead);
      destruct(room_mis);
      room_mis = load_object(gdir + guild + "/room/mission");
      npc = present("mission_ctrl_npc_guild", room_mis);
   }

   return npc;
}

void load_guild_area(string guild)
{
   string gdir = find_guild_dir(guild);
   TBUG(gdir + guild + "/backroom");
   load_object(gdir + guild + "/room/lead");
   load_object(gdir + guild + "/room/mission");
}

mapping query_buff_list()
{
   return buff_list;
}

int run_mission(int tier, string name, string guild)
{
   if (guild_mission_obs[tier] && guild_mission_obs[tier][name])
   {
      object mission_ob = new (base_name(guild_mission_obs[tier][name]), guild);
      add_log(guild, "Tier " + tier + " mission '" + name + "'");
      TBUG("Mission: " + base_name(guild_mission_obs[tier][name]));
      if (mission_ob)
         active_missions += ({mission_ob});
   }
}

varargs int start_favour(int tier, string name, string guild, int end_time)
{
   int favour_cost;
   object favour_ob;
   TBUG("Starting buff: " + name + " tier: " + tier + " for " + guild);
   if (guild_favour_obs[tier] && guild_favour_obs[tier][name])
   {
      string filename;
      int favour_length;
      object buff_ob;
      favour_ob = guild_favour_obs[tier][name];
      favour_cost = favour_ob->query_favour();
      filename = base_name(favour_ob);
      favour_length = favour_ob->query_length();
      buff_ob = new (filename, guild);
      if (end_time)
         buff_ob->set_ends_at(end_time);
      if (!buff_list[tier])
         buff_list[tier] = ([]);
      if (!buff_list[tier][guild])
         buff_list[tier][guild] = ([]);
      buff_list[tier][guild][name] = end_time ? end_time : time() + (60 * buff_ob->query_length());
      active_buffs += ({buff_ob});
      save_me();
   }
}

object *query_active_missions()
{
   return active_missions;
}

object *query_active_buffs()
{
   return active_buffs;
}

mapping query_mission_ctrl()
{
   return mission_ctrl;
}

mapping query_materials(string guild)
{
   return materials[guild] || ([]);
}

void empty_materials(string guild)
{
   materials[guild] = ([]);
}

//: FUNCTION add_material
// int add_material(mixed m)
// Adds a material (object) to the pouch and detroys it from inventory.
// If m is a string, it's added directly under that ID.
// The function returns 1 on success or 0 on failure.
varargs int add_material(string guild, mixed m, int c)
{
   string m_id;
   if (!m)
      return 0;
   if (c <= 0 && stringp(m))
      c = 1;
   if (objectp(m))
   {
      m_id = m->short();
      c = 1;
   }
   else
      m_id = m;
   if (!CRAFTING_D->valid_material(m_id) && m_id != "dollars")
      return 0;

   if (!materials[guild])
      materials[guild] = ([]);

   if (materials[guild][m_id])
      materials[guild][m_id] += c;
   else
      materials[guild][m_id] = c;
   if (objectp(m))
      destruct(m);
   if (m_id == "dollar" || m_id == "dollars")
      add_log(guild, "Added money " + c + " " + m_id + ".");
   else
      add_log(guild, "Added materials " + c + " " + m_id + ".");
   save_me();
   return 1;
}

//: FUNCTION has_material
// int has_material(string m, int count)
// Checks for count material from the pouch if available (return 1),
// otherwise return 0. Material could also be in inventory, which is
// also checked.
int has_material(string guild, string m, int count)
{
   object ims;
   if (materials[guild][m] >= count)
   {
      return 1;
   }

   return 0;
}

//: FUNCTION remove_material
// int remove_material(string m, int count)
// Removes count material from the pouch if available (return 1),
// otherwise it does nothing (return 0).
int remove_material(string guild, string m, int count)
{
   object *ims;

   if (!CRAFTING_D->valid_material(m))
   {
      map_delete(materials[guild], m);
      return 0;
   }

   if (materials[guild][m] >= count)
   {
      materials[guild][m] -= count;
      if (!materials[guild][m])
         map_delete(materials[guild], m);
      return 1;
   }
   save_me();

   return 0;
}

void add_log(string guild, string log)
{
   int size;

   if (log[ < 1] == '\n')
      log = log[0.. < 2];

   if (!guild_log[guild])
      guild_log[guild] = ({});

   guild_log[guild] += ({ctime(time()) + ": <" + this_body()->query_name() + "> " + log});
   size = sizeof(guild_log[guild]);
   if (size > GUILD_LOG_HISTORY_SIZE)
   {
      guild_log[guild] = guild_log[guild][(size - GUILD_LOG_HISTORY_SIZE)..];
   }
}

string *guild_log(string guild)
{
   return copy(guild_log[guild]);
}

int clear_guild_log(string guild)
{
   map_delete(guild_log, guild);
   return 1;
}

/*
Testing calls:
@GUILD_D->query_missions()
@GUILD_D->run_mission(1,"Wood gathering","yakitori")
@GUILD_D->start_favour(1,"XP Buff Small","yakitori")
@filter_array(objects(),(: base_name($1)=="/domains/std/guild/missions/wood_gathering" && clonep($1) :))->remove()
@GUILD_D->add_guild_skill("yakitori","combat/special/roundhouse")

*/
