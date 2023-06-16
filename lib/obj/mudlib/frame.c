/* Do not remove the headers from this file! see /USAGE for more info. */

/*
 * Frame layout module for fancy frames.
 * Tsath 2022/2023. Heavily inspired by frame layouts done by Diavolo@Merentha.
 */

#define RD 0
#define H 1
#define HD 2
#define LD 3
#define D 4
#define DR 5
#define X 6
#define DL 7
#define RU 8
#define HU 9
#define LU 10

#define FALLBACK_THEME "ascii"
#define COL_GRADIENT 0
#define COL_TITLE 1
#define COL_ACCENT 2
#define COL_WARNING 3

inherit M_COLOURS;

mapping themes = (["single":({"┌", "─", "┬", "┐", "│", "├", "┼", "┤", "└", "┴", "┘"}),
                   "double":({"╔", "═", "╦", "╗", "║", "╠", "╬", "╣", "╚", "╩", "╝"}),
                    "ascii":({"+", "-", "-", "+", "|", "|", "|", "|", "+", "-", "+"}),
                    "lines":({"-", "-", "-", "-", " ", " ", "-", " ", "-", "-", "-"}),
                     "none":({" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "})]);

mapping colours =
    (["fire":({"160,166,172,178,184,190,226,220,214,208,202,196", "208", "015", "197"}),
         "steel":({"239,240,241,242,255,237,238,239,240,241", "117", "015", "197"}),
         "fairy":({"093,099,105,111,117,123,159,153,147,141,135,129", "175", "015", "197"}),
          "cool":({"021,027,033,039,045,051,087,081,075,069,063,057", "159", "015", "197"}),
          "pink":({"165,171,177,183,189,195,231,225,219,213,207,201", "231", "015", "197"}),
         "blues":({"058,059,060,061,062,063", "105", "229", "197"}),
          "dusk":({"130,131,132,133,134,135", "220", "223", "197"}),
         "sunny":({"226,227,228,229,230,231", "214", "015", "197"}),
          "neon":({"088,089,090,091,092,093", "228", "045", "197"}),
        "nature":({"022,028,034,040,046,083,077,071,065,059", "192", "015", "197"}), "none":({"", "", "", ""}), ]);
string *bits;

/* Strings */
private
string title, header_content, footer_content, content, theme;
/* Ints */
private
int width,        // Width of the frame, default user screen width
    title_margin, // Margin the header takes from left side, default 2
    text_margin;  // Margin around header text
/* Booleans*/
private
int add_header, add_footer, left_header;

// Colour configuration storage.
private
string *hcolours;

private
string owner;

void select_theme(string t)
{
   theme = t;

   if (member_array(theme, keys(themes)) != -1)
      bits = themes[theme];
   else
      bits = themes[FALLBACK_THEME];
}

string *query_colour_themes()
{
   return sort_array(keys(colours), 1);
}

string *query_themes()
{
   return keys(themes);
}

void set_left_header()
{
   left_header = 1;
}

private
string colour_str(string t, string col)
{
   return "<" + col + ">" + t + "<res>";
}

string title(string t)
{
   return colour_str(t, hcolours[COL_TITLE]);
}

string accent(string t)
{
   return colour_str(t, hcolours[COL_ACCENT]);
}

string warning(string t)
{
   return colour_str(t, hcolours[COL_WARNING]);
}

void set_title(string s)
{
   if (XTERM256_D->colourp(s))
      title = s;
   else if (hcolours)
      title = "<" + hcolours[COL_TITLE] + ">" + s + "<res>";
   else
      title = s;
}

void set_width(int w)
{
   width = w;
}

void set_title_margin(int hm)
{
   title_margin = hm;
}

void set_text_margin(int tm)
{
   text_margin = tm;
}

void init_user()
{
   width = (this_user()->query_screen_width() ? this_user()->query_screen_width() - 2 : 79);
   hcolours = (this_user()->frames_colour() != "none" ? colours[this_user()->frames_colour()] : 0);
   select_theme(this_user()->frames_theme());
   title_margin = 2;
   text_margin = 1;
   add_header = 0;
   add_footer = 0;
   left_header = 0;
}

void set_header_content(string hc)
{
   string *header_lines = explode(hc, "\n");
   int i = 0;
   while (i < sizeof(header_lines))
   {
      string line = header_lines[i];
      if (!XTERM256_D->colourp(line))
         line = "<" + hcolours[COL_ACCENT] + ">" + rtrim(line) + "<res>";
      else
         line = rtrim(line);
      header_lines[i] = line;
      i++;
   }
   header_content = implode(header_lines, "\n");
   add_header = 1;
}

void set_footer_content(string fc)
{
   footer_content = fc;
   add_footer = 1;
}

void set_content(string c)
{
   content = c;
}

void set_hcolours(string *hc)
{
   hcolours = hc;
}

private
string simple_header()
{
   string out = "";
   out += bits[RD] + repeat_string(bits[H], width - 2) + bits[LD] + "\n";
   return out;
}

