Sporth s => dac;

s.parse("
48 0 pset

0 p 0.15 + mtof 0.1 saw
0 p 0.1 - mtof 0.1 saw +
0 p mtof 0.1 saw +

");

10::second => now;
