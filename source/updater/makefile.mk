WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_UPDATER := $(WD)

SRC += $(WD)updater.c
