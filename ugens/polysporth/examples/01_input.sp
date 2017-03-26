# This example shows how to use the input table to make polysporth
# an effects processor.

# create an "input" variable
_in var

# An input signal of blips
200 1000 10 randh 0.5 sine
# Create a copy of the signal, set it to the input table
dup _in set

# call polysporth

(
1                # number of sporthlets to make
_out             # name of output table created by polysporth
"01_input.scm"   # name of scheme file to load
ps               # polysporth!  
)

# read from index 0 of output table, which is sporthlet 0
0 _out tget

# sum the dry and wet signal together
+ 
