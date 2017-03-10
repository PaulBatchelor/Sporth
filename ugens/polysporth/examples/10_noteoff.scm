; 10-noteoff.scm
; This example introduces how to use the new "noteoff" mode for sporthlets.
; Noteoff mode changes the behavior for how sporthlets get turned off via
; the polysporth scheduler. When the timer goes to 0, instead of turning
; off the sporthlet, it sends a noteoff tick signal called an "offtick". 
; It is up to the sporthlet to turn itself off. 
;
; The behavior in noteoff mode is especially ideal for instruments with
; a release envelopes, where the end time of the note is known, but the
; off time is unknown. This example implements such an instrument. 

; A nice little monitor to detect noteoffs
(define noteoff-monitor " dup pos * 'noteoff at' print drop ")

; Both the 'tick' and '_offtick' are start/ends of the note
; These drive a gate signal via toggle, which then feeds adsr
(define note-gate " tick _offtick fe + tog ")

(define (noteoff-thresh amt)
 (string-append " dup " amt " 1 thresh "))

; the main instrument: a simple FM oscillator whose release envelope controls
; the noteoff
(define (simple freq) 
 (string-append 
  freq " 0.5 1 1 1 fm " 
  ; the envelope, fed by the note-gate
  note-gate " 0.001 0.3 0.8 0.1 adsr "
  ; the threshold signal produces a trigger when the ADSR release portion
  ; is close to zero
  (noteoff-thresh "0.003") 
  ; A handy sporth-made monitor to print what time notes turn off
  ; Does not indicate *which* notes though (a future feature?)
  noteoff-monitor 
  " _noteoff fe * "))

; create two instances of "simple", whose frequency is set by 
; the first user argument 
(ps-eval 0 (simple "0 _args tget mtof"))
(ps-eval 1 (simple "0 _args tget mtof"))

; set sporthlets 0 and 1 to be in "noteoff" mode
; in noteoff mode, these sporthlets turn themselves off via sending
; a trigger signal to the _noteoff ugen
(ps-noteoff-mode 0)
(ps-noteoff-mode 1)

(ps-noteblock-begin)
; this first event reaches for sporthlet 0
; note that (ps-pitch) is being used to convert a string to a MIDI note number
; in this case "C-4" would produce 60
(ps-note 0 1 0 2 (ps-pitch "C-4"))
; this second event reaches for sporthlet 1
(ps-note 0 1 4 2 (ps-pitch "D-4"))
; this third event can reach for sporthlet 0 because it is turned off by then
(ps-note 0 1 8 2 (ps-pitch "B-4"))
(ps-noteblock-end)
