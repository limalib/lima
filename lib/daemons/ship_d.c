/*
** Ship_d for handling player ship.
** Tsath, 2023
**
** This works in conjunction with
**    /std/ship
**    ^common/item/docking_terminal
**    /obj/mudlib/docking_menu
**
*/

inherit M_DAEMON_DATA;
inherit SPACE_CLASSES;

// How often do we check if we should bill people
#define BILL_CYCLE_HOURS 2
// Default bank used if nothing custom set for player
#define DEFAULT_BANK "omega"
// Number of seconds per wek
#define WEEK 604800
// Ship states, don't touch
#define SHIP_PERSISTED 1
#define SHIP_DIRTY 2

mapping ship_locations = ([]);
mapping owners = ([]);
mapping bank_connection = ([]);
mapping last_payment = ([]);
mapping debt = ([]);
int next_id = 0;
int money_made;
int money_lost;

string ship_name(string type)
{
   return chr(random(25) + 65) + chr(random(25) + 65) + chr(random(25) + 65) + "-" + random(10000);
}

// This function begins a new lease. The file must the base_name of the file of
// the ship.
varargs int set_owner(object owner, string file, string sname, string ss, string loc, string da, int lt)
{
   string name = lower_case(owner->query_name());
   class ship_info si = new (class ship_info);
   object ship_object = load_object(file);

   if (!owners[name])
      owners[name] = ({});

   if (!ship_object)
      return 0;

   si->vfile = file + "/" + name + "/" + next_id;
   si->type = ship_object->query_ship_type();
   si->name = sname || ship_name(si->type);
   si->starsystem = ss || "Omega";
   si->location = loc || "Omega Terminal";
   si->docked_at = da;
   si->long_term = lt || 1;

   next_id++;
   owners[name] += ({si});
   save_me();
   return 1;
}

// Cancels a ship, no questions asked. Any checks if this is allowed must be
// asked before this function.
int cancel_ship(object owner, string ship)
{
   string name = lower_case(owner->query_name());
   string save_file;
   if (!owners[name])
      return 0;
   owners[name] -= ({ship});
   if (!sizeof(!owners[name]))
      map_delete(owners, name);
   save_file = sprintf("/data/ship/%c/%s.o", name[0], replace_string(ship[1..], "/", "_"));
   TBUG(save_file);
   unguarded(1, ( : rm, save_file:));
   save_me();
   return 1;
}

// Sets the connection the player wants to use as a bank.
void set_bank_connection(string bank, object player)
{
   string name = lower_case(player->query_name());
   bank_connection[name] = bank;
   save_me();
}

// A quick notification to the owner via his pager if he has one. If not, tough.
void notify_owner(mixed who, string s)
{
   object body = objectp(who) ? who : find_body(who);
   if (body && present("guild_vdu_ob", body))
   {
      tell(body, "Your VDU says, \"<048>" + upper_case(s) + ".<res>\".\n");
   }
}

// Return all the bank connections we have. Mostly for debug purposes.
mapping query_bank_connections()
{
   return bank_connection;
}

// Returns the bank connection of a player. If no connection is set, set it to
// DEFAULT_BANK.
string query_bank(object player)
{
   string name = lower_case(player->query_name());
   if (!bank_connection[name])
      bank_connection[name] = DEFAULT_BANK;
   return bank_connection[name];
}

// Sets the entry point to each ship. This is used by the ship terminals
// to figure out what department resides behind it.
int add_ship_location(string entry, string file)
{
   ship_locations[entry] = file;
   save_me();
}

string ship_filename(string virtual_path)
{
   return "/" + implode(explode(virtual_path, "/")[0.. < 3], "/");
}

// Returns the full name of any ship owned by owner at the location.
varargs class ship_info *query_owned_ship(object owner, string location)
{
   string lcname = lower_case(owner->query_name());
   class ship_info *infos = ({});
   if (!lcname || !owners[lcname])
      return 0;

   foreach (class ship_info si in owners[lcname])
   {
      if (si && strsrch(si->location, location || owner->query_location()) == 0)
         infos += ({si});
   }
   return infos;
}

// Returns all ship location pairs.
mapping query_ship_locations()
{
   return ship_locations;
}

// Returns the ship filename based on the street location. Basically a single
// query version of query_ship_locations().
string query_ship_filename(string entry)
{
   return ship_locations[entry];
}

// This function handles persistence of ships if needed and removal of the
// in-memory ship objects. It tracks the state of the ship so it know if it
// should remove and save or just remove.
void attempt_clean_up(object ship)
{
   if (!ship)
      return;
   if (ship->query_state() == SHIP_DIRTY)
      return;
   if (ship->is_ship_empty() && ship->query_state() == SHIP_PERSISTED)
      ship->remove();
   else
      call_out("attempt_clean", 60, ship);
}

// The actual saving of the ship is begun from here due to permissions on each
// single ship file is not high enough to initiate a save to /data/ship/
// (permission denied).
void save_ship_state(object ship)
{
   string ship_save = ship->save_to_string();
   ship->unguarded_save();
   call_out("attempt_clean_up", 60, ship);
}

// This functions saved that body paid his bill. The 'time' parameter is
// optional, and if none given, NOW is assumed as time(). Can be given parameter
// to back-date the payment.
varargs void set_pay_bill(object body, int time)
{
   string name = lower_case(body->query_name());
   last_payment[name] = time > 0 ? time : time();
   save_me();
}

// Debug function for seeing what's inside the last payment mapping.
mapping query_last_payments()
{
   return last_payment;
}

