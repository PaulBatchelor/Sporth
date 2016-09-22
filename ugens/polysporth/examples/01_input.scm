; Similar to the previous example, we create and turn on a sporthlet for 
; three seconds. This reads from the input table and sends it to jcrev.
; When a Sporthlet is turned off, it no longer get computed. 

(ps-eval 0 "0 _in tget jcrev ")
(ps-turnon 0 3)