void remove()
{
   destruct();
}

private
string create_header()
{
   string out="";
   int i = 0;
   int simple_header = theme == "lines" || theme == "none";
   int header_width = colour_strlen(title) + (text_margin * 2);
   string *headers = explode(header_content || "", "\n");

   if (!title)
      return simple_header();

   if (add_header && title_margin == 1)
      title_margin = 2;

   if (!simple_header)
   {
      out += repeat_string(" ", title_margin) + bits[RD];
      while (i < header_width)
      {
         out += bits[H];
         i++;
      }

      out += bits[LD] + "\n";
   }

   add_header=1;

   out += bits[RD] + (add_header ? bits[HD] : "") +
                      repeat_string(bits[H], title_margin - (add_header ? 2 : 1)) + bits[DL] +
                      repeat_string(" ", text_margin) + title + repeat_string(" ", text_margin) + bits[DR] +
                      repeat_string(bits[H], width - header_width - title_margin - (add_header ? 4 : 3)) +
                      (add_header ? bits[HD] : "") + bits[LD] + "\n";

   if (!simple_header)
   {
      out +=
          bits[D] + (add_header ? bits[D] : "") + repeat_string(" ", title_margin - (add_header ? 2 : 1)) + bits[RU];

      i = 0;
      while (i < header_width)
      {
         out += bits[H];
         i++;
      }
      out += bits[LU] + repeat_string(" ", width - header_width - title_margin - (add_header ? 4 : 3)) +
                         (add_header ? bits[D] : "") + bits[D] + "\n"; // End of Title box section
   }

   foreach (string h in headers)
   {
      int col_lendiff = strlen(h) - colour_strlen(h);
      int content_width = width - 6 + col_lendiff;
      if (title_margin > 0)
         out += bits[D] + (add_header && !simple_header ? bits[D] : " ") + " " +
                            sprintf("%-" + content_width + "." + content_width + "s", h) + " " +
                            (add_header ? bits[D] : "") + bits[D] + "\n";
   }

   if (simple_header)
      out += "";
   else if (add_header)
      out += bits[D] + (add_header ? bits[RU] : "") + repeat_string(bits[H], width - 4) +
                         (add_header ? bits[LU] : "") + bits[D] + "\n";
   return out;
}

private
string create_footer()
{
   string out = "";
   int i = 0;
   string *footers = explode(footer_content || "", "\n");

   out += bits[D] + bits[RD] + repeat_string(bits[H], (width - 4)) + bits[LD] + bits[D] + "\n";

   foreach (string f in footers)
   {
      int col_lendiff = strlen(f) - colour_strlen(f);
      int content_width = width - 6 + col_lendiff;
      out += bits[D] + bits[D] + " " + sprintf("%-" + content_width + "." + content_width + "s", f) + " " + bits[D] +
             bits[D] + "\n";
   }

   out +=
       bits[RU] + bits[HU] + repeat_string(bits[H], width - 4) + bits[HU] + bits[LU] + "\n"; // End of Title box section

   return out;
}

private
string create_left_header()
{
   string out = "";
   string *headers = explode(header_content || "", "\n");
   int header_width, max_header_width, i = 0;
   string *contents = explode(content || "", "\n");
   int content_length = max(({sizeof(headers), sizeof(contents)}));

   if (title)
      header_width = colour_strlen(title) + (text_margin * 2);

   max_header_width = max(map_array(headers, ( : colour_strlen($1) :)));
   max_header_width = max_header_width + (2 * text_margin);

   // Line 1 - title only
   if (title)
      out += repeat_string(" ", title_margin + max_header_width + 1) + bits[RD] + repeat_string(bits[H], header_width) +
             bits[LD];
   out += "\n";

   // Line 2 - both cases
   if (title)
      out += repeat_string(" ", max_header_width + 1) + bits[RD] + repeat_string(bits[H], text_margin) + bits[DL] +
             repeat_string(" ", text_margin) + title + repeat_string(" ", text_margin) + bits[DR] +
             repeat_string(bits[H], width - max_header_width - 6 - header_width) + bits[LD] + "\n";
   else
      out += repeat_string(" ", max_header_width + 1) + bits[RD] +
             repeat_string(bits[H], width - max_header_width - 3) + bits[LD] + "\n";

   // Line 3
   if (title)
      out += bits[RD] + repeat_string(bits[H], max_header_width) + bits[DL] + repeat_string(" ", title_margin - 1) +
             bits[RU] + repeat_string(bits[H], header_width) + bits[LU] +
             repeat_string(" ", width - max_header_width - header_width - 6) + bits[D] + "\n";
   else
   {
      string c = contents[0];
      int col_lendiff = strlen(c) - colour_strlen(c);
      int content_width = width - max_header_width + col_lendiff - 5;
      out += bits[RD] + repeat_string(bits[H], max_header_width - 2) + repeat_string(bits[H], title_margin) + bits[DL] +
             " ";
      out += sprintf("%-" + content_width + "." + content_width + "s", c) + " " + bits[D] + "\n";
      contents = contents[1..];
      content_length = max(({sizeof(headers), sizeof(contents)}));
   }

   while (i < content_length)
   {
      string head, cont;
      int col_lendiff;
      int content_width;
      head = i < sizeof(headers) ? headers[i] : "";
      cont = i < sizeof(contents) ? contents[i] : "";
      col_lendiff = strlen(cont) - colour_strlen(cont);
      content_width = width - max_header_width + col_lendiff - 5;

      if (i < sizeof(headers)) // Header content | Stuff |
         out += bits[D] + repeat_string(" ", text_margin) + head + "<res>" +
                repeat_string(" ", max_header_width - text_margin - colour_strlen(head)) + bits[D] + " ";
      else if (i == sizeof(headers)) // End of header
         out += bits[RU] + repeat_string(bits[H], max_header_width) + "<res>" + bits[DL] + " ";
      else // No header, just spacing.
         out += repeat_string(" ", max_header_width + 1) + bits[D] + " ";

      // Content
      out += sprintf("%-" + content_width + "." + content_width + "s", cont) + " " + bits[D] + "\n";
      i++;
   }

   if (i <= sizeof(headers))
      out += bits[RU] + repeat_string(bits[H], max_header_width) + "<res>" +
             (i == sizeof(contents) ? bits[HU] + repeat_string(bits[H], width - max_header_width - 3) + bits[LU]
                                    : bits[DL] + repeat_string(" ", width - max_header_width - 3) + bits[D]) +
             "\n";

   if (i != sizeof(contents) || sizeof(contents) > sizeof(headers))
      out += repeat_string(" ", max_header_width + 1) + bits[RU] +
             repeat_string(bits[H], width - max_header_width - 3) + bits[LU] + "\n";

   return out;
}

