./sporthlive -o raw -d 0 -r 96000 test.sp |\
#sporthlive -o raw -d 0 -r 96000 out.sp |\
jack_wrapper 96000 1
#sporthlive -o test.wav -d 10s -r 96000 test.sp 
