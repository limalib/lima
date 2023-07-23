/* Do not remove the headers from this file! see /USAGE for more info. */

inherit LIVING;
inherit M_VENDOR;
inherit M_ACTIONS;
inherit M_TRIGGERS;

void setup()
{
   set_gender(1);
   set_name("Liam");
   add_id("employee");
   set_proper_name("Liam Johnson");
   set_in_room_desc("Liam Johnson, the canteen cashier looks bored behind the counter.");
   set_long("Liam Johnson is a friendly and efficient staff member who mans the till at the canteen. With his warm "
            "smile and quick wit, he ensures smooth transactions and assists customers with their orders. Liam has a "
            "youthful charm, and his energetic nature makes him a favorite among the station's visitors. He wears a "
            "neat uniform and is always ready to provide assistance with a touch-screen menu.");
   add_pattern("%s says: I am your manager%s.", ( : "say Are you Mr. Stevens?" :));
   add_pattern("%s says: I am your boss%s.", ( : "say Are you Mr. Stevens?" :));

   /* Determines what type of currency we'll use */
   set_currency_type("credit");

   /* Tells vendor we will sell things */
   set_for_sale(1);

   set_sell((["^common/consumable/astro_wrap":-1,
         "^common/consumable/penne_arrabiata":-1, "^common/consumable/cosmic_burger":10, "^std/consumable/beer":-1,
          "^common/consumable/nebula_noodles":-1, "^common/consumable/nebula_nectar":-1,
                    "^std/consumable/peanuts":20]));
   set_clear_numbers(1);
   set_actions(
       120,
       ({"say Good day! What can I ring up for you today? Our holographic menu offers an array of stellar options!",
         "say Do you need any extra toppings or a refreshing beverage to go with your meal? Just let me know, and "
         "I'll make it happen!",
         "say Thank you for dining with us! Your total comes to "+(random(10)+random(10)+5)+" credits. Payment options include credits, crypto, "
         "or a gravity-defying dance routine!",
         "whistle"}));
}
