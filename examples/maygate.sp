"scale" "60 62 64 67 71 74" gen_vals

96 60 div  metro
0.5 maytrig
dup dup
1 "scale" tseq
swap 0.01 0.1 0.4 tenv
swap
mtof 0.2 1 1 1 fm mul

dup rot
0.8 maygate mul
dup 0.93 10000 revsc drop 0.3 mul

add

