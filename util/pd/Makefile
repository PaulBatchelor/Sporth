current: 
	echo make pd_linux

clean: ; rm -f *.pd_linux *.o

# If you're building kona, this is where the kona directory is
K_PATH=~/playground/kona/kona

# Location of the soundpipe source code
SP_PATH=~/proj/soundpipe

SPORTH_O=`find ../ -name "*.o" | grep -v sporth~ | grep -v chorth`
SP_O=`find $(SP_PATH) -name "*.o" `

# make linux BUILD_KONA=1

ifdef BUILD_KONA
LDFLAGS += -lpthread -ldl -lm
FLAGS += -DBUILD_KONA 
K_O=`find $(K_PATH) -name "*.o" | egrep -v "\.t\.o"`
endif

# ----------------------- LINUX i386 -----------------------

pd_linux: sporth~.pd_linux 

sporthlive: sporthlive~.pd_linux 


.SUFFIXES: .pd_linux

LINUXCFLAGS = -DPD -O2 -funroll-loops -fomit-frame-pointer \
    -Wall -W -Wshadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch -fPIC

LINUXINCLUDE =  -I../../src -I $(SP_PATH)/h

.c.pd_linux:
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	#ld -shared -o $*.pd_linux $*.o $(SP_O) $(SPORTH_O) $(K_) -lc -lm -lsndfile
	ld -o $*.pd_linux $*.o -lc -lsporth -lsoundpipe -lsndfile -lm -shared
	strip --strip-unneeded $*.pd_linux
	rm $*.o

sporthlive~.pd_linux:
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	#ld -shared -o $*.pd_linux $*.o $(SP_O) $(SPORTH_O) $(K_) -lc -lm -lsndfile
	ld -o $*.pd_linux $*.o -lc -lsporth -lsoundpipe -lsndfile -lm -llo -shared
	strip --strip-unneeded $*.pd_linux
	rm $*.o

# ----------------------- Mac OSX -----------------------

pd_darwin: sporth~.pd_darwin

.SUFFIXES: .pd_darwin

DARWINCFLAGS = -DPD -O2 -Wall -W -Wshadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch

.c.pd_darwin:
	cc $(DARWINCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	cc -bundle -undefined suppress -flat_namespace -o $*.pd_darwin $*.o 
	rm -f $*.o

