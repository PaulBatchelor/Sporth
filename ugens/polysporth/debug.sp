"in" 10 zeros
#60 0 'in' tset
60 120 2 * / dmetro 
dup 0 "in" tset
9 "in" "out" "test.scm" polysporth

# Keys
0 "out" tget
1 "out" tget + 
2 "out" tget + 
3 "out" tget +
4 "out" tget + 
5 "out" tget + 

# Drums 
6 "out" tget +
7 "out" tget +

# Lead
8 "out" tget dup dup 3 3 10000 zrev drop 0.2 * + +

0.9 *
