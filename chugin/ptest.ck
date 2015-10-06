Sporth s => dac;

s.parse(
"0 p 0.01 port mtof 1 p 0.003 port 1 1 2 p 0.01 port fm " +
"0.5 * dup dup 0.94 10000 revsc drop 0.1 * +"
);

110 => float bpm;

(60.0 / bpm) :: second => dur t;

[0, 3, 7, 10] @=> int scale[];

0 => int stp;
s.pset(1, 0.5);
0 => int nbars;
60 => int base;
while(1) 
{
    if(stp == 0) { 
        s.pset(1, 0.7); 
        nbars++;
    } else s.pset(1, 0.4);
    
    if(nbars == 5) {
        if(base == 60) 53 => base; else 60 => base;
        1 => nbars;
    }
    s.pset(0, base + scale[stp] + 12 * Std.rand2(-1, 1));
    s.pset(2, Std.rand2f(0.1, 3));
    t * 0.25 => now;
    (stp + 1) % scale.cap() => stp; 
}
