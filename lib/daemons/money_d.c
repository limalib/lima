/* Do not remove the headers from this file! see /USAGE for more info. */

//: MODULE
// The money daemon manages the legal types of money within the game and their
// exchange rates.
//
// The money system consists of currencies and denominations.
// 
// Different currencies can only be exchanged at a bank (for a fee) and
// vendors only use one currency.  You can pay with any of the
// denominations of the vendor's currency though and you get change if
// needed.  If you want to drop or give money to somebody you can only
// use the denominations which you really have though.
// 
// Each currency consists of its name, the plural name, a list of
// denominations and an exchange rate.  By default a currency has a base
// denomination with the same name and the exchange rate factor 1.0.  The
// exchange rate is the value of one unit of a denomination of a currency
// with factor 1.0.  For example if the exchange rate of dollars is 1000
// then 1 dollar is worth as much as an object with set_value(1000).
// 
// Each denomination consists of its name, the plural name to use, and an
// exchange rate factor relative to the base denomination of the currency
// (e.g. the base denomination has the factor 1.0).  A denomination cent
// would have the factor 0.01 for example which means 1 cent is worth as
// much as an object with set_value(10) (if the exchange rate of dollars
// is 1000).  Each denomination belongs to only one currency.
// 
// Currencies are administrated with admtool at daemons/currency.
// 
// When a new currency is created in admtool (by "add currency" or "add a
// denomination to a currency") the exchange rate is set to 1000.  If
// "add currency" a base denomination is added.  If you want to change
// some property of a denomination or the plural of a currency just add
// it again with the same name.
// 
// Objects have an inherent "value".  This is then translated into a
// particular currency via that currency's value.actual exchange rate.
//
// Currencies have different denomiations, e.g. dollar have dollar and cent.
// Calculation is always based on the lowest denomination. Functions for
// displaying a currency with denominations are provided


// Created Wed Jul  3 20:28:42 MET DST 1996 <Valentino>
//
// 961209, Deathblade: Revised extensively and installed into Lima.
//                     Kept Valentino's per-area currencies, but tossed
//                     the materials concept.
//
// Rewritten 10 Feb 98 by MonicaS, removed per-area currencies, added denominations
//
// Tsath 2023: Modifed to handle floating currencies, so base currency doesn't have
// to be pennies, but pennies can be 0.01 dollar, e.g.

inherit M_DAEMON_DATA;

class denomination
{
   float factor;
   string plural;
   string currency;
}

// Mapping of currency names to unit values.  A given currency is worth
// this many "units," which is used for conversion purposes.
private mapping rates = ([]);

// Mapping of currency names to plural of the names
private
mapping plurals = ([]);

// Mapping of currency names to sorted array of denomination names
private
mapping denominations = ([]);

// Mapping of denomination name to class denomination
private
mapping denomination = ([]);

// Mapping of plural names to singular names
nosave mapping singulars = ([]);

//: FUNCTION singular_name
// singular_name returns the canonical name for a denomination/currency
// e.g. lover case, without spaces and singular.
string singular_name(string name)
{
   if (!name)
      return;
   name = lower_case(trim(name));
   if (singulars[name])
      name = singulars[name];
   return name;
}

//: FUNCTION query_exchange_rate
// Returns the exchange rate to map from an object's inherent value to its
// actual value in a particular currency.  The value returned is the number
// of units this currency is worth.
nomask int query_exchange_rate(string type)
{
   return rates[singular_name(type)];
}

//: FUNCTION query_currency_types
// Returns the names of the currencies that are valid within the mud
nomask string *query_currency_types()
{
   return keys(denominations);
}

//: FUNCTION query_denominations
// Returns the array of denominations of a currency
// or all available denominations
varargs nomask string *query_denominations(string type)
{
   if (type)
   {
      return denominations[singular_name(type)];
   }
   else
      return keys(denomination);
}

//: FUNCTION is_currency
// tests if name is a currency
nomask int is_currency(string name)
{
   return denominations[singular_name(name)] != 0;
}

//: FUNCTION is_denomination
// tests if name is a denomination
nomask int is_denomination(string name)
{
   return denomination[singular_name(name)] != 0;
}

//: FUNCTION query_currency
// Returns the currency of a denomination
nomask string query_currency(string name)
{
   name = singular_name(name);
   if (!denomination[name])
      error("unknown denomination: " + name + ".\n");
   return denomination[name].currency;
}

//: FUNCTION query_plural
// Returns the plural name of a denomination
nomask string query_plural(string name)
{
   name = singular_name(name);
   if (denomination[name])
      return denomination[name].plural;
   if (denominations[name]) // test if currency
      if (plurals[name])
         return plurals[name];
      else
         return M_GRAMMAR->pluralize(name);
   error("unknown denomination/currency: " + name + ".\n");
}

