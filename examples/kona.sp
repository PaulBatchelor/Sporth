# Generate chromatic sequence
"k" "48 + (2 * !12)" kona

"maj" "s:0 2 4 5 7 9 11;do:{(s@(!x)!#s)+(12*((!x)%#s))+48};do 14" kona
# cycle through generated table
0.1 dmetro dup 0 pset

# frequency from tseq
0 "maj" tseq mtof 
# amplitude
0.3 
# carrier
1 
# randomized modulator
0 p 1 4 trand floor
# randomized mod index
0 p 0 4 trand 
fm
