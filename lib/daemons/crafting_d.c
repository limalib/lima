/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** crafting_d.c -- Crafting, materials, salvage and repairs.
** This daemon manages materials, loot, salvage, upgrade and crafting.
** Originally made for PLEXUS mud, but adapted for Nuclear War MUD 2.
**
** Tsath 2019-10-14 - 2020-12-20
**
*/

inherit M_DAEMON_DATA;
// inherit M_DICE;

#define MATERIALS_CONFIG_FILE "/data/config/crafting-materials"
#define PRIV_NEEDED "Mudlib:daemons"
#define CRAFTING_ITEMS "/domains/std/crafting/"
#define REPAIR_FACTOR 10.0

nosave int *repair_values = ({10, 100, 1000, 10000, 100000});
nosave string *cache_names = ({});
nosave mapping cache_cats = ([]);
private
nosave string currency_type = DOMAIN_D->query_currency();

// Left here mostly as an example for understanding. The materials are defined and read from MATERIALS_CONFIG_FILE at
// start of this daemon.
private
nosave mapping special_mats = (
    ["metal":([128:"steel",
                 64:"darksteel", 32:"silver", 16:"gold", 8:"platinum", 4:"titanium", 2:"adamantine", 1:"orichalcum", ]),
        "wood":([128:"fir", 64:"pine", 32:"oak", 16:"cedar", 8:"larch", 4:"hemlock", 2:"ebony", 1:"bloodwood", ]), ]);

// 128, then 192 (128+64), 224 (192+32), etc. These are tried to the "special_mats" mapping.
// Make sure to update these sums if you change the array. These are used for random selection of power level.
// random(229), and then the number has to be 0..128 or 129..192 etc. Hence only 1:255 chance for level 8 item.
private
nosave int *mat_sums = ({128, 192, 224, 240, 248, 252, 254, 255});
private
nosave mapping crafting_recipes = ([]);
private
nosave mapping recipe_file = ([]);
private
nosave mapping crafting_stations = ([]);

/* Saved variables */
mapping materials = ([]);
mapping upgrade_mat_schemes = ([]);
int longest_material = 0;

/* Functions */

string *query_material_categories();

void load_crafting_recipes()
{
   string *files = get_dir(CRAFTING_ITEMS "*.c");
   string contents;
   string *ar_contents;
   string err;

   foreach (string item in files)
   {
      string crafting_str;
      object obj;
      if (err = catch (obj = load_object(CRAFTING_ITEMS + item)))
      {
         write("Error while loading crafting item:\n" + err + "\n");
         continue;
      }

      contents = read_file(CRAFTING_ITEMS + item);
      if (strlen(contents))
      {
         int found = 0;
         ar_contents = explode(contents, "\n");
         foreach (string line in ar_contents)
         {
            if (line[0..10] == "//CRAFTING:")
            {
               crafting_str = trim(line[11..]);
            }
            else if (line[0..11] == "// CRAFTING:")
            {
               crafting_str = trim(line[12..]);
            }
         }
      }

      if (!crafting_str)
      {
         write("Missing //CRAFTING: string for '" + item + "'.\n");
         continue;
      }

      recipe_file[crafting_str] = item;
      crafting_recipes[crafting_str] = obj->query_recipe();
      if (!crafting_stations[obj->query_station() || "unknown"])
         crafting_stations[obj->query_station() || "unknown"] = ({});
      crafting_stations[obj->query_station() || "unknown"] += ({crafting_str});
      //        TBUG("Adding " + crafting_str + " to " + obj->query_station());
      crafting_str = 0;
   }
   save_me();
}

mapping query_crafting_stations()
{
   return crafting_stations;
}

varargs mapping query_crafting_recipes(string station)
{
   mapping new_map = ([]);

   if (!crafting_stations[station])
      return ([]);

   foreach (
       string key in filter_array(keys(crafting_recipes), (
                                                              : member_array($1, crafting_stations[$(station)]) != -1
                                                              :)))
   {
      new_map[key] = crafting_recipes[key];
   }
   return new_map;
}

mapping query_crafting_recipe(string name)
{
   return crafting_recipes[name];
}

int valid_crafting_recipe(string name)
{
   return crafting_recipes[name] != 0;
}

mixed *query_all_special_mats()
{
   return ({special_mats, mat_sums});
}

