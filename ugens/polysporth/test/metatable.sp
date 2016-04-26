"in" 4 metatable_create
"saw" 4096 "0 1 4096 -1" gen_line
"sine" 4096 gen_sine
"saw" "in" 0 metatable_add
"sine" "in" 1 metatable_add

0
(60 120 2 * / dmetro )
9 "in" "out" "metatable.scm" polysporth

0 "out" tget 
1 "out" tget +
2 "out" tget +
