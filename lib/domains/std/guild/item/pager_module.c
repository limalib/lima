/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OBJ;
inherit M_GETTABLE;

string guild;
string letter;

void setup(string g)
{
   if (g)
      guild = g;
   set_id("pager module", "module");
   set_adj(guild);
   add_save(({"guild", "letter"}));
   set_value(50);
}

string long()
{
   return "A pager module. Insert it into a pager for it to work." +
          (guild ? " This one has a label on it saying '" + capitalize(guild) + "'."
                 : " This module is empty, ready for a program to be installed.");
}

int is_pager_module()
{
   return 1;
}

void set_module_name(string n)
{
   guild = n;
   set_adj(guild);
}

void set_letter(string l)
{
   letter = l;
}

string query_letter()
{
   return letter || guild[0..0];
}

string query_module_name()
{
   return guild;
}

// Can be put into things.
mixed direct_put_obj_wrd_obj(object ob1, string wrd, object ob2)
{
   return 1;
}
