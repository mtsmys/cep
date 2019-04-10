CC          := gcc 
INCLUDEDIR  := ./include/
INCLUDEPATH := -I. -I$(INCLUDEDIR) 
CFLAGS      := $(INCLUDEPATH) -O3 -Wall -fPIC -Wno-pointer-sign -MMD -MP
SRCDIR      := ./src/
SRCS        := $(SRCDIR)m2m/M2MSystem.c \
               $(SRCDIR)m2m/lib/io/M2MHeap.c \
               $(SRCDIR)m2m/lib/io/M2MByteOrder.c \
               $(SRCDIR)m2m/lib/lang/M2MString.c \
               $(SRCDIR)m2m/lib/time/M2MDate.c \
               $(SRCDIR)m2m/lib/log/M2MLogLevel.c \
               $(SRCDIR)m2m/lib/log/M2MLogger.c \
               $(SRCDIR)m2m/lib/util/M2MBase64.c \
               $(SRCDIR)m2m/lib/util/list/M2MList.c \
               $(SRCDIR)m2m/lib/security/acl/M2MACLPermission.c \
               $(SRCDIR)m2m/lib/security/acl/M2MACLUser.c \
               $(SRCDIR)m2m/lib/security/acl/M2MACLGroup.c \
               $(SRCDIR)m2m/lib/security/acl/M2MACL.c \
               $(SRCDIR)m2m/lib/security/M2MCRC32.c \
               $(SRCDIR)m2m/lib/security/M2MSHA1.c \
               $(SRCDIR)m2m/lib/security/M2MXorshift.c \
               $(SRCDIR)m2m/lib/net/network/M2MIPAddress.c \
               $(SRCDIR)m2m/lib/io/M2MFileDescriptor.c \
               $(SRCDIR)m2m/lib/io/M2MDirectory.c \
               $(SRCDIR)m2m/lib/io/M2MFile.c \
               $(SRCDIR)m2m/lib/log/M2MFileAppender.c \
               $(SRCDIR)m2m/lib/db/M2MSQLiteDataType.c \
               $(SRCDIR)m2m/lib/db/M2MSQLite.c \
               $(SRCDIR)m2m/lib/db/M2MColumn.c \
               $(SRCDIR)m2m/lib/db/M2MColumnList.c \
               $(SRCDIR)m2m/lib/db/M2MTableManager.c \
               $(SRCDIR)m2m/app/cep/M2MDataFrame.c \
               $(SRCDIR)m2m/app/cep/M2MCEP.c \
               $(SRCDIR)m2m/app/graph/M2MNode.c \
               $(SRCDIR)m2m/app/graph/M2MGraph.c 
OBJS        := $(SRCS:%.c=%.o) 
DEPS        := $(SRCS:%.c=%.d) 
TARGET      := libcep.so.0.6.0 
SONAME      := libcep.so.0 
TARGETLINK  := libcep.so 
LDFLAGS     := -shared -Wl,-soname=$(SONAME) 
LIBS        := -lsqlite3 -lpthread 
DESTDIR     := /usr/lib/arm-linux-gnueabihf/
INCDESTDIR  := /usr/include/ 


.PHONY: all
all: $(TARGET)

-include $(DEPS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) *~

.PHONY: install
install:
	mv $(TARGET) $(DESTDIR)
	ldconfig
	ln -fs $(DESTDIR)$(TARGET) $(DESTDIR)$(TARGETLINK)
	cp -r $(INCLUDEDIR)m2m $(INCDESTDIR)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $(OBJS)

