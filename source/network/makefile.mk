WD           := $(dir $(lastword $(MAKEFILE_LIST)))
WD_NETWORK := $(WD)

SRC += $(WD)checksum.c
SRC += $(WD)dl.c
SRC += $(WD)http.c
SRC += $(WD)network.c
SRC += $(WD)verify.c
