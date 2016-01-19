Sporth s => dac;

s.parse("
'k' '0 2 4 6' kona
0 p 0.1 sine 350 0.1 sine +
");

s.pset(0, 440);

while(1) 
{
    1::second => now;
}
