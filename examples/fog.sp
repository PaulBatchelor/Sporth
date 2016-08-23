'env' 4096 '0.5 0.5 270 0.5' gen_composite

'buf' 44100 3 * zeros

'oneart.wav' diskin

0.8 0 p * +

dup tick 'buf' tblrec 

1 
80
0.25 2 0.1 randi 
0 1 2 randi
1 80 
0.005 0.001 0.02 
0 100 'env' 'buf' fog
dup 0 pset

+ 
