/* Do not remove the headers from this file! see /USAGE for more info. */

//: PLAYERCOMMAND
// USAGE
//
//  |  ``mudlist``
//  |  ``mudlist <pattern>``
//
// This command gives a full list of all muds and their addresses that
// this mud knows about.  If you supply a pattern, the command only tells
// you what muds match the pattern.
//
// Examples:
//
//   |  ``mudlist``	   - get all muds
//   |  ``mudlist -a``	- get all muds, including those that are not up
//   |  ``mudlist mud``	- get all muds that contain "mud" somewhere
//	  |	                 in their name
//   |  ``mudlist ^foo``	-- get all muds whose name starts with "foo"
//
// Wizards receive extra information about the muds, such as the library
// and driver versions, and the administrator of the mud.
//
// .. TAGS: RST

inherit CMD;
inherit M_FRAME;
inherit M_WIDGETS;
inherit M_GLOB;
inherit M_REGEX;

string colour_up_down(mixed value)
{
   if (value == "↑" || value == "U")
      return "<048>";
   else
      return "<160>";
}

int sort_by_name(string a, string b)
{
   return lower_case(a) < lower_case(b) ? -1 : 1;
}

private void main(mixed *arg, mapping flags)
{
   mapping mudlist = IMUD_D->query_mudlist();
   mapping matched_muds = ([]);
   string *muds = keys(mudlist);
   mixed *sorted_list;
   string *matches;
   int matched;
   int upcount;
   string match;
   mixed *mudinfo;
   string search_str;

   if (!arg || stringp(arg))
   {
      arg = ({0});
      flags = ([]);
   }

   if (!arg[0])
      matches = muds;
   else
   {
      matches = insensitive_regexp(muds, "^" + translate(arg[0]));
      if (!sizeof(matches))
      {
         outf("No muds out of %d match that pattern.\n", sizeof(mudlist));
         return;
      }
   }

   frame_init_user();

   search_str = flags["s"];
   if (search_str)
      search_str = lower_case(search_str);

   matched = sizeof(matches);

   if (flags["a"])
   {
      upcount = implode(matches, ( : $1 + ($(mudlist)[$2][0] == -1) :), 0);
   }
   else
   {
      matches = filter_array(matches, ( : $(mudlist)[$1][0] == -1 :));
      upcount = sizeof(matches);
   }

   foreach (match in matches)
   {
      mudinfo = mudlist[match];
      if (search_str)
      {
         if (!regexp(lower_case(mudinfo[1]), search_str))
         {
            --matched;
            if (mudinfo[0] == -1)
               --upcount;
            continue;
         }
      }
      matched_muds[match] = mudinfo;
   }

   mudlist = matched_muds;

   foreach (string key, mapping mi in mudlist)
   {
      mudlist[key] += ({key});
   }

   sorted_list = sort_array(values(mudlist), ( : $1[13] < $2[13] ? -1 : 1 :));

   frame_add_column("Up",
                    map(sorted_list, ( : $1[0] == -1 ? (uses_unicode() ? "↑" : "U") : (uses_unicode() ? "↓" : "D") :)),
                    ( : colour_up_down($1) :));
   frame_add_column("Name", map(sorted_list, ( : $1[13] :)), 0, 20);
   frame_add_column("Address", map(sorted_list, ( : $1[1] :)));
   frame_add_column("Port", map(sorted_list, ( : $1[2] :)));
   frame_add_column("Open Status", map(sorted_list, ( : $1[9] :)), 0, 18);
   if (wizardp(this_user()))
      {
         frame_add_column("Library", map(sorted_list, ( : $1[5] :)), 0, 18);
         frame_add_column("Driver", map(sorted_list, ( : $1[7] :)), 0, 18);
         frame_add_column("Administrator", map(sorted_list, ( : $1[10] :)), 0, 18);
      }

   set_frame_title(sprintf("Mudlist - %d matches", matched));
   out(frame_render_columns());
}

void player_menu_entry()
{
   bare_init();
   main(0, 0);
   done_outputing();
}
