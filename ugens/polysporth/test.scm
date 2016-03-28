(define pad-rng #(0 5))


(define infkick (lambda ()
    (ps-note 6 6 0 1 '())
    ;(ps-metanote 1 infkick)
))

(define chord (lambda (notes)
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 6 
        (list (list-ref notes 0) 0.1))
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 6 
        (list (list-ref notes 1) 0.2))
    (ps-note 
        (vector-ref pad-rng 0)
        (vector-ref pad-rng 1) 
        0 6 
        (list (list-ref notes 2) 0.2))
))

(define melody (lambda () 
    (ps-noteblock-begin)
    (ps-note 8 8 0 4 '(69.0))
    (ps-note 8 8 6 1 '(67.0))
    (ps-note 8 8 7 1 '(66.0))
    (ps-note 8 8 8 2 '(62.0))
    (ps-note 8 8 10 2 '(66.0))
    (ps-note 8 8 12 4 '(64.0))
    (ps-note 8 8 16 2 '(66.0))
    (ps-note 8 8 18 2 '(71.0))
    (ps-note 8 8 23 1 '(66.0))
    (ps-note 8 8 24 2 '(67.0))
    (ps-note 8 8 26 2 '(74.0))
    (ps-note 8 8 30 2 '(69.0))
    (ps-metanote 32 melody)
    (ps-noteblock-end)
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
    (chord '(59.0 62.0 66.0))
    (ps-noteblock-end)
))

(define chords (lambda () 
    (ps-noteblock-begin)
    (ps-metanote 0 chord-one)
    (ps-metanote 8 chord-two)
    (ps-metanote 12 chord-three)
    (ps-metanote 16 chord-four)
    (ps-metanote 24 chord-two)
    (ps-metanote 32 chords)
    (ps-noteblock-end)
))

(define instr "0 'args' tget mtof 1 'args' tget 1 1 1 fm tick 0.01 0.1 1 tenv * ")

(define lead " 0 'args' tget mtof 0.003 port 0.3 tri ")

(define eval-block (lambda (str start end) 
(if (> start end) 
    ()
    (begin 
        (ps-eval start str)
        (eval-block str (+ 1 start) end)
        ))
))

(define kick "
120 '+' prop dup
1 1.1 0.0001 expon 0 100 scale
0.4 sine 
swap 1 1.1 0.0001 expon * 
")

(define snare "120 '-+-+' prop 0.01 0.01 0.01 tenv 0.1 noise * ")


(ps-eval 6 kick)
(ps-turnon 6 -1)
(ps-eval 7 snare)
(ps-turnon 7 -1)
(ps-eval 8 lead)

(eval-block instr 0 5)
(ps-noteblock-begin)
(ps-metanote 0 melody)
(ps-metanote 0 chords)
(ps-noteblock-end)

(define tick (lambda () (display "tick") (newline) ))
(define none (lambda () ()))
(define run none)
