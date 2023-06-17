/* Do not remove the headers from this file! see /USAGE for more info. */

/*
** quests.c
**
** Show you all the quests on the mud.
**
** Created 24-NOV-95, Rust
*/

//:PLAYERCOMMAND
//USAGE quests
//
//Shows you all the quests on the mud.

inherit CMD;

string modify(string quest)
{
  string quests = this_body()->query_quests();
  if(!sizeof(quests) || member_array(quest, quests)<0)
    return quest;
  else
    return quest + "*";
}

nomask private void main()
{
  string domain = explode(base_name(environment(this_body())),"/")[1];
  string footer;
  mapping quests=([]);
  mapping quest_src = QUEST_D->query_domain_quests(domain);

  foreach (string quest in keys(quest_src))
  {
    string* parts = explode(quest,":");
    string main = parts[0];
    string task = parts[1];
    string status;
    
    if (!quests[main])
    {
        quests[main]=([]);
        quests[main]["parts"]=({});
    }

    quests[main]["parts"] += ({task});
    status = this_body()->quest_task(domain,main);
    if (status=="None" || !status) status="NOT_EVEN_STARTED";
    switch(status)
    {
      case "begin":
        status = "Started";
        break;
      case "end":
        status = "Finished";
        break;
      case "NOT_EVEN_STARTED":
      case "":
        status = "Not started";
        break;
      default:
        status = "On your way";
        break;
    }

    quests[main]["task"] = this_body()->quest_task(domain,main);
    quests[main]["status"]=status;
    quests[main]["name"]=QUEST_D->query_quest_name(domain,main);
  }

  footer = sprintf("In your current part of %s (%s) there %s %d quest%s available",
    mud_name(),
    capitalize(domain),
    sizeof(quests)==1 ? "is only" : "are",
    sizeof(quests),
    sizeof(quests)==1 ? "" : "s",
  );

  outf("%25-s  %12s   %s\n",
      "Quest Name",
      "Quest steps",
      "Status");
  outf(repeat_string("-",sizeof(footer))+"\n");

  foreach (string quest in keys(quests))
  {
    outf("%31-s  %6d   %s\n",
      quests[quest]["name"],
      sizeof(quests[quest]["parts"]),
      quests[quest]["status"],
      );
  }
  outf(repeat_string("-",sizeof(footer))+"\n");
  outf(footer+"\n");

}
