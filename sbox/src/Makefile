# -*- Makefile -*-

# $Id: Makefile,v 1.4 2010/04/25 04:30:33 akotov2 Exp $

topsrc=../..

include $(topsrc)/Makefile.config

SBOX_EVAL=$(SBOX_BUILD_DIR)/sbox_eval.o
SBOX_PROD=$(SBOX_BUILD_DIR)/sbox.o

BINARY_EVAL=$(DISTR_DIR)/sbox_eval.fcgi
BINARY_PROD=$(DISTR_DIR)/sbox.fcgi

all: $(BINARY_EVAL) $(BINARY_PROD)

$(BINARY_EVAL): $(SBOX_EVAL)
	$(LD) $(OUT) $(BINARY_EVAL) $(SBOX_EVAL) $(LDUTILS) $(LDINDIO) $(LDMYSQLPP) $(LDMYSQL) $(LDFCGI) $(LDEXPAT) $(LDTHREAD) $(LDL) $(LDSTL)
	
$(BINARY_PROD): $(SBOX_PROD)
	$(LD) $(OUT) $(BINARY_PROD) $(SBOX_PROD) $(LDUTILS) $(LDFCGI) $(LDEXPAT) $(LDTHREAD) $(LDL) $(LDSTL)
	
$(SBOX_EVAL): SBoxEval.cpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) $(CCINDIO) $(CCMYSQLPP) $(CCMYSQL) -c $< -o $@

$(SBOX_PROD): SBox.cpp
	$(CC) $(CCOPT) $(CCFLAGS) $(CCINCLUDE) $(CCUTILS) -c $< -o $@
	
clean:
	@rm -f $(BINARY_EVAL) $(BINARY_PROD) $(SBOX_EVAL) $(SBOX_PROD)  