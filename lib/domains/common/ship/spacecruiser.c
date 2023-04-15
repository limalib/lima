inherit SPACESHIP;

string query_owner();

void setup()
{
   ::setup();
   set_ship_brief("A small spacecruiser");
   set_ship_long(
       "A sleek and solitary machine that can take 2 people anywhere, fast. It is a cozy and reliable vessel.");
   set_ship_cost(50);
   set_ship_type("Excalibur Spacecruiser");
}

void init_room(object r)
{
   string owner = query_owner();
   r->set_brief(capitalize(owner) + "'s spacecruiser");
   r->set_long("This ship is a sleek and solitary machine that can take 2 people anywhere, fast. It is a cozy and "
               "reliable vessel.");
   r->set_light(1);
   r->set_room_on_ship(10);
   r->add_item("abandoned building", "building", "It seems to be near tearing itself down at any moment now.");
   /*
     r->set_objects((["^nuke/housing/item/apartment_door":({"north", "^abacus/room/w_str01"})]));
     r->set_saved_objects((["^nuke/housing/item/small_fridge":1]));
   r->set_state_description("apartment_door_off", "a closed");
   r->set_state_description("apartment_door_on", "an open");
   r->set_state_description("fridge_door_off", "a closed");
   r->set_state_description("fridge_door_on", "an open");
     */
}
