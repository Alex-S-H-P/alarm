PLAYER := "\"$(shell ./SOUND_PLAYER.sh)\""

all: alarm
	@echo -e "\033[32;1mDone\033[0m"
alarm: alarm.c SOUND_PLAYER.sh alarm.h
	@ $(info PLAYER is $(PLAYER))
	gcc alarm.c -o alarm $(CFLAGS) -DWARN_SOUND_PLAYER=$(PLAYER)
