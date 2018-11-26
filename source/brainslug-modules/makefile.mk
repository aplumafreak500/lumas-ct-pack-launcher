WD     := $(dir $(lastword $(MAKEFILE_LIST)))
WD_BSMOD := $(WD)

SRC += $(WD)bslug-modules.s
SRC += $(WD)bslug-symbols.s
