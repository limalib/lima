/* Do not remove the headers from this file! see /USAGE for more info. */

inherit ADMTOOL_BASE;
inherit CLASS_MISSION_INFO;

nomask string module_name()
{
   return "missions";
}

nomask string module_key()
{
   return "i";
}

private
nomask void list_missions()
{
   mapping missions = MISSION_D->missions();
   string *result = ({});

   if (sizeof(missions) == 0)
   {
      write("No missions are defined yet.\n");
      return;
   }

   result += ({sprintf("%-20.20s %-30.30s %-30.30s %s", "Mission", "Giver", "Controller", "Level Range")});
   foreach (string mission, class mission_info m in missions)
   {
      result += ({sprintf("%-20.20s %-30.30s %-30.30s %d-%d", m.name, m.giver, m.controller, m.rec_level_min,
                          m.rec_level_max)});
   }

   more(result);
}

private
nomask rcv_max_level(string mission, string giver, string controller, int min_level, string max_level)
{
   int max_level_int;

   max_level = trim(max_level);
   if (max_level == "")
   {
      write("Aborted.");
      return;
   }

   if (sscanf(max_level, "%d", max_level_int) != 1)
   {
      write("ERROR: not a number.\n");
      return;
   }

   MISSION_D->register_mission(mission, giver, controller, min_level, max_level_int);
   write("Mission added.\n");
}

private
nomask rcv_min_level(string mission, string giver, string controller, string min_level)
{
   int min_level_int;

   min_level = trim(min_level);
   if (min_level == "")
   {
      write("Aborted.");
      return;
   }

   if (sscanf(min_level, "%d", min_level_int) != 1)
   {
      write("ERROR: not a number.\n");
      return;
   }

   modal_simple(( : rcv_max_level, mission, giver, controller, min_level_int:), "Minimum level required? ");
}

private
nomask rcv_controller(string mission, string giver, string controller)
{
   controller = lower_case(trim(controller));

   if (controller == "")
   {
      write("Aborted.");
      return;
   }

   if (file_length(absolute_path(controller) + ".c") == -1)
   {
      write("ERROR: file does not exist.\n");
      return;
   }

   controller = absolute_path(controller) + ".c";

   modal_simple(( : rcv_min_level, mission, giver, controller:), "Minimum level required? ");
}

private
nomask rcv_giver(string mission, string giver)
{
   giver = lower_case(trim(giver));

   if (giver == "")
   {
      write("Aborted.");
      return;
   }

   if (file_length(absolute_path(giver) + ".c") == -1)
   {
      write("ERROR: file does not exist.\n");
      return;
   }

   giver = absolute_path(giver) + ".c";

   modal_simple(( : rcv_controller, mission, giver:), "Filename for the controller? ");
}

private
nomask void rcv_add_mission(string mission_name)
{
   string *result;

   mission_name = lower_case(trim(mission_name));
   if (mission_name == "")
      return;

   modal_simple(( : rcv_giver, mission_name:), "Filename for mission giver? ");
}

private
nomask void rcv_remove_mission(string mission_name)
{
   string *result;

   mission_name = lower_case(trim(mission_name));

   result = MISSION_D->unregister_mission(mission_name);
   if (!result)
      write("ERROR: '" + mission_name + "' no such mission exists.\n");
   else
      printf("Removed mission '%s'.\n", mission_name);
}

nomask class command_info *module_commands()
{
   return ({new (class command_info, key
                 : "l", desc
                 : "list missions", action
                 : (
                     : list_missions:)),
            new (class command_info, key
                 : "a", proto
                 : "[name]", desc
                 : "add a mission", args
                 : ({"New mission name? "}), action
                 : (
                     : rcv_add_mission:)),
            new (class command_info, key
                 : "r", proto
                 : "[name]", desc
                 : "remove a mission", args
                 : ({"Remove which mission? "}), action
                 : (
                     : rcv_remove_mission:))});
}
