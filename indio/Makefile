# -*- Makefile -*-

# $Id: Makefile,v 1.1 2009/05/28 08:36:17 akotov2 Exp $

topsrc=..
commons=clean depend

MAKE_SUBDIRS=src

.PHONY: $(commons) $(MAKE_SUBDIRS)

all: $(MAKE_SUBDIRS)

$(MAKE_SUBDIRS):
	@$(MAKE) -C $@

$(commons):
	@for dir in $(MAKE_SUBDIRS); do \
	   $(MAKE) -C $$dir $@;         \
	done
