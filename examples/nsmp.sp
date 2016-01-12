"seq" "0 0 0 1 2" gen_vals
"wav" "oneart.wav" loadfile 

#140 "+2(++)2(-+)4(-+--)" prop 
0.3 dmetro
dup dup 0 "seq" tseq
44100 "oneart.ini" "wav" nsmp
swap 0.01 0.1 0.1 tenv *
