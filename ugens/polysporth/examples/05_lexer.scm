; With polysporth, sporth patches can be procedurally generated. 
; This example shows two ways of doing it. The first way involves, calling
; ps-lexer, which evaluates one token at at time. The other way is to use
; ps-parse, which can parse an entire strings (but not initialize it)

; Needed for the map function
(load "../init.scm")

; A function to build a sine wave as a list of individual tokens
(define (sine freq amp)
  (list 
    (number->string freq)
    (number->string amp)
    "sine"))

; a small function that parses a list of strings
(define (parse s lst)
  (map (lambda (x) (ps-lexer s x)) lst))

; create our first sine the first way
(parse 0 (sine 440 0.5))

; create our second sine the second way..
(ps-parse 0 "350 0.5 sine + ")

; When you are done building your patch, you initialize it.
(ps-init-sporthlet 0)

; turn on the sporthlet 
; negative durations will leave on the sporthlet until you turn it off.
(ps-turnon 0 -1)
