WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_REGION := $(WD)

SRC += $(WD)jap.c
SRC += $(WD)usa.c
SRC += $(WD)pal.c
SRC += $(WD)kor.c
