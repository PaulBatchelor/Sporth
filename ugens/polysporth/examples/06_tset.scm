(ps-eval 0 "_val 3 zeros 0 _val tget 0.5 sine")
(define args (ps-ftbl "val"))
(define intbl (ps-ftbl "in"))
(ps-tset args 0 800)

(display "the in value is ")
(write (ps-tget intbl 0))
(newline)
(ps-turnon 0 -1)
