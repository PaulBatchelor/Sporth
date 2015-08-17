
SPORTHPATH=./

ifdef MASTER_MAKEFILE
SPORTHPATH=util/luasporth/
luasporth: $(SPORTHPATH)luasporth
endif
OBJ += $(SPORTHPATH)luasporth

$(SPORTHPATH)luasporth: $(SPORTHPATH)luasporth.c
	gcc $< -ldl -lsporth -lsoundpipe -lsndfile -llua -lm -o $@

