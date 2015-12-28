# Create argument ftable called 'args'
# p0 = dur (initialized at 4 seconds)
# p1 = note (initialized at middle C 60)
# p2 = amplitude (initialized at 0.1)
#
# every time tpoly is triggered, it will
# create a note event with these arguments
'args' '4.0 60 0.1' gen_vals

# Generate sequence of notes to cycle through
'notes' '60 62 71 64 67 74' gen_vals

# clock trigger, feeding into tpoly and tseq
1 metro dup

# cycle through the notes...
0 'notes' tseq 
# ...and set p1 of the arg table BEFORE tpoly
1 'args' tset

# tpoly: maximum number of notes
4 
# tpoly: number of user args 
# note (p1) and ampltitude (p2) included
# duration (p0) is always needed
2 

# name of argument table
'args' 

# name of table that poly creates and writes to
'poly' 

# When triggered, tpoly will read from the arg 
# table and write the information to the poly ftable
tpoly


# Create 4 identical instances (voices) of the instrument
# and sum them together.
# These voices will read from the poly ftable tpoly generated 

# -- Voice 0 --
# Get trigger 
0 0 'poly' polyget 
# attack
0.1
# hold time: get note duration 
0 1 'poly' polyget 
# subtract attack and release times
0.2 -
# release
0.1 
tenv

# Get note
0 2 'poly' polyget mtof
# get amplitude
0 3 'poly' polyget 
# car, mod, index
1 1 1 
fm
# multiply tenv and fm together
*

# -- Voice 1 --
# Get tick 
0 0 'poly' polyget 
# attack
0.1
# hold time: get note duration 
0 1 'poly' polyget 
# subtract attack and release times
0.2 -
# release
0.1 
tenv 

# Get note
0 2 'poly' polyget mtof
# get amplitude
0 3 'poly' polyget 
# car, mod, index
1 1 1 
fm
# multiply tenv and fm together
* +

# -- Voice 2 --
# Get tick 
0 0 'poly' polyget 
# attack
0.1
# hold time: get note duration 
0 1 'poly' polyget 
# subtract attack and release times
0.2 -
# release
0.1 
tenv 

# Get note
0 2 'poly' polyget mtof
# get amplitude
0 3 'poly' polyget 
# car, mod, index
1 1 1 
fm
# multiply tenv and fm together
* +

# -- Voice 3 --
# Get tick 
0 0 'poly' polyget 
# attack
0.1
# hold time: get note duration 
0 1 'poly' polyget 
# subtract attack and release times
0.2 -
# release
0.1 
tenv 

# Get note
0 2 'poly' polyget mtof
# get amplitude
0 3 'poly' polyget 
# car, mod, index
1 1 1 
fm
# multiply tenv and fm together
* +
