#Set LINGPATH to nothing by default

ifdef MASTER_MAKEFILE
LINGPATH=ugens/ling/
CFLAGS += -I ugens/ling/
ling: $(LINGPATH)ling
else
LING_HASHPATH=../../hash.o
CFLAGS = -I ../../h
endif


LING_OBJ += $(addprefix $(LINGPATH), parse.o func.o stack.o ling.o seq.o)

OBJ += $(LING_OBJ)

#BIN += $(LINGPATH)ling

#$(LINGPATH)ling: $(LINGPATH)ling.c $(LING_OBJ) $(LING_HASHPATH)
#	gcc -Wall $(CFLAGS) $< $(LING_OBJ) $(LING_HASHPATH) -o $@
#
#ifndef MASTER_MAKEFILE
#clean:
#	rm -rf $(OBJ) ling
#endif
