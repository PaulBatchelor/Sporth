"seq" "60 62 64 67 71 72 74 76" gen_vals

8 metro dup 1 "seq" tseq dup
rot 0.01 0.1 0.1 tenv
swap mtof 0.3 1 1 1 fm mul dup
# Send to reverb if note is 72 (c)
rot 72 eq 0 1 branch mul
dup 0.97 10000 revsc drop 0.3 mul add
