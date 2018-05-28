# a demonstration of the 'edo' (equal division of the octave) ugen
# this example uses 22edo

# top line
'seq' '7 11 9 11 7 16 7 20 \
       25 23 25 20 17 20 14 11' gen_vals

# bass line
'bass_pitch' '-37 -37 -15 -15 -19 -15 -24 \
              -37 -37 -15 -15 -28 -24' gen_vals

'bass_amp'   '1 0 .7 0 0 0  1 0 .7 0 0 0  0 0 1 1 0 1\
              1 0 .7 0 0 0  1 0 .7 0 0 0  0 1 0 1' gen_vals

######################
# top line synthesis #
###################### 
# triangle wave
3 metro 0 'seq' tseq
22 edo 0.1 tri

# dup and add delay
dup 0.6 0.175 delay 0.5 *

# mix with itself
+

#######################
# bass line synthesis #
#######################
# bass pitch
360 '++--++----++-3[+]\
     ++--++---3[+]-3[+]' prop
0 'bass_pitch' tseq
22 edo 0.01 port
# bass amp
360 '2(++++)++ 2(++++)++ ++++ +3[+]\
     2(++++)++ 2(++++)++ +3[+] +3[+]' prop
0 'bass_amp' tseq 0.2 * saw 
1000 0.1 moogladder
+
