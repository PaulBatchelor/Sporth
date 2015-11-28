#default: hash stacks func
#default: func
default: sporth

MASTER_MAKEFILE=1

CFLAGS += -O3 -fPIC -L./ -I ./ -Wall

ifdef DEBUG_MODE
CFLAGS += -DDEBUG_MODE 
endif

UGENS = basic metro tenv fm revsc gen_sine osc gen_vals tseq in port \
	nsmp prop noise dcblock butlp buthp maygate randi rpt reverse \
	samphold delay switch mode clip p count f gen_sinesum gen_line \
	dmetro gbuzz jitter diskin pluck tin jcrev scale tenv2 moogladder \
   	vdelay t line expon ling mincer loadfile poly saw square bitcrush \
	dist tenvx pan randh

include ugens/ling/Makefile
include ugens/poly/Makefile

BIN += sporth examples/parse examples/user_function util/jack_wrapper util/val \
	  util/float2bin


OBJ += $(addprefix ugens/, $(addsuffix .o, $(UGENS)))

OBJ += func.o plumber.o stack.o parse.o hash.o

SPORTHLIBS = libsporth.a



ifndef NO_DYNAMIC
SPORTHLIBS += libsporth_dyn.so
endif

%.o: %.c h/ugens.h
	gcc $(CFLAGS) -g -c -Ih $< -o $@

ugens/%.o: ugens/%.c
	gcc $(CFLAGS) -g -Ih -c $< -o $@

util/jack_wrapper: util/jack_wrapper.c
	gcc $< -ljack -lsoundpipe -lsndfile -o jack_wrapper -lm

val: util/val

util/val: util/val.c
	gcc $< -o $@

float2bin: util/float2bin

util/float2bin: util/float2bin.c
	gcc $< -o $@


sporth: sporth.c $(OBJ) h/ugens.h
	gcc sporth.c $(CFLAGS) -g -Ih -o $@ $(OBJ) -lsoundpipe -lsndfile -lm

libsporth_dyn.so: $(OBJ)
	ld -shared -fPIC -o $@ $(OBJ)

libsporth.a: $(OBJ) tmp.h
	ar rcs libsporth.a $(OBJ)

tmp.h: $(OBJ)
	sh util/header_gen.sh

examples/parse: examples/parse.c libsporth.a h/ugens.h
	gcc $< $(CFLAGS) -g -Ih -o $@ libsporth.a -lsoundpipe -lsndfile -lm

examples/user_function: examples/user_function.c libsporth.a h/ugens.h
	gcc $< $(CFLAGS) -g -Ih -o $@ libsporth.a -lsoundpipe -lsndfile -lm

include util/luasporth/Makefile

install: $(SPORTHLIBS) sporth tmp.h 
	install sporth /usr/local/bin
	install tmp.h /usr/local/include/sporth.h
	install $(SPORTHLIBS) /usr/local/lib
	mkdir -p /usr/local/share/sporth
	install ugen_reference.txt /usr/local/share/sporth
	install util/ugen_lookup /usr/local/bin

clean:
	rm -rf $(OBJ) 
	rm -rf $(BIN)
	rm -rf tmp.h
	rm -rf libsporth.a libsporth.so

