_tab 4096 gen_sine

_seq '60 65 70 72' gen_vals

# set clock to p-reg 0
116 4 * bpm2dur dmetro 0 pset

# set exponential envelope to p-reg 1
0 p 0.002 0.01 
# lfo-based release time
((0.2 1 sine) 0.01 0.2 biscale)
tenvx 1 pset


# sequencer 
((((0 p 0 _seq tseq) (8 0.1 sine) +) 
# fifth leaps and throws via preg 3
0 p 0.2 maygate dup 3 pset 7 * + 
0.002 port) mtof) 0 phasor


# apply phasor through phase distortion

# input signal is reverse envelope
(1 1 p - )
# amount
(0.1 1 0.5 _tab osc -0.9 0.1 biscale)
0 scale pdhalf 

# put through interpolated table lookup
1 0 0 _tab tabread  0.5 *

# ampltidue envelope 
1 p *

# reverb throw, smooth out the gate signal
dup (3 p 0.001 port *) dup
0.97 4000 revsc drop 0.1 * + 