mixed *query_special_mats(string category)
{
   return ({special_mats[category], mat_sums});
}

mapping query_upgrade_schemes()
{
   return upgrade_mat_schemes;
}

string *query_upgrade_scheme(string category, string material)
{
   if (upgrade_mat_schemes[category] && upgrade_mat_schemes[category][material])
      return upgrade_mat_schemes[category][material];
   return 0;
}

private
void refresh_cache()
{
   cache_names = ({});
   foreach (string category in keys(materials))
   {
      foreach (string m in materials[category])
      {
         cache_names += ({m});
         cache_cats[m] = category;
      }
   }
}

void clean_up()
{
   destruct();
}

int valid_category(string c)
{
   return member_array(c, keys(materials)) != -1;
}

string material_category(string m)
{
   return cache_cats[m];
}

int valid_material(string m)
{
   return member_array(m, cache_names) != -1;
}

int add_category(string category, string *m)
{
   if (valid_category(category))
      return 0;

   if (!materials[category])
      materials[category] = m;
   foreach (string mat in m)
      cache_names += ({category + " " + mat});
   save_me();
   return 1;
}

string *query_salvage_categories()
{
   return keys(materials);
}

int remove_category(string c)
{
   if (!valid_category(c))
      return 0;

   map_delete(materials, c);
   refresh_cache();
   save_me();
   return 1;
}

string *query_material_categories()
{
   return keys(materials);
}

string *query_materials(string category)
{
   return materials[category] || ({});
}

int query_rarity(string mat)
{
   return 1 + member_array(mat, query_materials(material_category(mat)));
}

object create_material(string m)
{
   object mat;

   if (recipe_file[m])
   {
      mat = new (CRAFTING_ITEMS + recipe_file[m]);
      return mat;
   }
   else if (!valid_material(m))
      return 0;

   if (!mat)
      mat = new (MATERIAL, m);
   return mat;
}

string material_below(string category, string material)
{
   string *mats = query_materials(category);
   int pos;
   if (!sizeof(mats))
      return 0;
   pos = member_array(material, mats);
   pos--;

   if (pos >= 0)
   {
      return mats[pos];
   }
   return 0;
}

string material_above(string category, string material)
{
   string *mats = query_materials(category);
   int pos;
   if (!sizeof(mats))
      return 0;
   pos = member_array(material, mats);
   pos++;

   if (pos < (sizeof(mats)))
      return mats[pos];
   return 0;
}

object create_material_for_me(string m)
{
   create_material(m)->move(this_body());
}

string salvage_description(object *s)
{
   string *parts = ({});

   if (!sizeof(s))
      return "nothing";

   foreach (object ob in s)
   {
      parts += ({ob->short()});
   }
   return tidy_list(parts);
}

int *chance_array(int cnt)
{
   int *pows = ({});
   int total;
   int c = 0;
   int sum = 30;

   for (int i = cnt; i > 0; i--)
   {
      pows += ({pow(2, i + 1)});
      total += pow(2, i + 1);
   }

   foreach (int i in pows)
   {
      int perc = to_int(round((0.0 + i) / total * 70));
      sum += perc;
      pows[c] = sum;
      c++;
   }

   pows = ({30}) + pows;

   return pows;
}

object *salvage_parts(object ob)
{
   mapping salvage = ob->query_salvageable();
   mapping direct_salvage = ob->query_direct_salvage();
   object *salvaged = ({});
   string picked_material;
   int s_roll = random(100) + 1;
   int *chance_ar;
   int total = 0;

   if (!direct_salvage || !sizeof(keys(direct_salvage)))
   {

      foreach (string key in keys(salvage))
      {
         if (picked_material)
            continue;
         total += salvage[key];
         if (s_roll <= total)
         {
            picked_material = key;
         }
      }

      chance_ar = chance_array(sizeof(materials[picked_material]));

      for (int i = 0; i < ob->query_salvage_level(); i++)
      {
         int index = -1;
         int chance = chance_ar[0];
         int cnt = 0;

         s_roll = random(100) + 1 + ob->query_salvage_level();
         if (s_roll > 100)
            s_roll = 100;

         while (s_roll > chance)
         {
            cnt++;
            chance = chance_ar[cnt];
            index++;
         }

         if (index >= 0)
            salvaged += ({create_material(materials[picked_material][index])});
      }

      // TBUG("Salvaged: "+sprintf("%O",salvaged));
   }
   else
   {
      foreach (string mat, int num in direct_salvage)
      {
         for (int i = 0; i < num; i++)
         {
            salvaged += ({create_material(mat)});
         }
      }
   }
   ob->remove();
   return salvaged;
}

