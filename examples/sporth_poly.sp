# Load the file
3  # max voices
2  # number of parameters (midi number, amp)
"score.bin" "sco" poly

# Voices in Sporth have to be manually created

# Voice 1 

0 0 "sco" polyget 0.1 # tick 
0 1 "sco" polyget 0.2 - 0.1 tenv # duration - (atk + rel)
0 2 "sco" polyget mtof 0.1 sine * # midi note number 

# Voice 2

1 0 "sco" polyget 0.1 
1 1 "sco" polyget 0.2 - 0.1 tenv 
1 2 "sco" polyget mtof 0.1 sine * 
+

# Voice 3

2 0 "sco" polyget 0.1 
2 1 "sco" polyget 0.2 - 0.1  tenv 
2 2 "sco" polyget mtof 0.1 sine * 
+

