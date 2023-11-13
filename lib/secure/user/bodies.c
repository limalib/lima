/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * Handling of multiple bodies per user.
 *
 * Created 2022 by Tsath
 */

#define BODY_FNAME 0
#define BODY_LEVEL 1
#define BODY_RACE 2
#define BODY_GENDER 3

nomask void save_me();
mixed unguarded(mixed priv, function fp);

private
mapping bodies = ([]);
private
string selected;

void set_body(string name, string fname, string race, int gender)
{
   if (!mapp(bodies))
      bodies = ([]);
   bodies[name] = ({fname, 1, race, gender});
   
   //Don't save guests
   if (strlen(name) > 5 && name[0..4] == "guest")
      return;
   
   //Save all others here.
   save_me();
}

void remove_body(string name)
{
   if (!mapp(bodies))
      return;
   map_delete(bodies, name);
   unguarded(1, ( : rm, USER_PATH(name) + __SAVE_EXTENSION__:));
   unguarded(1, ( : call_other, LAST_LOGIN_D, "remove_user", name, 0 :));
}

string query_selected_body()
{
   return selected;
}

int query_num_bodies()
{
   if (!mapp(bodies))
      bodies = ([]);

   return sizeof(keys(bodies));
}

int set_selected_body(string name)
{
   name = lower_case(name);
   if (member_array(name, keys(bodies)) != -1)
   {
      selected = name;
      return 1;
   }
   return 0;
}

mapping query_bodies()
{
   if (!mapp(bodies))
      bodies = ([]);

   return bodies;
}

int update_body(object body)
{
   string name = lower_case(body->query_name());
   if (arrayp(bodies[name]))
   {
      bodies[name][BODY_LEVEL] = body->query_level();
      bodies[name][BODY_RACE] = body->query_race();
      bodies[name][BODY_GENDER] = body->query_gender();
      if (strlen(name) > 5 && name[0..4] == "guest")
         return 1;
      save_me();
      // TBUG("Body "+name+" updated in user.");
      return 1;
   }
   return 0;
}

protected
nomask void set_body_fname(string name, string new_body_fname)
{
   bodies[name][BODY_FNAME] = new_body_fname;
   if (strlen(name) > 5 && name[0..4] == "guest")
      return;
   save_me();
}

varargs string query_fname(string name)
{
   if (!name)
      name = query_selected_body();
   if (!mapp(bodies))
      set_body(name, "/std/race/human.c", "human", 0);
   if (bodies[name])
      return bodies[name][BODY_FNAME];
   return "/std/race/human.c";
}

int query_gender(string name)
{
   if (!mapp(bodies))
      set_body(name, "/std/race/human.c", "human", 0);
   if (bodies[name])
      return bodies[name][BODY_GENDER];
   return 0;
}
