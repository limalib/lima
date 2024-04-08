/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
//$$ see: mkdir, ls, pwd, ed
// USAGE:  ``cd [directory|obj]``
//
// Most file commands assume you're talking about your current working
// directory, if you do not specify a full path.  This command sets your
// current directory.  If no argument is given,  your home directory will
// become your current working dir.
//
// With the 'obj' argument it moves you to the directory that 'obj' is in.
//
// .. TAGS: RST

inherit CMD;

nomask private void main(string *argv)
{
   mixed fname;

   if (sizeof(argv))
      fname = argv[0];

   if (!fname)
   {
      fname = wiz_dir(this_user());
      if (!is_directory(fname))
         fname = "/help";
   }

   if (objectp(fname))
      fname = base_path(base_name(fname));

   if (fname[ < 2..] == ".c")
      fname = base_path(fname);

   if (!is_directory(fname))
   {
      out("No such directory: " + fname + "\n");
      return;
   }

   // Never send / at the end to set_pwd().
   fname = strlen(fname) > 1 && fname[ < 1] == '/' ? fname[0.. < 2] : fname;
   this_user()->query_shell_ob()->set_pwd(fname);
   outf("New cwd: %s\n", fname);
}