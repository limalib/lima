/* Do not remove the headers from this file! see /USAGE for more info. */

// RST_D by Tsath 2022-10-21
// Some code from DOC_D, and uses same autodoc formats.

#include <log.h>
#include <security.h>

inherit M_REGEX;
inherit M_DAEMON_DATA;

#define RST_DIR "/help/autodoc"
#define FILE_NAME 0
#define FILE_TYPE 1
#define FILE_PRETTY 2

//: MODULE
// The RST (reStructured Text) daemon handles finding source files which have been modified and
// updating the RST documentation. The intent of this daemon is to do formatting without forcing
// new autodoc standards nor introducing RST formatting into MUD help pages.

// Public functions --------------------------------------------------------

private
void continue_scan();
private
void prepare_ingame_pages();
private
int last_time;
private
mixed *files_to_do;
private
mixed *dirs_to_do;
private
string *ingame_files;

private
void delete_directory(string directory)
{
   if (file_size(directory) == -1)
      return;
   foreach (mixed *file in get_dir(sprintf("%s/", directory), -1))
   {
      string target = sprintf("%s/%s", directory, file[0]);
      if (file[1] == -2)
         delete_directory(target);
      else
         rm(target);
   }
   rmdir(directory);
}

private
void make_directories()
{
   /* Assume that if the filesize is -1 that a directory needs to be created */
   string *directories = ({"api", "daemon", "command", "player_command", "module", "mudlib", "verb", "ingame"});

   if (file_size(RST_DIR) == -1)
      mkdir(RST_DIR);

   foreach (string d in directories)
   {
      if (file_size(RST_DIR + "/" + d) == -1)
         mkdir(RST_DIR + "/" + d);
   }
}

//: FUNCTION scan_mudlib
//
// Recursively searches the mudlib for files which have been changed
// since the last time the docs were updated, and recreates the documentation
// for those files.
void scan_mudlib()
{
   printf("Starting RST scan ...\n");
   files_to_do = ({});
   dirs_to_do = ({"/"});
   if (!last_time)
   {
      make_directories();
   }
   prepare_ingame_pages();
   continue_scan();
}

//: FUNCTION complete_rebuild
//
// Rebuild all the data, regardless of modification time
void complete_rebuild()
{
   last_time = 0;
   prepare_ingame_pages();
   scan_mudlib();
}

// Everything below here is
private:
// ---------------------------------------------------------------------

nosave private string *filtered_dirs =
    ({"/data/", "/ftp/", "/help/", "/include/", "/log/", "/open/", "/tmp/", "/user/", "/wiz/", "/contrib/"});

string *mod_name(string file)
{
   string subdir = 0;
   if (strlen(file) > 9 && file[0..8] == "/daemons/")
      subdir = "daemon";
   else if (strlen(file) > 16 && file[0..15] == "/secure/daemons/")
      subdir = "daemon";
   else if (strlen(file) > 13 && file[0..12] == "/std/modules/")
      subdir = "module";
   else if (strlen(file) > 16 && file[0..15] == "/secure/modules/")
      subdir = "module";
   else if (strlen(file) > 5 && file[0..4] == "/std/")
      subdir = "mudlib";
   else if (strsrch(file, "cmds/player") != -1) // Extend this if you have other player dirs.
      subdir = "player command";
   else if (strsrch(file, "cmds/verbs") != -1)
      subdir = "verb";
   else if (strsrch(file, "cmds") != -1)
      subdir = "command";
   else
      subdir = "api";

   sscanf(file, "%s.c", file);
   return ({file[strsrch(file, "/", -1) + 1..], subdir});
}

string func_name(string bar)
{
   sscanf(bar, "%s(", bar);
   return bar;
}

string command_link(string cmd, string type, int same_level)
{
   if (type == "player command")
      return "`" + cmd + " <" + (same_level ? "" : "player_command/") + cmd + ".html>`_";
   if (type == "verb")
      return "`" + cmd + " <" + (same_level ? "" : "verb/") + cmd + ".html>`_";
   if (type == "command")
      return "`Command: " + cmd + " <" + (same_level ? "" : "command/") + cmd + ".html>`_";
   if (type == "daemon")
      return "`Daemon: " + cmd + " <" + (same_level ? "" : "daemon/") + cmd + ".html>`_";
   if (type == "module")
      return "`Module: " + cmd + " <" + (same_level ? "" : "module/") + cmd + ".html>`_";
   if (type == "mudlib")
      return "`" + cmd + " <" + (same_level ? "" : "mudlib/") + cmd + ".html>`_";
   if (type == "api")
      return "`" + cmd + " <" + (same_level ? "" : "api/") + cmd + ".html>`_";
   if (type == "ingame")
      return "`" + cmd + " <" + (same_level ? "../ingame/" : "ingame/") + cmd + ".html>`_";
}

