/* Do not remove the headers from this file! see /USAGE for more info. */

//   The following is an attempt at a value module.
//

int object_value = 0;

void set_value(int value)
{
   object_value = value;
}

int query_value()
{
   return object_value;
}

// Note about generic items:
// Define setup_args() if you create valuable items that M_VENDOR needs to figure out how to
// stack and clone correctly. These arguments will be parsed to setup as:
// setup(setup_args...);
mixed *setup_args()
{
   return ({});
}

mapping lpscript_attributes()
{
   return (["value":({LPSCRIPT_INT, "setup", "set_value"}), ]);
}
