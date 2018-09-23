# -*- Makefile -*-

# $Id: Makefile,v 1.16 2010/04/25 04:30:33 akotov2 Exp $

topsrc=../..

include $(topsrc)/Makefile.config

INCLUDE=$(QPROC_DIR)/include
QTEMPLATE=$(QPROC_BUILD_DIR)/qtemplate.o
RET_MODEL=$(QPROC_BUILD_DIR)/ret_model.o
QPROC_EVAL=$(QPROC_BUILD_DIR)/qproc_eval.o
QPROC_PROD=$(QPROC_BUILD_DIR)/qproc.o

OBJECTS=$(QTEMPLATE) $(RET_MODEL)
BINARY_EVAL=$(DISTR_DIR)/qproc_eval.fcgi
BINARY_PROD=$(DISTR_DIR)/qproc.fcgi

all: $(BINARY_EVAL) $(BINARY_PROD)

$(BINARY_EVAL): $(OBJECTS) $(QPROC_EVAL)
	$(LD) $(OUT) $(BINARY_EVAL) $(OBJECTS) $(QPROC_EVAL) $(LDUTILS) $(LDEXPAT) $(LDINDIO) $(LDMATH) $(LDMYSQLPP) $(LDFCGI) $(LDMYSQL) $(LDTHREAD) $(LDL) $(LDSTL)

$(BINARY_PROD): $(OBJECTS) $(QPROC_PROD)
	$(LD) $(OUT) $(BINARY_PROD) $(OBJECTS) $(QPROC_PROD) $(LDUTILS) $(LDEXPAT) $(LDINDIO) $(LDMATH) $(LDMYSQLPP) $(LDFCGI) $(LDMYSQL) $(LDTHREAD) $(LDL) $(LDSTL)
	
$(QTEMPLATE): QTemplate.cpp $(INCLUDE)/QTemplate.hpp $(INCLUDE_DIR)/Types.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) $(CCQPROC) -c $< -o $@

$(RET_MODEL): RetModel.cpp $(INCLUDE)/RetModel.hpp $(INDIO_DIR)/include/DBIndexIO.hpp $(INCLUDE)/QuestRankedList.hpp $(INCLUDE)/Question.hpp $(INCLUDE_DIR)/Types.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) $(CCINDIO) $(CCMYSQLPP) $(CCMYSQL) $(CCQPROC) -c $< -o $@

$(QPROC_EVAL): QProcEval.cpp $(INCLUDE)/RetModel.hpp $(INDIO_DIR)/include/DBIndexIO.hpp $(INCLUDE)/QuestRankedList.hpp $(INCLUDE)/Question.hpp $(INCLUDE_DIR)/Types.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) $(CCINDIO) $(CCMYSQLPP) $(CCMYSQL) $(CCQPROC) -c $< -o $@

$(QPROC_PROD): QProc.cpp $(INCLUDE)/RetModel.hpp $(INDIO_DIR)/include/DBIndexIO.hpp $(INCLUDE)/QuestRankedList.hpp $(INCLUDE)/Question.hpp $(INCLUDE_DIR)/Types.hpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) $(CCINDIO) $(CCMYSQLPP) $(CCMYSQL) $(CCQPROC) -c $< -o $@

clean:
	@rm -f $(OBJECTS) $(QPROC_EVAL) $(QPROC_PROD) $(BINARY_EVAL) $(BINARY_PROD)