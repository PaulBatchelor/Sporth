Sporth s => dac;
s.parse("0");
fun void launchSporth(string str)
{
    s.parse(str);
    me.yield();
}

OscIn oin;
OscMsg msg;
6449 => oin.port;
oin.addAddress( "/foo/sporth, s" );

while( true )
{
    oin => now;

    while( oin.recv(msg) )
    { 
        string str;
        msg.getString(0) => str;
        launchSporth(str);
        <<< "got (via OSC):", str >>>;
    }
}
