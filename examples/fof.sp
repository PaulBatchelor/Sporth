'env' 4096 '0.5 0.5 270 0.5' gen_sinecomp
'sine' 4096 gen_sine

'forms' '800 1150 2800 3500 4950' gen_vals
'bw' '80 90 120 130 140' gen_vals

48 4 1 sine -0.3 0.3 biscale + mtof 0 pset
0.5 1 pset

1 p 0 p 0 'forms' tget 0 0 'bw' tget 0.001 0.001 0.02 0 100 'env' 'sine' fof
1 p 0 p 1 'forms' tget 0 1 'bw' tget 0.001 0.001 0.02 0 100 'env' 'sine' fof +
1 p 0 p 2 'forms' tget 0 2 'bw' tget 0.001 0.001 0.02 0 100 'env' 'sine' fof + 
1 p 0 p 3 'forms' tget 0 3 'bw' tget 0.001 0.001 0.02 0 100 'env' 'sine' fof + 
1 p 0 p 4 'forms' tget 0 4 'bw' tget 0.001 0.001 0.02 0 100 'env' 'sine' fof +

0.2 *

dup dup 0.97 10000 revsc drop 0.1 * +
