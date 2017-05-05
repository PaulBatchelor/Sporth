# Simple Crossfade example, using port filters and switches

# input 1
440 0.5 sine

# input 2
550 0.5 sine

# s s

1 metro 0 1 switch 0.005 port dup

# s s t t

rot mul

# s t t s
# s t m

rot rot

# t m s
# m s t

1 swap sub mul

# m s t 1
# m s 1 t
# m s (1 - t)
# m s * (1 - t)

add

# m1 +  m2
