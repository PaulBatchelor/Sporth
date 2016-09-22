; In addition to a clock, polysporth also has a separate callback function.
; When triggred, it will call a user-defined function written in scheme. 

(define (run) 
  (begin 
    (write "bing!")
    (newline)))

(ps-set-callback run)


