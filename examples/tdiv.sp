60 140 4 * / dmetro 

dup 3 0 tdiv 0.01 0.01 0.2 tenv 
62 mtof 0.5 1 1 1 fm *

swap dup 4 1 tdiv 0.01 0.01 0.2 tenv 
64 mtof 0.5 1 1 1 fm *

swap dup 5 2 tdiv 0.01 0.01 0.2 tenv 
71 mtof 0.5 1 1 1 fm *

swap 7 3 tdiv 0.001 0.01 0.6 tenv 
66 mtof 0.5 1 1 1 fm *

mix -6 ampdb *