void salvage_obj(object ob)
{
   object *salvaged = ({});
   string short = ob->short();   // Keep the short since the ob is gone after we salvage it.
   salvaged = salvage_parts(ob); // Ob no longer exists.
   if (sizeof(salvaged))
   {
      printf("You salvage %s from the %s.\n", salvage_description(salvaged), short);

      foreach (object mat in salvaged)
      {
         this_body()->add_material(mat);
      }
   }
   else
      printf("You salvage nothing from the %s.\n", short);
}

float money_to_repair(object ob)
{
   if (!ob)
      return 0;
   return ob->missing_durability() / REPAIR_FACTOR;
}

string repair_cost_string(object ob)
{
   return MONEY_D->currency_to_string(money_to_repair(ob), "dollars");
}

mapping estimate_objects(object player, object *obs)
{
   mapping pouch = copy(player->query_materials());
   mapping picked_mats = ([]);
   int remainder = 0;
   if (!arrayp(obs))
      obs = ({obs});

   foreach (object ob in obs)
   {
      mapping salvage = ob->query_salvageable();
      int missing_dura = ob->missing_durability();
      int rep_size = sizeof(repair_values) - 1;
      if (!salvage)
         continue;

      foreach (string mat in keys(salvage))
      {
         int needed = salvage[mat] * missing_dura / 100;
         //            TBUG("Salvage[" + mat + "] needed: " + needed);
         if (!arrayp(picked_mats[mat]))
            picked_mats[mat] = ({0, 0, 0, 0, 0});
         for (int i = rep_size; i >= 0; i--)
         {
            if (repair_values[i] < needed)
            {
               int picked;
               picked = CLAMP(needed / repair_values[i], 0, pouch[materials[mat][i]]);
               picked_mats[mat][i] += picked;
               needed -= picked * repair_values[i];
               //                  TBUG("Now picked " + picked + " of " + materials[mat][i] + ". Needed now: " +
               //                  needed);
               pouch[materials[mat][i]] -= picked;
               if (i == 0 && needed != 0 && picked_mats[mat][i] < pouch[materials[mat][i]])
               {
                  //                        TBUG("Added one extra " + materials[mat][i] + " to cover the remainder (" +
                  //                        needed + ").");
                  picked_mats[mat][i]++;
                  needed = 0;
               }
            }
         }
         remainder += needed;
         //            TBUG("Remainder: " + remainder);
      }
   }
   picked_mats["money"] = remainder / REPAIR_FACTOR;
   return picked_mats;
}

int pay_for_repair_with_money(object player, mixed obs)
{
   float total_cost = 0;
   int all_good = 1;
   mapping money;

   if (objectp(obs))
      obs = ({obs});

   foreach (object ob in obs)
   {
      total_cost += ob->missing_durability() / REPAIR_FACTOR;
   }

   if (total_cost && player->query_amt_currency(currency_type) > total_cost)
   {

      money = MONEY_D->handle_subtract_money(player, total_cost, currency_type);
      player->my_action(
          "$N $vrepair for " + MONEY_D->currency_to_string(total_cost, currency_type) + ". $N $vgive " +
          MONEY_D->currency_to_string(money[0], currency_type) + ", " +
          (sizeof(money[1]) ? " and get " + MONEY_D->currency_to_string(money[1], currency_type) + " as change" : "") +
          ".\n");
      all_good = 1;
   }

   if (all_good)
   {
      obs->durability_after_repair();
      obs->remove_adj("tattered");
   }
   return all_good;
}

