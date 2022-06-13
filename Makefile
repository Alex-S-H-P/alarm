all: alarm
	@echo "\033[32;1mDone\033[0m"
alarm: alarm.c
	gcc alarm.c -o alarm $(CFLAGS)
