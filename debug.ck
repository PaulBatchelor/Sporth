Sporth s => dac;
s.parse("
'seq' '60 62 64 66 67 69 72' gen_vals

90 '3[3(+++)2(++)2[3(+++)]2(++)]' prop dup
1 'seq' tseq mtof 
0.4 48 mtof pluck 

");
while(1)
30::second => now;
