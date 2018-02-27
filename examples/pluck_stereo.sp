# an extension/hacking of 'pluck.sp' to add stereo, and show the use
# of just intonation as an option. Make sure to render with '-c 2' as a 
# commandline parameter. Hack by Aaron Krister Johnson.

"seq" "1 1.1667 1 0.875 1.125 1 1.5 1.5 1 1.1667 1 0.875 1.125 1 1.5 1.666 1.75 2" gen_vals

_t 2 zeros

# invent fset and fget functions to make this shorter
"seq2" "1 1 0.875 1.5 1.5 1.3333 1.666 1.75 2 1" gen_vals

# 125 0 pset
125 0 _t tset

# trigger
60 0 _t tget / 0.5 * dmetro 0.333 maytrig dup
# freq
0 "seq" tseq 256 *  
# amp
0.4 
# lowest expected frequency
110
pluck 

# trigger
0 _t tget "+-+--+--" prop dup
# freq
0 "seq2" tseq 256 * dup 1 _t tset
# amp
0.4 
# lowest expected frequency
86.0
pluck 

# AM to pitch shift two octaves down
1 _t tget 0.25 * 1 sine *

+ 

# save above result for later stereo,
# instead, operate on copy for first channel:
dup

# in
dup
# feedback
0.86
# deltime
60 0 _t tget / 0.75 * 
delay

# cutoff
5000 
butlp 

# mix amount, consume to make dry+wet
0.4 * +

# in
swap dup
# feedback
0.86
# deltime
60 0 _t tget / 1.25 * 
delay

# cutoff
5000
butlp

# mix amount, consume to make dry+wet
0.4 * +
