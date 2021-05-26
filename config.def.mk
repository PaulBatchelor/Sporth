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
butlp \
buthp \
butbp \
butbr \
cdb \
changed \
clock \
count \
crossfade \
dcblock \
delay \
diode \
dmetro \
dtrig \
eqfil \
eval \
expon \
f \
fm \
fosc \
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
loadwav \
loadspa \
lpc \
lsys \
mark \
maygate \
maytrig \
metro \
mode \
noise \
nsmp \
osc \
p \
paulstretch \
peaklim \
phaser \
phasor \
phasewarp \
pinknoise \
polysporth \
port \
print \
prop \
pshift \
rand \
randi \
randh \
ref \
render \
reverse \
revsc \
rpt \
samphold \
saturator \
say \
scale \
scrambler \
scrubby \
sdelay \
slice \
slist \
smoothdelay \
spa \
sparec \
srand \
switch \
t \
tabread \
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
tphasor \
trand \
tseg \
tseq \
v \
vdelay \
voc \
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