int pay_for_repair(object player, mixed obs)
{
   mapping estimate = estimate_objects(player, obs);
   mapping pouch = player->query_materials();
   int all_good = 1;
   mapping money;
   int rep_size = sizeof(repair_values) - 1;

   foreach (string mat in keys(estimate))
   {
      if (mat == "money" || !all_good)
         continue;
      for (int i = rep_size; i >= 0; i--)
      {
         string material;
         if (i + 1 > sizeof(materials[mat]))
            continue;
         TBUG("mat: " + mat + " i: " + i);
         material = materials[mat][i];
         if (!player->remove_material(material, estimate[mat][i]))
         {
            TBUG("Could not find " + material + " / " + estimate[mat][i]);
            all_good = 0;
         }
      }
   }
   if (estimate["money"] && player->query_amt_currency(currency_type) > (estimate["money"] / REPAIR_FACTOR))
   {
      money = MONEY_D->handle_subtract_money(player, estimate["money"], currency_type);
      player->my_action(
          "$N $vrepair for " + MONEY_D->currency_to_string(estimate["money"], currency_type) + ". You hand over " +
          MONEY_D->currency_to_string(money[0], currency_type) + "" +
          (sizeof(money[1]) ? " and get " + MONEY_D->currency_to_string(money[1], currency_type) + " as change" : "") +
          ".\n");
      all_good = 1;
   }

   if (all_good)
   {
      obs->durability_after_repair();
      obs->remove_adj("tattered");
   }
   return all_good;
}

varargs string *buy_durability(mixed m, int rest)
{
   if (intp(m) || floatp(m))
      return MONEY_D->money_to_array((m - rest) / REPAIR_FACTOR);
   if (objectp(m))
      return MONEY_D->money_to_array((m->missing_durability() - rest) / REPAIR_FACTOR);
   if (arrayp(m))
   {
      float total_dura = 0.0;
      foreach (object ob in m)
         total_dura += ob->missing_durability();
      total_dura -= (rest * REPAIR_FACTOR);
      return ({MONEY_D->currency_to_string(total_dura / REPAIR_FACTOR, currency_type)});
   }
}

mixed estimate_with_mats(object player, mixed obs)
{
   string returnStr = "";
   int possible = 1;
   int mats_used = 0;
   mapping picked_mats = estimate_objects(player, obs);
   mapping pouch = player->query_materials();
   int rep_size = sizeof(repair_values) - 1;
   if (objectp(obs))
      obs = ({obs});

   returnStr += "Repair estimate for: ";
   foreach (object ob in obs)
   {
      returnStr += capitalize(ob->short()) + ", ";
   }
   returnStr = returnStr[0.. < 3] + "\n\n";

   foreach (string mat in keys(picked_mats))
   {
      string mat_str = "";
      if (mat == "money")
         continue;
      for (int i = rep_size; i >= 0; i--)
      {
         if (picked_mats[mat][i])
         {
            mat_str += "[" + picked_mats[mat][i] + "/" + pouch[materials[mat][i]] + "] " + materials[mat][i] +
                       (picked_mats[mat][i] != 1 ? "s" : "") + ", ";
            mats_used = 1;
         }
      }

      if (strlen(mat_str))
      {
         returnStr += sprintf("%-5s: ", capitalize(mat)) + mat_str;
         returnStr = returnStr[0.. < 3] + "\n";
      }
   }

   if (picked_mats["money"] && player->query_amt_currency(currency_type) > (picked_mats["money"] / REPAIR_FACTOR))
      returnStr += "Money: " + MONEY_D->currency_to_string(picked_mats["money"], currency_type) + ".\n";
   if (picked_mats["money"] && player->query_amt_currency(currency_type) < (picked_mats["money"] / REPAIR_FACTOR))
   {
      returnStr +=
          "Money: " + format_list(buy_durability(picked_mats["money"])) + " %%^RED%%^(You lack money!)%%^RESET%%^\n";
      possible = 0;
   }

   returnStr +=
       (mats_used ? "(You save " + format_list(buy_durability(obs, picked_mats["money"])) + " by using materials)"
                  : "(Salvage weapons and armours to get materials for cheaper repairs)") +
       "\n\n";

   return ({possible, returnStr});
}