//: FUNCTION query_factor
// Returns the exchange rate factor of a denomination
nomask float query_factor(string name)
{
   name = singular_name(name);
   if (!name || !denomination[name])
      error("unknown denomination: " + name + ".\n");
   return denomination[name].factor;
}

//: FUNCTION add_currency
// Add a currency to the money system. The base denomination with the same
// name will be created too if flag is 0.
varargs nomask void add_currency(string type, string plural, int flag)
{
   if (!check_privilege("Mudlib:daemons"))
      error("illegal attempt to add a currency.\n");
   type = lower_case(trim(type));
   if (plural)
      plural = lower_case(plural);
   else
      plural = M_GRAMMAR->pluralize(type);
   plurals[type] = plural;
   singulars[plural] = type;
   // if the currency already exists only set plural
   if (!denominations[type])
   {
      rates[type] = 1000;
      if (!flag)
      {
         denomination[type] = new (class denomination, currency : type, plural : plural, factor : 1.0);
         denominations[type] = ({type});
      }
   }
   save_me();
}

//: FUNCTION remove_currency
// Removes a currency from the money system.
nomask void remove_currency(string type)
{
   if (!check_privilege("Mudlib:daemons"))
      error("illegal attempt to add a currency.\n");
   type = singular_name(type);
   if (!denominations[type])
      error("illegal currency type: " + type + "\n");
   foreach (string name in denominations[type])
   {
      map_delete(denomination, name);
   }
   map_delete(denominations, type);
   map_delete(rates, type);
   map_delete(singulars, plurals[type]);
   map_delete(plurals, type);
   save_me();
}

//: FUNCTION set_exchange_rate
// Set the exchange rate (that is the value) of a currency
nomask void set_exchange_rate(string type, int rate)
{
   if (!check_privilege("Mudlib:daemons"))
      error("illegal attempt to adjust exchange rate.\n");
   type = singular_name(type);
   if (!denominations[type])
      error("illegal currency type: " + type + "\n");
   if (rate == 0)
      remove_currency(type);
   else
      rates[type] = rate;
   save_me();
}

// denominations have to be sorted in reverse order, so that evaluation
// in currency_to_string() works.
private
int compare_denominations(string d1, string d2)
{
   if (denomination[d1].factor > denomination[d2].factor)
      return -1;
   else
      return (denomination[d1].factor < denomination[d2].factor);
}

//: FUNCTION add_denomination
// add a denomination to a currency
void add_denomination(string type, string name, string plural, float factor)
{
   if (!check_privilege("Mudlib:daemons"))
      error("illegal attempt to add a denomination.\n");
   type = singular_name(type);
   name = lower_case(trim(name));
   if (denomination[name] && denomination[name].currency != type)
      error("denomination " + name + " already exists in currency " + denomination[name].currency + "\n");
   plural = lower_case(plural);
   singulars[plural] = name;
   denomination[name] = new (class denomination, currency : type, plural : plural, factor : factor);
   if (!denominations[type])
   {
      if (type == name)
         add_currency(type, plural, 1);
      else
         add_currency(type, 0, 1);
      denominations[type] = ({name});
   }
   else
   {
      denominations[type] |= ({name});
      denominations[type] = sort_array(denominations[type], ( : compare_denominations:));
   }
   save_me();
}

//: FUNCTION remove_denomination
// removes a denomination from a currency
void remove_denomination(string name)
{
   string type;

   if (!check_privilege("Mudlib:daemons"))
      error("illegal attempt to removes a denomination.\n");
   name = singular_name(name);
   if (!denomination[name])
      error("unknown denomination: " + name + "\n");
   type = denomination[name].currency;
   if (sizeof(denominations[type]) == 1)
      remove_currency(type);
   else
   {
      denominations[type] -= ({name});
      if (!denominations[name])
         map_delete(singulars, denomination[name].plural);
      map_delete(denomination, name);
      save_me();
   }
}

//: FUNCTION denomination_to_string
// create a string with correct use of plural from an amount of a denomination.
nomask string denomination_to_string(int amount, string type)
{
   type = singular_name(type);
   amount = to_int(amount);
   if (!denomination[type])
      error("illegal denomination: " + type + "\n");
   if (amount > 1)
      return amount + " " + denomination[type].plural;
   else
      return amount + " " + type;
}