private
string create_content()
{
   string out = "";
   string *contents = explode(content || "", "\n");

   foreach (string c in contents)
   {
      int col_lendiff = strlen(c) - colour_strlen(c);
      int content_width = width - 4 + col_lendiff;
      out += bits[D] + " " + sprintf("%-" + content_width + "." + content_width + "s", c) + " " + bits[D] + "\n";
   }
   return out;
}

private
string end_frame()
{
   string out = "";
   out += bits[RU] + repeat_string(bits[H], width - 2) + bits[LU] + "\n"; // End of Title box section
   return out;
}

varargs private string h_colours(string output, mixed colstring)
{
   string frames = implode(bits, "");
   mixed *pieces = pcre_assoc(output, ({"[" + frames + "]"}), ({1}));
   string *bits = pieces[0];
   string *matches = pieces[1];
   string new_out = "";
   string *colours;
   int i = 0;
   int position = 0;

   colours = explode((arrayp(colstring) ? colstring[COL_GRADIENT] : colstring) || hcolours[COL_GRADIENT], ",");

   if (!sizeof(colours))
      return output;
   while (i < sizeof(bits))
   {
      if (strsrch(bits[i], "\n") != -1)
         position = 0;

      new_out += (matches[i] ? use_colour(colours, position, width) + bits[i] + "<res>" : bits[i]);
      position += strlen(bits[i]);
      i++;
   }

   return new_out;
}

string query_title(string theme)
{
   return colours[theme][COL_TITLE];
}

string query_accent(string theme)
{
   return colours[theme][COL_ACCENT];
}

string query_warning(string theme)
{
   return colours[theme][COL_WARNING];
}

string demo_string(string theme, int w)
{
   return themes[theme][RD] + themes[theme][H] + themes[theme][H] + themes[theme][H] +
          repeat_string(themes[theme][H], (w / 2) - 6) + themes[theme][HD] + themes[theme][H] + themes[theme][H] +
          themes[theme][H] + repeat_string(themes[theme][H], (w / 2) - 6) + themes[theme][H] + themes[theme][H] +
          themes[theme][H] + themes[theme][LD];
}

string colour_demo(string theme, string colour, int w)
{
   if (member_array(theme, keys(themes)) == -1)
      theme = "single";
   return h_colours(demo_string(theme, w), colours[colour]);
}

int clean_up(int i)
{
   destruct();
}

string short()
{
   return owner ? owner + "'s frame" : "Orphaned frame";
}

void create(object ob)
{
   owner=base_name(ob);
}

string render()
{
   string out = "";

   if (!bits)
      error("Need to set frame theme before render() using frame->set_theme().\n" +
            "Current themes: " + format_list(query_themes()) + ".");

   if (left_header)
   {
      if (!header_content)
         error("Cannot generate left header frame without content.");
      out += create_left_header();
   }
   else
   {
      out = create_header();

      if (content)
         out += create_content();

      if (footer_content)
         out += create_footer();
      else
         out += end_frame();
   }

   if (hcolours && this_user()->terminal_mode() != "plain")
      out = h_colours(out);
   return out;
}