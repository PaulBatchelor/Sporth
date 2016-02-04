
SPORTHPATH=./

ifdef MASTER_MAKEFILE
SPORTHPATH=util/luasporth/
luasporth: $(SPORTHPATH)luasporth
endif
BIN += $(SPORTHPATH)luasporth

$(SPORTHPATH)luasporth: $(SPORTHPATH)luasporth.c
	gcc -g -I/home/paul/playground/kona/kona/src/ $< -ldl -lsporth -lsoundpipe -lsndfile -lpthread -llua -lm -o $@

SPORTHPATH= 
