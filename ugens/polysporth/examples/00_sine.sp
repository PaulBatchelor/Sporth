# This example shows how to load polysporth, and use it make a sine wave.

# create an "input" table
_in 1 zeros

# now we call polysporth

(
1                # number of sporthlets to make
_out             # name of output table created by polysporth
"00_sine.scm"    # name of scheme file to load
ps               # polysporth! 
)

# read from index 0, which is sporthlet 0
0 _out tget

