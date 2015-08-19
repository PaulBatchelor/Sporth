110 "3(+++)2(+++)" prop

0.01 
0.1 0.5 sine 0.5 + 0.1 * 0.007 + 
0.1 0.5 sine 0.5 + 0.1 * 0.007 + 

tenv 


71 mtof 0.2 sine 
73 mtof 0.2 sine + 
76 mtof 0.2 sine + 
swap dup rot *
 
swap 0.0 0.1 delay
69 mtof 0.3 sine * 
+ 


