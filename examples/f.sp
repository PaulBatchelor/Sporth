# dynamic ugen loading
# corresponding c code is in the file called gain.c

# load the plugin into ftable "test"
"test" "./gain.so" fl

# create a unit-amplitude sine wave
# 440 1 sine 
# 
# # execute the ugen 
# 0.1 "test" fe

# close the library only call this once at the end!
"test" fc
