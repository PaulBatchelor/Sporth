"tbl" sr 5 *  "mincerbuf.sp" gen_sporth

4 metro 0.4 maytrig -1 1 switch 0.001 port "tbl" tbldur / 0 phasor 
0 0 0 "tbl" tabread

4 metro 0.4 maytrig -3 3 switch 0.001 port "tbl" tbldur / 0 phasor 
1 0 0 "tbl" tabread

+
