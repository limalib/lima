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
// rarely. They are left for the wizards to kick off. This daemon handles things that happen often enough that players
// should experience them a few times per week depending on the settings in the */include/config/time.h* file.
//
// .. TAGS: RST

#include <config/time.h>

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

//: FUNCTION time_to_weekday
// Returns the week day name given time or uses time().
varargs string time_to_weekday(int time)
{
   int t = time || time();
   int real_day = to_int(strftime("%w", time||time()));
   int day_number = ((time || time()) % 86400) / (86400 / GAME_DAYS_PER_DAY);
   int game_day = ((GAME_DAYS_PER_DAY * real_day) + day_number) % DAYS_PER_WEEK;
   return day_names[game_day];
}

//: FUNCTION time_to_str
// Translates the integer time into a string with weekday, hour, minute and seconds.
// The string returned will be in game time as defined in *config/time.h*.
varargs string time_to_str(int time)
{
   int t = (((time || time()) % 86400) * GAME_DAYS_PER_DAY) % 86400;
   int hours = t / 3600;
   int minutes = (t % 3600) / 60;
   int seconds = (t % 60);
   /*
  TBUG("*********");
  TBUG("(" + GAME_DAYS_PER_DAY + "*" + real_day + ")+" + day_number);
  TBUG("real_day: " + real_day + " day_number: " + day_number + " game_day: " + game_day);
   */
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
   int real_day = to_int(strftime("%w", time()));
   int day_number = (time() % 86400) / (86400 / GAME_DAYS_PER_DAY);
   // gameday+(GAME DAYS PER DAY*real day)%7
   int game_day = ((GAME_DAYS_PER_DAY * real_day) + day_number) % DAYS_PER_WEEK;
   int total;
   int skip_days;

   sscanf(date, "%s %d:%d:%d", day, hours, minutes, seconds);
   delta = ((hours * 3600) + (minutes * 60) + seconds) / GAME_DAYS_PER_DAY;
   /*
      TBUG("*********");
      TBUG("D: " + day + " H: " + hours + " min: " + minutes + " sec: " + seconds + " Delta: " + delta);
      TBUG("(" + GAME_DAYS_PER_DAY + "*" + real_day + ")+" + day_number);
      TBUG("real_day: " + real_day + " day_number: " + day_number + " game_day: " + game_day);
      */
   skip_days = member_array(day, day_names) - 1;
   if (skip_days < game_day)
      skip_days += sizeof(day_names);
   total =
       (real_time() - (real_time() % (86400 / GAME_DAYS_PER_DAY))) + delta + ((86400 / GAME_DAYS_PER_DAY) * skip_days);
   /*
     TBUG("Current day: " + game_day + " Must goto " + skip_days);
     TBUG("Day number: " + member_array(day, day_names));
     TBUG("Matching day: " + 14400 * skip_days);
     TBUG(time_to_str(real_time() - (real_time() % 14400)));
     TBUG("*****" + (real_time() - (real_time() % 14400)));
     TBUG(delta);
     TBUG(((86400 / GAME_DAYS_PER_DAY) * skip_days) + "******");
     TBUG(total);
     */
   return total;
}

void create()
{
   day_names = explode(DAY_NAMES, ",");
}