WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_UI := $(WD)

SRC += $(WD)channel.c
SRC += $(WD)cups.c
SRC += $(WD)dialog.c
SRC += $(WD)font.c
SRC += $(WD)launcher.c
SRC += $(WD)main.c
SRC += $(WD)splash.c
SRC += $(WD)ui.c
SRC += $(WD)updater.c
