############################################################################

all:

clean:
	$(MAKE) -C scripts clean
	$(MAKE) -C scripts/lxdialog clean
	rm -f config.tk autoconf.h .config .config.old

############################################################################
#
# The config stuff
#

config.tk: Makefile config.in $(SCRIPTSDIR)/header.tk $(SCRIPTSDIR)/tail.tk
	$(MAKE) -C $(SCRIPTSDIR) tkparse
	ARCH=m68knommu $(SCRIPTSDIR)/tkparse < config.in > config.tmp
	@if [ -f /usr/local/bin/wish ];	then \
		echo '#!'"/usr/local/bin/wish -f" > config.tk; \
	else \
		echo '#!'"/usr/bin/wish -f" > config.tk; \
	fi
	cat $(SCRIPTSDIR)/header.tk >> ./config.tk
	cat config.tmp >> config.tk
	rm -f config.tmp
	echo "set defaults \"defconfig\"" >> config.tk
	echo "set autoconf_file \"autoconf.h\"" >> config.tk
	echo "set help_file \"Configure.help\"" >> config.tk
	cat $(SCRIPTSDIR)/tail.tk >> config.tk
	chmod 755 config.tk

xconfig: config.tk
	wish -f config.tk

oldconfig:
	@HELP_FILE=Configure.help \
		AUTOCONF_FILE=autoconf.h \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure -d config.in

config:
	@HELP_FILE=Configure.help \
		AUTOCONF_FILE=autoconf.h \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure config.in

menuconfig:
	$(MAKE) -C $(SCRIPTSDIR)/lxdialog all
	@HELP_FILE=$(MENUCONFIG_NAME)/Configure.help \
		AUTOCONF_FILE=autoconf.h \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Menuconfig config.in

############################################################################
