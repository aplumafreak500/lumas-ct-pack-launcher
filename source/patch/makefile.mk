WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_PATCHES := $(WD)

SRC += $(WD)cd.c
SRC += $(WD)ctww.c
SRC += $(WD)dwc.c
SRC += $(WD)files.c
SRC += $(WD)languages.c
SRC += $(WD)mystuff.c
SRC += $(WD)patch.c
SRC += $(WD)region.c
SRC += $(WD)supermenu.c
SRC += $(WD)tracks.c
