# PolySporth

PolySporth is a utility, offering a number of extensions and enhancements 
to the Sporth language. It has been designed to address shortcomings inherrent to Sporth,
which include polyphony, events and notes, as well as general code abstraction. 

Polysporth is spawned from inside of Sporth as a ugen. From there, PolySporth is able to 
be controlled via Scheme (tinyscheme). PolySporth has access to many under-the-hood features of soundplumber,
Sporth's sound engine based on Soundpipe. Coupled with the Scheme language, this
allows for abstractions not possible with Sporth alone, such as procedural code
generation, parameterized functions, and reusable code.

