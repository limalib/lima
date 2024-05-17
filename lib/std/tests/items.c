/* Do not remove the headers from this file! see /USAGE for more info. */

inherit M_TEST_SUITE;

object troll;

int clone_actors()
{
    troll = lab_clone("/domains/std/monster/troll");
    if (!troll)
    {
        debug("Failed to clone troll");
        return 0;
    }
    destruct(present("sword", troll));
    return 1;
}

int clone_items(mixed i)
{
    return std_clone_move(troll, i);
}

int check_wield(string w)
{
    return inv_do(troll, "wield", w, "query_wielding", 0);
}

int check_unwield(string w)
{
    return inv_do(troll, "unwield", w, "query_wielding", 1);
}

string *item_names()
{
    string *items = ({});
    object *obs = all_inventory(troll);

    foreach (object o in obs)
    {
        items += ({o->query_primary_id()});
    }

    return items;
}

int check_weapons()
{
    string *items = item_names();
    if (!sizeof(items))
    {
        debug("Failed to populate item names");
        return 0;
    }

    foreach (string i in items)
    {
        object ob;
        ob = present(i, troll);

        if (!ob)
        {
            debug("object no longer exists");
            return 0;
        }

        if (ob->is_weapon() && !ob->query_must_dual_wield())
        {
            cmd(troll, "wield " + i);
            if (sizeof(ob->query_wielding()) != 1)
            {
                debug("Failed to wield " + i);
                return 0;
            }
            cmd(troll, "unwield " + i);
            if (ob->query_wielding())
            {
                return 0;
            }
        }
        
        if (ob->is_weapon() && (ob->query_must_dual_wield() || ob->query_can_dual_wield()))
        {
            cmd(troll, "dualwield " + i);
            if (sizeof(ob->query_wielding()) != 2)
            {
                debug("Failed to dual wield " + i);
                return 0;
            }
            cmd(troll, "unwield " + i);
            if (ob->query_wielding())
            {
                return 0;
            }
        }
    }

    return 1;
}

int check_armours()
{
    string *items = item_names();

    if (!sizeof(items))
    {
        debug("Failed to populate item names");
        return 0;
    }

    foreach (string i in items)
    {
        object ob;
        ob = present(i, troll);

        if (!ob)
        {
            debug("object no longer exists");
            return 0;
        }

        if (ob->is_armour())
        {
            cmd(troll, "wear " + i);
            if (!ob->ob_state())
            {
                debug("Failed to wear " + i);
                return 0;
            }
            cmd(troll, "remove " + i);
            if (ob->ob_state())
            {
                return 0;
            }
        }
    }

    return 1;
}

int clean_up()
{
    destruct(troll);
    return 1;
}

void init_tests()
{
    string *wpns = ({});
    string *arms = ({});
    string *items = ({});

    //Gets all the files in the weapons and armour directory
    wpns += get_dir("/domains/std/weapon/*.c"); 
    arms += get_dir("/domains/std/armour/*.c"); 

    foreach (string w in wpns)
    {
        items += ({"weapon/" + w}); // Replaces the weapon/ portion of the string in preperation for cloning
    }

    foreach (string a in arms)
    {
        items += ({"armour/" + a}); // Replace the armour/ portion of the string in preperation for cloning
    }

    add_test_true("Clone Actor to lab", ( : clone_actors:));

    /* Clones all Weapons and Armours*/
    foreach (string i in items)
    {
        add_test_true("Clone weapon: " + i, ( : clone_items, i:));
    }

    add_test_true("Check weapons", ( :check_weapons: ));
    add_test_true("Check armour", ( :check_armours: ));
    add_test_true("Clean up", ( :clean_up: ));
}