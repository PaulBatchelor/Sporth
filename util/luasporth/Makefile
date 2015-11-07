
SPORTHPATH=./

ifdef MASTER_MAKEFILE
SPORTHPATH=util/luasporth/
luasporth: $(SPORTHPATH)luasporth
endif
BIN += $(SPORTHPATH)luasporth

$(SPORTHPATH)luasporth: $(SPORTHPATH)luasporth.c
	gcc $< -ldl -lsporth -lsoundpipe -lsndfile -llua -lm -o $@

SPORTHPATH= 
