_ps "../polysporth.so" fl

_in 1 zeros

# set dmetro to the trigger callback, set the clock to zero
( 0.5 dmetro 0 1 _in _out "05_lexer.scm" _ps fe )

0 3 _out ftsum

_ps fc
