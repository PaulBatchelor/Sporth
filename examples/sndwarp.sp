_oneart "oneart.wav" loadfile
_win 4096 '0.5 1 0 0.5' gen_composite

0.5 # amp
6 # timewarp
2 # resample
0 # begin
0.1 # window size
0.02 # randomness
8 # overlap
0 # timemode
8 # max overlap
_win # window
_oneart # sample

sndwarp

