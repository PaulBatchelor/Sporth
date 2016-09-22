# PolySporth

PolySporth is a utility, offering a number of extensions and enhancements 
to the Sporth language. It has been designed to address shortcomings inherrent to Sporth,
which include polyphony, events and notes, as well as general code abstraction. 

Polysporth is spawned from inside of Sporth as a ugen. From there, PolySporth is able to 
be controlled via Scheme (tinyscheme). Sporthlets can be
toggled on and off, and they scheduled using a notelist interface that 
slightly resembles a Csound score. Meta-notes also exist, which allow scheme functions
to be called instead of sporthlet voices.  Contiguous sporthlets can be grouped together
to form voice groups, which can used to implement an efficient means of 
polyphony. PolySporth has access to many under-the-hood features of soundplumber,
Sporth's sound engine based on Soundpipe. Coupled with the Scheme language, this
allows for abstractions not possible with Sporth alone, such as procedural code
generation, parameterized functions, and reusable code.


# Installation

You first will need to have the latest version of 
[Sporth](https://www.github.com/paulbatchelor/sporth.git) installed. 

From there, the plugin can be compiled by running "make", which will produce
the file "polysporth.so", which should be dropped into whatever directory 
you are writing sporth code in.

# Future plans

This is a work in progress. Future plans include:

- More scheme-based abstractoins 
- Dynamic loading of scheme plugins.
- More examples and documentation
