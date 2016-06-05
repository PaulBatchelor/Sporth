public class SporthRepl {
    Sporth s;
    OscIn oin;
    OscMsg msg;

    fun void init() {
        s => dac;
        s.parse("0");
    }

    fun void launchSporth(string str)
    {
        s.parse(str);
        me.yield();
    }

    fun void probe() {
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
    }
}

SporthRepl repl;

repl.init();
repl.probe();
