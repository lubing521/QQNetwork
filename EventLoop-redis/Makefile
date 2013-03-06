uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OPTIMIZATION?=-O2

LDFLAGS = -Wl,-rpath,/home/y/lib64

CCOPT= $(CFLAGS) $(ARCH) $(PROF)

PREFIX?=/usr/local

AR = ar
#OBJ = anet.o daytimetcpsrv.o anet_test.o

TARGET = libae.a
TESTSRV = echo-srv

all: $(TARGET) $(TESTSRV)

anet.o: anet.c anet.h fmacros.h
zmalloc.o: zmalloc.c zmalloc.h
ae.o: ae.h ae.c ae_epoll.c

$(TARGET): anet.o zmalloc.o ae.o
	$(AR) r $(TARGET) anet.o zmalloc.o ae.o


$(TESTSRV): $(TARGET) anet_test.c anet.h anet_test.h zmalloc.c zmalloc.h
	$(CC)  -Wall -I. -o $(TESTSRV) anet_test.c anet_test.h $(TARGET)
.PHONY: all clean test

test:


#%.o: %.c 
#	$(QUIET_CC)$(CC)  -c $(CFLAGS) $(ALLOC_FLAGS) $(DEBUG) $(COMPILE_TIME) $<

clean:
	rm -rf $(TARGET) $(TESTSRV) $(TESTCLI) *.a *.o *.gcda *.gcno *.gcov

# unused now

gprof:
	$(MAKE) PROF="-pg"

gcov:
	$(MAKE) PROF="-fprofile-arcs -ftest-coverage"

noopt:
	$(MAKE) OPTIMIZATION=""

32bitgprof:
	$(MAKE) PROF="-pg" ARCH="-arch i386"

