#include "alarm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

const char *SOUND_PLAYER = WARN_SOUND_PLAYER;

const char *getArg(int const idx, int const argc, char const *argv[]) {
  if (idx >= argc) {
    return NULL;
  } else {
    return argv[idx];
  }
}

static struct alarm_command current_command = {
    "echo -en \"\\007 \" && sleep 1 && echo -en \"\\007 \" && sleep 2 && echo "
    "-en \"\\007 \" && sleep 1 && echo -en \"\\007 \" && sleep 1"};

void handle_unknown_command(const char *cmd) {
  printf(C_ERR "Unknown command" C_HIGHL "%s" C_CLR "\n", cmd);
}

void display_help() {
  printf("%s\n%s\n\t%s\n\t\n" HELP_COMMAND_SEQUENCE HELP_COMMAND_SEQUENCE
             HELP_COMMAND_SEQUENCE HELP_COMMAND_SEQUENCE,
         C_CORR "Usage" C_CLR " :",
         "alarm [OPTION] [ARGUMENTS] [OPTION2] [ARGUMENTS2] ...",
         C_BOLD "[OPTIONS]" C_CLR, C_GREY "--help :" C_CLR, "Shows this",
         "no argument", C_GREY "--at :" C_CLR,
         "Sets a new alarm at specified time",
         "TIME : when the alarm rings in " C_BOLD "[HOURS]" C_CLR "h" C_BOLD
         "[MINUTES]" C_CLR " with HOURS and MINUTES integers",
         C_GREY "--in :" C_CLR,
         "Sets a new alarm that will after specified duration",
         "DURATION : when the alarm rings in " C_BOLD "[HOURS]" C_CLR "h" C_BOLD
         "[MINUTES] " C_CLR "with HOURS and "
         "MINUTES integers",
         C_GREY "--play :" C_CLR,
         "Sets the music file to be played by the following alarms",
         "FILE : the music/sound file's path " C_BOLD "[FILE]" C_CLR
         " with FILE "
         "readable");
}

void ring(const char *cmd) {
  printf("ringing \"%s\"\n", cmd);
  system(cmd);
}

void planAt(int hour, int minute, const char *cmd) {
  time_t now = time(NULL);
  time_t *Uthen = (time_t *)malloc(sizeof(typeof(time_t)));
  *Uthen = time(NULL);
  struct tm *then = localtime(Uthen);
  if (then->tm_hour > hour || (then->tm_hour = hour && then->tm_min > minute)) {
    *Uthen += SECONDS_PER_DAY;
    then = localtime(Uthen);
  }
  then->tm_hour = hour;
  then->tm_min = minute;
  then->tm_sec = 0;
  *Uthen = mktime(then);
  time_t diff = *Uthen - now;
  free(Uthen);
  if (fork() == 0) {
    // we are in a child process now
    sleep(diff);
    ring(cmd);
  } else {
    return;
  }
}

void planIn(int hour, int minute, const char *cmd) {
  if (fork() == 0) {
    printf("%ds before ringing \n", hour * 3600 + minute * 60);
    sleep(hour * 3600 + minute * 60);
    ring(cmd);
  } else {
    return;
  }
}

int handle_command(int const argv_index, int const argc, char const *argv[]) {
  printf("%d / %d : %s\n", argv_index, argc, argv[argv_index]);
  const char *cmd = getArg(argv_index, argc, argv);
  if (strcmp(cmd, "--help") == 0) {
    display_help();
  } else if (strcmp(cmd, "--play") == 0) {
    const char *filePath = getArg(argv_index + 1, argc, argv);
    if (filePath == NULL) {
      printf(C_ERR "Cannot find the argument for the file to be played" C_CLR
                   "\n");
      exit(EXIT_FAILURE);
    }
    if (access(filePath, F_OK) >= 0) {
      char cmd[255] = "";
      strcat(cmd, SOUND_PLAYER);
      strcat(cmd, "\"");
      strcat(cmd, filePath);
      strcat(cmd, "\"");
      current_command.command = cmd;
    } else {
      printf(C_ERR "Cannot access the file \"%s\". Are you sure this file "
                   "exists ?" C_CLR "\n",
             filePath);
      exit(EXIT_FAILURE);
    }
    return 2;
  } else if (strcmp(cmd, "--at") == 0) {
    const char *ringAt = getArg(argv_index + 1, argc, argv);
    if (ringAt == NULL) {
      printf(C_ERR "Cannot find the argument for the time to ring at" C_CLR
                   "\n");
      exit(EXIT_FAILURE);
    }
    int hour;
    int minute;
    if (sscanf(ringAt, "%dh%d", &hour, &minute) != 2) {
      printf(C_ERR "Cannot find the hour and minute in argument \"%s\"" C_CLR
                   "\n",
             ringAt);
      exit(EXIT_FAILURE);
    }
    planAt(hour, minute, current_command.command);
    return 2;
  } else if (strcmp(cmd, "--in") == 0) {
    const char *ringIn = getArg(argv_index + 1, argc, argv);
    if (ringIn == NULL) {
      printf(C_ERR "Cannot find the argument for the time to ring at" C_CLR
                   "\n");
      exit(EXIT_FAILURE);
    }
    int hour;
    int minute;
    if (sscanf(ringIn, "%dh%d", &hour, &minute) != 2) {
      printf(C_ERR "Cannot find the hour and minute in argument \"%s\"" C_CLR
                   "\n",
             ringIn);
      exit(EXIT_FAILURE);
    }
    planIn(hour, minute, current_command.command);
    return 2;
  } else {
    handle_unknown_command(cmd);
    exit(EXIT_FAILURE);
  }
  return 1;
}

int main(int argc, char const *argv[]) {
  if (argc <= 1) {
    printf(C_ERR "Unexpected usage." C_CLR " Not enough arguments\n\t%s\n%s\n",
           "Example usage : " C_HIGHL "alarm 9h30" C_CLR,
           "Please consult " C_HIGHL "malarm --help" C_CLR
           " for more informations");
    return EXIT_FAILURE;
  } else {
    int argv_pointer = 1;
    do {
      argv_pointer += handle_command(argv_pointer, argc, argv);
    } while (argv_pointer < argc);
  }
  return EXIT_SUCCESS;
}
