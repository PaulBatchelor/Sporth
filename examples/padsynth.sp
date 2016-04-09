# set base frequency
60 mtof 0 pset  

'amps' '0.5 0.5 0.1 0.1 0.2 0.2' gen_vals

'padsynth' 262144 0 p 40 'amps' gen_padsynth

36 mtof 0 p / 'padsynth' tbldur / 0 phasor
1 0 0 'padsynth' tabread  0 0.1 3 randi * 

60 mtof 0 p / 'padsynth' tbldur / 0 phasor
1 0 0 'padsynth' tabread  0 0.1 2 randi * 

65 mtof 0 p / 'padsynth' tbldur / 0 phasor
1 0 0 'padsynth' tabread  0 0.15 1 randi *

70 mtof 0 p / 'padsynth' tbldur / 0 phasor
1 0 0 'padsynth' tabread 0 0.2 0.5 randi * 

+ + +

#dup jcrev 4 * swap 0.2 * +

dup dup 0.9 3000 revsc 0.1 * drop +
