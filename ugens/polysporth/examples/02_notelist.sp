_ps "../polysporth.so" fl

_in 1 zeros

# call polysporth

( 0 0.5 dmetro 
4 # make 4 sporthlets 
_in _out "02_notelist.scm" _ps fe )

# sum all the values in the out ftable

0 3 _out ftsum

# close out the plugin 
_ps fc
