_vals "6770 96139 159104 228847"  gen_vals
_smp "oneart.wav" loadfile

0.4 dmetro dup 0 _vals tblsize trand floor _vals _smp slice
