WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_CTGP := $(WD)

SRC += $(WD)main.dol.c
SRC += $(WD)staticr.rel.c
SRC += $(WD)region.c
SRC += $(WD)strap.szs.c