void process_file(string fname)
{
   string file = read_file(fname);
   string line;
   string *lines;
   string *fixme = ({});
   string *todo = ({});
   string *hook = ({});
   string *file_info;
   string *cmd_info = ({});
   string *module_info = ({});
   string *c_functions = ({});
   string rstfile, rstout, description = "";
   mixed *assoc;
   int i, len;
   int write_file = 0;

   if (last_time && get_dir(fname, -1)[0][2] < last_time)
      return;

   if (!file)
      return;
   lines = explode(file, "\n");
   file_info = mod_name(fname);

   rstfile = RST_DIR + "/" + replace_string(file_info[FILE_TYPE], " ", "_") + "/" + file_info[FILE_NAME] + ".rst";

   // This sets FILE_PRETTY
   switch (file_info[FILE_TYPE])
   {
   case "daemon":
      file_info += ({"Daemon " + file_info[FILE_NAME] + "\n"});
      break;
   case "player command":
      file_info += ({"Player Command *" + file_info[FILE_NAME] + "*\n"});
      break;
   case "verb":
      file_info += ({"Verb *" + file_info[FILE_NAME] + "*\n"});
      break;
   case "command":
      file_info += ({"Command *" + file_info[FILE_NAME] + "*\n"});
      break;
   case "module":
      file_info += ({"Module *" + file_info[FILE_NAME] + "*\n"});
      break;
   case "mudlib":
      file_info += ({"Mudlib *" + file_info[FILE_NAME] + "*\n"});
      break;
   case "api":
      file_info += ({file_info[FILE_NAME] + "\n"});
      break;
   default:
      file_info += ({file_info[FILE_NAME] + "\n"});
      break;
   }

   // rstout = repeat_string("*", strlen(file_info[FILE_PRETTY])) + "\n";
   rstout = file_info[FILE_PRETTY];
   rstout += repeat_string("*", strlen(file_info[FILE_PRETTY])) + "\n\n";
   rstout += "Documentation for the " + file_info[FILE_NAME] + " " + file_info[FILE_TYPE] + " in *" + fname + "*.\n\n";

   len = sizeof(lines);
   while (i < len)
   {
      if (regexp(lines[i], "^[ \t]*//###"))
      {
         while (sscanf(lines[i], "%*(^[ \t]*//###)%s", line))
         {
            fixme += ({trim(line) + " (line " + (i + 1) + ")"});
            i++;
         }
         write_file = 1;
      }
      else if (lines[i][0..2] == "//:")
      {
         line = lines[i][3..];
         line = trim(line);
         i++;
         if (line == "TODO")
         {
            string t = "";
            while (lines[i][0..1] == "//")
            {
               if (lines[i][2] == ' ')
                  t += lines[i][3..];
               else
                  t += lines[i][2..];
               i++;
            }
            todo += ({t});
            write_file = 1;
         }
         else if (line == "MODULE")
         {
            while (lines[i][0..1] == "//")
            {
               if (lines[i][2] == ' ')
                  module_info += ({lines[i][3..] + "\n"});
               else
                  module_info += ({lines[i][2..] + "\n"});

               i++;
            }
            write_file = 1;
         }
         else if (line == "COMMAND" || line == "PLAYERCOMMAND" || line == "ADMINCOMMAND")
         {
            while (lines[i][0..1] == "//")
            {
               if (lines[i][2] == ' ')
                  cmd_info += ({lines[i][3..] + "\n"});
               else
                  cmd_info += ({lines[i][2..] + "\n"});
               i++;
            }
            write_file = 1;
         }
         else if (sscanf(line, "HOOK %s", line) == 1)
         {
            while (lines[i][0..1] == "//")
            {
               if (lines[i][2] == ' ')
                  hook += ({lines[i][3..]});
               else
                  hook += ({lines[i][2..]});
               i++;
            }
            write_file = 1;
         }
         else if (sscanf(line, "FUNCTION %s", line) == 1)
         {
            string prototype;
            string match;

            while ((i < sizeof(lines)) && (lines[i][0..1] == "//"))
            {
               // Skip first space for layout reasons.
               if (strlen(lines[i]) > 3 && lines[i][2] == ' ')
                  description += "\n" + lines[i][3..];
               else
                  description += "\n" + lines[i][2..];
               i++;
            }
            // ### regexp() doesn't match any ";", had to replace_string() them
            match = regexp(map(lines[i..i + 19], ( : replace_string($1, ";", "#") :)), "\\<" + line + "\\>", 1);
            if (sizeof(match) > 0)
            {
               if (sscanf(implode(lines[i + match[1] - 1..i + match[1] + 4], "\n"),
                          "%([ \t]*([a-zA-Z_][a-zA-Z0-9_* \t\n]*|)\\<" + line +
                              "\\>[ \t\n]*\\([ \t\na-zA-Z_0-9*,.]*(\\)|))",
                          prototype))
               {
                  c_functions += ({".. c:function:: " + prototype + "\n" + description + "\n\n\n"});
                  write_file = 1;
               }
            }
            description = "";
         }
         else
         {
            LOG_D->log(LOG_AUTODOC, "Bad header tag: " + fname + " line " + i + ": " + line + "\n");
         }
      }
      else
         i++;
   }

   if (sizeof(module_info) > 0)
   {
      string moduleh = "Module Information";

      rstout += moduleh + "\n" + repeat_string("=", strlen(moduleh)) + "\n\n";
      foreach (string mi in module_info)
      {
         rstout += mi;
      }
      rstout += "\n";
   }

   if (sizeof(cmd_info) > 0)
   {
      string cfunch = file_info[FILE_TYPE] == "player command" ? "Player Command" : "Command";
      int todoi = 1;
      int usage = 0;

      rstout += cfunch + "\n" + repeat_string("=", strlen(cfunch)) + "\n\n";
      foreach (string cinf in cmd_info)
      {
         if (strlen(cinf) > 2 && (cinf[0..6] == "$$ see:" || cinf[0..5] == "$$see:"))
         {
            string *sees = explode(cinf[7..], ",");
            rstout += "See: ";
            foreach (string s in sees)
            {
               s = trim(s);
               // Might be a link to a help page
               if (member_array(s, ingame_files) != -1)
                  rstout += command_link(s, "ingame", 1) + " ";
               else
                  rstout += command_link(s, file_info[FILE_TYPE], 1) + " ";
            }
            rstout += "\n\n";
         }
         else
         {
            if (strsrch(cinf, "USAGE") != -1)
            {
               // Ugly, but seems to cover most situations.
               usage = 1;
               cinf = replace_string(trim(cinf), "USAGE:", "USAGE");
               cinf = trim(replace_string(trim(cinf), "USAGE ", "USAGE::\n\n\t"));
               rstout += trim(cinf) + "\n";
               continue;
            }
            if (trim(cinf) == "")
               usage = 0;

            if (usage)
               rstout += "\t" + trim(cinf) + "\n";
            else
               rstout += cinf;
         }
      }
      rstout += "\n\n";
   }

   if (sizeof(hook) > 0)
   {
      string hookh = "Hooks";

      rstout += hookh + "\n" + repeat_string("=", strlen(hookh)) + "\n\n";
      foreach (string hookline in hook)
      {
         rstout += hookline + "\n";
      }
   }

   if (sizeof(c_functions) > 0)
   {
      string cfunch = "Functions";

      rstout += cfunch + "\n" + repeat_string("=", strlen(cfunch)) + "\n";
      foreach (string cfunc in c_functions)
      {
         rstout += cfunc;
      }
   }

   if (sizeof(todo) > 0)
   {
      string todoh = "TODO list";
      int todoi = 1;

      rstout += todoh + "\n" + repeat_string("=", strlen(todoh)) + "\n\n";

      foreach (string t in todo)
      {
         rstout += todoi + ". " + implode(todo, " ") + "\n";
         todoi++;
      }
      rstout += "\n";
   }

   if (sizeof(fixme) > 0)
   {
      foreach (string fixstr in fixme)
      {
         rstout += ".. note:: " + fixstr + "\n";
      }
   }

   // Add extra padding around code-block:: c
   rstout = replace_string(rstout, ".. code-block:: c", "\n.. code-block:: c\n");

   // Add footer.
   rstout += "\n*File generated by " + lima_version() + " reStructured Text daemon.*\n";

   if (write_file)
   {
      rm(rstfile);
      write_file(rstfile, rstout);
      printf("%s written to %s ...\n", fname, rstfile);
   }
}

