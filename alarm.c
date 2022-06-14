#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define XSTR(x) STR(x)
#define STR(x) #x

#define HELP_COMMAND_SEQUENCE "\t%s\n\t\t%s\n\t\t%s\n"
#define SECONDS_PER_DAY 86400

#ifndef WARN_SOUND_PLAYER
#define WARN_SOUND_PLAYER "vlc "
#endif

#pragma message XSTR(WARN_SOUND_PLAYER)

const char *SOUND_PLAYER = WARN_SOUND_PLAYER;

struct alarm_command {
  const char *command;
};

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
  printf("Unknown command %s\n", cmd);
}

void display_help() {
  printf("%s\n%s\n\t%s\n\t\n" HELP_COMMAND_SEQUENCE HELP_COMMAND_SEQUENCE
             HELP_COMMAND_SEQUENCE HELP_COMMAND_SEQUENCE,
         "\033[32;1mUsage\033[0m :",
         "alarm [OPTION] [ARGUMENTS] [OPTION2] [ARGUMENTS2] ...",
         "\033[1m[OPTIONS]\033[0m", "\033[37;1m--help :\033[0m", "Shows this",
         "no argument", "\033[37;1m--at :\033[0m",
         "Sets a new alarm at specified time",
         "TIME : when the alarm rings in "
         "\033[36;1m[HOURS]\033[0mh\033[36;1m[MINUTES]\033[0m with HOURS and "
         "MINUTES integers",
         "\033[37;1m--in :\033[0m",
         "Sets a new alarm that will after specified duration",
         "DURATION : when the alarm rings in "
         "\033[36;1m[HOURS]\033[0mh\033[36;1m[MINUTES]\033[0m with HOURS and "
         "MINUTES integers",
         "\033[37;1m--play :\033[0m",
         "Sets the music file to be played by the following alarms",
         "FILE : the music/sound file's path \033[36;1m[FILE]\033[0m with FILE "
         "readable");
}

void ring(const char *cmd) { system(cmd); }

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
      printf("\033[31;1mCannot find the argument for the file to be played\n");
      exit(EXIT_FAILURE);
    }
    if (access(filePath, R_OK)) {
      char cmd[255] = "";
      strcat(cmd, SOUND_PLAYER);
      strcat(cmd, filePath);
      current_command.command = cmd;
    } else {
      printf("\033[31;1mCannot access the file %s. Are you sure this file "
             "exists ?",
             filePath);
      exit(EXIT_FAILURE);
    }
    return 2;
  } else if (strcmp(cmd, "--at") == 0) {
    const char *ringAt = getArg(argv_index + 1, argc, argv);
    if (ringAt == NULL) {
      printf("\033[31;1mCannot find the argument for the time to ring at\n");
      exit(EXIT_FAILURE);
    }
    int hour;
    int minute;
    if (sscanf(ringAt, "%dh%d", &hour, &minute) != 2) {
      printf("\033[31;1mCannot find the hour and minute in argument \"%s\"\n",
             ringAt);
      exit(EXIT_FAILURE);
    }
    planAt(hour, minute, current_command.command);
    return 2;
  } else if (strcmp(cmd, "--in")) {
    const char *ringIn = getArg(argv_index + 1, argc, argv);
    if (ringIn == NULL) {
      printf("\033[31;1mCannot find the argument for the time to ring at\n");
      exit(EXIT_FAILURE);
    }
    int hour;
    int minute;
    if (sscanf(ringIn, "%dh%d", &hour, &minute) != 2) {
      printf("\033[31;1mCannot find the hour and minute in argument \"%s\"\n",
             ringIn);
      exit(EXIT_FAILURE);
    }
    planAt(hour, minute, current_command.command);
    return 2;
  } else {
    handle_unknown_command(cmd);
    exit(EXIT_FAILURE);
  }
  return 1;
}

int main(int argc, char const *argv[]) {
  if (argc <= 1) {
    printf(
        "\033[31;1mUnexpected usage.\033[0m Not enough arguments\n\t%s\n%s\n",
        "Example usage : \033[36malarm 9h30\033[0m",
        "Please consult \033[37;1malarm --help\033[0m for more informations");
    return EXIT_FAILURE;
  } else {
    int argv_pointer = 1;
    do {
      argv_pointer += handle_command(argv_pointer, argc, argv);
    } while (argv_pointer < argc);
  }
  return EXIT_SUCCESS;
}
