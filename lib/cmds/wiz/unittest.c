/* Do not remove the headers from this file! see /USAGE for more info. */

// Tsath, 2023

#define TEST_DIR "/std/tests/"

//: COMMAND
// USAGE:  test <all|test> [debug]
//
// This command runs unit tests and reports back.
// Examples:
//   unittest all       - run all tests
//   unittest all debug - run all tests in debug mode
//   unittest items     - run just the 'items' suite
//   unittest list      - shows all the test suites

inherit CMD;

private
void main(string arg)
{
   int debug = 0;
   string *args;

   if (!arg)
   {
      out("Usage:\n"
          "   unittest all        - to run all suites\n"
          "   unittest all debug  - to run all suites in debug mode\n"
          "   unittest items      - to run only 'items' suite\n"
          "   unittest list       - list all suites\n");
      return;
   }
   args = explode(arg, " ");

   if (sizeof(args) == 2 && args[1] == "debug")
      debug = 1;

   if (args[0] == "list")
   {
      string *suites = get_dir(TEST_DIR + "*.c");
      out("The following suites are defined:\n  " + format_list(suites));
      return;
   }

   if (args[0] == "all")
      TEST_D->test_all(debug);
   else
      TEST_D->test_suite(args[0], debug);
}
