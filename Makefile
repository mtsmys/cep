CC          := gcc 
INCLUDEDIR  := ./include/
INCLUDEPATH := -I. -I$(INCLUDEDIR) 
CFLAGS      := $(INCLUDEPATH) -O3 -Wall -fPIC -Wno-pointer-sign -MMD -MP
SRCDIR      := ./src/
SRCS        := $(SRCDIR)m2m/io/M2MHeap.c \
               $(SRCDIR)m2m/lang/M2MString.c \
               $(SRCDIR)m2m/io/M2MDirectory.c \
               $(SRCDIR)m2m/io/M2MFile.c \
               $(SRCDIR)m2m/util/M2MBase64.c \
               $(SRCDIR)m2m/util/list/M2MList.c \
               $(SRCDIR)m2m/util/logging/M2MLogger.c \
               $(SRCDIR)m2m/db/M2MSQLRunner.c \
               $(SRCDIR)m2m/db/M2MSQLiteConfig.c \
               $(SRCDIR)m2m/db/M2MDataType.c \
               $(SRCDIR)m2m/db/M2MColumn.c \
               $(SRCDIR)m2m/db/M2MColumnList.c \
               $(SRCDIR)m2m/db/M2MTableManager.c \
               $(SRCDIR)m2m/cep/M2MCEPRecord.c \
               $(SRCDIR)m2m/cep/M2MCEP.c 
OBJS        := $(SRCS:%.c=%.o) 
DEPS        := $(SRCS:%.c=%.d) 
TARGET      := libcep.so.0.3.0 
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

