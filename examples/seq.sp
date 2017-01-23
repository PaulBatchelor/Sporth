# A trivial sequencer
# Create a table of values,
# These do not get pushed to the stack
"seq" "60 62 64 67" gen_vals
# create a metronome at 3Hz, and
# duplicate its signal
3 metro
# stack: metro
# duplicate the last (only) item on stack
dup
# stack: metro metro
# A simple trigger based envelope generator
# This pops off the metro value from
# the stack.
0.01 0.1 0.1 tenv
# stack: metro tenv
# swap last two values pushed on the stack
swap
# stack: tenv metro
# reads a value from the table "seq"
# This pops off the metro value
# from the stack.
0 "seq" tseq
# stack: tenv tseq
# pops last value from the stack, and applies
# a MIDI to frequency conversion
mtof
# stack: tenv mtof
# pops last value from the stack, and uses
# it as the frequency for the sine wave
0.5 sine
# stack: tenv sine
# pops two values from the stack, and
# multiplies them together
mul
# stack: (nothing, as it should be)
# If this were non-zero, we would run
# into stack-overflow issues.