void prepare_ingame_pages()
{
   string *files = filter_array(get_dir("/help/player/*", -1), ( : $1[1] != -2 :));
   ingame_files = ({});

   foreach (mixed *file in files)
   {
      cp("/help/player/" + file[0], RST_DIR + "/ingame/" + file[0] + ".rst");
      ingame_files += ({file[0]});
   }
}

void make_index(string header, string type, string *files, string filename)
{
   string output;
   output = repeat_string("*", strlen(header)) + "\n";
   output += header + "\n";
   output += repeat_string("*", strlen(header)) + "\n\n";

   if (sizeof(files))
      foreach (string file in files)
      {
         string *chunks = explode(file, "/");
         string cmd;
         cmd = sizeof(chunks) > 0 ? chunks[ < 1][0.. < 5] : "";
         output += "- " + command_link(cmd, type, 0) + "\n";
      }

   output += "\n*File generated by reStructured Text daemon.*\n";

   rm(filename);
   write_file(filename, output);
}

void write_indices()
{
   /* Player commands index */
   string *files = get_dir(RST_DIR + "/player_command/*.rst");
   make_index("Player Commands", "player command", files, RST_DIR + "/Player_Commands.rst");

   /* Verbs index */
   files = get_dir(RST_DIR + "/verb/*.rst");
   make_index("Verbs", "verb", files, RST_DIR + "/Verbs.rst");

   /* Commands index */
   files = get_dir(RST_DIR + "/command/*.rst");
   make_index("Commands", "command", files, RST_DIR + "/Commands.rst");

   /* Daemons index */
   files = get_dir(RST_DIR + "/daemon/*.rst");
   make_index("Daemons", "daemon", files, RST_DIR + "/Daemons.rst");

   /* API index */
   files = get_dir(RST_DIR + "/api/*.rst");
   make_index("API", "api", files, RST_DIR + "/API.rst");

   /* Modules index */
   files = get_dir(RST_DIR + "/module/*.rst");
   make_index("Module", "module", files, RST_DIR + "/Modules.rst");

   /* API index */
   files = get_dir(RST_DIR + "/mudlib/*.rst");
   make_index("Mudlib", "mudlib", files, RST_DIR + "/Mudlib.rst");

   /* API index */
   files = get_dir(RST_DIR + "/ingame/*.rst");
   make_index("In game help files", "ingame", files, RST_DIR + "/Ingame.rst");

   cp("/USAGE", RST_DIR + "/Usage.rst");
}

