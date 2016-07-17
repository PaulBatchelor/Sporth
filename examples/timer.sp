# a simple clock divider example

# subdivisions
_s '2 3 4 6 8 16' gen_vals

# master clock
135 bpm2dur dmetro

# randomized frequency
dup 300 1000 trand

# FM synth
0.4 1 1 1 fm

# bring back the clock
swap

# send a copy of the signal into the timer
dup timer
# swap, and send into tseq, then to dmetro
swap 1 _s tseq /
dmetro

# send new dmetro clock into exponential envelope
0.001 0.01 0.1  tenvx *
