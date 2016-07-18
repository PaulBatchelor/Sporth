# CDB: constant database using tinyCDB public domain code
# this is a fast way to store long strings
# You will have to run this in the example directory
# for this to work...


# open up t.db
_s _t.db cdbload


# get key for oa ("oneart.wav")
# _oa is shorthand for "oa" 
_oa _s cdb 

# send string to diskin
diskin
