'seq1' '67 72 65 63' gen_vals
'seq2' '74 75 70 63' gen_vals

0.0625
dmetro dup tog 0.04 port swap
dup 
2 0 count 0 eq * 

dup dup 0.5 maygate rot rot

0 'seq1' tseq 

swap 0 'seq2' tseq branch

mtof 
0.001 0.9 0.1 randi
0.5 blsquare * 
0.1 1 sine 300 4000 biscale
0.1 moogladder 
dup
# max notch1 freq
4000
# min notch1 freq
100
# notch width
4000
# notch freq
3.5
# vibrato mode
0
# depth 
1
# feedback gain
0.1
# invert
0
# level
0
# lfobpm
24

phaser

drop

dup jcrev +
