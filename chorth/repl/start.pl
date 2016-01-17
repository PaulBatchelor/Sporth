system("rm -f sporthpipe; mkfifo sporthpipe");
$pid = fork();
if($pid == 0) {
    $run = 1;
    while($run) {
        open FILE, "sporthpipe";
        while(<FILE>)
        {
            if(m/^ \(/) {
                $str .= $_;
            } 
            if (m/\)$/) {
                system("oscsend localhost 6449 /foo/sporth s '$str'");
                $str = ''
            }
            if(m/quit/) {
                close FILE;
                unlink "sporthpipe";
                kill 'KILL', $pid;
                $run = 0;
            }
        }
    }
}

system("rlwrap -c chuck --shell repl.ck");
