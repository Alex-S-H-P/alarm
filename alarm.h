#ifndef ALARM_HEADER_LOCK
#define ALARM_HEADER_LOCK 1

#define XSTR(x) STR(x)
#define STR(x) #x

#define HELP_COMMAND_SEQUENCE "\t%s\n\t\t%s\n\t\t%s\n"
#define SECONDS_PER_DAY 86400

#ifndef COLORIZED
#define COLORIZED 1
#endif

#if COLORIZED

#ifndef C_ERR
#define C_ERR "\033[31;1m"
#endif
#ifndef C_HIGHL
#define C_HIGHL "\033[36;1m"
#endif
#ifndef C_CORR
#define C_CORR "\033[32;1m"
#endif
#ifndef C_GREY
#define C_GREY "\033[37m"
#endif
#ifndef C_CLR
#define C_CLR "\033[0m"
#endif
#define C_BOLD "\033[1m"
#else
#define C_ERR ""
#define C_HIGHL ""
#define C_CORR ""
#define C_GREY ""
#define C_BOLD ""
#endif

#ifndef WARN_SOUND_PLAYER
#define WARN_SOUND_PLAYER "vlc "
#endif

#pragma message XSTR(WARN_SOUND_PLAYER)

struct alarm_command {
  const char *command;
};

const char *getArg(int const idx, int const argc, char const *argv[]);

void handle_unknown_command(const char *cmd);

void display_help();

void ring(const char *cmd);

void planAt(int hour, int minute, const char *cmd);

void planIn(int hour, int minute, const char *cmd);

int handle_command(int const argv_index, int const argc, char const *argv[]);

int main(int argc, char const *argv[]);

#endif
