'imp' 'imp.wav' loadfile
'seq' '60 62 64 66 67 69 72' gen_vals

1.5 metro 
0.3 maytrig 
dup
1 'seq' tseq mtof 
0.4 
48 mtof 
pluck 

dup 1024 'imp' conv 0.03 * +