//: FUNCTION calculate_denominations
// calculate denominations which add up to a certain amount.
mapping calculate_denominations(float f_amount, string currency)
{
   mapping money = ([]);
   int amount;

   currency = singular_name(currency);
   if (!denominations[currency])
      error("illegal currency type: " + currency + "\n");
   // avoid rounding errors
   f_amount += denomination[denominations[currency][ < 1]].factor * 0.1;
   foreach (string type in denominations[currency])
   {
      amount = to_int(f_amount / denomination[type].factor);
      if (amount)
      {
         f_amount -= to_float(amount) * denomination[type].factor;
         money[type] = amount;
      }
   }
   return money;
}

//: FUNCTION currency_to_string
// create a string with denominations from an amount of money.
// The money is a mapping from denomination to amount or a float.
// If the currency is not 0 only money of that type of currency is regarded.
// The output is only sorted if you specify the currency.
varargs nomask string currency_to_string(mixed money, string currency)
{
   int amount;
   string name, str = "";
   string *types;

   if (currency)
   {
      currency = singular_name(currency);
      if (!denominations[currency])
         error("illegal currency type: " + currency + "\n");
      if (!mapp(money))
         money = calculate_denominations(money, currency);
      types = denominations[currency];
   }
   else
   {
      types = keys(money);
   }

   foreach (name in types)
   {
      if (amount = money[name])
      {
         if (str != "")
            str += ", ";
         if (amount > 1)
            str += amount + " " + denomination[name].plural;
         else
            str += amount + " " + name;
      }
   }
   if (str == "")
      if (currency)
         str = "no " + query_plural(currency);
      else
         str = "no money";
   return str;
}

//: FUNCTION handle_subtract_money
// substracts an amount of currency from a player and adds change.
// returns an array of two mappings: substract and change, which
// consist of the denominations which were used.
mapping *handle_subtract_money(object player, float f_amount, string type)
{
   mapping substract = ([]), change;
   int amount;

   type = singular_name(type);
   if (is_currency(type))
   {

      mapping money = player->query_money() || ([]);
      string *types = denominations[type];
      string currency = type;
      for (int i = sizeof(types) - 1; i >= 0; i--)
      {
         type = types[i];
         amount = to_int(f_amount / denomination[type].factor + 0.9999);
         if (amount <= money[type])
         {
            substract[type] = amount;
            f_amount -= to_float(amount) * denomination[type].factor;
            break;
         }
      }
      if (f_amount > 0)
      {
         foreach (type in types)
         {
            amount = to_int(f_amount / denomination[type].factor + 0.9999);
            if (amount > money[type])
            {
               substract[type] = money[type];
               f_amount -= to_float(money[type]) * denomination[type].factor;
            }
            else
            {
               substract[type] = amount;
               f_amount -= to_float(amount) * denomination[type].factor;
               break;
            }
         }
      }
      change = calculate_denominations(-f_amount, currency);
   }
   else
   {
      string currency = query_currency(type);
      amount = to_int(f_amount + 0.9999);
      substract[type] = amount;
      f_amount = (to_float(amount) - f_amount) * denomination[type].factor;
      change = calculate_denominations(f_amount, currency);
   }
   TBUG(amount + f_amount);
   player->subtract_money(type, (amount + f_amount));
   foreach (type, amount in substract)
   {
      // Because of the floats we sometimes get rounding errors,
      // so let's clean up a little.
      if (change[type])
      {
         change[type]--;
         substract[type]--;
      }
   }
   return ({substract, change});
}

string money_string(object body)
{
   string money_info = "";
   mapping curr;
   int i;
   curr = body->query_money() || ([]);
   if (!sizeof(keys(curr)))
   {
      money_info += "You are carrying no money.\n";
   }
   else
   {
      foreach (string key, int amount in curr)
      {
         mapping money = MONEY_D->calculate_denominations(amount, key);
         string *denom_order = MONEY_D->query_denominations(key);
         string *money_str = ({});
         int j = 0;
         while (j < sizeof(denom_order))
         {
            string denom = denom_order[j];
            int count = money[denom];
            if (count)
               money_str += ({count + " " + denom + (count == 1 ? "" : "s")});
            j++;
         }
         if (sizeof(curr) > 1)
            money_info += sprintf("%s: %s", capitalize(key), format_list(money_str) + "\n");
         else
            money_info += sprintf("%s", format_list(money_str) + "\n");
      }
   }
   return money_info;
}

create()
{
   ::create();
   restore_me();
   foreach (string type, string plural in plurals)
   {
      singulars[plural] = type;
   }
   foreach (string type, class denomination d in denomination)
   {
      singulars[d.plural] = type;
   }
}

void clean_up()
{
   destruct();
}
