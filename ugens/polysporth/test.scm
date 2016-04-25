(define sine "440 0.4 sine")

(ps-eval 0 sine)

(ps-noteblock-begin)
(ps-note 0 0 0 4 '(69.0))
(ps-noteblock-end)

(define tick (lambda () (display "tick") (newline) ))
(define none (lambda () ()))
(define run none)
