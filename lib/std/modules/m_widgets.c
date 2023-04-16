/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** M_WIDGETS (Tsath 2019-2020)
**
** This module returns nicely formatted ANSI/xterm type widgets that can be reused
** all over the MUD. The string return are not sprintf() compatible, so you
** need to write() or out() them.
**
*/

inherit M_COLOURS;

private
nosave int unicodetheme = member_array(this_user()->frames_theme(), ({"ascii", "lines", "none"})) == -1;
private
nosave string barchar = unicodetheme ? "▅" : "=";
private
nosave string nobarchar = unicodetheme ? "▅" : ".";

//: FUNCTION i_simplify
// Returns TRUE if the current user (not the object receiving the message!)
// has simplify turned on.
nomask int i_simplify()
{
   return get_user_variable("simplify") != 0;
}

//: FUNCTION i_emoji
// Returns TRUE if the current user (not the object receiving the message!)
// has emojis turned on.
nomask int i_emoji()
{
   return get_user_variable("emoji") != 0 && !i_simplify();
}

int default_user_width()
{
   return this_user()->query_screen_width() - 2;
}

int is_unicodetheme()
{
   return unicodetheme;
}

//: FUNCTION on_off_widget
// [On ] (green) or [Off] (red) depending on int on.
// "On " or "Off" for simplified view.
string on_off_widget(int on)
{
   if (i_simplify())
      return on ? "On " : "Off ";
   if (on)
      return sprintf("[<046>On<res> ]");
   else
      return sprintf("[<124>Off<res>]");
}

varargs string fancy_divider(int width)
{
   if (!width)
      width = default_user_width();
   if (i_simplify())
      return "";
   return repeat_string("-=", ((width - 1) / 2)) + "\n";
}

varargs string simple_divider(int width)
{
   if (!width)
      width = default_user_width();
   if (i_simplify())
      return "";
   return repeat_string("-", width) + "\n";
}

//: FUNCTION green_bar
// A bar that is green with white dots when spent
string green_bar(int value, int max, int width)
{
   int green, white;
   if (i_simplify())
      return value + "/" + max;
   if (value > max)
      value = max;
   green = (value * 1.00 / max) * (width)-1;
   white = width - 2 - green;

   return sprintf("[" + (white <= 0 ? "<040>" : "<024>") + "%s<res><" + (unicodetheme ? "238" : "007") + ">%s<res>]",
                  repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION critical_bar
// A bar that change colour the lower it gets
string critical_bar(int value, int max, int width)
{
   int green, white;
   float p;

   string bar_colour = "<040>";
   if (i_simplify())
      return "";
   p = value / (max * 1.0);

   if (p < 0.10)
      bar_colour = "<129>";
   else if (p < 0.20)
      bar_colour = "<196>";
   else if (p < 0.50)
      bar_colour = "<226>";

   if (value > max)
      value = max;
   green = (value * 1.00 / max) * (width)-1;
   if (green < 0)
      green = 0;
   white = width - 1 - green;
   //	TBUG("Green: "+green+" White: "+white+" value: "+value+" max: "+max+" width: "+width);

   return sprintf("[" + bar_colour + "%s<res><" + (unicodetheme ? "238" : "007") + ">%s<res>]",
                  repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION reverse_critical_bar
// A bar that change colour the lower it gets
string reverse_critical_bar(int value, int max, int width)
{
   int green, white;
   float p;
   string bar_colour = "<129>";
   if (i_simplify())
      return "";
   if (!max)
      return "";
   p = value / (max * 1.0);

   if (p < 0.30)
      bar_colour = "<040>";
   else if (p < 0.60)
      bar_colour = "<226>";
   else if (p < 0.80)
      bar_colour = "<196>";

   if (value > max)
      value = max;
   green = (value * 1.00 / max) * (width)-1;
   if (green < 0)
      green = 0;
   white = width - 1 - green;

   return sprintf("[" + bar_colour + "%s<res><" + (unicodetheme ? "238" : "007") + ">%s<res>]",
                  repeat_string(barchar, green), repeat_string(nobarchar, white));
}

//: FUNCTION slider_red_green
// A slider which is red below the middle and green above, and marks the
// value with a X. 0 is the middle and max is minus on red axis and + on green axis.
string slider_red_green(int value, int max, int width)
{
   string return_string;
   int marker;
   string x_char = "X";
   string line_char = "-";
   if (unicodetheme)
   {
      x_char = "●";
      line_char = "▬";
   }

   if (i_simplify())
      return "";
   width = width - 2; // [, X and ]
   marker = width * ((value + (max / 2.0)) / (max * 1.0));

   return_string = repeat_string(line_char, marker) + x_char + repeat_string(line_char, width - marker);
   return_string = return_string[0..(width / 2)] + return_string[(width / 2 + 1)..];
   return_string = gradient_string(return_string, ({
                                                      "058",
                                                      "064",
                                                      "070",
                                                      "076",
                                                      "076",
                                                      "082",
                                                      "190",
                                                      "226",
                                                      "220",
                                                      "214",
                                                      "208",
                                                      "202",
                                                      "196",
                                                  }));
   return_string = replace_string(return_string, x_char, "<015>" + x_char + "<res>");
   return "[" + return_string + "<res>]";
}

//: FUNCTION slider_colours_sum
// A slider with multiple colours and cumulative ranges and a marker.
// The colours mapping should be on the format:
//   ([20:"040",50:"041",100:"042"])
// where each number is bigger and strings are ANSI colours.
string slider_colours_sum(int value, mapping colours, int width)
{
   int marker;
   int max = sort_array(keys(colours), -1)[0];
   string return_string;
   int colour_add = 0;
   int next_pos = 0;
   string x_char = "X";
   string line_char = "-";
   string colour_after_marker; // Save this colour to make things easier later.

   if (unicodetheme)
   {
      x_char = "●";
      line_char = "▬";
   }
   if (i_simplify())
      return "";
   width = width - 3; // [ and ]
   marker = width * (1.0 * value / max);
   if (marker == 0)
      return_string = x_char + repeat_string(line_char, width - 1);
   else if (marker == (width - 1))
      return_string = repeat_string(line_char, width) + x_char;
   else
      return_string = repeat_string(line_char, marker) + x_char + repeat_string(line_char, width - marker);

   foreach (int val in sort_array(keys(colours), 1))
   {
      string col = colours[val];
      if (!next_pos)
         return_string = "<" + upper_case(col) + ">" + return_string;
      if (next_pos)
         return_string = return_string[0..(next_pos + colour_add)] + "<" + upper_case(col) + ">" +
                         return_string[((next_pos + colour_add) + 1)..];
      colour_add += strlen(col) + 2;
      next_pos = width * (1.0 * val / max);
      if (next_pos > marker && !colour_after_marker)
         colour_after_marker = col;
   }

   return_string = replace_string(return_string, x_char, "<015>" + x_char + "<" + colour_after_marker + ">");

   return "[" + return_string + "<res>]";
}
