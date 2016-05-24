_amen 'amen_loop.wav' loadfile
_split '3 4 8 16 32 64' gen_vals
_rep '3 4 6 7 8' gen_vals

_amen tbldur 16 / dmetro 0 pset
0 p dup 0.9 0.5 branch maygate 1 -1 branch 0.003 port _amen tbldur / 0 phasor
1 0 0 _amen tabread

0 p 0.1 1 sine 0.01 0.9 biscale maygate
137.1420
0 p 0 _split tseq
0 p 0 _rep tseq
1.0
rpt

'dist' './dist.so' fl
# Parameters linked into pd patch
1 p 2 p 'dist' fe
'dist' fc

dup
200 buthp
0 p 0.1 maygate 0.1 0.2 0.1 tenv *
dup 0.7 0.3281 delay 0.5 * swap
dup 20 10 10000 zrev drop 0.8 * + +
