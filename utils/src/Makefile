# -*- Makefile -*-

# $Id: Makefile,v 1.4 2009/06/01 08:44:33 akotov2 Exp $

topsrc=../..

include $(topsrc)/Makefile.config

CPP_SRC=$(wildcard *.cpp)
CPP_OBJ=$(patsubst %.cpp,$(UTILS_BUILD_DIR)/%.o,$(CPP_SRC))

C_SRC=$(wildcard *.c)
C_OBJ=$(patsubst %.c,$(UTILS_BUILD_DIR)/%.o,$(C_SRC))

INCLUDE=$(UTILS_DIR)/include
UTILS_LIB=$(LIBS_DIR)/libutils.so

$(UTILS_LIB): $(CPP_OBJ) $(C_OBJ)
	$(LD) $(LDSHARED) $(OUT) $(UTILS_LIB) $(CPP_OBJ) $(C_OBJ)

$(UTILS_BUILD_DIR)/bufstream.o: bufstream.cpp $(INCLUDE)/bufstream.h $(INCLUDE)/config.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/cm_misc.o: cm_misc.cpp $(INCLUDE)/config.h $(INCLUDE)/memory.h $(INCLUDE)/except.h $(INCLUDE)/envparm.h $(INCLUDE)/strutils.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/dll.o: dll.c $(INCLUDE)/dll.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/encoding.o: encoding.cpp $(INCLUDE)/encoding.h $(INCLUDE)/config.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/environ.o: environ.c $(INCLUDE)/environ.h $(INCLUDE)/config.h $(INCLUDE)/memory.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/event.o: event.c $(INCLUDE)/event.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/files.o: files.c $(INCLUDE)/files.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/memory.o: memory.c $(INCLUDE)/memory.h $(INCLUDE)/config.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/mutex.o: mutex.c $(INCLUDE)/mutex.h $(INCLUDE)/config.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/process.o: process.c $(INCLUDE)/process.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@
	
$(UTILS_BUILD_DIR)/file_utils.o: file_utils.cpp $(INCLUDE)/file_utils.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/restart.o: restart.c $(INCLUDE)/restart.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/smldata.o: smldata.cpp $(INCLUDE)/config.h $(INCLUDE)/bufstream.h $(INCLUDE)/encoding.h $(INCLUDE)/smldata.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/smlenc.o: smlenc.cpp $(INCLUDE)/config.h $(INCLUDE)/cmtypes.h $(INCLUDE)/envparm.h $(INCLUDE)/encoding.h $(INCLUDE)/smldata.h $(INCLUDE)/smlxt.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/smlproc.o: smlproc.cpp $(INCLUDE)/config.h $(INCLUDE)/cmtypes.h $(INCLUDE)/smldata.h $(INCLUDE)/smlproc.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/smlxt.o: smlxt.cpp $(INCLUDE)/config.h $(INCLUDE)/cmtypes.h $(INCLUDE)/smldata.h $(INCLUDE)/envparm.h $(INCLUDE)/encoding.h $(INCLUDE)/encoding.h $(INCLUDE)/smlxt.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@
	
$(UTILS_BUILD_DIR)/strutils.o: strutils.cpp $(INCLUDE)/strutils.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/thread.o: thread.c $(INCLUDE)/thread.h $(INCLUDE)/config.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/thread_pool.o: thread_pool.cpp $(INCLUDE)/config.h $(INCLUDE)/thread_pool.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/thread_queue.o: thread_queue.cpp $(INCLUDE)/config.h $(INCLUDE)/memory.h $(INCLUDE)/thread_queue.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/tmutil.o: tmutil.c $(INCLUDE)/tmutil.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

$(UTILS_BUILD_DIR)/vprocbase.o: vprocbase.cpp $(INCLUDE)/config.h $(INCLUDE)/vprocbase.h
	$(CC) $(CCFLAGS) $(CCSH) $(CCOPT) $(CCUTILS) -c $< -o $@

clean:
	@rm -f $(UTILS_BUILD_DIR)/*.o $(UTILS_LIB)
