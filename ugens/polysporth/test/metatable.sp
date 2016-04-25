"in" 4 metatable_create
"sine" 4096 "0 1 4096 -1" gen_line
"sine" "in" 0 metatable_add

0
(60 120 2 * / dmetro )
9 "in" "out" "metatable.scm" polysporth

0 "out" tget 
1 "out" tget +
2 "out" tget +
