
(define make-note (lambda () 
    ;(display "hello world")
    (ps-noteblock-begin)
    (ps-note 0 3 3 4 #(69.0 0.3))
    ;(ps-note 0 3 1 4 #(68.0 0.3))
    ;(ps-note 0 3 0 4 #(64.0 0.3))
    ;(ps-note 0 3 0 4 #(71.0 0.3))
    (ps-noteblock-end)
    ;(newline)
))


(define instr "0 'args' tget mtof 1 'args' tget 1 1 1 fm tick 0.01 0.1 1 tenv *")

(ps-eval 0 instr)
(ps-eval 1 instr)
(ps-eval 2 instr)
(ps-eval 3 instr)

(ps-noteblock-begin)
(ps-metanote 0 make-note)
(ps-metanote 0 make-note)
;(ps-note 0 3 4 1 #(64 0.5))
(ps-noteblock-end)

(define tick (lambda () (display "tick") (newline) ))
(define none (lambda () ()))
(define run none)

(run)
