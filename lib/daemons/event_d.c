/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** event_d.c -- Events on the MUD. Can run both game and back-end events (more classic CRON like event).
** Tsath 2024-05-10 (Created)
*/

//: MODULE
// The event_d handles events on the MUD, and can schedule things either in "real world time" or on MUD time. How time
// is defined on the mud is defined in <config/time.h> and should be configured there via admtool.
//
//        ``admtool`` -> Game configuration -> Settings
//
// The event_d only handles day names, hours, minutes and seconds and disregards day count, day in a  month, month names
// and years. This is left up to you to extend should you want to.
//
// This daemon on purpose does not handle event like the yearly filling of the Xmas stockings or things that happen
// rarely - they are left for the wizards to kick off. This daemon handles things that happen often enough that players
// should experience them a few times per week depending on the settings in the */include/config/time.h* file.
//
// Thus the main focus is on weekly scheduled events, i.e. John goes to the bank at 16 with money suitcase Wednesdays
// and Saturdays ("00 16 2,5"), Bob goes to the cafe to buy a sandwich for lunch everyday (except Sunday) at 11:45
// ("45 11 0-6").
//
// .. TAGS: RST

#include <config/time.h>

#define SECS_PER_DAY 86400

inherit M_DAEMON_DATA;

private
nosave string *day_names;

int register_event()
{
}

//: FUNCTION week_day
// If given an integer argument, returns the name of the day.
// If given a string name, returns the day number (0-X) - see *config/time.h*.
// If not not found, returns -1.
mixed week_day(mixed d)
{
   return intp(d) ? day_names[d] : member_array(d, day_names);
}

//: FUNCTION week_days
// Returns an array of the week days as defined in *config/time.h*.
string *week_days()
{
   return day_names;
}

//: FUNCTION game_days_per_day
// Returns the number of game days we have per real days as defined in *config/time.h*.
int game_days_per_day()
{
   return GAME_DAYS_PER_DAY;
}

int adjusted_time()
{
   return time() + (3600 * ADJUST_HOURS);
}

//: FUNCTION time_to_weekday
// Returns the week day name given time or uses time().
varargs string time_to_weekday(int time)
{
   int t = time || adjusted_time();
   int real_day = to_int(strftime("%w", time || adjusted_time()))

#ifdef WEEK_BEGINS_MONDAY
                  + DAYS_PER_WEEK - 1;
#else
       ;
#endif
   int day_number = ((time || adjusted_time()) % SECS_PER_DAY) / (SECS_PER_DAY / GAME_DAYS_PER_DAY);
   int game_day = ((GAME_DAYS_PER_DAY * real_day) + day_number) % DAYS_PER_WEEK;
   return day_names[game_day];
}

//: FUNCTION time_to_str
// Translates the integer time into a string with weekday, hour, minute and seconds.
// The string returned will be in game time as defined in *config/time.h*.
varargs string time_to_str(int time)
{
   int t =
       (((time ? time + (ADJUST_HOURS * 3600) : adjusted_time()) % SECS_PER_DAY) * GAME_DAYS_PER_DAY) % SECS_PER_DAY;
   int hours = t / 3600;
   int minutes = (t % 3600) / 60;
   int seconds = (t % 60);
   return sprintf("%s %2.2d:%2.2d:%2.2d", time_to_weekday(time), hours, minutes, seconds);
}

//: FUNCTION str_to_time
// Translates a string time into an integer that matches the *next* timestamp that matches
// the string. This will be from maximum one week away from the time given.
varargs int str_to_time(string date)
{
   int hours, minutes, seconds;
   string day;
   int delta;
   int real_day = (to_int(strftime("%w", adjusted_time()))
#ifdef WEEK_BEGINS_MONDAY
                   + DAYS_PER_WEEK - 1) %
                  DAYS_PER_WEEK;
#else
                  );
#endif

   int day_number = (adjusted_time() % SECS_PER_DAY) / (SECS_PER_DAY / GAME_DAYS_PER_DAY);
   int game_day = ((GAME_DAYS_PER_DAY * real_day) + day_number) % DAYS_PER_WEEK;
   int total;
   int skip_days;

   sscanf(date, "%s %d:%d:%d", day, hours, minutes, seconds);
   delta = ((hours * 3600) + (minutes * 60) + seconds) / GAME_DAYS_PER_DAY;

   // Might end up negative, but that's handled below.
   skip_days = member_array(day, day_names) - game_day;

   // Calculate the total
   total = (adjusted_time() - (adjusted_time() % (SECS_PER_DAY / GAME_DAYS_PER_DAY))) + delta +
           ((SECS_PER_DAY / GAME_DAYS_PER_DAY) * skip_days);

      TBUG("skip_days: " + skip_days + " game_day: " + game_day + " real_day: " + real_day);

   // This part adds a week if we're planning for things in the past.
   if (total - adjusted_time() < 0)
      total += DAYS_PER_WEEK * (SECS_PER_DAY/GAME_DAYS_PER_DAY);

   // Subtract adjustment to total before returning real timestamp.
   return total - (ADJUST_HOURS * 3600);
}

int *range(string s, string type)
{
   int *r = ({});
   int max;
   if (strsrch(s, "-") != -1)
   {
      int *range = map(explode(s, "-"), ( : to_int($1) :));
      for (int i = range[0]; i <= range[1]; i++)
         r += ({i});
   }
   else if (strsrch(s, ",") != -1)
      r = map(explode(s, ","), ( : to_int($1) :));
   else if (s == "*")
   {
      switch (type)
      {
      case "minute":
         max = 60;
         break;
      case "hour":
         max = 24;
         break;
      default:
         max = DAYS_PER_WEEK;
         break;
      }
      for (int i = 0; i < max; i++)
         r += ({i});
   }
   else
      r = ({to_int(s)});
   return r;
}

varargs void schedule_event(string timing, mixed ob, string extra)
{
   string *entries = ({});
   string minutes, hours, days;
   int *m_ar, *h_ar, *d_ar;
   sscanf(timing, "%s %s %s", minutes, hours, days);

   m_ar = range(minutes, "minute");
   h_ar = range(hours, "hour");
   d_ar = range(days, "day");

   foreach (int d in d_ar)
      foreach (int h in h_ar)
         foreach (int m in m_ar)
         {
            string s = sprintf("%s %2.2d:%2.2d:00", week_day(d), h, m);
            TBUG("Will run in " + time_to_string(str_to_time(s) - time()));
            STATE_D->add_to_queue_at_time(ob, str_to_time(s) - (3600 * ADJUST_HOURS), extra);
         }
}

void create()
{
   day_names = explode(DAY_NAMES, ",");
}

/*
@EVENT_D->schedule_event("0,20,40 6,12,18 0,2,4")

*/