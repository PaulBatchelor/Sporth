Sporth s => dac;
s.parse("
'sine' 4096 gen_sine
'seq' '60 62 64' gen_vals

10 metro 0 'seq' tseq mtof 0.1 0 'sine' osc 
");
while(1)
30::second => now;
