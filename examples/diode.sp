(
_seq "0 3 -7 -2" gen_vals
_seq2 "0 2 3 7 9 10 12" gen_vals
_clk var
_dry var
_ping var
_bpm 135 varset
0 _bpm get 4 clock _clk set

_clk get 0 _seq tseq 40 + _clk get 0.3 maygate 12 * + 
_clk get 0.6 maygate 0.002 0.01 branch port mtof 0.7 saw 
30 inv 1 sine 300 3000 biscale 0.75 0.96 (0.1 1 1 randh) randi diode
2 0.3 0.2 0.01 dist
_clk get 0.005 10 inv 1 sine 0.01 0.1 biscale 0.1 tenvx * 

# Square leads
_clk get 4 0 tdiv 0.2 maytrig _clk get dup 3 0 tdiv switch
1 _seq2 tseq 62 + dup 
mtof 0.015 (0.005 0.5 2 jitter) + port 0.2 0.1 square 
swap 12.1 - mtof 0.02 port 0.2 0.2 square + 35 inv 1 sine 500 1000 biscale butlp  
_clk get 16 0 tdiv 0.8 maygate 0.1 port * 200 buthp -8 ampdb *
+

# ping

_clk get 0.02 maytrig 0.001 0.005 0.01 tenvx 74 mtof 0.1 1 
_clk get 0 7 trand floor 1 fm * 
dup _ping set + 

_dry set

_dry get 500 buthp dup 0.91 8000 revsc drop -10 ampdb * 
_dry get 1000 buthp _ping get +  0.7 _bpm get bpm2dur 0.75 *  delay 1000 butlp +

_dry get + 
dup
)
