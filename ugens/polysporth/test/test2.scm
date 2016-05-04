(load "ps.scm")

(define meta "
'saw' 4096 '0 1 4096 -1' gen_line
'sine' 4096 gen_sine
")
(define synth 
"tick 0 pset 0 'args' tget 
5 1 0 'sine' osc 
0.5 * + mtof 
0.1 0 'saw' osc 
1000 butlp 
0 p 0.1 1 0.1 tenv * ")

(define ins #(1 3))

(ps-eval 0 meta)

(ps-eval 1 synth)
(ps-eval 2 synth)
(ps-eval 3 synth)

(ps-noteblock-begin)
(nt ins 0 4 '(69.0))
(nt ins 2 4 '(71.0))
(nt ins 4 4 '(73.0))
(ps-noteblock-end)
