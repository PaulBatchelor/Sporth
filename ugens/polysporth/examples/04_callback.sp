_in 1 zeros

# set dmetro to the trigger callback, set the clock to zero
( 0.5 dmetro 0 1 _in _out "04_callback.scm" ps )

0 3 _out ftsum
