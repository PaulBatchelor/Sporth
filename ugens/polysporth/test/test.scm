(define sine "0 'args' tget mtof 0.1 sine")

(ps-eval 0 sine)
(ps-eval 1 sine)
(ps-eval 2 sine)

(ps-noteblock-begin)
(ps-note 0 2 0 4 '(69.0))
(ps-note 0 2 2 4 '(71.0))
(ps-note 0 2 4 4 '(73.0))
(ps-noteblock-end)

(define none (lambda () ()))
(define run none)
