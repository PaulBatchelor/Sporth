; Polysporth will call this file. In here, we will make a sine wave on 
; sporthlet 0. We will then turn on sporthlet 0 which will last for 3 clicks.
; Since our clock is set to 1 second. It will last 3 seconds before being
; turned off.

(ps-eval 0 "440 0.5 sine")
(ps-turnon 0 3)
