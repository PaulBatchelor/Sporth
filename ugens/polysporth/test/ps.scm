(define nt (lambda (i start dur args)
    (ps-note (vector-ref i 0) (vector-ref i 1) start dur args)))

(define eval-block
  (lambda (start num str)
    (if (= num 1)
      (ps-eval start str)
      (begin
        (ps-eval start str)
        (eval-block (+ start 1) (- num 1) str)))))

(define mk-chord
  (lambda (ins start dur space seq)
    (if (null? seq)
      ()
      (begin
        (nt ins start dur (list (car seq)))
        (write (car seq))
        (newline)
        (mk-chord ins (+ start space) dur space (cdr seq))))))
