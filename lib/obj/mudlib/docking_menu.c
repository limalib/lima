/*
**	Housing Terminal by Tsath 2020 for Nuke Mudlib
**
*/

#include <menu.h>

inherit MENUS;
inherit SPACE_CLASSES;

class ship_info *owned_ships;
object frame = new (FRAME);
object terminal;

MENU toplevel;
// submenus of the toplevel (main) menu
// MENU soulmenu;

MENU_ITEM quit_item;
MENU_ITEM goto_main_menu_item;
MENU_ITEM ship_info_item;
MENU_ITEM dock_ship_item;
MENU_ITEM statement_item;
MENU_ITEM release_ship_item;
MENU_ITEM purchase_ship_item;
MENU_ITEM change_bank_item;
MENU_ITEM terms_item;
MENU_ITEM main_seperator;
MENU_ITEM sub_seperator;

// right now, we're just going to call the help command.
// private MENU helpmenu;

string menu_text(string s)
{
   return sprintf("[-- <222>%-64s<res>-----]", s);
}

void error_text(string s)
{
   write(sprintf("**** <162>%-s<res>\n", s));
}

void accent_text(string s)
{
   write(sprintf("[" + frame->accent("<162>%-s<res>]\n"), s));
}

void quit_menu_application(int booked)
{
   if (!booked)
   {
      terminal->stop_using();
      this_body()->simple_action("$N $vturn away from the terminal again.");
   }
   else
   {
      this_body()->simple_action("$N $vhave occupied this docking terminal.");
   }

   ::quit_menu_application();
}

void show_ship_info()
{
   if (!sizeof(owned_ships))
   {
      write("You have no ships docked here.\n");
      return;
   }

   write(menu_text("Ship Inventory"));
   printf(frame->accent("%-30.30s %-10.10s %-17.17s"), "Ship Type", "Insignia", "Docked at");
   foreach (class ship_info si in owned_ships)
   {
      printf("<bld>%-30.30s<res> %-10.10s %-17.17s\n", si->type, si->name,
             si->docked_at || (si->long_term ? "Long term parking" : "Unknown"));
   }

   if (sizeof(owned_ships))
   {
      write("\n\n");
      accent_text("You currently pay storage fees at " + this_body()->query_location() + ".");
      write("\n\n");
   }
}

void do_enter_ship()
{
   int result;
   object env = environment(this_body());
   float behind = SHIP_D->missing_payments(lower_case(this_body()->query_name()));
   if (behind > 2.0)
   {
      error_text("You are over " + to_int(behind) + " week" + (to_int(behind) == 1 ? "" : "s") +
                 " behind on rent. Your access has been revoked until settled.");
      error_text("Make sure you have money in the bank.");
      error_text("Change your banking connection if you changed bank.\n\n");
      return;
   }
   if (behind > 1.2)
   {
      error_text("You are over " + to_int(behind) + " week" + (to_int(behind) == 1 ? "" : "s") +
                 " behind on rent. You will be locked out if this is not settled.");
   }

   // FIXME result = this_body()->move(owned_ship);

   if (env && env->is_exo_frame())
   {
      write("You bring your exo frame.\n");
      // FIXME
      // env->move(owned_ship);
   }

   this_body()->force_look();
   quit_menu_application(0);
}

void do_dock_ship(mapping ships, string number)
{
   int num = to_int(number);
   string docked_ship;
   int docking_completed;

   if (terminal->query_docking())
   {
      docked_ship = terminal->query_docking()->vfile;
      docking_completed = terminal->query_docking()->docking_time;
   }

   if (number == "q")
   {
      error_text("Docking procedure aborted.");
      this_body()->simple_action("$N $vabort the procedure.");
      return;
   }
   if (num <= 0 || num > sizeof(ships))
   {
      error_text("Invalid ship number. Docking aborted.");
      this_body()->simple_action("$N $vabort the procedure.");
      return;
   }
   TBUG(ships[num]);
   docked_ship = ships[num]->vfile;
   docking_completed = time() + random(60) + random(60);
   terminal->set_docking(docked_ship, docking_completed);
   write("Your " + ((class ship_info)ships[num])->type + " callsign " + ((class ship_info)ships[num])->name +
         " will be docked in " + time_to_string(docking_completed - time()) + ".");
   terminal->start_story();
   quit_menu_application(1);
}

