/* Do not remove the headers from this file! see /USAGE for more info. */

#define TEST_DIR "/std/tests/"

int fail;
int success;

//: FUNCTION test_suite
// Calls test suite in /std/tests/.
// Call with test_suite(1) to get debug output.
void test_suite(string suite_file, int debug)
{
   object suite;
   suite_file = TEST_DIR + suite_file;
   if (debug != -1)
      write("Loading fresh suite " + suite_file + " ...");
   suite = find_object(suite_file);
   destruct(suite); // Dest once, then load again to make sure we have a new one.
   suite = load_object(suite_file);
   if (debug != -1)
      write("Running suite " + suite_file + " ...");
   suite->do_debug(debug);
   suite->run_tests();
   if (debug != -1)
      suite->stat_me();
   fail += suite->query_fail();
   success += suite->query_success();
   if (debug != -1)
      write("\n\n");
}

//: FUNCTION test_all
// Calls all test suites in /std/tests/ and run them all.
// Call with test_all(1) to get debug output.
void test_all(int debug)
{
   string *suites = get_dir(TEST_DIR + "*.c");
   fail = 0;
   success = 0;

   foreach (string suite_file in suites)
   {
      test_suite(suite_file, debug);
   }
   write("\nTotal Suceeded " + success + "\nTotal Failed   " + fail);
}
