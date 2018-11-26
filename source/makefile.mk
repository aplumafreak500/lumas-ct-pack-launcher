WD     := $(dir $(lastword $(MAKEFILE_LIST)))
WD_SRC := $(WD)

SRC += $(WD)main.c
SRC += $(WD)version.c
SRC += $(WD)error.c

include $(WD_SRC)brainslug/makefile.mk
include $(WD_SRC)brainslug-modules/makefile.mk
include $(WD_SRC)ctgp/makefile.mk
include $(WD_SRC)files/makefile.mk
include $(WD_SRC)gameregion/makefile.mk
include $(WD_SRC)launcher/makefile.mk
include $(WD_SRC)network/makefile.mk
include $(WD_SRC)patch/makefile.mk
include $(WD_SRC)ui/makefile.mk
include $(WD_SRC)updater/makefile.mk