void continue_scan()
{
   mixed *files;
   mixed *item;

   for (int i = 0; i < 10; i++)
   {
      if (sizeof(dirs_to_do))
      {
         printf("Scanning %s ...\n", dirs_to_do[0]);
         files = get_dir(dirs_to_do[0], -1);
         foreach (item in files)
         {
            if (item[1] == -2)
            {
               string dir = dirs_to_do[0] + item[0] + "/";
               if (member_array(dir, filtered_dirs) != -1)
                  continue;
               dirs_to_do += ({dir});
            }
            else if (item[2] > last_time && item[0][ < 2.. < 1] == ".c")
            {
               files_to_do += ({dirs_to_do[0] + item[0]});
            }
         }
         dirs_to_do[0..0] = ({});
      }
      else if (sizeof(files_to_do))
      {
         /*
          ** We need an unguarded() for any writes that may occur... there
          ** is no user object, so protection checks will always fail.  This
          ** will terminate the checking at this daemon rather than fall
          ** off the stack and fail.  Note that we don't actually hit priv
          ** 1, but the maximum allowed.
          */
         unguarded(1, ( : process_file, files_to_do[0] :));
         files_to_do[0..0] = ({});
      }
      else
      {
         printf("Done with sub pages.\nWriting indices.\n");
         write_indices();
         printf("Done.\n");
         last_time = time();
         save_me();
         HELP_D->rebuild_data();
         return;
      }
   }
   call_out(( : continue_scan:), 1);
}

void do_sweep()
{
   scan_mudlib();
   call_out(( : do_sweep:), 86000);
}

void create()
{
   if (clonep())
   {
      destruct(this_object());
      return;
   }
   ::create();
   if (!last_time)
      do_sweep();
   else
      call_out(( : do_sweep:), 86000);
}