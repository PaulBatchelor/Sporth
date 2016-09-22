; This example expands upon the concept of notelists with metanotes. 
; Metanotes allow you to schedule functions as events in time. These
; metanotes can actual notes, or even metanotes.


; Create instrument
(define sine "0 _args tget mtof 0.1 sine")

; Allocate voices
(ps-eval 0 sine)
(ps-eval 1 sine)
(ps-eval 2 sine)
(ps-eval 3 sine)

; function to make chords from a vector
(define (mk_chord v)
  (begin 
    (ps-note 0 3 0 2 (vector-ref v 0))
    (ps-note 0 3 0 2 (vector-ref v 1))
    (ps-note 0 3 0 2 (vector-ref v 2))
    (ps-note 0 3 0 2 (vector-ref v 3))))

; define functions to be scheduled
(define (chord1) (mk_chord #(60 64 67 71)))
(define (chord2) (mk_chord #(60 64 65 69)))
(define (chord3) (mk_chord #(60 62 65 68)))
(define (chord4) (mk_chord #(60 64 67 59)))

; chord progression!
(ps-noteblock-begin)
(ps-metanote 0 chord1)
(ps-metanote 2 chord2)
(ps-metanote 4 chord3)
(ps-metanote 6 chord4)
(ps-noteblock-end)