void dock_ship()
{
   int count = 1;
   mapping ships = ([]);
   string docked_ship;
   int docking_completed;

   if (terminal->query_docking())
   {
      docked_ship = terminal->query_docking()->vfile;
      docking_completed = terminal->query_docking()->docking_time;
   }

   if (!sizeof(owned_ships))
   {
      error_text("You have no ships docked here.");
      return;
   }

   this_body()->simple_action("$N $vbegin a docking procedure.");
   write(menu_text("Ship Inventory"));
   printf(frame->accent("%-5.5s %-30.30s %-10.10s %-17.17s"), "[#]", "Ship Type", "Insignia", "Docked at");
   foreach (class ship_info si in owned_ships)
   {
      ships[count] = si;
      printf("<bld>%-5.5s %-30.30s<res> %-10.10s %-17.17s\n", "[" + count + "]", si->type, si->name,
             si->docked_at || (si->long_term ? "Long term parking" : "Unknown"));
      count++;
   }
   write("\n\n");

   modal_simple(( : do_dock_ship, ships:), "Ship # to dock (q to quit): ", 1);
}

void enter_ship()
{
   // FIXME
   if (owned_ships)
   {
      this_body()->simple_action(
          "$N $vpush a key on the docking terminal and the door opens briefly and $n $vslip into the building.");
      do_enter_ship();
   }
   else
   {
      error_text("Wait, where did you rental just go?\n");
   }
}

void release_ship(string answer)
{
   string name = lower_case(this_body()->query_name());
   mapping bills = SHIP_D->create_bills(1);
   int outstanding = 0;
   answer = lower_case(answer);
   if (answer != "yes" && answer != "y")
   {
      error_text("Aborted.");
      return;
   }
   terminal->clear_docking();
   quit_menu_application(0);

   /*
   if (!bills[name])
      outstanding = 0;
   else
      outstanding = bills[name];

   if (SHIP_D->settle_bill(name, outstanding))
   {
      SHIP_D->notify_owner(name, "Parking and docking fees paid " + outstanding + "crd. Docking cancelled.");

      FIXME SHIP_D->cancel_housing(this_body(), owned_ship);
      quit_menu_application(0);
   }
   else
   {
      SHIP_D->notify_owner(name, "Failed to settle  $" + outstanding + ". Lease not cancelled.");
      write("Not enough money in account to settle debts. Contact your bank.\n\nTry again later.\n");
      error_text("Cancelled");
   }
   */
}

void confirm_release()
{
   if (!terminal->query_docking())
   {
      error_text("No docking under way, and no ship currently docked.\n");
      return;
   }
   modal_simple(( : release_ship:), "Send the ship away? (y/n) ", 1);
}

void rent_property()
{
   string name = lower_case(this_body()->query_name());
   mapping bills = SHIP_D->create_bills(1);
   int outstanding = 0;

   if (!bills[name])
      outstanding = 0;
   else
      outstanding = bills[name];

   if (SHIP_D->settle_bill(name, outstanding))
   {
      SHIP_D->notify_owner(name, "Abacus housing rentals paid $" + outstanding + ". Ready for new leases.");
      // FIXME SHIP_D->set_owner(this_body(), ship_filename);
      write("All your bills have been settled.\nNew lease confirmed.\n\nHave a good day.\n");
      quit_menu_application(0);
   }
   else
   {
      SHIP_D->notify_owner(name, "Failed to settle  " + outstanding + "credits. Cannot lease new property.");
      write("Not enough money in account to settle debts. Contact your bank.\n\nTry again later.\n");
      error_text("Cancelled");
   }
}

void bank_statement()
{
   string bank = SHIP_D->query_bank(this_body());
   write("\n***You rental contract is tied to: " + upper_case(bank + " bank") + "\n\n");
   write(ACCOUNT_D->bank_statement(bank, "credit", this_body()) + "\n\n");
}

