/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** global.h
**
** These are standard definitions for using the Lima Mudlib.  You probably
** should not change anything in here unless you know what you're doing :-)
*/

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifndef __SENSIBLE_MODIFIERS__
#define nosave static
#define protected static
#endif

#include <config.h>
#include <mudlib.h>

#define mud_name()		MUD_NAME
#define driver_version()	__VERSION__

#define TELL_BUG(p,x) (tell_user(p,sprintf("%s: %O\n",__FILE__,(x))))

/* LIMA mudlib maintainers. Add your own in mudlib.h to avoid merge conflicts with this file. */
#define IBUG(x) TELL_BUG("iizuka",(x))
#define BBUG(x) TELL_BUG("rust",(x))
#define DBBUG(x) TELL_BUG("deathblade",(x))
#define ZBUG(x) TELL_BUG("zifnab",(x))
#define ZABUG(x) TELL_BUG("zakk", (x))
#define TBUG(x) TELL_BUG("tsath",(x))
#define ABUG(x) TELL_BUG("asmerian",(x))
#define KBUG(x) TELL_BUG("kobol",(x))
#define LBUG(x) TELL_BUG("loriel",(x))

#endif // __GLOBAL_H__
