# -*- Makefile -*-

# $Id: Makefile,v 1.5 2009/06/05 07:21:50 akotov2 Exp $

topsrc=../..

include $(topsrc)/Makefile.config

INCLUDE=$(INDIO_DIR)/include
DB_INDEX_IO=$(INDIO_BUILD_DIR)/db_index_io.o

OBJECTS=$(DB_INDEX_IO)
INDIO_LIB=$(LIBS_DIR)/libindio.so

$(INDIO_LIB): $(OBJECTS)
	$(LD) $(LDSHARED) $(OUT) $(INDIO_LIB) $(OBJECTS)

$(DB_INDEX_IO): DBIndexIO.cpp $(INCLUDE)/DBIndexIO.hpp $(INCLUDE_DIR)/IndexIO.hpp $(INCLUDE_DIR)/Types.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCINDIO) $(CCUTILS) $(CCMYSQLPP) $(CCMYSQL) -c $< -o $@
	
clean:
	@rm -f $(OBJECTS) $(INDIO_LIB)