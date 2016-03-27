(define pad-rng #(0 5))

(define infkick (lambda ()
    (ps-note 6 6 0 1 '())
    ;(ps-metanote 1 infkick)
))

(define chord (lambda (notes)
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 3 
        (list (list-ref notes 0) 0.1))
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 3 
        (list (list-ref notes 1) 0.1))
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 3 
        (list (list-ref notes 2) 0.1))
))

(define chord-one (lambda () 
    (ps-noteblock-begin)
    (chord '(62.0 66.0 69.0))
    (ps-noteblock-end)
))

(define chord-two (lambda () 
    (ps-noteblock-begin)
    (chord '(59.0 62.0 67.0))
    (ps-noteblock-end)
))

(define chord-three (lambda () 
    (ps-noteblock-begin)
    (chord '(57.0 61.0 64.0))
    (ps-noteblock-end)
))

(define chord-four (lambda () 
    (ps-noteblock-begin)
    ; for some reason the 62 is transposed an octave...
    (chord '(59.0 62.0 66.0))
    (ps-noteblock-end)
))


;(define instr "0 'args' tget mtof 1 'args' tget 1 1 1 fm tick 0.01 0.1 1 tenv *")
(define instr "0 'args' tget mtof 1 'args' tget 1 1 1 fm")

(define eval-block (lambda (str start end) 
(if (> start end) 
    ()
    (begin 
        (display start)
        (newline)
        (ps-eval start str)
        (eval-block str (+ 1 start) end)
        ))
))

(define kick "0 'in' tget 0 pset 
1000
0.1 sine
")

(ps-eval 6 kick)
(eval-block instr 0 5)
(ps-noteblock-begin)
(ps-turnon 6 -1)
(ps-metanote 0 chord-two)
(ps-metanote 2 chord-three)
;(ps-note 0 5 0 4 '(60.0 0.1))
;(ps-note 0 5 1 4 '(62.0 0.1))
;(ps-note 0 5 2 4 '(67.0 0.1))
;(ps-note 0 5 3 4 '(69.0 0.1))
;(ps-note 0 5 4 4 '(72.0 0.1))
;(ps-metanote 0 chord-one)
;(ps-metanote 4 chord-two)
;(ps-metanote 6 chord-three)
;(ps-metanote 8 chord-four)
;(ps-metanote 12 chord-two)
(ps-noteblock-end)

(define tick (lambda () (display "tick") (newline) ))
(define none (lambda () ()))
(define run none)
