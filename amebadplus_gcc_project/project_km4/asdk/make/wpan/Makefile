
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
TIMER = $(BASEDIR)/component/wpan/platform/timer
FLASH = $(BASEDIR)/component/wpan/platform/flash
PLATFORM_CONFIG = $(BASEDIR)/component/wpan/platform/config

vpath %.c $(TIMER) $(shell find $(TIMER) -type d)
vpath %.c $(FLASH) $(shell find $(FLASH) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#802154 timer
CSRC += $(TIMER)/rtk_802154_timer.c \
		$(TIMER)/rtk_802154_timer_sched.c
		
CSRC += $(FLASH)/rtk_802154_flash.c

MODULE_IFLAGS += -I$(TIMER)/
MODULE_IFLAGS += -I$(FLASH)/
MODULE_IFLAGS += -I$(PLATFORM_CONFIG)/
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebadplus/fwlib/include
MODULE_IFLAGS += -I$(BASEDIR)/component/os/os_dep/

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
ifeq ($(CONFIG_802154_THREAD_EN),y)	
	make -C openthread lib
endif
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS) $(ASM_OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

ifeq ($(CONFIG_802154_THREAD_EN),y)	
	make -C openthread clean
endif	
-include $(DEPS)
