/* Do not remove the headers from this file! see /USAGE for more info. */

#define TEST_DIR "/std/tests/"

void test_all()
{
    string *suites = get_dir(TEST_DIR + "*.c");

    foreach (string suite_file in suites)
    {
        object suite = load_object(TEST_DIR + suite_file);
        write("Running suite " + suite_file + " ...");
        suite->run_tests();
        suite->stat_me();
        write("\n\n");
    }
}