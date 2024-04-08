/* Do not remove the headers from this file! see /USAGE for more info. */

// Tsath, 2023

#define TEST_DIR "/std/tests/"

//: COMMAND
// USAGE:  ``unittest <all|test|list> [debug]``
//
// This command runs unit tests and reports back.
// Examples:
//   |  ``unittest all``         - run all tests
//   |  ``unittest all debug``   - run all tests in debug mode
//   |  ``unittest items``       - run just the 'items' suite
//   |  ``unittest items debug`` - run 'items' suite in debug
//   |  ``unittest list``        - shows all the test suites
//
// .. TAGS: RST

inherit CMD;

private
void main(string arg)
{
   int debug = 0;
   string *args;

   if (!arg)
   {
      out("Usage:\n"
          "   unittest all         - to run all suites\n"
          "   unittest all quiet   - all suites and only final results\n"
          "   unittest all debug   - to run all suites in debug mode\n"
          "   unittest items       - to run only 'items' suite\n"
          "   unittest items debug - to run 'items' suite in debug\n"
          "   unittest list        - list all suites\n");
      return;
   }
   args = explode(arg, " ");

   if (sizeof(args) == 2 && args[1] == "debug")
      debug = 1;

   if (args[0] == "list")
   {
      string *suites = get_dir(TEST_DIR + "*.c");
      out("The following suites are defined in " + TEST_DIR + ":\n\t" +
          replace_string(implode(suites, "\n\t"), ".c", ""));
      return;
   }

   if (sizeof(args) == 2 && args[0] == "all" && args[1] == "quiet")
      debug = -1;

   if (args[0] == "all")
      TEST_D->test_all(debug);
   else
      TEST_D->test_suite(args[0], debug);
}
