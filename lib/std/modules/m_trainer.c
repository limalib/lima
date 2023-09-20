/* Do not remove the headers from this file! see /USAGE for more info. */
// Work in NonProgress ... will be finished sometime after Vendor has been redone. probably for a7
// Finished 10-20 years after by Tsath.

//: MODULE
// M_TRAINER, module for turning NPCs into skill trainers. This module depends and inherits
// functionality from NPC_CONVERSATION to allow the training to use conversations as interface.
//
// Works:
// - Skill training
//

inherit M_CONVERSATION;
inherit CLASS_SKILL;

void add_response(string key, mixed act);
void add_option(string key, mixed act);
void add_to_start(string key);

mapping skill_restrictions = ([]);
mapping trainer_msgs = (["unable_msg":"\"I cannot teach you that.\", $N tells $t.",
                           "fail_msg":"$T $v1train for a while with $N, but $v1undergo no improvement.",
                            "low_msg":"$T $v1train with $N, but $v1improve little.",
                            "mid_msg":"$T $v1train with $N and $v1improve a little.",
                           "high_msg":"$T $v1train hard with $N, and $v1improve significantly",
                      "very_high_msg":"$T $v1have an excellent lesson with $N, and $v1improve remarkably.",
                         "reject_msg":"\"You are more skilled then me! I cannot teach you no more.\", $N tells $t.",
                         "no_pts_msg":"\"You need more practice before you can train, sorry\", $N tells $t.", ]);

void set_trainer_msgs(mapping msgs)
{
   string *msg_names = keys(trainer_msgs);
   foreach (string key, string msg in msgs)
   {
      if (!member_array(key, msg_names) + 1)
         error("Invalid msg name: " + key);
      trainer_msgs[key] = msg;
   }
}

int is_trainer()
{
   return 1;
}

void set_train_restrict(mixed *restrictions)
{
   string *valid_skills = SKILL_D->query_skills();

   foreach (string skill, string restriction in restrictions)
   {
      if (member_array(skill, valid_skills) == -1)
         error("Invalid skill: " + skill);
      skill_restrictions += ([skill:restriction]);
   }
}

void do_training(object trainee, string skill)
{
   class skill trainee_skill = trainee->query_skill(skill);
   TBUG(trainee);
   TBUG(skill);
   if (!trainee_skill)
      return;
   else
   {
      int trainee_skillpts = trainee_skill->skill_points;
      int trainer_skill = this_object()->query_skill_pts(skill);
      string msg;

      if (trainee_skill->training_points == 0)
         msg = trainer_msgs["no_pts_msg"];
      else if (member_array(skill, keys(skill_restrictions)) != -1 ||
               member_array(skill, keys(this_object()->query_skills())) == -1)
         msg = trainer_msgs["unable_msg"];
      else if (trainee_skillpts < trainer_skill)
      {
         for (int i = 0; i < trainee_skill->training_points; i++)
         {
            trainee->test_skill(skill, trainee_skillpts + 1);
         }
         trainee->clear_training_points(skill);

         switch (trainee->query_skill_pts(skill) - trainee_skillpts)
         {
         case 0..1:
            msg = trainer_msgs["fail_msg"];
            break;
         case 2..6:
            msg = trainer_msgs["low_msg"];
            break;
         case 7..13:
            msg = trainer_msgs["mid_msg"];
            break;
         case 14..18:
            msg = trainer_msgs["high_msg"];
            break;
         default:
            msg = trainer_msgs["very_high_msg"];
         }
      }
      else
         msg = trainer_msgs["reject_msg"];

      this_object()->targetted_action(msg, trainee);
   }
}

void setup_trainer_conversation(int skill_max)
{
   add_option("rank", "What skill rank are you?");
   add_response("rank", "I can train your skills to around rank " + SKILL_D->skill_title_from_pts(skill_max) + ".");
   add_to_start("rank");

   foreach (string skill in keys(this_object()->query_skills()))
   {
      add_option(skill, "I want to train my " + skill + " skill.");
      add_response(skill, "Okay, " + skill + " it is...#T#" + skill);
      add_to_start(skill);
   }
}