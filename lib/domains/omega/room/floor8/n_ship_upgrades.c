inherit INDOOR_ROOM;

void setup()
{
  set_area("omega_terminal", "landing_term");
  set_brief("Omega Ship Services");
  set_long("As you step into the Omega Ship Services nestled within the bustling corridors of the "
           "terminal, you're greeted by a soft hum of machinery and the sight "
           "of sleek, metallic spaceship components lining the shelves.\n"
           "Above the counter, a large holographic display hovers, showcasing a comprehensive list "
           "of available spaceship upgrades. Each option is accompanied by a detailed description "
           "and the corresponding price, allowing customers to make informed decisions. The list "
           "includes a variety of enhancements to boost a ship's performance, durability, and "
           "functionality, catering to the needs of different spacefarers.");
  set_smell("The air carries a faint scent of lubricants and ionized particles, a testament to the "
            "high-tech nature of the shop's offerings.");
  set_listen("There is a soft hum of machinery in the shop.");

  add_item("sign", (["look":"Please ask Max about (swords, keys, etc),\n"
                            "       buy (sword or sword #) from biff,\n"
                            "       sell (sword, first sword etc) to biff.\n",
                       "read":"Something will be here soon!", ]));

  set_objects((["../../npc/floor8/max_nova.c":1]));
  set_exits((["east":"n_hall3"]));
  set_exit_msg("east",
               ({"$N $vslide through the automatic doors the last moment before they close.",
                 "$N $vexit through the automatic doors south.", "$N $vleave south through the automatic doors."}));
  set_enter_msg("east", ({"$N $varrive through the automatic doors.", "$N $vwalk in."}));
  set_combat_forbidden(1);
}

/*
As you step into the small shop nestled within the bustling corridors of the modern space station, you're greeted by a soft hum of machinery and the sight of sleek, metallic spaceship components lining the shelves. The shop maintains an artificial gravity system, allowing customers to move about comfortably, with their feet firmly planted on the floor. The air carries a faint scent of lubricants and ionized particles, a testament to the high-tech nature of the shop's offerings.

Behind the counter stands a friendly and knowledgeable technician, ready to assist customers with their spaceship upgrade needs. His name is Max Nova, a seasoned expert in ship modifications and a familiar face among the station's spacefaring community. Max has an easy smile and an enthusiasm for all things space-related, making him the perfect guide for those seeking to enhance their interstellar travels.

Above the counter, a large holographic display hovers, showcasing a comprehensive list of available spaceship upgrades. Each option is accompanied by a detailed description and the corresponding price, allowing customers to make informed decisions. The list includes a variety of enhancements to boost a ship's performance, durability, and functionality, catering to the needs of different spacefarers.

Among the upgrades available, you spot the following:

Hyperdrive Stabilizer: Improve your ship's warp capability and reduce travel time between star systems.
Shield Reinforcement Module: Enhance your ship's defensive capabilities by fortifying its shields against energy-based attacks.
Navigation AI Upgrade: Install an advanced artificial intelligence system to optimize navigation, course plotting, and interstellar route selection.
Engine Overclocking Kit: Amp up your ship's speed and acceleration, granting you an edge in fast-paced space chases.
Cargo Expansion Module: Increase your ship's storage capacity, allowing for larger hauls and trading ventures.
Energy Weapon Refit: Retrofit your ship's weaponry with cutting-edge energy-based armaments, delivering devastating firepower.

As you peruse the selection, Max stands ready to provide insights, answer questions, and guide you through the available options. With his expertise and friendly demeanor, he ensures that every customer's ship leaves the shop better equipped for the vast expanse of space.*/