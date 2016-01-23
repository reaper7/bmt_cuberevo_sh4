CROSS = sh4-linux-
CC = $(CROSS)gcc
STRIP = $(CROSS)strip

objs += main.o

CFLAGS += -I./
CFLAGS += -I../../include/
CFLAGS += -I../../include/sbox
CFLAGS += -I../../include/modules
CFLAGS += -I../../include/tools
CFLAGS += -I../../../cuberevo-plugin/plugin-include
CFLAGS += -I./include
CFLAGS += -Wall
CFLAGS += -Os

CFLAGS += -DDEBUG
CFLAGS += -DDEBUG_LEVEL=5

LIBS += ../../../cuberevo-plugin/plugin-include/front_display/front_display.o
LIBS += ../../../cuberevo-plugin/plugin-include/config_manager/config_manager.o

PLUGIN = scriptoman
PLUG_DIR=/var/plug-in
VERSION=0.0.2

all : clean $(PLUGIN).plugin install

clean :
	rm -f $(PLUGIN).plugin
	rm -f *.o
	
$(PLUGIN).plugin : $(objs) 
	$(CC) $(CFLAGS) -shared -nostartfiles -o $@ $(LIBS) $^
	$(STRIP) $@

install:
	ncftpput  -u root -p yourpassword 192.168.0.3 ${PLUG_DIR} ${PLUGIN}.plugin

targz:
	tar cvfz cuberevo-ip9000-${PLUGIN}-plugin.tar.gz  ${PLUGIN}.plugin include/*.h Makefile *.c

svnup: clean
	svn commit -m 'Some changes...'

