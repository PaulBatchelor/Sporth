# Sporth

Sporth (short for SoundPipe fORTH), is a small stack-based musical language, roughly
inspired by stack languages like Forth and PostScript.

## Features

- Stack oriented paradigm
- Written entirely in C
- 100+ unit generators to choose from
- Powered by the Soundpipe DSP library
- Unix-Friendly
- Small codebase
- Powerful C API
- Easily extendable
- Easily embeddable


## Installation

In order to compile Sporth, [SoundPipe](https://pbat.ch/proj/soundpipe.html) needs to be installed.

Then:

1. make
2. sudo make install

## Quick start

To see Sporth in action, run this command from the inside the project directory:

```
sporth -d 5s -o dialtone.wav examples/dialtone.sp
```

This will generate a 5 second audio clip of sound.

More information on Sporth can be found
[here](http://paulbatchelor.github.io/proj/sporth.html).

## Examples

Several examples demonstrating specific ugens can be found in
the [examples](examples) folder of the repository. More musical
examples can be found on the
[Sporthlings page](https://www.pbat.ch/sporthlings).

## Licensing

This project is dual-licensed under MIT or
[UNLICENSE](https://unlicense.org).