; This example attempts show how polysporth can be used to somewhat emulate 
; notelists like those found in Csound.

; This is our instrument. 
(define sine 
" 
# the args table is how we get values
# arg table 0 is the midi number 
0 _args tget 
mtof 0.1 sine
")


; In polysporth, voices must be manually allocated before they are used.
; Contiguous sporthlets can be bundled together to form voice groups
(ps-eval 0 sine)
(ps-eval 1 sine)
(ps-eval 2 sine)
(ps-eval 3 sine)

; We create notes inside a noteblock
; Notes can be scheduled in ticks, which are supplied by the clock in sporth.
; Our dmetro is set to 0.5, so one tick is 0.5 seconds.
(ps-noteblock-begin)
; a note takes the following arguments
(ps-note 
  0 3 ; voice group, sporthlets 0-3
  0   ; start time, in ticks
  8   ; duration, in ticks
  60 ) ; optional arguments, which get put into a notes arg table
; Notes need to be put in sequential order (there is no sort routine... yet)
(ps-note 0 3 1 8 64)
(ps-note 0 3 2 8 71)
(ps-note 0 3 3 8 67)
(ps-noteblock-end)
