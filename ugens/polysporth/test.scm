;Chord 1
(ps-eval 0 "60 mtof 0.1 sine")
(ps-eval 1 "63 mtof 0.1 sine")
(ps-eval 2 "68 mtof 0.1 sine")

;Chord 2
(ps-eval 3 "58 mtof 0.1 sine")
(ps-eval 4 "63 mtof 0.1 sine")
(ps-eval 5 "67 mtof 0.1 sine")

(ps-note-append 0 0 0 4)
(ps-note-append 1 1 0 4)
(ps-note-append 2 2 0 4)

(ps-note-append 3 3 4 4)
(ps-note-append 4 4 0 4)
(ps-note-append 5 5 0 4)
(define run (lambda () () ))

(run)