// When a player reenters their ship, we need to restore the state from disk.
// Much like saving, the ships have no permission to do this, so the SHIP_D
// initiates the restore.
void restore_ship_state(object ship)
{
   string rfile;

   rfile = read_file(ship->save_to());
   if (rfile)
      ship->restore_ship(rfile);
}

// Returns the mapping of owners and what they own. The key is the owner name in
// lower case, and the value is an array of string ship IDs.
// i.e. ({ "/domains/nuke/ship/small_apartment/tsath/42" })
mapping query_owners()
{
   return owners;
}

/*
void clear_data()
{
    owners = ([]);
    next_id = 0;
}
*/

// Returns a factor of how many fractional weeks we're missing payments for. So
// 1.5 means we're missing payments for 1 week and 3.5 days.
float missing_payments(string name)
{
   int last_pay = last_payment[name];
   int now = time();
   float diff = ((now - last_pay) * 1.0) / WEEK;
   return diff;
}

/*
    This function creates a mapping of name : amount pairs for all the money
    owed by the leasers.

    This only works if bills are settled before renting new ships or when
    leaving the lease. It assumes all properties are held since last payment,
    which means property changes must cause a settlement of some kind. Give it
    the flag settle_all=1 to pay every last penny owned.
*/
varargs mapping create_bills(int settle_all)
{
   mapping ship_cost = ([]);
   mapping bills = ([]);
   string *ship_files = get_dir("/domains/common/ship/*.c");

   foreach (string ship in ship_files)
   {
      object ship_ob = load_object("/domains/common/ship/" + ship);
      if (!ship_ob)
         continue;

      ship_cost[base_name(ship_ob)] = ship_ob->query_ship_cost();
   }

   foreach (string who, string * ships in owners)
   {
      float miss = missing_payments(who);
      // Postpone payments if a week didn't pass
      if (miss < 1.0 && !settle_all)
         continue;
      foreach (class ship_info ship in ships)
      {
         string ship_base;
         int cost;
         ship_base = "/" + implode(explode(ship->vfile, "/")[0.. < 3], "/");
         cost = ship_cost[ship_base];
         if (!cost)
            TBUG("Missing cost for bill on " + ship->name + " for " + who);
         bills[who] += cost;
      }
      bills[who] *= miss;
      if (!bills[who])
         map_delete(bills, who);
   }
   return bills;
}

// This function attempts to settle a bill for 'name' of 'amount'. It returns 0
// or 1 depending on the success of this. If the person has no default bank, we
// set it to abacus.
int settle_bill(string name, int amount)
{
   string bank = bank_connection[name];
   int account_value;

   // Set a default bank, if player has none
   if (!bank)
   {
      bank = DEFAULT_BANK;
      bank_connection[name] = bank;
   }

   // How much do we have in the bank
   account_value = ACCOUNT_D->query_account(bank, name, "credit");
   if (account_value < amount)
      return 0;

   // Do the withdrawal and update our payment info.
   ACCOUNT_D->withdraw(bank, name, amount, "Abacus Ship, rental");
   money_made += amount;
   last_payment[name] = time();
   save_me();
   return 1;
}

//  This function runs every BILL_CYCLE_HOURS hours in a call_out. It collects
//  money if people are above 1 week and sends them a pager notification on what
//  happened.
void schedule_bills()
{
   // Simple create_bills(0) so we just get whoever needs to pay after this week.
   mapping bills = create_bills();
   call_out("schedule_bills", (3600 * BILL_CYCLE_HOURS));

   // Iterate through them
   foreach (string who, int amount in bills)
   {
      // If for some reason no amount? Skip
      if (!amount)
         continue;

      // Try to settle the bill
      if (settle_bill(who, amount))
         notify_owner(who, "Abacus ship rentals paid $" + amount + ". All leases extended.");
      else
         notify_owner(who, "Failed to pay $" + amount + " for leases. Find a ship terminal.");
   }
}

int late_payments(string who)
{
   // Simple create_bills(0) so we just get whoever needs to pay after this week.
   mapping bills = create_bills();
   int last_pay;
   if (!bills[who])
      return 0;

   last_pay = (time() - last_payment[who]) / WEEK;
   return last_pay;
}

string stat_me()
{
   string retstr;
   retstr = "Ship daemon stats\n--------------------\n";
   retstr +=
       sprintf("%25s: %-5s %25s: %-5s\n", "Ships owned", sizeof(flatten_array(values(SHIP_D->query_owners()))) + "",
               "Customers ever", sizeof(keys(SHIP_D->query_bank_connections())) + "");
   retstr += sprintf("%25s: %-5s %25s: %-5s\n", "Money made", "$ " + money_made, "Money lost ", "" + "$ " + money_lost);
   retstr += sprintf("%25s: %-5s %25s: %-5s\n", "Outstanding money", "$ " + array_sum(values(create_bills(1))),
                     "Money lost ", "" + money_lost);

   return retstr;
}

// Initial creation of this daemon. Starts the call_out() and sets privilges.
void create()
{
   ::create();
   set_privilege("Mudlib:daemons");
   call_out("schedule_bills", (3600 * BILL_CYCLE_HOURS));
}

/*
@filter_array(objects(),(: strsrch(base_name($1),"small_apartment/")!=-1 :))->remove()
goto small_apartment/tsath/1
more /data/ship/t/domains_nuke_ship_small_apartment_tsath_1.o
ud `SHIP_D` small_apartment
*/