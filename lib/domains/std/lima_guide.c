/* Do not remove the headers from this file! see /USAGE for more info. */

inherit BOOK;

void setup()
{
   add_id("guide");
   set_proper_name("a leather bound book");
   set_title("The LIMA Guide Book");
   set_in_room_desc("There is a very ornately bound book here.");
   set_long("The cover is carved with an intricate pattern inlaid with gold, at the very top of the front cover is a "
            "single word, that appears to be pulsing with a life of its own, LIMA.");
   set_text("This book will at some point contain some interesting bits of information about the LIMA mudlib.  However "
            "at this time, much like the documentation of the lib itself, it is unfinished.");
   set_entries((["1":"Do read https://docs.limalib.dev/",
                 "2":"See https://github.com/limalib/lima"]));
   set_synonyms((["docs":"1", "source":"2"]));
}
