/* Do not remove the headers from this file! see /USAGE for more info. */

// Tsath 2024

//: COMMAND
// USAGE
//   |  ``docs``
//   |  ``docs refresh``    - rebuild HELP_D.
//   |  ``docs playerdoc``  - recreate /help/player/commands.rst
//
// This command shows status of the documentation.
//
// .. TAGS: RST

#include <commands.h>

inherit CMD;
inherit M_WIDGETS;

private
void update_stats()
{
   HELP_D->rebuild_data();
}

private
void update_playerdoc()
{
   string out = "Commands\n========\n\n"
                ".. TAGS: RST\n.. INFO: This help file is created using 'docs playerdoc'. Don't "
                "edit manually.\n\nThe following commands are currently available:\n\n";
   string *verbs = map(get_dir(CMD_DIR_VERBS + "/*.c"), ( : $1[0.. < 3] :));
   CMD_D->find_cmd_in_path("who", ({CMD_DIR_PLAYER}));
   out += colour_table(CMD_D->query_cmds(CMD_DIR_PLAYER + "/"), 80);
   out += "\n\nYou can also try many \"real life verbs\", which have no help because they use\n"
          "real english syntax.  For example:\n"
          "   ``look at rust``\n"
          "   ``move the yellow table``\n\n"
          "The following verbs are currently available:\n\n";
   out += colour_table(verbs, 80);
   out += "\n\nSome of the verbs have aliases, like 'repair' can be 'fix' or 'patch'.\n";

   write_file("/help/player/commands.rst", out, 1);
   write("Done: commands.rst updated.");
}

private
void main(mixed arg)
{
   object ob;
   string *checked_dirs = ({"autodoc", "player", "admin", "wizard"});
   string *help_pages;
   string *next_files = ({});

   int rst_files, md_files, other_files;
   mapping rst_categories = ([]);
   mapping rst_tag_cats = ([]);
   int width = this_user()->query_screen_width() - 30;
   string *rst_tags = HELP_D->query_tags()["RST"];

   // Build help pages index of unique pages
   help_pages = clean_array(flatten_array(values(HELP_D->query_topics())));
   if (arg == "refresh")
      return update_stats();
   if (arg == "playerdoc")
      return update_playerdoc();

   foreach (string f in filter_array(help_pages, ( : $1[ < 4..] == ".rst" :)))
   {
      string *parts = explode(f, "/");
      if (sizeof(parts) > 2 && parts[0] == "help" && member_array(parts[1], checked_dirs) != -1)
      {
         string category = parts[ < 2];
         rst_categories[category]++;
         if (!rst_tag_cats[category])
            rst_tag_cats[category] = 0;
         if (member_array(f, rst_tags) != -1)
            rst_tag_cats[category]++;
         else if (arg == category)
            next_files += ({f});
      }
   }

   next_files = sort_array(next_files, 1);

   // Print overall file counts
   rst_files = sizeof(filter_array(help_pages, ( : $1[ < 4..] == ".rst" :)));
   md_files = sizeof(filter_array(help_pages, ( : $1[ < 3..] == ".md" :)));
   other_files = sizeof(help_pages) - md_files - rst_files;
   write("                           Rst files: " + rst_files + "    MD files: " + md_files +
         "    Other files: " + other_files + "\n");

   // Print categories
   foreach (string cat, int val in rst_categories)
   {
      write(sprintf("[%3.3s/%3.3s] %15.15s %s", "" + rst_tag_cats[cat], "" + val, cat,
                    green_bar(rst_tag_cats[cat], val, width)));
   }

   if (sizeof(next_files))
   {
      if (sizeof(next_files) > 10)
         next_files = next_files[0..9];
      write("Next " + sizeof(next_files) + " files in " + arg + " category:\n");
      foreach (string f in next_files)
      {
         write("  - " + f);
      }
   }
}
