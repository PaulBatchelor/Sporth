Sporth s => dac;
// 101010
s.parse("
60 mtof 0 pset  
'stars' '77 80 72 82' gen_vals
'amps' '0 0 1 3 0 1' gen_vals

'padsynth' 262144 0 p 40 'amps' gen_padsynth

36 mtof 0 p / 'padsynth' tbldur / 0 phasor
0 0 0 'padsynth' tabread 5 *  
");

30::second => now;
