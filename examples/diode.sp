(
_seq "0 3 -7 -2" gen_vals
_clk var
_dry var
_bpm 135 varset
0 _bpm get 4 clock _clk set

_clk get 0 _seq tseq 40 + _clk get 0.3 maygate 12 * + 
_clk get 0.6 maygate 0.002 0.01 branch port mtof 0.8 saw 
30 inv 1 sine 300 3000 biscale 0.75 0.96 (0.1 1 1 randh) randi diode
2 0.3 0.1 0.0 dist
_clk get 0.001 0.1 0.1 tenvx * 

_dry set

_dry get 500 buthp dup 0.91 8000 revsc drop -10 ampdb * 
_dry get 1000 buthp 0.7 _bpm get bpm2dur 0.75 *  delay 1000 butlp +

_dry get + 
)
