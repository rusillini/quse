# -*- Makefile -*-

# $Id: Makefile,v 1.2 2009/06/09 03:14:31 akotov2 Exp $

topsrc=../..

include $(topsrc)/Makefile.config

INCLUDE=$(INDEXER_DIR)/include
DIRECTORY=$(INDEXER_BUILD_DIR)/directory.o
PATTERN=$(INDEXER_BUILD_DIR)/pattern.o
PARSE_TREE=$(INDEXER_BUILD_DIR)/parse_tree.o
INDEX_WRITER=$(INDEXER_BUILD_DIR)/index_writer.o
INDEXER=$(INDEXER_BUILD_DIR)/indexer.o

OBJECTS=$(DIRECTORY) $(PARSE_TREE) $(PATTERN) $(INDEX_WRITER) $(INDEXER)
BINARY=$(DISTR_DIR)/indexer

$(BINARY): $(OBJECTS)
	$(LD) $(OUT) $(BINARY) $(OBJECTS) $(LDINDIO) $(LDUTILS) $(LDEXPAT) $(LDTHREAD) $(LDMYSQLPP) $(LDL) $(LDSTL)

$(DIRECTORY): Directory.cpp $(INCLUDE)/Directory.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCUTILS) $(CCINDEXER) -c $< -o $@

$(PATTERN): Pattern.cpp $(INCLUDE)/Pattern.hpp $(INCLUDE)/ParseTree.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCUTILS) $(CCINDEXER) -c $< -o $@

$(PARSE_TREE): ParseTree.cpp $(INCLUDE)/ParseTree.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINDEXER) -c $< -o $@

$(INDEX_WRITER): IndexWriter.cpp $(INCLUDE)/IndexWriter.hpp $(INCLUDE)/Pattern.hpp $(INCLUDE)/ParseTree.hpp $(INCLUDE)/Directory.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINDEXER) $(CCINCLUDE) $(CCINDIO) $(CCUTILS) $(CCMYSQLPP) $(CCMYSQL) $ -c $< -o $@

$(INDEXER): Indexer.cpp $(INCLUDE)/IndexWriter.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINDEXER) $(CCINCLUDE) $(CCINDIO) $(CCUTILS) $(CCMYSQLPP) $(CCMYSQL) -c $< -o $@

clean:
	@rm -f $(OBJECTS) $(BINARY)