void terms_and_cond()
{
   string bank = SHIP_D->query_bank(this_body());
   error_text("TERMS & CONDITIONS FOR DOCKING AGREEMENTS FROM OMEGA DOCKS LTD.\n\n");
   write("These terms and conditions apply to all rentals by OMEGA DOCKS LTD.\n" +
         "towards the DOCKEE(user of the dock)\n\n" +
         "\ta.   The dock lease is billed from when docking is ordered until the DOCKEE leaves the\n" +
         "\t     docking area.\n\n" +
         "\t1b.  Any panels or technical parts of the dock area must be returned in a similar state.\n" +
         "\t    Damages will be assessed and added on top of rent.\n\n" +
         "\t1c. DOCKEE will not be able to undock without paying outstanding fees.\n\n" +
         "\t2.  The lease can be terminated at any time by the DOCKEE if remaining rent\n" +
         "\t    is paid in full at that point in time.\n\n" +
         "\t3.  A lease cannot be terminated by the DOCKEE if the remaining amount is not settled.\n\n" +
         "\t4.  4 OR MORE weeks outstanding rent by DOCKEE may cause the termination of contract,\n" +
         "\t    and contents of the dock will be sold to cover loss of rent.\n");
}

void change_bank(string *banks, string pick)
{
   int ipick = to_int(pick);
   if (!ipick || ipick > sizeof(banks) || ipick <= 0)
   {
      error_text("Invalid bank id.\n");
      return;
   }
   ipick--;
   SHIP_D->set_bank_connection(banks[ipick], this_body());
   write("Bank connection changed " + capitalize(banks[ipick]) + ".\n**Only 'credit' accepted as currency.\n\n");
}

void pick_a_bank()
{
   string *banks = keys(ACCOUNT_D->query_accounts(this_body()));
   int count = 1;
   if (!sizeof(banks))
   {
      error_text("You have no money in banks.");
      return;
   }
   write("You have money in the following banks:\n");
   foreach (string b in banks)
   {
      write("\t [" + count + "] " + capitalize(b) +
            " Bank - (Account #: " + ACCOUNT_D->fake_account_id(b, this_body()) + ")\n");
      count++;
   }
   modal_simple(( : change_bank, banks:), "Enter a number to pick that bank connection: ", 1);
}

void create(object t)
{
   if (!clonep())
      return;
   terminal = t;

   owned_ships = SHIP_D->query_owned_ship(this_body(), this_body()->query_location());

   toplevel = new_menu(menu_text(implode(explode(upper_case(this_body()->query_location()) + " DOCKS", ""), " ")));
   sub_seperator = new_seperator(menu_text("Safe, Fast, Precise"));
   main_seperator = new_seperator("    Enter your choice\n");
   quit_item = new_menu_item("Quit", ( : quit_menu_application, 0 :), "q");
   goto_main_menu_item = new_menu_item("Return to main menu", toplevel, "m");

   ship_info_item = new_menu_item("Show Ship Information", ( : show_ship_info:), "s", 1);

   add_menu_item(toplevel, sub_seperator);
   add_menu_item(toplevel, main_seperator);

   if (sizeof(owned_ships))
   {
      if (!terminal->query_docking())
      {
         dock_ship_item = new_menu_item("Dock a ship", ( : dock_ship:), "d");
         add_menu_item(toplevel, dock_ship_item);
      }
      else
      {
         dock_ship_item = new_menu_item("Enter a docked ship", ( : enter_ship:), "e");
         add_menu_item(toplevel, dock_ship_item);
      }
      release_ship_item = new_menu_item("Cancel your docking", ( : confirm_release:), "C");
      add_menu_item(toplevel, release_ship_item);
      statement_item = new_menu_item("Show Bank Statement", ( : bank_statement:), "b", 1);
      add_menu_item(toplevel, statement_item);
      change_bank_item = new_menu_item("Change Banking Connection", ( : pick_a_bank:), "c");
      add_menu_item(toplevel, change_bank_item);
   }

   purchase_ship_item = new_menu_item("Purchase ship", ( : rent_property:), "p");
   add_menu_item(toplevel, purchase_ship_item);

   terms_item = new_menu_item("Terms & Conditions", ( : terms_and_cond:), "t", 1);
   add_menu_item(toplevel, terms_item);
   add_menu_item(toplevel, quit_item);
   add_menu_item(toplevel, ship_info_item);
}

void start_menu()
{
   init_menu_application(toplevel);
}
