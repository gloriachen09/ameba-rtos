
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                                   VARIABLES                                 #
#*****************************************************************************#
DIRNET = $(BASEDIR)/component/network
DIR = $(BASEDIR)/component/wifi/api

vpath %.c $(DIR)
vpath %.c $(DIRNET) $(shell find $(DIRNET) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
# AP
ifeq ($(CONFIG_AS_AP), y)
CSRC += $(DIRNET)/ttcp/ttcp.c \
		$(DIRNET)/ping/ping_test.c

endif


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
