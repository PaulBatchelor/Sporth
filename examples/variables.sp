# clock
_bpm 120 4 * bpm2dur varset
_clock var

# fm
_freq var
_amp 0.9 varset
_mod 3.001 varset
_car 1 varset

# envelope
_env var
_atk 0.01 varset
_hold 0.01 varset
_rel 0.01 varset

# set the clock

_bpm get dmetro _clock set

# set the random notes

_clock get (440 880 trand) _freq set

# set the envelope
_clock get (_atk get) (_hold get) (_rel get) tenvx _env set

# the FM oscillator
# parentheses are optional

(_freq get) (_amp get) (_mod get) (_car get) (4 _env get *) fm 


# multiply by the envelope
_env get *
