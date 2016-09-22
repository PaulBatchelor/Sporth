# This example shows how to use the input table to make polysporth
# an effects processor.

# load the plugin 
_ps "../polysporth.so" fl

# create an "input" table
_in 1 zeros

# An input signal of blips
200 1000 10 randh 0.5 sine
# Create a copy of the signal, set it to the input table
dup 0 _in tset

# call polysporth

(
0                # callback clock disabled
1 dmetro         # our clock producing ticks once every seond
1                # number of sporthlets to make
_in              # name of input table
_out             # name of output table created by polysporth
"01_input.scm"   # name of scheme file to load
_ps              # the named ftable storing the polysporth plugin
fe               # f-execute
)

# read from index 0 of output table, which is sporthlet 0
0 _out tget

# sum the dry and wet signal together
+ 

# close out the plugin 
_ps fc
