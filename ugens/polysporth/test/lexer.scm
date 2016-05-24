(define (sine id amp freq) 
  (begin 
    (ps-lexer id (number->string freq))
    (ps-lexer id (number->string amp))
    (ps-lexer id "sine")))

(ps-lexer 0 "440")
(ps-lexer 0 "0.5")
(ps-lexer 0 "sine")
; This needs to be called after you push stuff
(ps-init-sporthlet 0)

(sine 1 0.5 350)
; then call ps-init
(ps-init-sporthlet 1)

(ps-noteblock-begin)
(ps-note 0 0 0 4 '(0))
(ps-note 1 1 0 4 '(0))
(ps-noteblock-end)
