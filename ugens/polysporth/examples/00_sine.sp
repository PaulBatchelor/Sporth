# This example shows how to load polysporth, and use it make a sine wave.

# load the plugin into an ftable called _ps via fl (fload)
_ps "../polysporth.so" fl

# create an "input" table
_in 1 zeros

# now we call polysporth

(
0                # callback clock disabled
1 dmetro         # our clock producing ticks once every seond
1                # number of sporthlets to make
_in              # name of input table
_out             # name of output table created by polysporth
"00_sine.scm"    # name of scheme file to load
_ps              # the named ftable storing the polysporth plugin
fe               # f-execute
)

# read from index 0, which is sporthlet 0
0 _out tget

# close out the plugin f-close
_ps fc
