/* Do not remove the headers from this file! see /USAGE for more info. */

inherit M_TEST_SUITE;
inherit CLASS_STATMODS;

mapping races = ([]);
string example_race;

int load_races()
{
   string *rcs = ({});
   string *rcs2 = ({});
   string err;
   int result = 1;
   rcs += get_dir("/std/race/*.c");

   // Standard race load
   foreach (string r in rcs)
   {
      object obj;
      err = catch (obj = load_object("/std/race/" + r));
      if (err)
      {
         debug("Failed to load " + r);
         result = 0;
      }
      races[r] = obj;
      if (!example_race)
         example_race = r;
   }

   // If we have a restricted directory, check this one as well.
   if (file_length("/std/race/restricted") != -1)
   {
      rcs2 += get_dir("/std/race/restricted/*.c");
      foreach (string r in rcs2)
      {
         object obj;
         err = catch (obj = load_object("/std/race/restricted/" + r));
         if (err)
         {
            debug("Failed to load " + r);
            result = 0;
         }
         races[r] = obj;
      }

      rcs += rcs2;
   }

   return result && sizeof(races) == sizeof(rcs);
}

int check_adj_sums()
{
   int result = 1;

   foreach (string file, object ob in races)
   {
      class stat_roll_mods srm;
      if (!ob)
      {
         debug("Failed to load object: " + file);
         continue;
      }

      // Check if class is returned correctly
      srm = (class stat_roll_mods)ob->query_roll_mods();
      if (!srm || !classp(srm))
      {
         debug("Incorrect class returned by " + file);
         result = 0;
         continue;
      }

      // These should all sum to 0
      if (srm.str_adjust + srm.agi_adjust + srm.wil_adjust + srm.int_adjust != 0)
      {
         debug("Adjustment values wrong for " + file);
         result = 0;
      }
   }
   return result;
}

int check_descriptions()
{
   int result = 1;

   foreach (string file, object ob in races)
   {
      if (!stringp(ob->short_description()) && strlen(ob->short_description()) > 10)
      {
         debug("Missing short_description() in " + file);
         result = 0;
      }
   }
   return result;
}

int check_duplicate_races()
{
   int result = 1;
   mapping dup = ([]);

   foreach (string file, object ob in races)
   {
      if (dup[ob->query_race()])
      {
         debug("Duplicate race '" + ob->query_race() + "'");
         result = 0;
      }
      else
         dup[ob->query_race()] = 1;
   }
   return result;
}

int clean_up()
{
   int result = 1;
   mapping dup = ([]);

   foreach (string file, object ob in races)
   {
      string err;
      err = catch (destruct(ob));
      if (err)
      {
         debug("Error cleainng up " + file);
         result = 0;
      }
   }
   return result;
}

void init_tests()
{
   add_test_true("Load all races", ( : load_races:));
   add_test_true("Check adjustment sums", ( : check_adj_sums:));
   add_test_true("Check short_description()", ( : check_descriptions:));
   add_test_true("No duplicate races", ( : check_duplicate_races:));
   add_test_true("Clean up", ( : clean_up:));
}