int add_upgrade_scheme(string category, string special_material, string *recipes)
{
   int halt = 0;

   if (!valid_category(category))
   {
      write("Invalid salvage category '" + category + "'.\n");
      return 0;
   }

   if (member_array(special_material, values(special_mats[category])) == -1)
   {
      write("Invalid special material for " + category + ": '" + special_material + "'.\n");
      return 0;
   }

   if (!sizeof(recipes))
   {
      write("Invalid empty recipe list for " + category + ".\n");
      return 0;
   }

   foreach (string recipe in recipes)
   {
      if (!query_crafting_recipe(recipe))
      {
         write("Unknown recipe '" + recipe + "' for category '" + category + "' material '" + special_material +
               "'.\n");
         halt = 1;
      }
   }
   if (halt)
      return;

   if (!upgrade_mat_schemes[category])
      upgrade_mat_schemes[category] = ([]);
   upgrade_mat_schemes[category][special_material] = recipes;

   save_me();
   return 1;
}

private
int strtype(string category)
{
   int i = -1;
   string tmp;

   if (category[0..6] == "quality")
      i = 1;
   else if (strsrch(category, "/") != -1)
      i = 2;
   else if (sscanf(category, "[%s]", tmp) == 1)
      i = 0;

   return i;
}

int query_longest_mat()
{
   return longest_material;
}

void load_config_from_file()
{
   int errors_found = 0;
   mapping materials = ([]);
   mapping special_mats = ([]);
   mapping upgrade_mat_schemes = ([]);
   string *material_input;

   if (!sizeof(stat(MATERIALS_CONFIG_FILE)))
      error("Critical error: Missing config file '" + MATERIALS_CONFIG_FILE + "'.");

   material_input = explode(read_file(MATERIALS_CONFIG_FILE), "\n");

   foreach (string line in material_input)
   {
      string category, tmp, tmp2, *mats, *tmpar;
      int count, linetype;
      mapping tmpmap = ([]);

      // Skip comments.
      if (line[0] == '#' || sscanf(line, "%s:%s", category, tmp) != 2)
         continue;
      category = trim(category);
      mats = explode(tmp, ",");
      mats -= ({""});
      count = sizeof(mats) - 1;

      // Figure out what kind of linetype we have
      linetype = strtype(category);

      // Trim all the parts of the array
      mats = map(mats, ( : trim($1) :));

      switch (linetype)
      {
      case 0:
         // Materials
         category = category[1.. < 2]; // Strip the brackets.
         add_category(category, mats);
         foreach (string m in mats)
            if (strlen(m) > longest_material)
               longest_material = strlen(m);
         break;
      case 1:
         // Qualities
         foreach (string m in mats)
         {
            tmpmap[pow(2, count)] = m;
            count--;
         }
         special_mats[category] = tmpmap;
         break;
      case 2:
         // Upgrade schemes
         if (sscanf(category, "%s/%s", tmp, tmp2) != 2)
         {
            write("Bad format for: " + category);
            errors_found++;
         }
         else if (!add_upgrade_scheme(tmp, tmp2, mats))
            errors_found++;
         break;
      default:
         error("Unknown type for '" + category + "'.");
         errors_found++;
      }
   }

   if (!errors_found)
   {
      write(__FILE__ + ": " + MATERIALS_CONFIG_FILE + " loaded.");
      write("Saved.");
      save_me();
   }
   else
      write(__FILE__ + ": Not saved. " + errors_found + " errors while loading, see messages above.");
}

int count_materials()
{
   return array_sum(values(map_mapping(materials, ( : sizeof($2) :))));
}

void stat_me()
{
   foreach (string category in keys(materials))
   {
      write("%^BOLD%^Category: %^RESET%^%^YELLOW%^" + category + "%^RESET%^\n");
      write(" " + format_list(materials[category]));
      write("\n");
   }
   write("\n\n%^BLUE%^Crafting recipes:%^RESET%^ \n");
   write("  " + implode(sort_array(keys(crafting_recipes), 1), "\n  "));
   write("\n");

   write("\n%^BLUE%^Upgrade schemes: %^RESET%^\n");
   foreach (string name, mapping m in upgrade_mat_schemes)
   {
      write("Category: " + name + "\n");
      foreach (string name2, string * sar in m)
      {
         write(" [" + name2 + "]\n\t" + implode(sar, ",") + "\n");
      }
      write("\n");
   }
}

void create()
{
   ::create();
   load_crafting_recipes();
   load_config_from_file();
   refresh_cache();
}

//@CRAFTING_D->remove_category("plastic")
//@CRAFTING_D->add_category("plastic",({"soft pvc", "molded plastic", "fiberglass spool","ethylene granulate"}))