
include $(MAKE_INCLUDE_GEN)



.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/example/aivoice/full_flow_offline

MODULE_IFLAGS += -I$(BASEDIR)/component/aivoice/include

vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += \
	$(DIR)/app_example.c \
	$(DIR)/aivoice_algo_example.c \
    $(DIR)/testwav.c \

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#

COBJS = $(notdir $(CSRC:.c=.o))
OBJS += $(COBJS)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
