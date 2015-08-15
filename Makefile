#default: hash stacks func
#default: func
default: sporth

CFLAGS += -O3

ifdef DEBUG_MODE
CFLAGS += -DDEBUG_MODE
endif

UGENS = basic metro tenv fm revsc gen_sine osc gen_vals tseq in port \
	nsmp prop noise dcblock butlp buthp maygate randi rpt reverse \
	samphold delay switch mode clip p count f gen_sinesum gen_line

OBJ += $(addprefix ugens/, $(addsuffix .o, $(UGENS)))

OBJ += func.o plumber.o stack.o parse.o hash.o

%.o: %.c h/ugens.h
	gcc $(CFLAGS) -g -c -Ih $< -o $@

ugens/%.o: ugens/%.c
	gcc $(CFLAGS) -g -Ih -c $< -o $@

util/jack_wrapper: jack_wrapper.c
	gcc $< -lsoundpipe -lsndfile -ljack -o jack_wrapper -lm

val: util/val

util/val: util/val.c
	gcc $< -o $@

sporth: sporth.c $(OBJ) h/ugens.h
	gcc sporth.c $(CFLAGS) -g -Ih -o $@ $(OBJ) -lsoundpipe -lsndfile -lm

libsporth.a: $(OBJ) tmp.h
	ar rcs libsporth.a $(OBJ)

tmp.h: $(OBJ)
	sh util/header_gen.sh

examples/parse: examples/parse.c libsporth.a h/ugens.h
	gcc $< $(CFLAGS) -g -Ih -o $@ libsporth.a -lsoundpipe -lsndfile -lm

examples/user_function: examples/user_function.c libsporth.a h/ugens.h
	gcc $< $(CFLAGS) -g -Ih -o $@ libsporth.a -lsoundpipe -lsndfile -lm

install: libsporth.a sporth tmp.h
	install sporth /usr/local/bin
	install tmp.h /usr/local/include/sporth.h
	install libsporth.a /usr/local/lib
	mkdir -p /usr/local/share/sporth
	install ugen_reference.txt /usr/local/share/sporth
	install util/ugen_lookup /usr/local/bin

clean:
	rm -rf sporth $(OBJ) util/jack_wrapper util/val examples/parse
	rm -rf examples/user_function
	rm -rf tmp.h
	rm -rf libsporth.a

