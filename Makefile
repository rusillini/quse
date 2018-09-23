# -*- Makefile -*-

# $Id: Makefile,v 1.4 2009/06/04 00:30:41 akotov2 Exp $

topsrc=.
commons=clean depend

MAKE_SUBDIRS=utils indio index qproc sbox

all: $(MAKE_SUBDIRS)
min: utils
.PHONY: all min $(commons) $(MAKE_SUBDIRS)

$(MAKE_SUBDIRS):
	@$(MAKE) -C $@

$(commons):
	@for dir in $(MAKE_SUBDIRS); do \
	   $(MAKE) -C $$dir $@;         \
	done

# dependencies
index qproc sbox: indio
index qproc sbox: utils