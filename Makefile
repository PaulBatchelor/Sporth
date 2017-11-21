default: sporth

MASTER_MAKEFILE=1

CFLAGS += -O3 -fPIC -I/usr/local/include -Wall -ansi

include config.mk
ifdef DEBUG_MODE
CFLAGS += -DDEBUG_MODE -DPOLY_DEBUG
endif

ifdef LIVE_CODING
CFLAGS += -DLIVE_CODING
OBJ += util/listener.o
LIBS += -lpthread
endif

ifdef BUILD_JACK
CFLAGS += -DBUILD_JACK -DLIVE_CODING
OBJ += util/sp_jack.o 
LIBS += -ljack 
endif

ifdef NO_LIBDL
CFLAGS += -DNO_LIBDL
else
LIBS += -ldl
endif

include ugens/cdb/Makefile

ifdef BUILD_POLYSPORTH
include ugens/polysporth/Makefile
else
CFLAGS += -DNO_POLYSPORTH
endif

BIN += util/val util/float2bin util/sporthdot util/lsys util/ugen_dump \
	   util/sporth_tex


OBJ += $(addprefix ugens/, $(addsuffix .o, $(UGENS)))

OBJ += func.o plumber.o stack.o parse.o hash.o ftmap.o

SPORTHLIBS = libsporth.a

LIBS += -lsoundpipe -lsndfile -lm 

config.mk: config.def.mk
	cp config.def.mk config.mk

%.o: %.c h/ugens.h
	$(CC) $(CFLAGS) -g -c -Ih -I. $< -o $@

ugens/%.o: ugens/%.c
	$(CC) $(CFLAGS) -g -Ih -I. -c $< -o $@

util/jack_wrapper: util/jack_wrapper.c
	$(CC) $< -ljack $(LIBS) -o jack_wrapper -lm

val: util/val

util/val: util/val.c
	$(CC) $< -o $@

ugen_dump: util/ugen_dump
 
util/ugen_dump: util/ugen_dump.c
	$(CC) $(CFLAGS) -Ih $< -o $@

float2bin: util/float2bin

util/float2bin: util/float2bin.c
	$(CC) $< -o $@

jacksporth: util/jacksporth
util/jacksporth: util/jacksporth.c libsporth.a
	$(CC) $< -L. -lsporth $(LIBS) -lm -ljack -llo -o $@ 

sporthdot: util/sporthdot
util/sporthdot: util/sporthdot.c libsporth.a
	$(CC) $< $(CFLAGS) -Ih libsporth.a $(LIBS) -lm -ljack -o $@

lsys: util/lsys
util/lsys: ugens/lsys.c
	$(CC) -DLSYS_STANDALONE $< -o $@

sporth_tex: util/sporth_tex

util/sporth_tex: util/sporth_tex.c libsporth.a
	$(CC) -Ih $(CFLAGS) $< -o $@ libsporth.a $(LIBS)

sporth: sporth.c $(OBJ) h/ugens.h
	$(CC) sporth.c -L/usr/local/lib $(CFLAGS) -g -Ih -o $@ $(OBJ) $(KOBJ) $(LIBS) 

libsporth.a: $(OBJ) sporth.h
	$(AR) rcs libsporth.a $(KOBJ) $(OBJ) 

sporth.h: $(OBJ)
	sh util/header_gen.sh

install: $(SPORTHLIBS) sporth sporth.h
	install sporth /usr/local/bin
	install sporth.h /usr/local/include/sporth.h
	install ugens/cdb/cdb.h /usr/local/include/cdb.h
	install $(SPORTHLIBS) /usr/local/lib
	mkdir -p /usr/local/share/sporth
	install ugen_reference.txt /usr/local/share/sporth
	install util/ugen_lookup /usr/local/bin
	install util/spparse /usr/local/bin
	install util/sp_eval_file /usr/local/bin
	./util/installer.sh $(BIN)
	./ugens/polysporth/install.sh

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf sporth.h
	rm -rf libsporth.a libsporth_dyn.so
	rm -rf sporth

