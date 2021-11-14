#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "discord.h"


int countLines(const char *filename) // this function shamelessly stolen from the internet
{

  FILE *fp;
  int count = 0;  // Line counter (result)
  // char filename[MAX_FILE_NAME];
  char c;  // To store a character read from file

  // Get file name from user. The file should be
  // either in current folder or complete path should be provided
  // printf("Enter file name: ");
  // scanf("%s", filename);

  // Open the file
  fp = fopen(filename, "r");

  // Check if file exists
  if (fp == NULL)
  {
      printf("Could not open file %s", filename);
      return 0;
  }

  // Extract characters from file and store in character c
  for (c = getc(fp); c != EOF; c = getc(fp))
      if (c == '\n') // Increment count if this character is newline
          count = count + 1;

  // Close the file
  fclose(fp);

  return count;


}

const char* send_phrase(void)
{


      int i=0;
      int numProgs=0;
      char* programs[countLines("kabalek_phrases.txt")]; // max number of lines
      char line[100];


      FILE *file;
      file = fopen("kabalek_phrases.txt", "r");

      while(fgets(line, sizeof line, file)!=NULL) {
          //check to be sure reading correctly
          //printf("%s", line);
          //add each filename into array of programs
          programs[i]=strdup(line);
          i++;
          //count number of programs in file
          numProgs++;
      }

      //check to be sure going into array correctly
      /* for (int j=0 ; j<numProgs+1; j++) {
          printf("\n%s", programs[j]); // everything in array
          // printf("%d", j);
          // printf("%d", numProgs);
      // DOES THIS NOT FINISH????
      // I do not think it does
          printf("Here");
      // so that stops after the loop is over but yet it does not exit the loop
      // tf?
      // this whole thing was not neccessary so just gonna comment it out but I should figure out why tf this was breaking it some other time
      }
      */
      fclose(file);
      size_t totalLines = sizeof programs / sizeof programs[0];
      int randIndex = rand() % (int) totalLines;
      // log_info("%s, randIndex %i, totalLines%i",programs[randIndex], randIndex, (int) totalLines);

      return programs[randIndex];

}


void on_ready(struct discord *client, const struct discord_user *bot)
{
  log_info("Kabalek has connected to Discord as %s#%s!",
           bot->username, bot->discriminator);

}


// do nothing on reactions
void on_reaction_add(struct discord *client,
                     const struct discord_user *bot,
                     const uint64_t user_id,
                     const uint64_t channel_id,
                     const uint64_t message_id,
                     const uint64_t guild_id,
                     const struct discord_guild_member *member,
                     const struct discord_emoji *emoji)
{
  // make sure bot doesn't interact with other bots
  if (member->user->bot) return;

  /* discord_create_reaction(client, channel_id, message_id, emoji->id,
                          emoji->name); */
  return;
}

// if message contains kalabek
void on_message_create(struct discord *client,
                       const struct discord_user *bot,
                       const struct discord_message *msg)
{
  // make sure bot doesn't echoes other bots
  if (msg->author->bot) return;

  struct discord_create_message_params params = { .content = msg->content };

   // word "kalabek" will trigger send_phrase
   if (0 == strcmp(msg->content, "kabalek")) {
     struct discord_create_message_params params = { .content = send_phrase() };
     discord_create_message(client, msg->channel_id, &params, NULL);
     log_info("Sent-Phrase using Kabalek");
   }

   // if message contains "wrong", send Amateur Hour
   if (strstr(msg->content, "wrong") != NULL) {
     struct discord_create_message_params params = { .content = "Amateur Hour! Amateur Hour!" };
     discord_create_message(client, msg->channel_id, &params, NULL);
     log_info("Sent-AH using Wrong");
   }
   
   // if message contains "Kabalek", chance to send send_phrase
   if ((strstr(msg->content, "kalebek") != NULL)||(strstr(msg->content, "Kalebek") != NULL)
      ||(strstr(msg->content, "KALEBEK") != NULL)||(strstr(msg->content, "kalabek") != NULL)||(strstr(msg->content, "Kalabek") != NULL)
         ||(strstr(msg->content, "KALABEK") != NULL)) {

     if(rand() % 6 == 1){
       struct discord_create_message_params params = { .content = send_phrase() };
       discord_create_message(client, msg->channel_id, &params, NULL);
       log_info("Sent-Phrase using KalEbek");
     }


   }

 }

enum discord_event_scheduler scheduler(struct discord *client,
                                       struct discord_user *bot,
                                       struct sized_buffer *event_data,
                                       enum discord_gateway_events event)
{
  return DISCORD_EVENT_WORKER_THREAD;
}


int main(int argc, char *argv[])
{

  srand(time(NULL));
  const char *config_file;
  if (argc > 1)
    config_file = argv[1];
  else
    config_file = "../config.json";

  discord_global_init();

  struct discord *client = discord_config_init(config_file);
  assert(NULL != client && "Couldn't initialize client");

  /* trigger event callbacks in a multi-threaded fashion */
  discord_set_event_scheduler(client, &scheduler);

  discord_set_on_ready(client, &on_ready);
  discord_set_on_message_create(client, &on_message_create);
  discord_set_on_message_reaction_add(client, &on_reaction_add);

  printf("\nTYPE ANY KEY TO START BOT\n");
  fgetc(stdin); // wait for input

  discord_run(client);

  discord_cleanup(client);

  discord_global_cleanup();
}
