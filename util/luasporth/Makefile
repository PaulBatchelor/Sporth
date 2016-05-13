.PHONY: luasporth 

SPORTHPATH=

ifdef MASTER_MAKEFILE
SPORTHPATH=util/luasporth/
luasporth.so: $(SPORTHPATH)luasporth.so
endif

BIN += $(SPORTHPATH)luasporth.so

luasporth: $(SPORTHPATH)luasporth.so

$(SPORTHPATH)luasporth.so: $(SPORTHPATH)luasporth.c
	gcc -fPIC -shared $< -o $(SPORTHPATH)luasporth.so -lsporth -llua 

ifndef MASTER_MAKEFILE
clean:
	rm -rf $(SPORTHPATH)luasporth.so
endif
