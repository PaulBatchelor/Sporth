Sporth s => dac;

s.parse("0 p 0.1 sine 350 0.1 sine +");

s.pset(0, 440);

while(1) 
{
    1::second => now;
}
