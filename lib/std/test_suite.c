/* Do not remove the headers from this file! see /USAGE for more info. */

inherit M_WIDGETS;

#define ASSERT_TRUE 1
#define ASSERT_FALSE 0

private
mixed *tests = ({});
private
int success = 0;
private
int fail = 0;
private
int debug = 0;

class test
{
   string desc;
   function f;
   mixed args;
   int assertion;
   int result;
}

private mixed
run_test(class test t)
{
   t.result = evaluate(t.f, t.args) == t.assertion;
   return t;
}

private
varargs void add_test(string desc, function f, mixed args, int assertion)
{
   class test t = new (class test);
   if (!arrayp(args))
      args = ({});
   t->desc = desc;
   t->f = f;
   t->args = args;
   t->assertion = assertion;

   tests += ({t});
}

void do_debug(int d)
{
   debug = d;
}

int debug(string s)
{
   if (debug)
      write(s);
}

void init_tests()
{
   //
}

void cmd(object ob, string s)
{
   ob->do_game_command(s);
}

object clone(string file)
{
   object ob;
   string err;

   err = catch (ob = new (file));
   if (err)
   {
      debug("Failed to clone " + file);
      return 0;
   }
   return ob;
}

//: FUNCTION lab_clone
// Clones object from file argument and sends it to LAB room.
// The object is returned if cloned successfully.
object lab_clone(string file)
{
   object ob = clone(file);
   if (!ob)
      return 0;
   ob->move(LAB);
   return ob;
}

//: FUNCTION std_clone
// Clone and return a list of items from ^std/
// If only one of them fails, an empty list is returned.
object *std_clone(mixed *items)
{
   string err;
   object *item_obs = ({});
   object ob;
   if (!arrayp(items))
      items = ({items});

   foreach (string item in items)
   {
      err = catch (ob = new ("/domains/std/" + item));
      if (err)
      {
         debug("Failed to clone " + item);
         return ({});
      }
      item_obs += ({ob});
   }
   return item_obs;
}

//: FUNCTION std_clone_move
// Clones a list of items and moves them to ob.
// Returns true if all cloned and moved successfully, otherwise false.
int std_clone_move(object ob, mixed items)
{
   object *item_obs = std_clone(items);
   if (!sizeof(item_obs))
   {
      debug("Failed to clone items");
      return 0;
   }

   foreach (object item in item_obs)
   {
      if (!item->move(ob))
         return 0;
   }
   return 1;
}

//: FUNCTION inv_do
// Object 'who' does 'doo what', e.g. troll does 'wear hat'.
// Function does a call_other to the 'what' in inventory for function 'check'.
// Result of call_other will fail if value is fail_on.
// Example: inv_do(troll, "wield", "axe", "query_wielding", 0)
//    Troll wields axe, and if axe->query_wielding() == 0 we fail.
//
// If check is set to "gone", the test will return true if the object is gone.
varargs int inv_do(object who, string doo, string what, string check, int fail_on)
{
   object ob;
   cmd(who, doo + " " + what);
   ob = present(what, who);
   if (!ob && check == "gone")
      return 1;
   if (!ob || call_other(ob, check) == fail_on)
   {
      debug("Failed to " + doo + " " + what);
      return 0;
   }
   return 1;
}

varargs void add_test_true(string desc, function f, mixed args)
{
   add_test(desc, f, args, ASSERT_TRUE);
}

varargs void add_test_fail(string desc, function f, mixed args)
{
   add_test(desc, f, args, ASSERT_FALSE);
}

//: FUNCTION run_tests
// Run all the tests added and report results.
void run_tests()
{
   success = 0;
   fail = 0;
   foreach (class test t in tests)
   {
      string eval = t.assertion ? " True" : "False";
      string out;
      string test_out = replace_string(replace_string(sprintf("%O", t.f), "(:", ""), ":)", "");
      t = run_test(t);

      if (debug)
         out = on_off_widget(t.result) + sprintf(" <110>%-40.40s<res> Assertion: %-5.5s [%s]", t.desc, eval, test_out);
      else
         out = on_off_widget(t.result) + sprintf(" <110>%-40.40s<res>", t.desc);
      write(out);
      if (t.result)
         success++;
      else
         fail++;
   }
}

void create()
{
   init_tests();
}

void stat_me()
{
   write("Tests defined: " + sizeof(tests));
   write("Suceeded " + on_off_widget(1) + " " + success + "\nFailed   " + on_off_widget(0) + " " + fail);
}