# L-Systems Test
# Paul Batchleor
# August 2016

_tick var
0.1 dmetro _tick set

_tick get dup
8 "a|a:ab|b:a" lsys 10 eq *
0.01 0.01 0.01 tenvx 1000 0.4 sine *

_tick get dup
4 "a|a:ab|b:a" lsys 10 eq *
0.01 0.01 0.01 tenvx 500 0.4 sine *

_tick get dup
3 "a|a:ab|b:a" lsys 10 eq *
0.01 0.01 0.01 tenvx 1500 0.4 sine *

mix

