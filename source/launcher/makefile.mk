WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_LAUNCHER := $(WD)

SRC += $(WD)checksum.c
SRC += $(WD)cups.c
SRC += $(WD)di.c
SRC += $(WD)gecko.c
SRC += $(WD)launcher.c
SRC += $(WD)patch.c
