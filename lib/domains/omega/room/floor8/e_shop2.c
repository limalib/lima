inherit INDOOR_ROOM;

void setup()
{
    set_area("omega_terminal", "landing_term");
    set_brief("StarPort -R- Us");
    set_long(
      "This is a huge store which looks like it takes up what usually would be 3 stalls."
      "There is a sign above the counter that is in front of you." 
    );

    add_item("sign", ([
	"look" : "Please ask biff about (swords, keys, etc),\n"
	"       buy (sword or sword #) from biff,\n"          
	"       sell (sword, first sword etc) to biff.\n",
	"read" : "Something will be here soon!",
      ]) );
    set_objects( ([
	"/domains/std/shopkeeper.c" : 1,
    "^common/obj/auto_door":({"south", "e_hall3"}),
      ]) );
    set_exits(([
    "east" : "e_shop1",
    ]));

}