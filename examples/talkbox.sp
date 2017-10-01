_dry var
"oneart.wav" diskin 0.5 * 
48 mtof 0.5 saw 
53 mtof 0.5 saw +
58 mtof 0.5 saw +
60 mtof 0.5 saw +
-5 ampdb * 
1.0 talkbox
_dry set

_dry get dup 0.97 10000 revsc + -20 ampdb * dcblk 
_dry get 0.5 0.7 delay -6 ampdb * 1000 butlp +

_dry get +

