.PHONY: luasporth luacall

SPORTHPATH=

ifdef MASTER_MAKEFILE
SPORTHPATH=util/luasporth/
luasporth.so: $(SPORTHPATH)luasporth.so
else
default: luasporth luacall
endif

BIN += $(SPORTHPATH)luasporth.so
BIN += $(SPORTHPATH)luacall.so

luasporth: $(SPORTHPATH)luasporth.so
luacall: $(SPORTHPATH)luacall.so

$(SPORTHPATH)luasporth.so: $(SPORTHPATH)luasporth.c
	gcc -fPIC -shared $< -o $(SPORTHPATH)luasporth.so -lsporth -lsoundpipe -llua 

$(SPORTHPATH)luacall.so: $(SPORTHPATH)luacall.c
	gcc -fPIC -shared $< -o $(SPORTHPATH)luacall.so -lsporth -lsoundpipe -llua 

ifndef MASTER_MAKEFILE
clean:
	rm -rf $(SPORTHPATH)luasporth.so
endif
