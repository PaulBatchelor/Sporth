UGENS = \
adsr \
autowah \
basic \
biscale \
bitcrush \
bitwise \
blsaw \
blsquare \
bltriangle \
bpm \
brown \
cdb \
changed \
clock \
count \
crossfade \
delay \
diode \
dmetro \
dtrig \
eval \
f \
ftsum \
gen_eval \
gen_padsynth \
gen_sine \
gen_sporth \
gen_vals \
in \
incr \
jcrev \
line \
load \
loadspa \
lpc \
lsys \
mark \
maygate \
maytrig \
metro \
noise \
nsmp \
p \
paulstretch \
peaklim \
phaser \
pinknoise \
polysporth \
print \
prop \
pshift \
rand \
randh \
ref \
render \
reverse \
rpt \
samphold \
saturator \
say \
scale \
scrambler \
sdelay \
slice \
slist \
smoothdelay \
spa \
sparec \
srand \
switch \
t \
tadsr \
talkbox \
tblrec \
tdiv \
tenv \
tenv2 \
tenvx \
tgate \
thresh \
tin \
tick \
timer \
tog \
trand \
tseg \
tseq \
v \
voc \
vocoder \
writecode \
wpkorg35 \
zeros \
zitarev

# Disable libdl
# NO_LIBDL=1

# Build jack driver
# BUILD_JACK=1

# polysporth support
BUILD_POLYSPORTH=1

# Build live coding UDP listener
LIVE_CODING=1

# GDB Debugging Symbols
CFLAGS += -g

CFLAGS +=-DNO_LIBSNDFILE
