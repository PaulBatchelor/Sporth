(load "ps.scm")

(define meta "
'saw' 4096 '0 1 4096 -1' gen_line
'sine' 4096 gen_sine
")

(define synth
"tick 0 'args' tget
5 1 0 'sine' osc
0.5 * + mtof
0.1 0 'saw' osc
1000 butlp swap
0.1 2 0.1 tenv * ")

(define ins #(1 5))

(ps-eval 0 meta)

(eval-block 1 5 synth)
(ps-set-release 3)
(ps-noteblock-begin)
(mk-chord ins 0 8 2 '(60.0 63.0 70.0 74.0))
(ps-noteblock-end)
