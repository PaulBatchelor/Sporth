#default: hash stacks func
#default: func
default: sporth

CFLAGS += -O3

ifdef DEBUG_MODE
CFLAGS += -DDEBUG_MODE
endif

UGENS = basic metro tenv fm revsc gen_sine osc gen_vals tseq in port \
	nsmp prop noise dcblock butlp buthp maygate randi rpt reverse \
	samphold delay switch mode clip p count

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

libsporth.a: $(OBJ)
	ar rcs libsporth.a $(OBJ)

examples/parse: examples/parse.c libsporth.a h/ugens.h
	gcc $< $(CFLAGS) -g -Ih -o $@ libsporth.a -lsoundpipe -lsndfile -lm

install:
	install sporth /usr/local/bin

clean:
	rm -rf sporth $(OBJ) util/jack_wrapper util/val examples/parse

