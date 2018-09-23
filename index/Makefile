# -*- Makefile -*-

# $Id: Makefile,v 1.1.1.1 2009/03/19 19:14:15 akotov2 Exp $

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
