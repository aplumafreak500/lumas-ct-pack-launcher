WD     := $(dir $(lastword $(MAKEFILE_LIST)))
WD_BS := $(WD)

include $(WD_BS)apploader/makefile.mk
include $(WD_BS)di/makefile.mk
include $(WD_BS)libelf/makefile.mk
include $(WD_BS)search/makefile.mk
include $(WD_BS)modules/makefile.mk
