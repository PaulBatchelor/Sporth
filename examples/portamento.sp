# Portamento without initial jump on first note

"seq" "60 63 65" gen_vals

5 metro
dup
0 "seq" tseq

swap 1 1 count

0 eq swap dup 0.01 port branch

mtof 0.5 sine

