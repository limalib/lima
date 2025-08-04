/* Do not remove the headers from this file! see /USAGE for more info. */

inherit OUTDOOR_ROOM;

void setup()
{
   set_brief("Shadowy Swamp");
   set_area("behaviour_area","wiz_area");
   set_long("You stand in a shadowy part of the swamp where the trees grow even closer together, "
            "casting deep, shifting shadows across the waterlogged ground. The air is cooler here, "
            "and the silence is broken only by the distant call of a heron and the soft rustle of "
            "creatures moving through the undergrowth. Moss hangs in long strands from the branches, "
            "and the ground is dotted with clusters of mushrooms and the occasional glint of a "
            "dragonfly's wings.");
   set_listen("You hear the distant call of a heron and the soft rustle of unseen creatures.");
   set_smell("The air smells of damp earth, moss, and faintly of wildflowers.");
   add_item("trees", "Tall, close-growing trees with thick trunks and heavy moss draping their branches.");
   add_item("moss", "Long, green strands of moss hang from the branches, swaying gently in the breeze.");
   add_item("mushrooms", "Clusters of pale mushrooms grow in the dampest patches of ground.");
   add_item("heron", "A solitary heron stands motionless in the shallows, watching for fish.");
   add_item("dragonfly", "Brightly colored dragonflies dart above the water, their wings catching the light.");
   add_item("ground", (["look":"The ground is soft and spongy, covered in moss and dotted with mushrooms.",
                          "search":"You disturb a small lizard that scurries away into the undergrowth."]));
   add_item("lizard", "A quick, green lizard darts between the roots, barely visible.");
   add_item("undergrowth", "Thick undergrowth provides cover for small animals and insects.");
   set_exits((["north":"/domains/pinto/room/swamp1.c", "south":"/domains/pinto/room/swamp3.c"]));
   set_objects((["^std/weapon/whiskey_bottle":1]));
}
