/* Do not remove the headers from this file! see /USAGE for more info. */

// Tsath 2024

//: COMMAND
// USAGE
//   |  ``docs``
//   |  ``docs refresh`` - rebuild HELP_D.
//
// This command shows status of the documentation.
//
// .. TAGS: RST

inherit CMD;
inherit M_WIDGETS;

private
void update_stats()
{
   HELP_D->rebuild_data();
}

private
void main(mixed arg)
{
   object ob;
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

   foreach (string f in filter_array(help_pages, ( : $1[ < 4..] == ".rst" :)))
   {
      string *parts = explode(f, "/");
      if (sizeof(parts) > 3 && parts[0] == "help" && parts[1] == "autodoc")
      {
         rst_categories[parts[2]]++;
         if (!rst_tag_cats[parts[2]])
            rst_tag_cats[parts[2]] = 0;
         if (member_array(f, rst_tags) != -1)
            rst_tag_cats[parts[2]]++;
         else if (arg == parts[2])
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
      write("Next "+sizeof(next_files)+" files in " + arg + " category:\n");
      foreach (string f in next_files)
      {
         write("  - "+f);
      }
   }
}
