WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_FILES := $(WD)

SRC += $(WD)brres.c
SRC += $(WD)gct.c
SRC += $(WD)mp3.c
SRC += $(WD)rlyt.c
SRC += $(WD)tex0.c
SRC += $(WD)tpl.c
SRC += $(WD)ttf.c
SRC += $(WD)u8.c
SRC += $(WD)yaz0.c
