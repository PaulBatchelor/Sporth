; ps-parse allows you to evaluate chunks of Sporth code before evaluating.
; this is ideal for abstractions and modular code. 

; Internally, each word parsed in represents a "pipe". Every time ps-parse
; is called, pipes are appeneded to the end of the other pipes, known as
; plumbing. When pipes are done being added, ps-init-sporthlet must be called,
; In this function, plumber_compute is called in PLUMBER_INIT mode. After this
; the sporthlet can be treated like any other sporthlet.


; Define a function that wraps a sporth sine wave
(define (sine freq amp)
  (string-append (number->string freq) " " (number->string amp) " sine"))


; parse a sine wave
(ps-parse 0 (sine 440 0.3))

; Parse another sine wave
(ps-parse 0 (sine 350 0.3))

; Add them together
(ps-parse 0 "+")

; Let's some random blips. This time, we only evaluate a string.

(ps-parse 0 "200 2000 10 randh 0.1 sine +")

; Done adding things, so initialize the plumbing 
(ps-init-sporthlet 0)

; Print out the pipe data to STDOUT

(ps-show-pipes 0)

(ps-turnon 0 -1)
